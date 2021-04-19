#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "rhessys.h"
#include "output_filter/output_filter_output.h"
#include "pointer_set.h"


bool output_format_csv_write_data(char * const error, size_t error_len,
                struct date date, OutputFilter * const f,
                EntityID id, MaterializedVariable * const vars, bool flush);

bool output_format_netcdf_write_data(char * const error, size_t error_len,
                struct date date, OutputFilter * const f,
                EntityID id, MaterializedVariable * const vars, bool flush);

inline static void reset_materialized_variable_array_values(OutputFilter const * const f) {
	if (f == NULL) return;
	for (int i = 0; i < f->num_variables; i++) {
		// double_val should be the widest value, so this should zero any value.
		f->output->materialized_variables[i].u.double_val = 0.0;
	}
}

inline static void scale_materialized_variable_array_values(OutputFilter const * const f, double scalar) {
	// TODO: Generalize this to handle all numeric types for value (not just double)
	if (f == NULL) return;

	MaterializedVariable *mat_vars = f->output->materialized_variables;

	for (int i = 0; i < f->num_variables; i++) {
		if (mat_vars[i].data_type == DATA_TYPE_DOUBLE) {
			mat_vars[i].u.double_val /= scalar;
		}
	}
}

inline static size_t compute_struct_member_offset(OutputFilterVariable const * const v) {
    size_t offset = v->offset;
    if (v->sub_struct_var_offset != SIZE_MAX) {
        // If sub_struct_var_offset has been set for this variable
        // (which means this must be a sub-struct variable) apply it to the offset
        offset += v->sub_struct_var_offset;
    }
    return offset;
}

inline static void accum_materialized_variable(MaterializedVariable *accum, MaterializedVariable *value,
		double scalar) {
	// TODO: Generalize this to allow scaling for all numeric types (not just double)
	switch (value->data_type) {
	case DATA_TYPE_BOOL:
		accum->u.bool_val |= value->u.bool_val;
		break;
	case DATA_TYPE_CHAR:
		accum->u.char_val += value->u.char_val;
		break;
	case DATA_TYPE_INT:
		accum->u.int_val += value->u.int_val;
		break;
	case DATA_TYPE_LONG:
		accum->u.long_val += value->u.long_val;
		break;
	case DATA_TYPE_FLOAT:
		accum->u.float_val += value->u.float_val;
		break;
	case DATA_TYPE_DOUBLE:
		accum->u.double_val += value->u.double_val * scalar;
		break;
	default:
		fprintf(stderr, "WARNING: output_filter_output::accum_materialized_variable(): Unable to accumulate materialized variable of type %d.\n",
				value->data_type);
		break;
	}
	// Make sure metadata get copied at least once
	accum->data_type = value->data_type;
	accum->meta = value->meta;
}

inline static void reset_accum_obj(PointerSet *set, size_t len) {
	if (set == NULL) return;
	memset(set->ptr, 0, len);
	reset_accum_obj(set->next, len);
}

inline static void add_to_accum_reset_list(PointerSet **set_ptr, void *entity) {
	if (set_ptr == NULL) {
		fprintf(stderr, "WARNING: output_filter_output::add_to_accum_reset_list(): set_ptr was NULL but should not be!");
	} else if (*set_ptr == NULL) {
		*set_ptr = newPointerSet(entity);
	} else {
		pointerSetAppend(*set_ptr, entity);
	}
}

static void reset_accumulator_patch(PointerSet **acc_objs_to_reset) {
	if (*acc_objs_to_reset) {
		reset_accum_obj(*acc_objs_to_reset, sizeof(struct accumulate_patch_object));
		freePointerSet(*acc_objs_to_reset);
		*acc_objs_to_reset = NULL;
	}
}

inline static void reset_accumulator_hillslope(PointerSet **acc_objs_to_reset) {
	reset_accumulator_patch(acc_objs_to_reset);
}

static void reset_accumulator_stratum(PointerSet **acc_objs_to_reset) {
	if (*acc_objs_to_reset) {
		reset_accum_obj(*acc_objs_to_reset, sizeof(struct accumulate_strata_object));
		freePointerSet(*acc_objs_to_reset);
		*acc_objs_to_reset = NULL;
	}
}

