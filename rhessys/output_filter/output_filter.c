#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
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
	if (existing->variable_type == ANY_VAR) {
		// * supersedes all.
		return true;
	} else if (existing->sub_struct_varname == NULL && new_var->sub_struct_varname == NULL){
		// Simple variable name, just compare names
		return strcmp(existing->name, new_var->name) == 0;
	} else if (existing->sub_struct_varname != NULL && new_var->sub_struct_varname != NULL) {
		// Compound variable name, compare both parts
		int name_cmp = strcmp(existing->name, new_var->name);
		int sub_struct_varname_cmp = strcmp(existing->sub_struct_varname, new_var->sub_struct_varname);
		if (name_cmp == 0 && sub_struct_varname_cmp == 0) {
			// Both parts are equal
			return true;
		}
	}
	return false;
}

// Variable expression support
OutputFilterExprAst *new_of_expr_ast(int nodetype, OutputFilterExprAst *l, OutputFilterExprAst *r) {
    OutputFilterExprAst *ast = (OutputFilterExprAst *) malloc(sizeof(OutputFilterExprAst));
    ast->nodetype = nodetype;
    ast->l = l;
    ast->r = r;
    return ast;
}

OutputFilterExprAst *new_of_expr_const(double d) {
    OutputFilterExprNumval *n = (OutputFilterExprNumval *) malloc(sizeof(OutputFilterExprNumval));
    n->nodetype = OF_VAR_EXPR_AST_NODE_CONST;
    n->number = d;
    return (OutputFilterExprAst *) n;
}

OutputFilterExprName *new_of_expr_name(const char *name,
                                       const char *sub_struct_name,
                                       OutputFilterVariable *var) {
    OutputFilterExprName *n = (OutputFilterExprName *) malloc(sizeof(OutputFilterExprName));
    n->nodetype = OF_VAR_EXPR_AST_NODE_NAME;
    n->name = name;
    n->sub_struct_name = sub_struct_name;
    n->var = var;
    return (OutputFilterExprAst *) n;
}

// What will be the return type? Not a double?
double of_expr_eval(OutputFilterExprAst *a) {
    // TBD
}

void free_of_expr_ast(OutputFilterExprAst *a) {
    switch (a->nodetype) {
        /* two subtrees */
        case '+':
        case '-':
        case '*':
        case '/':
            free_of_expr_ast(a->r);
            /* one subtree */
        case OF_VAR_EXPR_AST_NODE_UNARY_MINUS:
            free_of_expr_ast(a->l);
            /* no subtree */
        case OF_VAR_EXPR_AST_NODE_CONST:
        case OF_VAR_EXPR_AST_NODE_NAME:
            free(a);
            break;

        default:
            printf("free_of_expr_ast: unknown node type: %c\n", a->nodetype);
    }
}

void print_of_expr_ast(OutputFilterExprAst *a, int level) {
    fprintf(stderr, "%*s", level*2, "");
    fprintf(stderr, "OutputFilterExprAst@%p {\n", a);
    switch(a->nodetype) {
        case OF_VAR_EXPR_AST_NODE_NAME:
            fprintf(stderr, "%*s", level*4, "");
            fprintf(stderr, "IDENTIFIER(%s);\n", ((OutputFilterExprName *)a)->name);
            break;
        case OF_VAR_EXPR_AST_NODE_CONST:
            fprintf(stderr, "%*s", level*4, "");
            fprintf(stderr, "CONSTANT(%f);\n", ((OutputFilterExprNumval *)a)->number);
            break;
        case '+':
            print_of_expr_ast(a->l, level+1);
            fprintf(stderr, "%*s", level*4, "");
            fprintf(stderr, "OP(+)\n");
            print_of_expr_ast(a->r, level+1);
            break;
        case '-':
            print_of_expr_ast(a->l, level+1);
            fprintf(stderr, "%*s", level*4, "");
            fprintf(stderr, "OP(-)\n");
            print_of_expr_ast(a->r, level+1);
            break;
        case '*':
            print_of_expr_ast(a->l,level+1);
            fprintf(stderr, "%*s", level*4, "");
            fprintf(stderr, "OP(*)\n");
            print_of_expr_ast(a->r, level+1);
            break;
        case '/':
            print_of_expr_ast(a->l, level+1);
            fprintf(stderr, "%*s", level*4, "");
            fprintf(stderr, "OP(/)\n");
            print_of_expr_ast(a->r, level+1);
            break;
        case OF_VAR_EXPR_AST_NODE_UNARY_MINUS:
            fprintf(stderr, "%*s", level*4, "");
            fprintf(stderr, "UNARY_OP(-)\n");
            print_of_expr_ast(a->l, level+1);
            break;
    }
    fprintf(stderr, "%*s", level*2, "");
    fprintf(stderr, "}\n");
}

// output_filter_basin_list
OutputFilterBasin *create_new_output_filter_basin() {
	OutputFilterBasin *new_basin = (OutputFilterBasin *) malloc(sizeof(OutputFilterBasin));
	new_basin->next = NULL;
	return new_basin;
}

