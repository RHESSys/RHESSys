#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "output_filter.h"

/**
 * Returns true of new_patch supersedes existing.
 */
static bool patch_supersedes(const OutputFilterPatch *existing, const OutputFilterPatch *new_patch) {
	switch(new_patch->output_patch_type) {
	case ALL_PATCHES:
		// * supersedes all.
		return true;
	case BASIN:
		if (existing->output_patch_type < BASIN &&
				new_patch->basinID == existing->basinID) {
			return true;
		} else {
			return false;
		}
	case HILLSLOPE:
		if (existing->output_patch_type < HILLSLOPE &&
				new_patch->basinID == existing->basinID &&
				new_patch->hillslopeID == existing->hillslopeID) {
			return true;
		} else {
			return false;
		}
	case ZONE:
		if (existing->output_patch_type < ZONE &&
				new_patch->basinID == existing->basinID &&
				new_patch->hillslopeID == existing->hillslopeID &&
				new_patch->zoneID == existing->zoneID) {
			return true;
		} else {
			return false;
		}
	case PATCH:
	default:
		return false;
	}
}

/**
 * Returns true of new_var supersedes existing.
 */
static bool new_var_supersedes(const OutputFilterVariable *existing, const OutputFilterVariable *new_var) {
	switch(new_var->variable_type) {
	case ANY_VAR:
		// * supersedes all.
		return true;
	case NAMED:
	default:
		return false;
	}
}

/**
 * Returns true if existing is a parent to or duplicates new_patch.
 */
static bool patch_is_parent_or_duplicate(const OutputFilterPatch *existing, const OutputFilterPatch *new_patch) {
	switch(existing->output_patch_type) {
	case ALL_PATCHES:
		// * is parent to all.
		return true;
	case BASIN:
		if (existing->basinID == new_patch->basinID) {
			return true;
		} else {
			return false;
		}
	case HILLSLOPE:
		if (existing->basinID == new_patch->basinID &&
			existing->hillslopeID == new_patch->hillslopeID) {
			return true;
		} else {
			return false;
		}
	case ZONE:
		if (existing->basinID == new_patch->basinID &&
			existing->hillslopeID == new_patch->hillslopeID &&
			existing->zoneID == new_patch->zoneID) {
			return true;
		} else {
			return false;
		}
	case PATCH:
	default:
		if (existing->basinID == new_patch->basinID &&
			existing->hillslopeID == new_patch->hillslopeID &&
			existing->zoneID == new_patch->zoneID &&
			existing->patchID == new_patch->patchID) {
			return true;
		} else {
			return false;
		}
	}
}

/**
 * Returns true if existing supersedes or duplicates new_var.
 */
static bool var_supersedes_or_duplicates(const OutputFilterVariable *existing, const OutputFilterVariable *new_var) {
	switch(existing->variable_type) {
	case ANY_VAR:
		// * supersedes all.
		return true;
	case NAMED:
	default:
		return strcmp(existing->name, new_var->name) == 0;
	}
}

OutputFilterPatch *create_new_output_filter_patch() {
	OutputFilterPatch *new_patch = (OutputFilterPatch *) malloc(sizeof(OutputFilterPatch));
	new_patch->next = NULL;
	return new_patch;
}

OutputFilterPatch *add_to_output_filter_patch_list(OutputFilterPatch * const head,
		OutputFilterPatch * const new_patch) {
	if (head == NULL) return new_patch;
	// Iterate over patch list, checking to see if new_patch is duplicative, superseded by an existing
	// patch, or supersedes and existing patch...
	OutputFilterPatch *tmp = head;
	while (tmp != NULL) {
		if (patch_is_parent_or_duplicate(tmp, new_patch)) {
			// new_patch duplicates or is superseded by tmp, don't add to list
			return NULL;
		} else if (patch_supersedes(tmp, new_patch)) {
			// new_patch supersedes tmp, make new_patch replace tmp
			tmp->output_patch_type = new_patch->output_patch_type;
			tmp->basinID = new_patch->basinID;
			tmp->hillslopeID = new_patch->hillslopeID;
			tmp->zoneID = new_patch->zoneID;
			tmp->patchID = new_patch->patchID;
			free(new_patch);
			return tmp;
		}
		if (tmp->next == NULL) {
			// Don't run off the end of the list (i.e. preserve tmp so that we can
			// assign the new entry to the tmp->next).
			break;
		}
		tmp = tmp->next;
	}
	// new_patch is not a duplicate of, nor superseded by any current entries in the
	// list, add it to the end of the list.
	tmp->next = new_patch;
	return tmp->next;
}