inline static MaterializedVariable materialize_named_variable(OutputFilterVariable const * const v,
                                                              void * const entity, size_t offset) {
    MaterializedVariable mat_var;
    switch (v->data_type) {
        case DATA_TYPE_BOOL:
            mat_var.data_type = v->data_type;
            mat_var.u.bool_val = *((bool *) (entity + offset));
#if OF_DEBUG
            fprintf(stderr, "\t\t\tvar: %s, value: %h\n", v->name, mat_var.u.bool_val);
#endif
            return mat_var;
        case DATA_TYPE_CHAR:
            mat_var.data_type = v->data_type;
            mat_var.u.char_val = *((char *) (entity + offset));
#if OF_DEBUG
            fprintf(stderr, "\t\t\tvar: %s, value: %c\n", v->name, mat_var.u.char_val);
#endif
            return mat_var;
        case DATA_TYPE_STRING:
            mat_var.data_type = v->data_type;
            mat_var.u.char_array = (char *) (entity + offset);
#if OF_DEBUG
            fprintf(stderr, "\t\t\tvar: %s, value: %s\n", v->name, mat_var.u.char_array);
#endif
            return mat_var;
        case DATA_TYPE_INT:
            mat_var.data_type = v->data_type;
            mat_var.u.int_val = *((int *) (entity + offset));
#if OF_DEBUG
            fprintf(stderr, "\t\t\tvar: %s, value: %d\n", v->name, mat_var.u.int_val);
#endif
            return mat_var;
        case DATA_TYPE_LONG:
            mat_var.data_type = v->data_type;
            mat_var.u.long_val = *((long *) (entity + offset));
#if OF_DEBUG
            fprintf(stderr, "\t\t\tvar: %s, value: %l\n", v->name, mat_var.u.long_val);
#endif
            return mat_var;
        case DATA_TYPE_LONG_ARRAY:
            mat_var.data_type = v->data_type;
            mat_var.u.long_array = (long *) (entity + offset);
#if OF_DEBUG
            fprintf(stderr, "\t\t\tvar: %s, value: %p\n", v->name, mat_var.u.long_array);
#endif
            return mat_var;
        case DATA_TYPE_FLOAT:
            mat_var.data_type = v->data_type;
            mat_var.u.float_val = *((float *) (entity + offset));
#if OF_DEBUG
            fprintf(stderr, "\t\t\tvar: %s, value: %f\n", v->name, mat_var.u.float_val);
#endif
            return mat_var;
        case DATA_TYPE_DOUBLE:
            mat_var.data_type = v->data_type;
            mat_var.u.double_val = *((double *) (entity + offset));
#if OF_DEBUG
            fprintf(stderr, "\t\t\tvar: %s, value: %f\n", v->name, mat_var.u.double_val);
#endif
            return mat_var;
        case DATA_TYPE_DOUBLE_ARRAY:
            mat_var.data_type = v->data_type;
            mat_var.u.double_array = (double *) (entity + offset);
#if OF_DEBUG
            fprintf(stderr, "\t\t\tvar: %s, value: %p\n", v->name, mat_var.u.double_array);
#endif
            return mat_var;
        default:
            mat_var.data_type = DATA_TYPE_UNDEFINED;
            return mat_var;
    }
}

inline static double mat_var_scalar_to_double(MaterializedVariable a) {
    double result;
    switch (a.data_type) {
        case DATA_TYPE_BOOL:
            result = a.u.bool_val;
            return result;
        case DATA_TYPE_INT:
            result = a.u.int_val;
            return result;
        case DATA_TYPE_LONG:
            result = a.u.long_val;
            return result;
        case DATA_TYPE_FLOAT:
            result = a.u.float_val;
            return result;
        case DATA_TYPE_DOUBLE:
            result = a.u.double_val;
            return result;
        default:
            fprintf(stderr,
                    "WARNING: mat_var_scalar_to_double(): Encountered non-scalar/non-numeric value when trying to convert variable of type %d to type double. Returning NaN.\n",
                    a.data_type);
            result = NAN;
            return result;
    }
}

inline static MaterializedVariable mat_var_add(MaterializedVariable l,
                                               MaterializedVariable r) {
    double left, right;
    MaterializedVariable v;
    v.data_type = DATA_TYPE_DOUBLE;
    left = mat_var_scalar_to_double(l);
    if (left == NAN) {
        fprintf(stderr,
                "WARNING: mat_var_add(): Left operand is NaN.\n");
        v.u.double_val = NAN;
        return v;
    }
    right = mat_var_scalar_to_double(r);
    if (right == NAN) {
        fprintf(stderr,
                "WARNING: mat_var_add(): Right operand is NaN.\n");
        v.u.double_val = NAN;
        return v;
    }

    v.u.double_val = left + right;
    return v;
}

inline static MaterializedVariable mat_var_sub(MaterializedVariable l,
                                               MaterializedVariable r) {
    double left, right;
    MaterializedVariable v;
    v.data_type = DATA_TYPE_DOUBLE;
    left = mat_var_scalar_to_double(l);
    if (left == NAN) {
        fprintf(stderr,
                "WARNING: mat_var_sub(): Left operand is NaN.\n");
        v.u.double_val = NAN;
        return v;
    }
    right = mat_var_scalar_to_double(r);
    if (right == NAN) {
        fprintf(stderr,
                "WARNING: mat_var_sub(): Right operand is NaN.\n");
        v.u.double_val = NAN;
        return v;
    }

    v.u.double_val = left - right;
    return v;
}

inline static MaterializedVariable mat_var_mul(MaterializedVariable l,
                                               MaterializedVariable r) {
    double left, right;
    MaterializedVariable v;
    v.data_type = DATA_TYPE_DOUBLE;
    left = mat_var_scalar_to_double(l);
    if (left == NAN) {
        fprintf(stderr,
                "WARNING: mat_var_mul(): Left operand is NaN.\n");
        v.u.double_val = NAN;
        return v;
    }
    right = mat_var_scalar_to_double(r);
    if (right == NAN) {
        fprintf(stderr,
                "WARNING: mat_var_mul(): Right operand is NaN.\n");
        v.u.double_val = NAN;
        return v;
    }

    v.u.double_val = left * right;
    return v;
}

