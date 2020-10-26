#include <stdlib.h>
#include <stdio.h>

#include "rhessys.h"
#include "output_filter/output_filter_output.h"
#include "pointer_set.h"


inline static void reset_materialized_variable_array(OutputFilter *f) {
	if (f == NULL) return;
	memset(f->output->materialized_variables, 0, f->num_named_variables * sizeof(MaterializedVariable));
}

inline static void accum_materialized_variable(MaterializedVariable *accum, MaterializedVariable *value,
		double dbl_scalar) {
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
		accum->u.double_val += value->u.double_val * dbl_scalar;
		break;
	default:
		fprintf(stderr, "WARNING: output_filter_output::accum_materialized_variable(): Unable to accumulate materialized variable of type %d",
				value->data_type);
		break;
	}
	accum->data_type = value->data_type;
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

static void reset_accumulator_stratum(PointerSet **acc_objs_to_reset) {
	if (*acc_objs_to_reset) {
		reset_accum_obj(*acc_objs_to_reset, sizeof(struct accumulate_strata_object));
		freePointerSet(*acc_objs_to_reset);
		*acc_objs_to_reset = NULL;
	}
}

// TODO: Maybe: Update to support hierarchy level to support basin level output? Though this may not be necessary as
// output_basin should be able to infer the correct entity before materialize_variable() is called.
inline static MaterializedVariable materialize_variable(OutputFilterVariable const * const v, void * const entity) {
	MaterializedVariable mat_var;
	size_t offset = v->offset;
	if (v->sub_struct_var_offset != SIZE_MAX) {
		// If sub_struct_var_offset has been set for this variable
		// (which means this must be a sub-struct variable) apply it to the offset
		offset += v->sub_struct_var_offset;
	}
	switch (v->data_type) {
	case DATA_TYPE_BOOL:
		mat_var.data_type = v->data_type;
		mat_var.u.bool_val = *((bool *)(entity + offset));
		fprintf(stderr, "\t\t\tvar: %s, value: %h\n", v->name, mat_var.u.bool_val);
		break;
	case DATA_TYPE_CHAR:
		mat_var.data_type = v->data_type;
		mat_var.u.char_val = *((char *)(entity + offset));
		fprintf(stderr, "\t\t\tvar: %s, value: %c\n", v->name, mat_var.u.char_val);
		break;
	case DATA_TYPE_STRING:
		mat_var.data_type = v->data_type;
		mat_var.u.char_array = (char *)(entity + offset);
		fprintf(stderr, "\t\t\tvar: %s, value: %s\n", v->name, mat_var.u.char_array);
		break;
	case DATA_TYPE_INT:
		mat_var.data_type = v->data_type;
		mat_var.u.int_val = *((int *)(entity + offset));
		fprintf(stderr, "\t\t\tvar: %s, value: %d\n", v->name, mat_var.u.int_val);
		break;
	case DATA_TYPE_LONG:
		mat_var.data_type = v->data_type;
		mat_var.u.long_val = *((long *)(entity + offset));
		fprintf(stderr, "\t\t\tvar: %s, value: %l\n", v->name, mat_var.u.long_val);
		break;
	case DATA_TYPE_LONG_ARRAY:
		mat_var.data_type = v->data_type;
		mat_var.u.long_array = (long *)(entity + offset);
		fprintf(stderr, "\t\t\tvar: %s, value: %p\n", v->name, mat_var.u.long_array);
		break;
	case DATA_TYPE_FLOAT:
		mat_var.data_type = v->data_type;
		mat_var.u.float_val = *((float *)(entity + offset));
		fprintf(stderr, "\t\t\tvar: %s, value: %f\n", v->name, mat_var.u.float_val);
		break;
	case DATA_TYPE_DOUBLE:
		mat_var.data_type = v->data_type;
		mat_var.u.double_val = *((double *)(entity + offset));
		fprintf(stderr, "\t\t\tvar: %s, value: %f\n", v->name, mat_var.u.double_val);
		break;
	case DATA_TYPE_DOUBLE_ARRAY:
		mat_var.data_type = v->data_type;
		mat_var.u.double_array = (double *)(entity + offset);
		fprintf(stderr, "\t\t\tvar: %s, value: %p\n", v->name, mat_var.u.double_array);
		break;
	default:
		mat_var.data_type = DATA_TYPE_UNDEFINED;
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

static bool apply_to_strata_in_patch(char * const error, size_t error_len,
		struct date date,
		OutputFilter const * const filter, OutputFilterStratum const * const s, EntityID id,
		PointerSet **acc_objs_to_reset,
		bool (*output_fn)(char * const, size_t, struct date date, void * const, EntityID, OutputFilter const * const)) {
	for (size_t i = 0; i < s->patch->num_canopy_strata; i++) {
		struct canopy_strata_object *stratum = s->patch->canopy_strata[i];
		id.canopy_strata_ID = stratum->ID;
		void *entity = determine_stratum_entity(filter->timestep, stratum, acc_objs_to_reset);
		bool status = (*output_fn)(error, error_len, date, entity, id, filter);
		if (!status) return false;
	}
	return true;
}

static bool apply_to_patches_in_zone(char * const error, size_t error_len,
		struct date date,
		OutputFilter const * const filter, OutputFilterPatch const * const p, EntityID id,
		PointerSet **acc_objs_to_reset,
		bool (*output_fn)(char * const, size_t, struct date date, void * const, EntityID, OutputFilter const * const)) {
	for (size_t i = 0; i < p->zone->num_patches; i++) {
		struct patch_object *patch = p->zone->patches[i];
		id.patch_ID = patch->ID;
		void *entity = determine_patch_entity(filter->timestep, patch, acc_objs_to_reset);
		bool status = (*output_fn)(error, error_len, date, entity, id, filter);
		if (!status) return false;
	}
	return true;
}

static bool apply_to_strata_in_zone(char * const error, size_t error_len,
		struct date date,
		OutputFilter const * const filter, OutputFilterStratum const * const s, EntityID id,
		PointerSet **acc_objs_to_reset,
		bool (*output_fn)(char * const, size_t, struct date date, void * const, EntityID, OutputFilter const * const)) {
	for (size_t i = 0; i < s->zone->num_patches; i++) {
		struct patch_object *patch = s->zone->patches[i];
		id.patch_ID = patch->ID;
		for (size_t j = 0; j < patch->num_canopy_strata; j++) {
			struct canopy_strata_object *stratum = patch->canopy_strata[j];
			id.canopy_strata_ID = stratum->ID;
			void *entity = determine_stratum_entity(filter->timestep, stratum, acc_objs_to_reset);
			bool status = (*output_fn)(error, error_len, date, entity, id, filter);
			if (!status) return false;
		}
	}
	return true;
}

static bool apply_to_patches_in_hillslope(char * const error, size_t error_len,
		struct date date,
		OutputFilter const * const filter, OutputFilterPatch const * const p, EntityID id,
		PointerSet **acc_objs_to_reset,
		bool (*output_fn)(char * const, size_t, struct date date, void * const, EntityID, OutputFilter const * const)) {
	for (size_t i = 0; i < p->hill->num_zones; i++) {
		struct zone_object *z = p->hill->zones[i];
		id.zone_ID = z->ID;
		for (size_t j = 0; j < z->num_patches; j++) {
			struct patch_object *patch = z->patches[j];
			id.patch_ID = patch->ID;
			void *entity = determine_patch_entity(filter->timestep, patch, acc_objs_to_reset);
			bool status = (*output_fn)(error, error_len, date, entity, id, filter);
			if (!status) return false;
		}
	}
	return true;
}

static bool apply_to_strata_in_hillslope(char * const error, size_t error_len,
		struct date date,
		OutputFilter const * const filter, OutputFilterStratum const * const s, EntityID id,
		PointerSet **acc_objs_to_reset,
		bool (*output_fn)(char * const, size_t, struct date date, void * const, EntityID, OutputFilter const * const)) {
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
				bool status = (*output_fn)(error, error_len, date, entity, id, filter);
				if (!status) return false;
			}
		}
	}
	return true;
}

