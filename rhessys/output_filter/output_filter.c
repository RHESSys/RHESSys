#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "output_filter.h"

/**
 * Returns true of new_patch supersedes existing.
 */
static bool patch_supersedes(const OutputFilterPatch *existing, const OutputFilterPatch *new_patch) {
	switch(new_patch->output_patch_type) {
	case PATCH_TYPE_BASIN:
		if (existing->output_patch_type < PATCH_TYPE_BASIN &&
				new_patch->basinID == existing->basinID) {
			return true;
		} else {
			return false;
		}
	case PATCH_TYPE_HILLSLOPE:
		if (existing->output_patch_type < PATCH_TYPE_HILLSLOPE &&
				new_patch->basinID == existing->basinID &&
				new_patch->hillslopeID == existing->hillslopeID) {
			return true;
		} else {
			return false;
		}
	case PATCH_TYPE_ZONE:
		if (existing->output_patch_type < PATCH_TYPE_ZONE &&
				new_patch->basinID == existing->basinID &&
				new_patch->hillslopeID == existing->hillslopeID &&
				new_patch->zoneID == existing->zoneID) {
			return true;
		} else {
			return false;
		}
	case PATCH_TYPE_PATCH:
	default:
		return false;
	}
}

/**
 * Returns true of new_stratum supersedes existing.
 */