inline static MaterializedVariable mat_var_div(MaterializedVariable l,
                                               MaterializedVariable r) {
    double left, right;
    MaterializedVariable v;
    v.data_type = DATA_TYPE_DOUBLE;
    left = mat_var_scalar_to_double(l);
    if (left == NAN) {
        fprintf(stderr,
                "WARNING: mat_var_div(): Left operand is NaN.\n");
        v.u.double_val = NAN;
        return v;
    }
    right = mat_var_scalar_to_double(r);
    if (right == NAN) {
        fprintf(stderr,
                "WARNING: mat_var_div(): Right operand is NaN.\n");
        v.u.double_val = NAN;
        return v;
    } else if (right == 0.0) {
        // Should we return 0.0 instead?
        fprintf(stderr,
                "WARNING: mat_var_div(): Denominator is 0.0, returning NaN.\n");
        v.u.double_val = NAN;
        return v;
    }

    v.u.double_val = left / right;
    return v;
}

inline static MaterializedVariable mat_var_neg(MaterializedVariable l) {
    double operand;
    MaterializedVariable v;
    v.data_type = DATA_TYPE_DOUBLE;
    operand = mat_var_scalar_to_double(l);
    if (operand == NAN) {
        fprintf(stderr,
                "WARNING: mat_var_neg(): Negation operand is NaN.\n");
        v.u.double_val = NAN;
        return v;
    }

    v.u.double_val = -operand;
    return v;
}

static MaterializedVariable eval_expr(void * const entity,
                                      OutputFilterExprAst * const expr) {
    MaterializedVariable mat_var;
    OutputFilterVariable *v;
    switch (expr->nodetype) {
        case '+':
            mat_var = mat_var_add(eval_expr(entity, expr->l),
                                  eval_expr(entity, expr->r));
            return mat_var;
        case '-':
            mat_var = mat_var_sub(eval_expr(entity, expr->l),
                                  eval_expr(entity, expr->r));
            return mat_var;
        case '*':
            mat_var = mat_var_mul(eval_expr(entity, expr->l),
                                  eval_expr(entity, expr->r));
            return mat_var;
        case '/':
            mat_var = mat_var_div(eval_expr(entity, expr->l),
                                  eval_expr(entity, expr->r));
            return mat_var;
        case OF_VAR_EXPR_AST_NODE_UNARY_MINUS:
            mat_var = mat_var_neg(eval_expr(entity, expr->l));
            return mat_var;
            /* no subtree */
        case OF_VAR_EXPR_AST_NODE_NAME:
            v = ((OutputFilterExprName *)expr)->var;
            mat_var = materialize_named_variable(v, entity, compute_struct_member_offset(v));
            return mat_var;
        case OF_VAR_EXPR_AST_NODE_CONST:
            mat_var.data_type = DATA_TYPE_DOUBLE;
            mat_var.u.double_val = ((OutputFilterExprNumval *)expr)->number;
            return mat_var;
        default:
            fprintf(stderr,
                    "WARNING: undefined data type in expression variable.\n");
            mat_var.data_type = DATA_TYPE_UNDEFINED;
            return mat_var;
    }
}

static MaterializedVariable materialize_expr_variable(OutputFilterVariable const * const v,
                                                      void * const entity) {
    if (v->expr == NULL) {
        fprintf(stderr, "WARNING: materialize_expr_variable: expression variable '%s' has no expression defined.",
                v->name);
        return (MaterializedVariable){.data_type=DATA_TYPE_UNDEFINED};
    }
    return eval_expr(entity, v->expr);
}

inline static MaterializedVariable materialize_variable(OutputFilterVariable const * const v, void * const entity) {
	MaterializedVariable mat_var;
	if (v->variable_type == NAMED) {
        mat_var = materialize_named_variable(v, entity, compute_struct_member_offset(v));
    } else if (v->variable_type == VAR_TYPE_EXPR) {
        mat_var = materialize_expr_variable(v, entity);
	}
	// Copy pointer to any metadata for this variable
	mat_var.meta = v->meta;
	return mat_var;
}

inline static void *determine_stratum_entity(OutputFilterTimestep timestep,
		struct canopy_strata_object *stratum,
		PointerSet **accum_objs_to_reset) {
	void *entity = NULL;
	switch (timestep) {
	case TIMESTEP_MONTHLY:
		entity = (void *)(&(stratum->acc_month));
		add_to_accum_reset_list(accum_objs_to_reset, entity);
		break;
	case TIMESTEP_YEARLY:
		entity = (void *)(&(stratum->acc_year));
		add_to_accum_reset_list(accum_objs_to_reset, entity);
		break;
	case TIMESTEP_HOURLY:
	case TIMESTEP_DAILY:
	default:
		entity = (void *)stratum;
		break;
	}
	return entity;
}

inline static void *determine_patch_entity(OutputFilterTimestep timestep,
		struct patch_object *patch,
		PointerSet **acc_objs_to_reset) {
	void *entity = NULL;
	switch (timestep) {
	case TIMESTEP_MONTHLY:
		entity = (void *)(&(patch->acc_month));
		add_to_accum_reset_list(acc_objs_to_reset, entity);
		break;
	case TIMESTEP_YEARLY:
		entity = (void *)(&(patch->acc_year));
		add_to_accum_reset_list(acc_objs_to_reset, entity);
		break;
	case TIMESTEP_HOURLY:
	case TIMESTEP_DAILY:
	default:
		// Not sure this is quite right for hourly but going with it for now
		entity = (void *)patch;
		break;
	}
	return entity;
}