OutputFilterBasin *add_to_output_filter_basin_list(OutputFilterBasin * const head,
		OutputFilterBasin * const new_basin) {
	if (head == NULL) return new_basin;
	// Iterate over basin list, checking to see if new_basin is duplicative
	OutputFilterBasin *tmp = head;
	while (tmp != NULL) {
		if (tmp->basinID == new_basin->basinID) {
			// new_patch duplicates tmp, don't add to list
			return NULL;
		}
		if (tmp->next == NULL) {
			// Don't run off the end of the list (i.e. preserve tmp so that we can
			// assign the new entry to the tmp->next).
			break;
		}
		tmp = tmp->next;
	}
	// new_basin is not a duplicate of any current entries in the list, add it to the end of the list.
	tmp->next = new_basin;
	return tmp->next;
}

void free_output_filter_basin_list(OutputFilterBasin *head) {
	if (head == NULL) { return; }
	if (head->next != NULL) {
		free_output_filter_basin_list(head->next);
	}
	free(head);
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
OutputFilterVariable *create_new_output_filter_variable(HierarchyLevel level, char *name) {
	OutputFilterVariable *new_var = (OutputFilterVariable *) malloc(sizeof(OutputFilterVariable));
	new_var->next = NULL;
	new_var->hierarchy_level = level;
	new_var->variable_type = NAMED;
	new_var->data_type = DATA_TYPE_UNDEFINED;
	new_var->name = strdup(name);
	new_var->sub_struct_varname = NULL;
	new_var->offset = SIZE_MAX;
	new_var->sub_struct_var_offset = SIZE_MAX;
	new_var->expr = NULL;
	new_var->meta = NULL;
	return new_var;
}

OutputFilterVariable *create_new_output_filter_sub_struct_variable(HierarchyLevel level, char *name,
                                                                   char *sub_struct_varname) {
	OutputFilterVariable *new_var = (OutputFilterVariable *) malloc(sizeof(OutputFilterVariable));
	new_var->next = NULL;
	new_var->hierarchy_level = level;
	new_var->variable_type = NAMED;
	new_var->data_type = DATA_TYPE_UNDEFINED;
	new_var->name = strdup(name);
	new_var->sub_struct_varname = strdup(sub_struct_varname);
	new_var->offset = SIZE_MAX;
	new_var->sub_struct_var_offset = SIZE_MAX;
	new_var->expr = NULL;
	new_var->meta = NULL;
	return new_var;
}

OutputFilterVariable *create_new_output_filter_variable_any() {
	OutputFilterVariable *new_var = (OutputFilterVariable *) malloc(sizeof(OutputFilterVariable));
	new_var->next = NULL;
	new_var->hierarchy_level = OF_HIERARCHY_LEVEL_UNDEFINED;
	new_var->variable_type = ANY_VAR;
	new_var->data_type = DATA_TYPE_UNDEFINED;
	new_var->sub_struct_varname = NULL;
	new_var->name = NULL;
	new_var->offset = SIZE_MAX;
	new_var->sub_struct_var_offset = SIZE_MAX;
    new_var->expr = NULL;
	return new_var;
}

OutputFilterVariable *create_new_output_filter_expr_variable(HierarchyLevel level, char *name,
                                                             OutputFilterExprAst *expr) {
    OutputFilterVariable *new_var = (OutputFilterVariable *) malloc(sizeof(OutputFilterVariable));
    new_var->next = NULL;
    new_var->hierarchy_level = level;
    new_var->variable_type = VAR_TYPE_EXPR;
    new_var->data_type = DATA_TYPE_UNDEFINED;
    new_var->name = strdup(name);
    new_var->sub_struct_varname = NULL;
    new_var->offset = SIZE_MAX;
    new_var->sub_struct_var_offset = SIZE_MAX;
    new_var->expr = expr;
    new_var->meta = NULL;
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
			if (tmp->sub_struct_varname != NULL) {
				free(tmp->sub_struct_varname);
			}
			if (new_var->sub_struct_varname == NULL) {
				tmp->sub_struct_varname = NULL;
			} else {
				tmp->sub_struct_varname = strdup(new_var->sub_struct_varname);
			}
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
	new_filter->basins = NULL;
	new_filter->patches = NULL;
	new_filter->strata = NULL;
	new_filter->variables = NULL;
	new_filter->num_variables = 0;
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

void print_output_filter_basin(OutputFilterBasin *b, char *prefix) {
	if (prefix == NULL) {
		prefix = "";
	}
	fprintf(stderr, "%sOutputFilterBasin@%p {\n", prefix, (void *)b);
	if (b != NULL) {
		fprintf(stderr, "%s\tbasinID: %d,\n", prefix, b->basinID);
		fprintf(stderr, "%s\tbasin: %p,\n", prefix, b->basin);
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
		switch (v->hierarchy_level) {
		case OF_HIERARCHY_LEVEL_UNDEFINED:
			fprintf(stderr, "%s\thierarchy_level: undefined,\n", prefix);
			break;
		case OF_HIERARCHY_LEVEL_BASIN:
			fprintf(stderr, "%s\thierarchy_level: basin,\n", prefix);
			break;
		case OF_HIERARCHY_LEVEL_HILLSLOPE:
			fprintf(stderr, "%s\thierarchy_level: hillslope,\n", prefix);
			break;
		case OF_HIERARCHY_LEVEL_ZONE:
			fprintf(stderr, "%s\thierarchy_level: zone,\n", prefix);
			break;
		case OF_HIERARCHY_LEVEL_PATCH:
			fprintf(stderr, "%s\thierarchy_level: patch,\n", prefix);
			break;
		case OF_HIERARCHY_LEVEL_STRATUM:
			fprintf(stderr, "%s\thierarchy_level: stratum,\n", prefix);
			break;
		}

		switch (v->variable_type) {
		case ANY_VAR:
			fprintf(stderr, "%s\tvariable_type: any,\n", prefix);
			break;
		case NAMED:
			fprintf(stderr, "%s\tvariable_type: named,\n", prefix);
			break;
		case VAR_TYPE_EXPR:
		    fprintf(stderr, "%s\tvariable_type: expression,\n", prefix);
		    break;
		default:
            fprintf(stderr, "%s\tvariable_type: undefined,\n", prefix);
            break;
		}

		switch (v->data_type) {
		case DATA_TYPE_UNDEFINED:
			fprintf(stderr, "%s\ttype: undefined,\n", prefix);
			break;
		case DATA_TYPE_BOOL:
			fprintf(stderr, "%s\ttype: bool,\n", prefix);
			break;
		case DATA_TYPE_STRING:
			fprintf(stderr, "%s\ttype: string,\n", prefix);
			break;
		case DATA_TYPE_INT:
			fprintf(stderr, "%s\ttype: int,\n", prefix);
			break;
		case DATA_TYPE_LONG:
			fprintf(stderr, "%s\ttype: long,\n", prefix);
			break;
		case DATA_TYPE_LONG_ARRAY:
			fprintf(stderr, "%s\ttype: long[],\n", prefix);
			break;
		case DATA_TYPE_FLOAT:
			fprintf(stderr, "%s\ttype: float,\n", prefix);
			break;
		case DATA_TYPE_DOUBLE:
			fprintf(stderr, "%s\ttype: double,\n", prefix);
			break;
		case DATA_TYPE_DOUBLE_ARRAY:
			fprintf(stderr, "%s\ttype: double[],\n", prefix);
			break;
		case DATA_TYPE_FILE_PTR:
			fprintf(stderr, "%s\ttype: FILE *,\n", prefix);
			break;
		case DATA_TYPE_STRUCT:
			fprintf(stderr, "%s\ttype: struct,\n", prefix);
			break;
		case DATA_TYPE_STRUCT_PTR:
			fprintf(stderr, "%s\ttype: struct *,\n", prefix);
			break;
		case DATA_TYPE_STRUCT_PTR_ARRAY:
			fprintf(stderr, "%s\ttype: struct **,\n", prefix);
			break;
		default:
			break;
		}

		fprintf(stderr, "%s\tname: %s,\n", prefix, v->name);
		fprintf(stderr, "%s\tsub_struct_varname: %s,\n", prefix, v->sub_struct_varname);
		fprintf(stderr, "%s\toffset: %zu,\n", prefix, v->offset);
		fprintf(stderr, "%s\tsub_struct_var_offset: %zu,\n", prefix, v->sub_struct_var_offset);
		if (v->expr != NULL) {
            fprintf(stderr, "%s\texpr:\n", prefix);
            print_of_expr_ast(v->expr, 16);
        }
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
		case OUTPUT_FILTER_BASIN:
			fprintf(stderr, "\ttype: basin,\n");
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
			fprintf(stderr, "\ttimestep: undefined,\n");
			break;
		case TIMESTEP_HOURLY:
			fprintf(stderr, "\ttimestep: hourly,\n");
			break;
		case TIMESTEP_DAILY:
			fprintf(stderr, "\ttimestep: daily,\n");
			break;
		case TIMESTEP_MONTHLY:
			fprintf(stderr, "\ttimestep: monthly,\n");
			break;
		case TIMESTEP_YEARLY:
			fprintf(stderr, "\ttimestep: yearly,\n");
			break;
		}

		fprintf(stderr, "\toutput: ");
		print_output_filter_output(f->output, "\t");
		fprintf(stderr, ",\n");

		switch(f->type) {
		case OUTPUT_FILTER_BASIN:
			fprintf(stderr, "\basins: [\n");
			for (OutputFilterBasin *b = f->basins; b != NULL; b = b->next) {
				print_output_filter_basin(b, "\t\t");
				fprintf(stderr, ",\n");
			}
			fprintf(stderr, "\t],\n");
			break;
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

		fprintf(stderr, "\tnum_named_variables: %hu,\n", f->num_variables);

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