static bool stratum_supersedes(const OutputFilterStratum *existing, const OutputFilterStratum *new_stratum) {
	switch(new_stratum->output_stratum_type) {
	case STRATUM_TYPE_BASIN:
		if (existing->output_stratum_type < STRATUM_TYPE_BASIN &&
				new_stratum->basinID == existing->basinID) {
			return true;
		} else {
			return false;
		}
	case STRATUM_TYPE_HILLSLOPE:
		if (existing->output_stratum_type < STRATUM_TYPE_HILLSLOPE &&
				new_stratum->basinID == existing->basinID &&
				new_stratum->hillslopeID == existing->hillslopeID) {
			return true;
		} else {
			return false;
		}
	case STRATUM_TYPE_ZONE:
		if (existing->output_stratum_type < STRATUM_TYPE_ZONE &&
				new_stratum->basinID == existing->basinID &&
				new_stratum->hillslopeID == existing->hillslopeID &&
				new_stratum->zoneID == existing->zoneID) {
			return true;
		} else {
			return false;
		}
	case STRATUM_TYPE_PATCH:
		if (existing->output_stratum_type < STRATUM_TYPE_ZONE &&
				new_stratum->basinID == existing->basinID &&
				new_stratum->hillslopeID == existing->hillslopeID &&
				new_stratum->zoneID == existing->zoneID &&
				new_stratum->patchID == existing->patchID) {
			return true;
		} else {
			return false;
		}
	case STRATUM_TYPE_STRATUM:
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
	case PATCH_TYPE_BASIN:
		if (existing->basinID == new_patch->basinID) {
			return true;
		} else {
			return false;
		}
	case PATCH_TYPE_HILLSLOPE:
		if (existing->basinID == new_patch->basinID &&
			existing->hillslopeID == new_patch->hillslopeID) {
			return true;
		} else {
			return false;
		}
	case PATCH_TYPE_ZONE:
		if (existing->basinID == new_patch->basinID &&
			existing->hillslopeID == new_patch->hillslopeID &&
			existing->zoneID == new_patch->zoneID) {
			return true;
		} else {
			return false;
		}
	case PATCH_TYPE_PATCH:
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
 * Returns true if existing is a parent to or duplicates new_stratum.
 */
static bool stratum_is_parent_or_duplicate(const OutputFilterStratum *existing, const OutputFilterStratum *new_stratum) {
	switch(existing->output_stratum_type) {
	case STRATUM_TYPE_BASIN:
		if (existing->basinID == new_stratum->basinID) {
			return true;
		} else {
			return false;
		}
	case STRATUM_TYPE_HILLSLOPE:
		if (existing->basinID == new_stratum->basinID &&
			existing->hillslopeID == new_stratum->hillslopeID) {
			return true;
		} else {
			return false;
		}
	case STRATUM_TYPE_ZONE:
		if (existing->basinID == new_stratum->basinID &&
			existing->hillslopeID == new_stratum->hillslopeID &&
			existing->zoneID == new_stratum->zoneID) {
			return true;
		} else {
			return false;
		}
	case STRATUM_TYPE_PATCH:
		if (existing->basinID == new_stratum->basinID &&
			existing->hillslopeID == new_stratum->hillslopeID &&
			existing->zoneID == new_stratum->zoneID &&
			existing->patchID == new_stratum->patchID) {
			return true;
		} else {
			return false;
		}
	case STRATUM_TYPE_STRATUM:
	default:
		if (existing->basinID == new_stratum->basinID &&
			existing->hillslopeID == new_stratum->hillslopeID &&
			existing->zoneID == new_stratum->zoneID &&
			existing->patchID == new_stratum->patchID &&
			existing->stratumID == new_stratum->stratumID) {
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

// output_filter_patch_list
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

// output_filter_canopy_strata_list
OutputFilterStratum *create_new_output_filter_stratum() {
	OutputFilterStratum *new_stratum = (OutputFilterStratum *) malloc(sizeof(OutputFilterStratum));
	new_stratum->next = NULL;
	return new_stratum;
}

OutputFilterStratum *add_to_output_filter_stratum_list(OutputFilterStratum * const head,
		OutputFilterStratum * const new_stratum) {
	if (head == NULL) return new_stratum;
	// Iterate over stratum list, checking to see if new_stratum is duplicative, superseded by an existing
	// stratum, or supersedes and existing patch...
	OutputFilterStratum *tmp = head;
	while (tmp != NULL) {
		if (stratum_is_parent_or_duplicate(tmp, new_stratum)) {
			// new_stratum duplicates or is superseded by tmp, don't add to list
			return NULL;
		} else if (stratum_supersedes(tmp, new_stratum)) {
			// new_stratum supersedes tmp, make new_stratum replace tmp
			tmp->output_stratum_type = new_stratum->output_stratum_type;
			tmp->basinID = new_stratum->basinID;
			tmp->hillslopeID = new_stratum->hillslopeID;
			tmp->zoneID = new_stratum->zoneID;
			tmp->patchID = new_stratum->patchID;
			tmp->stratumID = new_stratum->stratumID;
			free(new_stratum);
			return tmp;
		}
		if (tmp->next == NULL) {
			// Don't run off the end of the list (i.e. preserve tmp so that we can
			// assign the new entry to the tmp->next).
			break;
		}
		tmp = tmp->next;
	}
	// new_stratum is not a duplicate of, nor superseded by any current entries in the
	// list, add it to the end of the list.
	tmp->next = new_stratum;
	return tmp->next;
}

void free_output_filter_stratum_list(OutputFilterStratum *head) {
	if (head == NULL) { return; }
	if (head->next != NULL) {
		free_output_filter_stratum_list(head->next);
	}
	free(head);
}

// output_filter_variable_list
OutputFilterVariable *create_new_output_filter_variable(char *name) {
	OutputFilterVariable *new_var = (OutputFilterVariable *) malloc(sizeof(OutputFilterVariable));
	new_var->next = NULL;
	new_var->variable_type = NAMED;
	new_var->data_type = DATA_TYPE_UNDEFINED;
	new_var->name = strdup(name);
	new_var->offset = SIZE_MAX;
	new_var->meta = NULL;
	return new_var;
}

OutputFilterVariable *create_new_output_filter_variable_any() {
	OutputFilterVariable *new_var = (OutputFilterVariable *) malloc(sizeof(OutputFilterVariable));
	new_var->next = NULL;
	new_var->variable_type = ANY_VAR;
	new_var->data_type = DATA_TYPE_UNDEFINED;
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
	output->meta = NULL;
	output->materialized_variables = NULL;
	output->fp = NULL;
	return output;
}

void free_output_filter_output(OutputFilterOutput *output) {
	free(output->path);
	free(output->filename);
	if (output->materialized_variables != NULL) free(output->materialized_variables);
	fclose(output->fp);
	free(output);
}

OutputFilter *create_new_output_filter() {
	OutputFilter *new_filter = (OutputFilter *) malloc(sizeof(OutputFilter));
	new_filter->type = OUTPUT_FILTER_UNDEFINED;
	new_filter->timestep = TIMESTEP_UNDEFINED;
	new_filter->next = NULL;
	new_filter->output = NULL;
	new_filter->patches = NULL;
	new_filter->strata = NULL;
	new_filter->variables = NULL;
	new_filter->num_named_variables = 0;
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

void print_output_filter_output(OutputFilterOutput *o, char *prefix) {
	if (prefix == NULL) {
		prefix = "";
	}
	fprintf(stderr, "%sOutputFilterOutput@%p {\n", prefix, (void *)o);
	if (o != NULL) {
		switch (o->format) {
		case OUTPUT_TYPE_CSV:
			fprintf(stderr, "%s\tformat: csv,\n", prefix);
			break;
		case OUTPUT_TYPE_NETCDF:
			fprintf(stderr, "%s\tformat: netcdf,\n", prefix);
			break;
		}
		fprintf(stderr, "%s\tpath: %s,\n", prefix, o->path);
		fprintf(stderr, "%s\tfilename: %s,\n", prefix, o->filename);
	}
	fprintf(stderr, "%s}", prefix);
}

void print_output_filter_patch(OutputFilterPatch *p, char *prefix) {
	if (prefix == NULL) {
		prefix = "";
	}
	fprintf(stderr, "%sOutputFilterPatch@%p {\n", prefix, (void *)p);
	if (p != NULL) {
		fprintf(stderr, "%s\tnext: %p,\n", prefix, (void *)p->next);
		switch (p->output_patch_type) {
		case PATCH_TYPE_BASIN:
			fprintf(stderr, "%s\toutput_patch_type: basin,\n", prefix);
			break;
		case PATCH_TYPE_HILLSLOPE:
			fprintf(stderr, "%s\toutput_patch_type: hillslope,\n", prefix);
			break;
		case PATCH_TYPE_ZONE:
			fprintf(stderr, "%s\toutput_patch_type: zone,\n", prefix);
			break;
		case PATCH_TYPE_PATCH:
			fprintf(stderr, "%s\toutput_patch_type: patch,\n", prefix);
			break;
		}

		fprintf(stderr, "%s\tbasinID: %d,\n", prefix, p->basinID);
		fprintf(stderr, "%s\tbasin: %p,\n", prefix, p->basin);

		fprintf(stderr, "%s\thillslopeID: %d,\n", prefix, p->hillslopeID);
		fprintf(stderr, "%s\thillslope: %p,\n", prefix, p->hill);

		fprintf(stderr, "%s\tzoneID: %d,\n", prefix, p->zoneID);
		fprintf(stderr, "%s\tzone: %p,\n", prefix, p->zone);

		fprintf(stderr, "%s\tpatchID: %d,\n", prefix, p->patchID);
		fprintf(stderr, "%s\tpatch: %p,\n", prefix, p->patch);
	}
	fprintf(stderr, "%s}", prefix);
}

void print_output_filter_stratum(OutputFilterStratum *s, char *prefix) {
	if (prefix == NULL) {
		prefix = "";
	}
	fprintf(stderr, "%sOutputFilterStratum@%p {\n", prefix, (void *)s);
	if (s != NULL) {
		fprintf(stderr, "%s\tnext: %p,\n", prefix, (void *)s->next);
		switch (s->output_stratum_type) {
		case STRATUM_TYPE_BASIN:
			fprintf(stderr, "%s\toutput_stratum_type: basin,\n", prefix);
			break;
		case STRATUM_TYPE_HILLSLOPE:
			fprintf(stderr, "%s\toutput_stratum_type: hillslope,\n", prefix);
			break;
		case STRATUM_TYPE_ZONE:
			fprintf(stderr, "%s\toutput_stratum_type: zone,\n", prefix);
			break;
		case STRATUM_TYPE_PATCH:
			fprintf(stderr, "%s\toutput_stratum_type: patch,\n", prefix);
			break;
		case STRATUM_TYPE_STRATUM:
			fprintf(stderr, "%s\toutput_stratum_type: stratum,\n", prefix);
			break;
		}

		fprintf(stderr, "%s\tbasinID: %d,\n", prefix, s->basinID);
		fprintf(stderr, "%s\tbasin: %p,\n", prefix, s->basin);

		fprintf(stderr, "%s\thillslopeID: %d,\n", prefix, s->hillslopeID);
		fprintf(stderr, "%s\thillslope: %p,\n", prefix, s->hill);

		fprintf(stderr, "%s\tzoneID: %d,\n", prefix, s->zoneID);
		fprintf(stderr, "%s\tzone: %p,\n", prefix, s->zone);

		fprintf(stderr, "%s\tpatchID: %d,\n", prefix, s->patchID);
		fprintf(stderr, "%s\tpatch: %p,\n", prefix, s->patch);

		fprintf(stderr, "%s\tstratumID: %d,\n", prefix, s->stratumID);
		fprintf(stderr, "%s\tstratum: %p,\n", prefix, s->stratum);
	}
	fprintf(stderr, "%s}", prefix);
}

void print_output_filter_variable(OutputFilterVariable *v, char *prefix) {
	if (prefix == NULL) {
		prefix = "";
	}
	fprintf(stderr, "%sOutputFilterVariable@%p {\n", prefix, (void *)v);
	if (v != NULL) {
		fprintf(stderr, "%s\tnext: %p,\n", prefix, (void *)v->next);
		switch (v->variable_type) {
		case ANY_VAR:
			fprintf(stderr, "%s\tvariable_type: any,\n", prefix);
			break;
		case NAMED:
			fprintf(stderr, "%s\tvariable_type: named,\n", prefix);
			break;
		}

		switch (v->data_type) {
		case DATA_TYPE_UNDEFINED:
			fprintf(stderr, "%s\tvariable_type: undefined,\n", prefix);
			break;
		case DATA_TYPE_BOOL:
			fprintf(stderr, "%s\tvariable_type: bool,\n", prefix);
			break;
		case DATA_TYPE_CHAR_ARRAY:
			fprintf(stderr, "%s\tvariable_type: string,\n", prefix);
			break;
		case DATA_TYPE_INT:
			fprintf(stderr, "%s\tvariable_type: int,\n", prefix);
			break;
		case DATA_TYPE_LONG:
			fprintf(stderr, "%s\tvariable_type: long,\n", prefix);
			break;
		case DATA_TYPE_LONG_ARRAY:
			fprintf(stderr, "%s\tvariable_type: long[],\n", prefix);
			break;
		case DATA_TYPE_FLOAT:
			fprintf(stderr, "%s\tvariable_type: float,\n", prefix);
			break;
		case DATA_TYPE_DOUBLE:
			fprintf(stderr, "%s\tvariable_type: double,\n", prefix);
			break;
		case DATA_TYPE_DOUBLE_ARRAY:
			fprintf(stderr, "%s\tvariable_type: double[],\n", prefix);
			break;
		case DATA_TYPE_FILE_PTR:
			fprintf(stderr, "%s\tvariable_type: FILE *,\n", prefix);
			break;
		case DATA_TYPE_STRUCT:
			fprintf(stderr, "%s\tvariable_type: struct,\n", prefix);
			break;
		case DATA_TYPE_STRUCT_ARRAY:
			fprintf(stderr, "%s\tvariable_type: struct[],\n", prefix);
			break;
		case DATA_TYPE_STRUCT_PTR_ARRAY:
			fprintf(stderr, "%s\tvariable_type: struct *[],\n", prefix);
			break;
		default:
			break;
		}

		fprintf(stderr, "%s\tname: %s,\n", prefix, v->name);
		fprintf(stderr, "%s\toffset: %zu,\n", prefix, v->offset);
	}
	fprintf(stderr, "%s}", prefix);
}

void print_output_filter(OutputFilter *f) {
	fprintf(stderr, "OutputFilter@%p {\n", (void *)f);
	if (f != NULL) {
		fprintf(stderr, "\tnext: %p,\n", (void *)f->next);

		switch (f->type) {
		case OUTPUT_FILTER_UNDEFINED:
			fprintf(stderr, "\ttype: undefined,\n");
			break;
		case OUTPUT_FILTER_PATCH:
			fprintf(stderr, "\ttype: patch,\n");
			break;
		case OUTPUT_FILTER_CANOPY_STRATUM:
			fprintf(stderr, "\ttype: canopy strata,\n");
			break;
		}

		switch (f->timestep) {
		case TIMESTEP_UNDEFINED:
			fprintf(stderr, "\timestep: undefined,\n");
			break;
		case TIMESTEP_HOURLY:
			fprintf(stderr, "\timestep: hourly,\n");
			break;
		case TIMESTEP_DAILY:
			fprintf(stderr, "\timestep: daily,\n");
			break;
		case TIMESTEP_MONTHLY:
			fprintf(stderr, "\timestep: monthly,\n");
			break;
		case TIMESTEP_YEARLY:
			fprintf(stderr, "\timestep: yearly,\n");
			break;
		}

		fprintf(stderr, "\toutput: ");
		print_output_filter_output(f->output, "\t");
		fprintf(stderr, ",\n");

		switch(f->type) {
		case OUTPUT_FILTER_PATCH:
			fprintf(stderr, "\tpatches: [\n");
			for (OutputFilterPatch *p = f->patches; p != NULL; p = p->next) {
				print_output_filter_patch(p, "\t\t");
				fprintf(stderr, ",\n");
			}
			fprintf(stderr, "\t],\n");
			break;
		case OUTPUT_FILTER_CANOPY_STRATUM:
			fprintf(stderr, "\tstrata: [\n");
			for (OutputFilterStratum *s = f->strata; s != NULL; s = s->next) {
				print_output_filter_stratum(s, "\t\t");
				fprintf(stderr, ",\n");
			}
			fprintf(stderr, "\t],\n");
			break;
		case OUTPUT_FILTER_UNDEFINED:
		default:
			break;
		}


		fprintf(stderr, "\tnum_named_variables: %hu,\n", f->num_named_variables);

		fprintf(stderr, "\tvariables: [\n");
		for (OutputFilterVariable *v = f->variables; v != NULL; v = v->next) {
			print_output_filter_variable(v, "\t\t");
			fprintf(stderr, ",\n");
		}
		fprintf(stderr, "\t],\n");
	}

	fprintf(stderr, "}\n");
}

bool return_with_error(char * const error, size_t error_len, char *error_mesg) {
	strncpy(error, error_mesg, error_len);
	return false;
}