inline static void *determine_hillslope_entity(OutputFilterTimestep timestep,
		struct hillslope_object *hillslope,
		PointerSet **acc_objs_to_reset) {
	void *entity = NULL;
	switch (timestep) {
	case TIMESTEP_MONTHLY:
		entity = (void *)(&(hillslope->acc_month));
		add_to_accum_reset_list(acc_objs_to_reset, entity);
		break;
	case TIMESTEP_YEARLY:
		entity = (void *)(&(hillslope->acc_year));
		add_to_accum_reset_list(acc_objs_to_reset, entity);
		break;
	case TIMESTEP_HOURLY:
	case TIMESTEP_DAILY:
	default:
		// Not sure this is quite right for hourly but going with it for now
		entity = (void *)hillslope;
		break;
	}
	return entity;
}

static bool apply_to_strata_in_patch(char * const error, size_t error_len, bool verbose,
		struct date date,
		OutputFilter const * const filter, OutputFilterStratum const * const s, EntityID id,
		PointerSet **acc_objs_to_reset,
		bool (*output_fn)(char * const, size_t, bool, struct date date, void * const, EntityID, OutputFilter const * const)) {
	for (size_t i = 0; i < s->patch->num_canopy_strata; i++) {
		struct canopy_strata_object *stratum = s->patch->canopy_strata[i];
		id.canopy_strata_ID = stratum->ID;
		void *entity = determine_stratum_entity(filter->timestep, stratum, acc_objs_to_reset);
		bool status = (*output_fn)(error, error_len, verbose, date, entity, id, filter);
		if (!status) return false;
	}
	return true;
}

static bool apply_to_patches_in_zone(char * const error, size_t error_len, bool verbose,
		struct date date,
		OutputFilter const * const filter, OutputFilterPatch const * const p, EntityID id,
		PointerSet **acc_objs_to_reset,
		bool (*output_fn)(char * const, size_t, bool, struct date date, void * const, EntityID, OutputFilter const * const)) {
	for (size_t i = 0; i < p->zone->num_patches; i++) {
		struct patch_object *patch = p->zone->patches[i];
		id.patch_ID = patch->ID;
		void *entity = determine_patch_entity(filter->timestep, patch, acc_objs_to_reset);
		bool status = (*output_fn)(error, error_len, verbose, date, entity, id, filter);
		if (!status) return false;
	}
	return true;
}

static bool apply_to_strata_in_zone(char * const error, size_t error_len, bool verbose,
		struct date date,
		OutputFilter const * const filter, OutputFilterStratum const * const s, EntityID id,
		PointerSet **acc_objs_to_reset,
		bool (*output_fn)(char * const, size_t, bool, struct date date, void * const, EntityID, OutputFilter const * const)) {
	for (size_t i = 0; i < s->zone->num_patches; i++) {
		struct patch_object *patch = s->zone->patches[i];
		id.patch_ID = patch->ID;
		for (size_t j = 0; j < patch->num_canopy_strata; j++) {
			struct canopy_strata_object *stratum = patch->canopy_strata[j];
			id.canopy_strata_ID = stratum->ID;
			void *entity = determine_stratum_entity(filter->timestep, stratum, acc_objs_to_reset);
			bool status = (*output_fn)(error, error_len, verbose, date, entity, id, filter);
			if (!status) return false;
		}
	}
	return true;
}

static bool apply_to_patches_in_hillslope(char * const error, size_t error_len, bool verbose,
		struct date date,
		OutputFilter const * const filter, OutputFilterPatch const * const p, EntityID id,
		PointerSet **acc_objs_to_reset,
		bool (*output_fn)(char * const, size_t, bool, struct date date, void * const, EntityID, OutputFilter const * const)) {
	for (size_t i = 0; i < p->hill->num_zones; i++) {
		struct zone_object *z = p->hill->zones[i];
		id.zone_ID = z->ID;
		for (size_t j = 0; j < z->num_patches; j++) {
			struct patch_object *patch = z->patches[j];
			id.patch_ID = patch->ID;
			void *entity = determine_patch_entity(filter->timestep, patch, acc_objs_to_reset);
			bool status = (*output_fn)(error, error_len, verbose, date, entity, id, filter);
			if (!status) return false;
		}
	}
	return true;
}

static bool apply_to_strata_in_hillslope(char * const error, size_t error_len, bool verbose,
		struct date date,
		OutputFilter const * const filter, OutputFilterStratum const * const s, EntityID id,
		PointerSet **acc_objs_to_reset,
		bool (*output_fn)(char * const, size_t, bool, struct date date, void * const, EntityID, OutputFilter const * const)) {
	for (size_t i = 0; i < s->hill->num_zones; i++) {
		struct zone_object *z = s->hill->zones[i];
		id.zone_ID = z->ID;
		for (size_t j = 0; j < z->num_patches; j++) {
			struct patch_object *patch = z->patches[j];
			id.patch_ID = patch->ID;
			for (size_t k = 0; k < patch->num_canopy_strata; k++) {
				struct canopy_strata_object *stratum = patch->canopy_strata[k];
				id.canopy_strata_ID = stratum->ID;
				void *entity = determine_stratum_entity(filter->timestep, stratum, acc_objs_to_reset);
				bool status = (*output_fn)(error, error_len, verbose, date, entity, id, filter);
				if (!status) return false;
			}
		}
	}
	return true;
}