static bool apply_to_patches_in_basin(char * const error, size_t error_len,
		struct date date,
		OutputFilter const * const filter, OutputFilterPatch const * const p, EntityID id,
		PointerSet **acc_objs_to_reset,
		bool (*output_fn)(char * const, size_t, struct date date, void * const, EntityID, OutputFilter const * const)) {
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
				bool status = (*output_fn)(error, error_len, date, entity, id, filter);
				if (!status) return false;
			}
		}
	}
	return true;
}

static bool apply_to_strata_in_basin(char * const error, size_t error_len,
		struct date date,
		OutputFilter const * const filter, OutputFilterStratum const * const s, EntityID id,
		PointerSet **acc_objs_to_reset,
		bool (*output_fn)(char * const, size_t, struct date date, void * const, EntityID, OutputFilter const * const)) {
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
					bool status = (*output_fn)(error, error_len, date, entity, id, filter);
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

static bool output_variables(char * const error, size_t error_len,
		struct date date, void * const entity, EntityID id, OutputFilter const * const f) {
	fprintf(stderr, "\t\toutput_variables(num_named_variables: %hu)...\n", f->num_named_variables);

	char *local_error;
	MaterializedVariable mat_var;
	MaterializedVariable *mat_vars = f->output->materialized_variables;
	num_elements_t curr_var = 0;

	for (OutputFilterVariable *v = f->variables; v != NULL; v = v->next) {
		switch (v->variable_type) {
		case NAMED:
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

static bool output_basin(char * const error, size_t error_len,
		struct date date, OutputFilter const * const f,
		PointerSet **patch_acc_objs_to_reset, PointerSet **stratum_acc_objs_to_reset) {
	fprintf(stderr, "\toutput_basin()...\n");

	bool status;
	EntityID id = {OUTPUT_FILTER_ID_EMPTY, OUTPUT_FILTER_ID_EMPTY, OUTPUT_FILTER_ID_EMPTY, OUTPUT_FILTER_ID_EMPTY, OUTPUT_FILTER_ID_EMPTY};
	MaterializedVariable mat_var;
	MaterializedVariable *mat_vars = f->output->materialized_variables;

	// TODO: Implement areal averaging over all patch and stratum variables in a basin
	// Iterate over all basins
	// Within each basin, iterate over all patch and stratum objects, performing areal averaging as we go
	// (using the OutputFilterOutput->materialized_variables as the accumulator scratch space)
	// Then, once all specified variables have been averaged, output them (they will have to be stored as materialized
	// variables since this is what the output drivers know how to write data).
	for (OutputFilterBasin *b = f->basins; b != NULL; b = b->next) {
		reset_materialized_variable_array(f);
		id.basin_ID = b->basinID;
		struct basin_object *basin = b->basin;
		for (size_t i = 0; i < basin->num_hillslopes; i++) {
			struct hillslope_object *h = basin->hillslopes[i];
			// TODO: Acummulate groundwater and baseflow (which are only done at the hillslope level)
			for (size_t j = 0; j < h->num_zones; j++) {
				struct zone_object *z = h->zones[j];
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
									// TODO: Figure out whether stratum variables should be scaled by patch area
									accum_materialized_variable(&mat_vars[var_num], &mat_var, 1.0);
								}
								var_num += 1;
							}
						}
					}
				}
			}
		}
		status = output_materialized_variables(error, error_len, date, id, f, mat_vars);
		if (status == false) {
			char *local_error = (char *)calloc(MAXSTR, sizeof(char));
			snprintf(local_error, MAXSTR, "output_filter_output::output_basin: failed to output materialized variables.");
			return_with_error(error, error_len, local_error);
		}
	}
	return true;
}