void free_output_filter_patch_list(OutputFilterPatch *head) {
	if (head == NULL) { return; }
	if (head->next != NULL) {
		free_output_filter_patch_list(head->next);
	}
	free(head);
}

OutputFilterVariable *create_new_output_filter_variable(char *name) {
	OutputFilterVariable *new_var = (OutputFilterVariable *) malloc(sizeof(OutputFilterVariable));
	new_var->next = NULL;
	new_var->variable_type = NAMED;
	new_var->name = strdup(name);
	new_var->offset = SIZE_MAX;
	return new_var;
}

OutputFilterVariable *create_new_output_filter_variable_any() {
	OutputFilterVariable *new_var = (OutputFilterVariable *) malloc(sizeof(OutputFilterVariable));
	new_var->next = NULL;
	new_var->variable_type = ANY_VAR;
	new_var->name = NULL;
	new_var->offset = SIZE_MAX;
	return new_var;
}

OutputFilterVariable *add_to_output_filter_variable_list(OutputFilterVariable * const head,
		OutputFilterVariable * const new_var) {
	if (head == NULL) return new_var;
	// Iterate over variable list, checking to see if new_variable is duplicative, superseded by an existing
	// variable, or supersedes and existing variable...
	OutputFilterVariable *tmp = head;
	while (tmp != NULL) {
		if (var_supersedes_or_duplicates(tmp, new_var)) {
			// new_var duplicates or is duplicated by tmp, don't add to list
			return NULL;
		} else if (new_var_supersedes(tmp, new_var)) {
			// new_var supersedes tmp, make new_var replace tmp
			tmp->variable_type = new_var->variable_type;
			free(tmp->name);
			tmp->name = strdup(new_var->name);
			free(new_var);
			return tmp;
		}
		if (tmp->next == NULL) {
			// Don't run off the end of the list (i.e. preserve tmp so that we can
			// assign the new entry to the tmp->next).
			break;
		}
		tmp = tmp->next;
	}
	// new_var is not a duplicate of, nor superseded by any current entries in the
	// list, add it to the end of the list.
	tmp->next = new_var;
	return tmp->next;
}

void free_output_filter_variable_list(OutputFilterVariable *head) {
	if (head == NULL) { return; }
	if (head->next != NULL) {
		free_output_filter_variable_list(head->next);
	}
	free(head->name);
	free(head);
}

OutputFilterOutput *create_new_output_filter_output() {
	OutputFilterOutput *output = (OutputFilterOutput *) malloc(sizeof(OutputFilter));
	output->filename = NULL;
	output->path = NULL;
	return output;
}

void free_output_filter_output(OutputFilterOutput *output) {
	free(output->path);
	free(output->filename);
	free(output);
}

OutputFilter *create_new_output_filter() {
	OutputFilter *new_filter = (OutputFilter *) malloc(sizeof(OutputFilter));
	new_filter->next = NULL;
	new_filter->output = NULL;
	new_filter->patches = NULL;
	new_filter->variables = NULL;
	new_filter->parse_error = false;
	return new_filter;
}

OutputFilter *add_to_output_filter_list(OutputFilter * const head,
		OutputFilter * const new_filter) {
	if (head == NULL) return new_filter;
	OutputFilter *tmp = head;
	while (tmp != NULL) {
		if (tmp->next == NULL) {
			// Don't run off the end of the list (i.e. preserve tmp so that we can
			// assign the new entry to the tmp->next).
			break;
		}
		tmp = tmp->next;
	}
	tmp->next = new_filter;
	return tmp->next;
}

void free_output_filter(OutputFilter *head) {
	if (head == NULL) { return; }
	if (head->next != NULL) {
		free_output_filter(head->next);
	}
	free_output_filter_variable_list(head->variables);
	free_output_filter_patch_list(head->patches);
	free_output_filter_output(head->output);
	free(head);
}