static bool apply_to_patches_in_basin(char * const error, size_t error_len, bool verbose,
		struct date date,
		OutputFilter const * const filter, OutputFilterPatch const * const p, EntityID id,
		PointerSet **acc_objs_to_reset,
		bool (*output_fn)(char * const, size_t, bool, struct date date, void * const, EntityID, OutputFilter const * const)) {
	for (size_t i = 0; i < p->basin->num_hillslopes; i++) {
		struct hillslope_object *h = p->basin->hillslopes[i];
		id.hillslope_ID = h->ID;
		for (size_t j = 0; j < h->num_zones; j++) {
			struct zone_object *z = h->zones[j];
			id.zone_ID = z->ID;
			for (size_t k = 0; k < z->num_patches; k++) {
				struct patch_object *patch = z->patches[k];
				id.patch_ID = patch->ID;
				void *entity = determine_patch_entity(filter->timestep, patch, acc_objs_to_reset);
				bool status = (*output_fn)(error, error_len, verbose, date, entity, id, filter);
				if (!status) return false;
			}
		}
	}
	return true;
}

static bool apply_to_strata_in_basin(char * const error, size_t error_len, bool verbose,
		struct date date,
		OutputFilter const * const filter, OutputFilterStratum const * const s, EntityID id,
		PointerSet **acc_objs_to_reset,
		bool (*output_fn)(char * const, size_t, bool, struct date date, void * const, EntityID, OutputFilter const * const)) {
	for (size_t i = 0; i < s->basin->num_hillslopes; i++) {
		struct hillslope_object *h = s->basin->hillslopes[i];
		id.hillslope_ID = h->ID;
		for (size_t j = 0; j < h->num_zones; j++) {
			struct zone_object *z = h->zones[j];
			id.zone_ID = z->ID;
			for (size_t k = 0; k < z->num_patches; k++) {
				struct patch_object *patch = z->patches[k];
				id.patch_ID = patch->ID;
				for (size_t l = 0; l < patch->num_canopy_strata; l++) {
					struct canopy_strata_object *stratum = patch->canopy_strata[l];
					id.canopy_strata_ID = stratum->ID;
					void *entity = determine_stratum_entity(filter->timestep, stratum, acc_objs_to_reset);
					bool status = (*output_fn)(error, error_len, verbose, date, entity, id, filter);
					if (!status) return false;
				}
			}
		}
	}
	return true;
}

static inline bool output_materialized_variables(char * const error, size_t error_len,
		struct date date, EntityID id, OutputFilter const * const f, MaterializedVariable *mat_vars) {
	// Output materialized variables array using appropriate driver
	switch (f->output->format) {
	case OUTPUT_TYPE_CSV:
		return output_format_csv_write_data(error, error_len,
				date, f, id, mat_vars, true);
	case OUTPUT_TYPE_NETCDF:
		return output_format_netcdf_write_data(error, error_len,
				date, f, id, mat_vars, true);
	default:
		fprintf(stderr, "output_materialized_variables: output format type %d is unknown or not yet implemented.",
				f->output->format);
		return false;
	}
}

static bool output_variables(char * const error, size_t error_len, bool verbose,
		struct date date, void * const entity, EntityID id, OutputFilter const * const f) {
	if (verbose) fprintf(stderr, "\t\toutput_variables(num_variables: %hu)...\n", f->num_variables);

	char *local_error;
	MaterializedVariable mat_var;
	MaterializedVariable *mat_vars = f->output->materialized_variables;
	num_elements_t curr_var = 0;

	for (OutputFilterVariable *v = f->variables; v != NULL; v = v->next) {
		switch (v->variable_type) {
		case NAMED:
		case VAR_TYPE_EXPR:
			// Materialize variable and add it to array
			mat_var = materialize_variable(v, entity);
			if (mat_var.data_type == DATA_TYPE_UNDEFINED) {
				local_error = (char *)calloc(MAXSTR, sizeof(char));
				snprintf(local_error, MAXSTR, "output_variables:  data type %d of variable %s is unknown or not yet implemented.",
						 v->data_type, v->name);
				return_with_error(error, error_len, local_error);
			}
			mat_vars[curr_var++] = mat_var;
			break;
		case ANY_VAR:
		default:
			local_error = (char *)calloc(MAXSTR, sizeof(char));
			snprintf(local_error, MAXSTR, "output_variables: variable type %d is unknown or not yet implemented.",
					v->variable_type);
			return_with_error(error, error_len, local_error);
		}
	}

	return output_materialized_variables(error, error_len, date, id, f, mat_vars);
}