static bool output_patch(char * const error, size_t error_len,
		struct date date, OutputFilter const * const filter,
		PointerSet **acc_objs_to_reset) {
	fprintf(stderr, "\toutput_patch()...\n");

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
			status = output_variables(error, error_len, date, entity, id, filter);
			break;
		case PATCH_TYPE_ZONE:
			id.basin_ID = p->basinID;
			id.hillslope_ID = p->hillslopeID;
			id.zone_ID = p->zoneID;
			id.patch_ID = OUTPUT_FILTER_ID_EMPTY;
			id.canopy_strata_ID = OUTPUT_FILTER_ID_EMPTY;
			status = apply_to_patches_in_zone(error, error_len, date, filter, p, id, acc_objs_to_reset,
					*output_variables);
			break;
		case PATCH_TYPE_HILLSLOPE:
			id.basin_ID = p->basinID;
			id.hillslope_ID = p->hillslopeID;
			id.zone_ID = OUTPUT_FILTER_ID_EMPTY;
			id.patch_ID = OUTPUT_FILTER_ID_EMPTY;
			id.canopy_strata_ID = OUTPUT_FILTER_ID_EMPTY;
			status = apply_to_patches_in_hillslope(error, error_len, date, filter, p, id, acc_objs_to_reset,
					*output_variables);
			break;
		case PATCH_TYPE_BASIN:
			id.basin_ID = p->basinID;
			id.hillslope_ID = OUTPUT_FILTER_ID_EMPTY;
			id.zone_ID = OUTPUT_FILTER_ID_EMPTY;
			id.patch_ID = OUTPUT_FILTER_ID_EMPTY;
			id.canopy_strata_ID = OUTPUT_FILTER_ID_EMPTY;
			status = apply_to_patches_in_basin(error, error_len, date, filter, p, id, acc_objs_to_reset,
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

static bool output_stratum(char * const error, size_t error_len,
		struct date date, OutputFilter const * const filter,
		PointerSet **acc_objs_to_reset) {
	fprintf(stderr, "\toutput_stratum()...\n");

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
			status = output_variables(error, error_len, date, entity, id, filter);
			break;
		case STRATUM_TYPE_PATCH:
			id.basin_ID = s->basinID;
			id.hillslope_ID = s->hillslopeID;
			id.zone_ID = s->zoneID;
			id.patch_ID = s->patchID;
			id.canopy_strata_ID = OUTPUT_FILTER_ID_EMPTY;
			status = apply_to_strata_in_patch(error, error_len, date, filter, s, id, acc_objs_to_reset,
					*output_variables);
			break;
		case STRATUM_TYPE_ZONE:
			id.basin_ID = s->basinID;
			id.hillslope_ID = s->hillslopeID;
			id.zone_ID = s->zoneID;
			id.patch_ID = OUTPUT_FILTER_ID_EMPTY;
			id.canopy_strata_ID = OUTPUT_FILTER_ID_EMPTY;
			status = apply_to_strata_in_zone(error, error_len, date, filter, s, id, acc_objs_to_reset,
					*output_variables);
			break;
		case STRATUM_TYPE_HILLSLOPE:
			id.basin_ID = s->basinID;
			id.hillslope_ID = s->hillslopeID;
			id.zone_ID = OUTPUT_FILTER_ID_EMPTY;
			id.patch_ID = OUTPUT_FILTER_ID_EMPTY;
			id.canopy_strata_ID = OUTPUT_FILTER_ID_EMPTY;
			status = apply_to_strata_in_hillslope(error, error_len, date, filter, s, id, acc_objs_to_reset,
					*output_variables);
			break;
		case STRATUM_TYPE_BASIN:
			id.basin_ID = s->basinID;
			id.hillslope_ID = OUTPUT_FILTER_ID_EMPTY;
			id.zone_ID = OUTPUT_FILTER_ID_EMPTY;
			id.patch_ID = OUTPUT_FILTER_ID_EMPTY;
			id.canopy_strata_ID = OUTPUT_FILTER_ID_EMPTY;
			status = apply_to_strata_in_basin(error, error_len, date, filter, s, id, acc_objs_to_reset,
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

bool output_filter_output_daily(char * const error, size_t error_len,
		struct date date, OutputFilter const * const filters) {
	fprintf(stderr, "output_filter_output_daily(): Where filtered output will happen...\n");

	char *local_error;
	bool status = true;

	for (OutputFilter const * f = filters; f != NULL; f = f->next) {
		if (f->timestep == TIMESTEP_DAILY) {
			switch (f->type) {
			case OUTPUT_FILTER_BASIN:
				status = output_basin(error, error_len, date, f, NULL, NULL);
				if (!status) return false;
				break;
			case OUTPUT_FILTER_PATCH:
				status = output_patch(error, error_len, date, f, NULL);
				if (!status) return false;
				break;
			case OUTPUT_FILTER_CANOPY_STRATUM:
				status = output_stratum(error, error_len, date, f, NULL);
				if (!status) return false;
				break;
			default:
				local_error = (char *)calloc(MAXSTR, sizeof(char));
				snprintf(local_error, MAXSTR, "output_filter_output_daily: output filter type %d is unknown or not yet implemented.", f->type);
				return_with_error(error, error_len, local_error);
			}
		}
	}

	return status;
}

bool output_filter_output_monthly(char * const error, size_t error_len,
		struct date date, OutputFilter const * const filters) {
	fprintf(stderr, "output_filter_output_monthly(): Where filtered output will happen...\n");

	char *local_error;
	bool status = true;

	PointerSet *acc_patch_obj_to_reset = NULL;
	PointerSet *acc_stratum_obj_to_reset = NULL;

	for (OutputFilter const * f = filters; f != NULL; f = f->next) {
		if (f->timestep == TIMESTEP_MONTHLY) {
			switch (f->type) {
			case OUTPUT_FILTER_BASIN:
				status = output_basin(error, error_len, date, f,
						&acc_patch_obj_to_reset, &acc_stratum_obj_to_reset);
				if (!status) return false;
				break;
			case OUTPUT_FILTER_PATCH:
				status = output_patch(error, error_len, date, f, &acc_patch_obj_to_reset);
				if (!status) return false;
				break;
			case OUTPUT_FILTER_CANOPY_STRATUM:
				status = output_stratum(error, error_len, date, f, &acc_stratum_obj_to_reset);
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
	reset_accumulator_patch(&acc_patch_obj_to_reset);
	reset_accumulator_stratum(&acc_stratum_obj_to_reset);

	return status;
}

bool output_filter_output_yearly(char * const error, size_t error_len,
		struct date date, OutputFilter const * const filters) {
	fprintf(stderr, "output_filter_output_yearly(): Where filtered output will happen...\n");

	char *local_error;
	bool status = true;

	PointerSet *acc_patch_obj_to_reset = NULL;
	PointerSet *acc_stratum_obj_to_reset = NULL;

	for (OutputFilter const * f = filters; f != NULL; f = f->next) {
		if (f->timestep == TIMESTEP_YEARLY) {
			switch (f->type) {
			case OUTPUT_FILTER_BASIN:
				status = output_basin(error, error_len, date, f,
						&acc_patch_obj_to_reset, &acc_stratum_obj_to_reset);
				if (!status) return false;
				break;
			case OUTPUT_FILTER_PATCH:
				status = output_patch(error, error_len, date, f, &acc_patch_obj_to_reset);
				if (!status) return false;
				break;
			case OUTPUT_FILTER_CANOPY_STRATUM:
				status = output_stratum(error, error_len, date, f, &acc_stratum_obj_to_reset);
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
	reset_accumulator_patch(&acc_patch_obj_to_reset);
	reset_accumulator_stratum(&acc_stratum_obj_to_reset);

	return status;
}