static bool output_basin(char * const error, size_t error_len, bool verbose,
		struct date date, OutputFilter const * const f,
		PointerSet **hillslope_acc_objs_to_reset, PointerSet **patch_acc_objs_to_reset, PointerSet **stratum_acc_objs_to_reset) {
	if (verbose) fprintf(stderr, "\toutput_basin()...\n");

	bool status;
	EntityID id = {OUTPUT_FILTER_ID_EMPTY, OUTPUT_FILTER_ID_EMPTY, OUTPUT_FILTER_ID_EMPTY, OUTPUT_FILTER_ID_EMPTY, OUTPUT_FILTER_ID_EMPTY};
	MaterializedVariable mat_var;
	MaterializedVariable *mat_vars = f->output->materialized_variables;

	double basin_area = 0.0;
	double hillslope_area = 0.0;

	// Iterate over all basins
	// Within each basin, iterate over all patch and stratum objects, performing areal averaging as we go
	// (using the OutputFilterOutput->materialized_variables as the accumulator scratch space)
	// Then, once all specified variables have been averaged, output them (they will have to be stored as materialized
	// variables since this is what the output drivers know how to write data).
	// TODO: To support expressions for basin level variables, what we need is a table mapping the name of
	//       variables to be accumulated to the accumulated value. This table will be composed of entries from individual
	//       named variables, and named variables that appear in expressions. Then, accumulation will happen for all
	//       variables in the table. Once accumulation has been done, then the variables will be processed in order
	//       from the filter's variable list. The accumulated value of simple named variables will be added to the
	//       materialized variable array, and expression variables will be evaluated and have their values also written
	//       to the materialized variable array. This will require a separate, or at least functionally abstracted
	//       version of the expression variable evaluation function so that instead of getting variable values from
	//       calling materialize_variable, we will instead get the value from the accumulation table.
	for (OutputFilterBasin *b = f->basins; b != NULL; b = b->next) {
		reset_materialized_variable_array_values(f);
		id.basin_ID = b->basinID;
		struct basin_object *basin = b->basin;
		for (size_t i = 0; i < basin->num_hillslopes; i++) {
			struct hillslope_object *hillslope = basin->hillslopes[i];
			hillslope_area = 0.0;

			for (size_t j = 0; j < hillslope->num_zones; j++) {
				struct zone_object *z = hillslope->zones[j];
				for (size_t k = 0; k < z->num_patches; k++) {
					struct patch_object *patch = z->patches[k];
					// Iterate over filter variables accumulating any patch variables
					int var_num = 0;
					for (OutputFilterVariable *v = f->variables; v != NULL; v = v->next) {
						if (v->variable_type == NAMED) {
							if (v->hierarchy_level == OF_HIERARCHY_LEVEL_PATCH) {
								void *entity = determine_patch_entity(f->timestep, patch, patch_acc_objs_to_reset);
								mat_var = materialize_variable(v, entity);
								accum_materialized_variable(&mat_vars[var_num], &mat_var, patch->area);
							}
							var_num += 1;
						}
					}
					for (size_t l = 0; l < patch->num_canopy_strata; l++) {
						struct canopy_strata_object *stratum = patch->canopy_strata[l];
						// Iterate over filter variables accumulating any stratum variables
						int var_num = 0;
						for (OutputFilterVariable *v = f->variables; v != NULL; v = v->next) {
							if (v->variable_type == NAMED) {
								if (v->hierarchy_level == OF_HIERARCHY_LEVEL_STRATUM) {
									void *entity = determine_stratum_entity(f->timestep, stratum, stratum_acc_objs_to_reset);
									mat_var = materialize_variable(v, entity);
									accum_materialized_variable(&mat_vars[var_num], &mat_var, patch->area);
								}
								var_num += 1;
							}
						}
					}
					basin_area += patch->area;
					hillslope_area += patch->area;
				}
			}

			// Iterate over filter variables accumulating any hillslope variables
			int var_num = 0;
			for (OutputFilterVariable *v = f->variables; v != NULL; v = v->next) {
				if (v->variable_type == NAMED) {
					if (v->hierarchy_level == OF_HIERARCHY_LEVEL_HILLSLOPE) {
						void *entity = determine_hillslope_entity(f->timestep, hillslope, hillslope_acc_objs_to_reset);
						mat_var = materialize_variable(v, entity);
						accum_materialized_variable(&mat_vars[var_num], &mat_var, hillslope_area);
					}
					var_num += 1;
				}
			}
		}
		// Scale values by basin area
		scale_materialized_variable_array_values(f, basin_area);
		status = output_materialized_variables(error, error_len, date, id, f, mat_vars);
		if (status == false) {
			char *local_error = (char *)calloc(MAXSTR, sizeof(char));
			snprintf(local_error, MAXSTR, "output_filter_output::output_basin: failed to output materialized variables.");
			return_with_error(error, error_len, local_error);
		}
	}
	return true;
}

static bool output_patch(char * const error, size_t error_len, bool verbose,
		struct date date, OutputFilter const * const filter,
		PointerSet **acc_objs_to_reset) {
	if (verbose) fprintf(stderr, "\toutput_patch()...\n");

	char *local_error;
	bool status;

	for (OutputFilterPatch *p = filter->patches; p != NULL; p = p->next) {
		status = true;
		EntityID id;
		switch (p->output_patch_type) {
		case PATCH_TYPE_PATCH:
			id.basin_ID = p->basinID;
			id.hillslope_ID = p->hillslopeID;
			id.zone_ID = p->zoneID;
			id.patch_ID = p->patchID;
			id.canopy_strata_ID = OUTPUT_FILTER_ID_EMPTY;
			void *entity = determine_patch_entity(filter->timestep, p->patch, acc_objs_to_reset);
			status = output_variables(error, error_len, verbose, date, entity, id, filter);
			break;
		case PATCH_TYPE_ZONE:
			id.basin_ID = p->basinID;
			id.hillslope_ID = p->hillslopeID;
			id.zone_ID = p->zoneID;
			id.patch_ID = OUTPUT_FILTER_ID_EMPTY;
			id.canopy_strata_ID = OUTPUT_FILTER_ID_EMPTY;
			status = apply_to_patches_in_zone(error, error_len, verbose, date, filter, p, id, acc_objs_to_reset,
					*output_variables);
			break;
		case PATCH_TYPE_HILLSLOPE:
			id.basin_ID = p->basinID;
			id.hillslope_ID = p->hillslopeID;
			id.zone_ID = OUTPUT_FILTER_ID_EMPTY;
			id.patch_ID = OUTPUT_FILTER_ID_EMPTY;
			id.canopy_strata_ID = OUTPUT_FILTER_ID_EMPTY;
			status = apply_to_patches_in_hillslope(error, error_len, verbose, date, filter, p, id, acc_objs_to_reset,
					*output_variables);
			break;
		case PATCH_TYPE_BASIN:
			id.basin_ID = p->basinID;
			id.hillslope_ID = OUTPUT_FILTER_ID_EMPTY;
			id.zone_ID = OUTPUT_FILTER_ID_EMPTY;
			id.patch_ID = OUTPUT_FILTER_ID_EMPTY;
			id.canopy_strata_ID = OUTPUT_FILTER_ID_EMPTY;
			status = apply_to_patches_in_basin(error, error_len, verbose, date, filter, p, id, acc_objs_to_reset,
					*output_variables);
			break;
		default:
			local_error = (char *)calloc(MAXSTR, sizeof(char));
			snprintf(local_error, MAXSTR, "output_patch_daily: patch type %d is unknown or not yet implemented.",
					p->output_patch_type);
			return_with_error(error, error_len, local_error);
		}
		if (!status) return false;
	}

	return true;
}

static bool output_stratum(char * const error, size_t error_len, bool verbose,
		struct date date, OutputFilter const * const filter,
		PointerSet **acc_objs_to_reset) {
	if (verbose) fprintf(stderr, "\toutput_stratum()...\n");

	char *local_error;
	bool status;

	for (OutputFilterStratum *s = filter->strata; s != NULL; s = s->next) {
		status = true;
		EntityID id;
		switch (s->output_stratum_type) {
		case STRATUM_TYPE_STRATUM:
			id.basin_ID = s->basinID;
			id.hillslope_ID = s->hillslopeID;
			id.zone_ID = s->zoneID;
			id.patch_ID = s->patchID;
			id.canopy_strata_ID = s->stratumID;
			void *entity = determine_stratum_entity(filter->timestep, s->stratum, acc_objs_to_reset);
			status = output_variables(error, error_len, verbose, date, entity, id, filter);
			break;
		case STRATUM_TYPE_PATCH:
			id.basin_ID = s->basinID;
			id.hillslope_ID = s->hillslopeID;
			id.zone_ID = s->zoneID;
			id.patch_ID = s->patchID;
			id.canopy_strata_ID = OUTPUT_FILTER_ID_EMPTY;
			status = apply_to_strata_in_patch(error, error_len, verbose, date, filter, s, id, acc_objs_to_reset,
					*output_variables);
			break;
		case STRATUM_TYPE_ZONE:
			id.basin_ID = s->basinID;
			id.hillslope_ID = s->hillslopeID;
			id.zone_ID = s->zoneID;
			id.patch_ID = OUTPUT_FILTER_ID_EMPTY;
			id.canopy_strata_ID = OUTPUT_FILTER_ID_EMPTY;
			status = apply_to_strata_in_zone(error, error_len, verbose, date, filter, s, id, acc_objs_to_reset,
					*output_variables);
			break;
		case STRATUM_TYPE_HILLSLOPE:
			id.basin_ID = s->basinID;
			id.hillslope_ID = s->hillslopeID;
			id.zone_ID = OUTPUT_FILTER_ID_EMPTY;
			id.patch_ID = OUTPUT_FILTER_ID_EMPTY;
			id.canopy_strata_ID = OUTPUT_FILTER_ID_EMPTY;
			status = apply_to_strata_in_hillslope(error, error_len, verbose, date, filter, s, id, acc_objs_to_reset,
					*output_variables);
			break;
		case STRATUM_TYPE_BASIN:
			id.basin_ID = s->basinID;
			id.hillslope_ID = OUTPUT_FILTER_ID_EMPTY;
			id.zone_ID = OUTPUT_FILTER_ID_EMPTY;
			id.patch_ID = OUTPUT_FILTER_ID_EMPTY;
			id.canopy_strata_ID = OUTPUT_FILTER_ID_EMPTY;
			status = apply_to_strata_in_basin(error, error_len, verbose, date, filter, s, id, acc_objs_to_reset,
					*output_variables);
			break;
		default:
			local_error = (char *)calloc(MAXSTR, sizeof(char));
			snprintf(local_error, MAXSTR, "output_stratum: stratum type %d is unknown or not yet implemented.",
					s->output_stratum_type);
			return_with_error(error, error_len, local_error);
		}
		if (!status) return false;
	}

	return true;
}

bool output_filter_output_daily(char * const error, size_t error_len, bool verbose,
		struct date date, OutputFilter const * const filters) {
	if (verbose) fprintf(stderr, "output_filter_output_daily(): BEGIN\n");

	char *local_error;
	bool status = true;

	for (OutputFilter const * f = filters; f != NULL; f = f->next) {
		if (f->timestep == TIMESTEP_DAILY) {
			switch (f->type) {
			case OUTPUT_FILTER_BASIN:
				status = output_basin(error, error_len, verbose, date, f, NULL, NULL, NULL);
				if (!status) return false;
				break;
			case OUTPUT_FILTER_PATCH:
				status = output_patch(error, error_len, verbose, date, f, NULL);
				if (!status) return false;
				break;
			case OUTPUT_FILTER_CANOPY_STRATUM:
				status = output_stratum(error, error_len, verbose, date, f, NULL);
				if (!status) return false;
				break;
			default:
				local_error = (char *)calloc(MAXSTR, sizeof(char));
				snprintf(local_error, MAXSTR, "output_filter_output_daily: output filter type %d is unknown or not yet implemented.", f->type);
				return_with_error(error, error_len, local_error);
			}
		}
	}

	if (verbose) fprintf(stderr, "output_filter_output_daily(): END\n");

	return status;
}

bool output_filter_output_monthly(char * const error, size_t error_len, bool verbose,
		struct date date, OutputFilter const * const filters) {
	if (verbose) fprintf(stderr, "output_filter_output_monthly(): BEGIN\n");

	char *local_error;
	bool status = true;

	PointerSet *acc_hillslope_obj_to_reset = NULL;
	PointerSet *acc_patch_obj_to_reset = NULL;
	PointerSet *acc_stratum_obj_to_reset = NULL;

	for (OutputFilter const * f = filters; f != NULL; f = f->next) {
		if (f->timestep == TIMESTEP_MONTHLY) {
			switch (f->type) {
			case OUTPUT_FILTER_BASIN:
				status = output_basin(error, error_len, verbose, date, f,
						&acc_hillslope_obj_to_reset, &acc_patch_obj_to_reset, &acc_stratum_obj_to_reset);
				if (!status) return false;
				break;
			case OUTPUT_FILTER_PATCH:
				status = output_patch(error, error_len, verbose, date, f, &acc_patch_obj_to_reset);
				if (!status) return false;
				break;
			case OUTPUT_FILTER_CANOPY_STRATUM:
				status = output_stratum(error, error_len, verbose, date, f, &acc_stratum_obj_to_reset);
				if (!status) return false;
				break;
			default:
				local_error = (char *)calloc(MAXSTR, sizeof(char));
				snprintf(local_error, MAXSTR, "output_filter_output_monthly: output filter type %d is unknown or not yet implemented.", f->type);
				return_with_error(error, error_len, local_error);
			}
		}
	}

	// Reset monthly accumulators as necessary
	reset_accumulator_hillslope(&acc_hillslope_obj_to_reset);
	reset_accumulator_patch(&acc_patch_obj_to_reset);
	reset_accumulator_stratum(&acc_stratum_obj_to_reset);

	if (verbose) fprintf(stderr, "output_filter_output_monthly(): END\n");

	return status;
}

bool output_filter_output_yearly(char * const error, size_t error_len, bool verbose,
		struct date date, OutputFilter const * const filters) {
	if (verbose) fprintf(stderr, "output_filter_output_yearly(): BEGIN\n");

	char *local_error;
	bool status = true;

	PointerSet *acc_hillslope_obj_to_reset = NULL;
	PointerSet *acc_patch_obj_to_reset = NULL;
	PointerSet *acc_stratum_obj_to_reset = NULL;

	for (OutputFilter const * f = filters; f != NULL; f = f->next) {
		if (f->timestep == TIMESTEP_YEARLY) {
			switch (f->type) {
			case OUTPUT_FILTER_BASIN:
				status = output_basin(error, error_len, verbose, date, f,
						&acc_hillslope_obj_to_reset, &acc_patch_obj_to_reset, &acc_stratum_obj_to_reset);
				if (!status) return false;
				break;
			case OUTPUT_FILTER_PATCH:
				status = output_patch(error, error_len, verbose, date, f, &acc_patch_obj_to_reset);
				if (!status) return false;
				break;
			case OUTPUT_FILTER_CANOPY_STRATUM:
				status = output_stratum(error, error_len, verbose, date, f, &acc_stratum_obj_to_reset);
				if (!status) return false;
				break;
			default:
				local_error = (char *)calloc(MAXSTR, sizeof(char));
				snprintf(local_error, MAXSTR, "output_filter_output_yearly: output filter type %d is unknown or not yet implemented.", f->type);
				return_with_error(error, error_len, local_error);
			}
		}
	}

	// Reset yearly accumulators as necessary
	reset_accumulator_hillslope(&acc_hillslope_obj_to_reset);
	reset_accumulator_patch(&acc_patch_obj_to_reset);
	reset_accumulator_stratum(&acc_stratum_obj_to_reset);

	if (verbose) fprintf(stderr, "output_filter_output_yearly(): END\n");

	return status;
}
