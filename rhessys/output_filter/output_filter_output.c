#include <stdlib.h>
#include <stdio.h>

#include "rhessys.h"
#include "output_filter/output_filter_output.h"
#include "pointer_list.h"


static PointerList_t *accum_patch_obj_to_reset = NULL;
static PointerList_t *accum_strata_obj_to_reset = NULL;


inline static void reset_accum_obj(PointerList_t *list, size_t len) {
	if (list == NULL) return;
	memset(list->ptr, 0, len);
	if (list->next) {
		reset_accum_obj(list->next, len);
	}
}

inline static void add_to_accum_reset_list(PointerList_t **list_ptr, void *entity) {
	if (*list_ptr == NULL) {
		*list_ptr = newPointerList(entity);
	} else {
		pointerListAppend(*list_ptr, entity);
	}
}

inline static MaterializedVariable materialize_variable(OutputFilterVariable const * const v, void * entity) {
	MaterializedVariable mat_var;
	size_t offset = v->offset;
	if (v->sub_struct_ptr) {
		// TODO: Remove comment
		// Change entity to point to struct pointed to by original entity+offset.
		// Do this by going to where the pointer to the sub-struct is within the original
		// struct (e.g. entity+offset), then cast to char * (even though that's not the type)
		// just so that we can de-reference the pointer to get the address of the substruct,
		// which is the new entity whose member (indicated by v->sub_struct_var_offset) we
		// want to access.
		//entity = (void *) ((char *)entity) + offset;

		// Variable of interest is in another struct pointed to from the main struct. The entity should already
		// be that other struct at this point, so we can ignore the offset to the other struct pointer.
		offset = 0;
	}
	if (v->sub_struct_var_offset != SIZE_MAX) {
		// If sub_struct_var_offset has been set for this variable
		// (which means this must be a sub-struct variable) apply it to the offset
		offset += v->sub_struct_var_offset;
	}
	switch (v->data_type) {
	case DATA_TYPE_BOOL:
		mat_var.data_type = v->data_type;
		if (entity != NULL) {
			mat_var.u.bool_val = *((bool *)(entity + offset));
		} else {
			mat_var.u.bool_val = false;
		}
		fprintf(stderr, "\t\t\tvar: %s, value: %h\n", v->name, mat_var.u.bool_val);
		break;
	case DATA_TYPE_CHAR:
		mat_var.data_type = v->data_type;
		if (entity != NULL) {
			mat_var.u.char_val = *((char *)(entity + offset));
		} else {
			mat_var.u.char_val = '\0';
		}
		fprintf(stderr, "\t\t\tvar: %s, value: %c\n", v->name, mat_var.u.char_val);
		break;
	case DATA_TYPE_STRING:
		mat_var.data_type = v->data_type;
		if (entity != NULL) {
			mat_var.u.char_array = (char *)(entity + offset);
		} else {
			mat_var.u.char_array = NULL;
		}
		fprintf(stderr, "\t\t\tvar: %s, value: %s\n", v->name, mat_var.u.char_array);
		break;
	case DATA_TYPE_INT:
		mat_var.data_type = v->data_type;
		if (entity != NULL) {
			mat_var.u.int_val = *((int *)(entity + offset));
		} else {
			mat_var.u.int_val = 0;
		}
		fprintf(stderr, "\t\t\tvar: %s, value: %d\n", v->name, mat_var.u.int_val);
		break;
	case DATA_TYPE_LONG:
		mat_var.data_type = v->data_type;
		if (entity != NULL) {
			mat_var.u.long_val = *((long *)(entity + offset));
		} else {
			mat_var.u.long_val = 0;
		}
		fprintf(stderr, "\t\t\tvar: %s, value: %l\n", v->name, mat_var.u.long_val);
		break;
	case DATA_TYPE_LONG_ARRAY:
		mat_var.data_type = v->data_type;
		if (entity != NULL) {
			mat_var.u.long_array = (long *)(entity + offset);
		} else {
			mat_var.u.long_array = NULL;
		}
		fprintf(stderr, "\t\t\tvar: %s, value: %p\n", v->name, mat_var.u.long_array);
		break;
	case DATA_TYPE_FLOAT:
		mat_var.data_type = v->data_type;
		if (entity != NULL) {
			mat_var.u.float_val = *((float *)(entity + offset));
		} else {
			mat_var.u.float_val = 0.0;
		}
		fprintf(stderr, "\t\t\tvar: %s, value: %f\n", v->name, mat_var.u.float_val);
		break;
	case DATA_TYPE_DOUBLE:
		mat_var.data_type = v->data_type;
		if (entity != NULL) {
			mat_var.u.double_val = *((double *)(entity + offset));
		} else {
			mat_var.u.double_val = 0.0;
		}
		fprintf(stderr, "\t\t\tvar: %s, value: %f\n", v->name, mat_var.u.double_val);
		break;
	case DATA_TYPE_DOUBLE_ARRAY:
		mat_var.data_type = v->data_type;
		if (entity != NULL) {
			mat_var.u.double_array = (double *)(entity + offset);
		} else {
			mat_var.u.double_array = NULL;
		}
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
		OutputFilterVariable *var,
		void *entity_in) {
	// Since determine_stratum_entity() will be called, via a function pointer, from output_variables()
	// the stratum is passed in as a void * pointer (since output_variables() can be called on
	// any entity type), so we need to cast back into struct canopy_strata_object *.
	struct canopy_strata_object *stratum = (struct canopy_strata_object *)entity_in;
	void *entity = NULL;
	switch (timestep) {
	case TIMESTEP_MONTHLY:
		entity = (void *)(&(stratum->acc_month));
		add_to_accum_reset_list(&accum_strata_obj_to_reset, entity);
		break;
	case TIMESTEP_YEARLY:
		entity = (void *)(&(stratum->acc_year));
		add_to_accum_reset_list(&accum_strata_obj_to_reset, entity);
		break;
	case TIMESTEP_HOURLY:
	case TIMESTEP_DAILY:
	default:
		if (var->sub_struct_ptr) {
			// Variable of interest is in a sub struct referenced by a pointer, check if substruct is one
			// that is supported by output filtering
			if (strcmp("hourly", var->name) == 0) {
				entity = (void *)(stratum->hourly);
			} else {
				char *local_error = (char *)calloc(MAXSTR, sizeof(char));
				snprintf(local_error, MAXSTR, "determine_stratum_entity:  sub struct pointer named '%s' is not supported for entity type canopy_strata_object.\n",
						 var->name);
				fprintf(stderr, local_error);
				free(local_error);
				return NULL;
			}
		} else {
			// Variable of interest is not in a sub struct referenced by a pointer, so just use the
			// base entity as the entity.
			entity = (void *)stratum;
		}
		break;
	}
	return entity;
}

inline static void *determine_patch_entity(OutputFilterTimestep timestep,
		OutputFilterVariable *var,
		void *entity_in) {
	// Since determine_patch_entity() will be called, via a function pointer, from output_variables()
	// the patch is passed in as a void * pointer (since output_variables() can be called on
	// any entity type), so we need to cast back into struct patch_object *.
	struct patch_object *patch = (struct patch_object *)entity_in;
	void *entity = NULL;
	switch (timestep) {
	case TIMESTEP_MONTHLY:
		entity = (void *)(&(patch->acc_month));
		add_to_accum_reset_list(&accum_patch_obj_to_reset, entity);
		break;
	case TIMESTEP_YEARLY:
		entity = (void *)(&(patch->acc_year));
		add_to_accum_reset_list(&accum_patch_obj_to_reset, entity);
		break;
	case TIMESTEP_HOURLY:
	case TIMESTEP_DAILY:
	default:
		if (var->sub_struct_ptr) {
			// Variable of interest is in a sub struct referenced by a pointer, check if substruct is one
			// that is supported by output filtering
			if (strcmp("hourly", var->name) == 0) {
				entity = (void *)(patch->hourly);
			} else if (strcmp("grow", var->name) == 0) {
				entity = (void *)(patch->grow);
			} else if (strcmp("zone", var->name) == 0) {
				entity = (void *)(patch->zone);
			} else {
				char *local_error = (char *)calloc(MAXSTR, sizeof(char));
				snprintf(local_error, MAXSTR, "determine_patch_entity:  sub struct pointer named '%s' is not supported for entity type patch_object.\n",
						 var->name);
				fprintf(stderr, local_error);
				free(local_error);
				return NULL;
			}
		} else {
			// Variable of interest is not in a sub struct referenced by a pointer, so just use the
			// base entity as the entity.
			entity = (void *)patch;
		}

		break;
	}
	return entity;
}

static bool apply_to_strata_in_patch(char * const error, size_t error_len,
		struct date date,
		OutputFilter const * const filter, OutputFilterStratum const * const s, EntityID id,
		bool (*output_fn)(char * const, size_t, struct date date, void * const, EntityID, OutputFilter const * const)) {
	for (size_t i = 0; i < s->patch->num_canopy_strata; i++) {
		struct canopy_strata_object *stratum = s->patch->canopy_strata[i];
		id.canopy_strata_ID = stratum->ID;
		// TODO: Pass determine_stratum_entity as function ptr instead
		void *entity = determine_stratum_entity(filter->timestep, stratum);
		bool status = (*output_fn)(error, error_len, date, entity, id, filter);
		if (!status) return false;
	}
	return true;
}

static bool apply_to_patches_in_zone(char * const error, size_t error_len,
		struct date date,
		OutputFilter const * const filter, OutputFilterPatch const * const p, EntityID id,
		bool (*output_fn)(char * const, size_t, struct date date, void * const, EntityID, OutputFilter const * const)) {
	for (size_t i = 0; i < p->zone->num_patches; i++) {
		struct patch_object *patch = p->zone->patches[i];
		id.patch_ID = patch->ID;
		// TODO: Pass determine_patch_entity as function ptr instead
		void *entity = determine_patch_entity(filter->timestep, patch);
		bool status = (*output_fn)(error, error_len, date, entity, id, filter);
		if (!status) return false;
	}
	return true;
}

static bool apply_to_strata_in_zone(char * const error, size_t error_len,
		struct date date,
		OutputFilter const * const filter, OutputFilterStratum const * const s, EntityID id,
		bool (*output_fn)(char * const, size_t, struct date date, void * const, EntityID, OutputFilter const * const)) {
	for (size_t i = 0; i < s->zone->num_patches; i++) {
		struct patch_object *patch = s->zone->patches[i];
		id.patch_ID = patch->ID;
		for (size_t j = 0; j < patch->num_canopy_strata; j++) {
			struct canopy_strata_object *stratum = patch->canopy_strata[j];
			id.canopy_strata_ID = stratum->ID;
			// TODO: Pass determine_stratum_entity as function ptr instead
			void *entity = determine_stratum_entity(filter->timestep, stratum);
			bool status = (*output_fn)(error, error_len, date, entity, id, filter);
			if (!status) return false;
		}
	}
	return true;
}

static bool apply_to_patches_in_hillslope(char * const error, size_t error_len,
		struct date date,
		OutputFilter const * const filter, OutputFilterPatch const * const p, EntityID id,
		bool (*output_fn)(char * const, size_t, struct date date, void * const, EntityID, OutputFilter const * const)) {
	for (size_t i = 0; i < p->hill->num_zones; i++) {
		struct zone_object *z = p->hill->zones[i];
		id.zone_ID = z->ID;
		for (size_t j = 0; j < z->num_patches; j++) {
			struct patch_object *patch = z->patches[j];
			id.patch_ID = patch->ID;
			// TODO: Pass determine_patch_entity as function ptr instead
			void *entity = determine_patch_entity(filter->timestep, patch);
			bool status = (*output_fn)(error, error_len, date, entity, id, filter);
			if (!status) return false;
		}
	}
	return true;
}

static bool apply_to_strata_in_hillslope(char * const error, size_t error_len,
		struct date date,
		OutputFilter const * const filter, OutputFilterStratum const * const s, EntityID id,
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
				// TODO: Pass determine_stratum_entity as function ptr instead
				void *entity = determine_stratum_entity(filter->timestep, stratum);
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
				// TODO: Pass determine_patch_entity as function ptr instead
				void *entity = determine_patch_entity(filter->timestep, patch);
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
					// TODO: Pass determine_stratum_entity as function ptr instead
					void *entity = determine_stratum_entity(filter->timestep, stratum);
					bool status = (*output_fn)(error, error_len, date, entity, id, filter);
					if (!status) return false;
				}
			}
		}
	}
	return true;
}

/* TODO: Refactor to take function of type: (void *)(*determine_entity)(OutputFilterTimestep timestep, OutputFilterVariable var, void *entity)
 *
 * */
static bool output_variables(char * const error, size_t error_len, struct date date,
		void *(*determine_entity)(OutputFilterTimestep timestep, OutputFilterVariable *var, void *entity),
		EntityID id, OutputFilter const * const f) {
	fprintf(stderr, "\t\toutput_variables(num_named_variables: %hu)...\n", f->num_named_variables);

	char *local_error;
	bool status;
	MaterializedVariable mat_var;
	MaterializedVariable *mat_vars = f->output->materialized_variables;
	num_elements_t curr_var = 0;

	for (OutputFilterVariable *v = f->variables; v != NULL; v = v->next) {
		switch (v->variable_type) {
		case NAMED:
			// Materialize variable and add it to array
			void *entity = (*determine_entity)(f->timestep, v, entity);
			if (entity == NULL) {
				local_error = (char *)calloc(MAXSTR, sizeof(char));
				snprintf(local_error, MAXSTR, "output_variables: received NULL entity, aborting.");
				return_with_error(error, error_len, local_error);
			}
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

	// Output materialized variables array using appropriate driver
	switch (f->output->format) {
	case OUTPUT_TYPE_CSV:
		status = output_format_csv_write_data(error, error_len,
				date, f, id, mat_vars, true);
		break;
	case OUTPUT_TYPE_NETCDF:
		status = output_format_netcdf_write_data(error, error_len,
				date, f, id, mat_vars, true);
		break;
	default:
		fprintf(stderr, "output_variables: output format type %d is unknown or not yet implemented.",
				f->output->format);
		return false;
	}

	return status;
}

static bool output_patch(char * const error, size_t error_len,
		struct date date, OutputFilter const * const filter) {
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
			// TODO: Pass determine_patch_entity as function ptr instead
			void *entity = determine_patch_entity(filter->timestep, p->patch);
			status = output_variables(error, error_len, date, entity, id, filter);
			break;
		case PATCH_TYPE_ZONE:
			id.basin_ID = p->basinID;
			id.hillslope_ID = p->hillslopeID;
			id.zone_ID = p->zoneID;
			id.patch_ID = OUTPUT_FILTER_ID_EMPTY;
			id.canopy_strata_ID = OUTPUT_FILTER_ID_EMPTY;
			status = apply_to_patches_in_zone(error, error_len, date, filter, p, id,
					*output_variables);
			break;
		case PATCH_TYPE_HILLSLOPE:
			id.basin_ID = p->basinID;
			id.hillslope_ID = p->hillslopeID;
			id.zone_ID = OUTPUT_FILTER_ID_EMPTY;
			id.patch_ID = OUTPUT_FILTER_ID_EMPTY;
			id.canopy_strata_ID = OUTPUT_FILTER_ID_EMPTY;
			status = apply_to_patches_in_hillslope(error, error_len, date, filter, p, id,
					*output_variables);
			break;
		case PATCH_TYPE_BASIN:
			id.basin_ID = p->basinID;
			id.hillslope_ID = OUTPUT_FILTER_ID_EMPTY;
			id.zone_ID = OUTPUT_FILTER_ID_EMPTY;
			id.patch_ID = OUTPUT_FILTER_ID_EMPTY;
			id.canopy_strata_ID = OUTPUT_FILTER_ID_EMPTY;
			status = apply_to_patches_in_basin(error, error_len, date, filter, p, id,
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
		struct date date, OutputFilter const * const filter) {
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
			// TODO: Pass determine_stratum_entity as function ptr instead
			void *entity = determine_stratum_entity(filter->timestep, s->stratum);
			status = output_variables(error, error_len, date, entity, id, filter);
			break;
		case STRATUM_TYPE_PATCH:
			id.basin_ID = s->basinID;
			id.hillslope_ID = s->hillslopeID;
			id.zone_ID = s->zoneID;
			id.patch_ID = s->patchID;
			id.canopy_strata_ID = OUTPUT_FILTER_ID_EMPTY;
			status = apply_to_strata_in_patch(error, error_len, date, filter, s, id,
					*output_variables);
			break;
		case STRATUM_TYPE_ZONE:
			id.basin_ID = s->basinID;
			id.hillslope_ID = s->hillslopeID;
			id.zone_ID = s->zoneID;
			id.patch_ID = OUTPUT_FILTER_ID_EMPTY;
			id.canopy_strata_ID = OUTPUT_FILTER_ID_EMPTY;
			status = apply_to_strata_in_zone(error, error_len, date, filter, s, id,
					*output_variables);
			break;
		case STRATUM_TYPE_HILLSLOPE:
			id.basin_ID = s->basinID;
			id.hillslope_ID = s->hillslopeID;
			id.zone_ID = OUTPUT_FILTER_ID_EMPTY;
			id.patch_ID = OUTPUT_FILTER_ID_EMPTY;
			id.canopy_strata_ID = OUTPUT_FILTER_ID_EMPTY;
			status = apply_to_strata_in_hillslope(error, error_len, date, filter, s, id,
					*output_variables);
			break;
		case STRATUM_TYPE_BASIN:
			id.basin_ID = s->basinID;
			id.hillslope_ID = OUTPUT_FILTER_ID_EMPTY;
			id.zone_ID = OUTPUT_FILTER_ID_EMPTY;
			id.patch_ID = OUTPUT_FILTER_ID_EMPTY;
			id.canopy_strata_ID = OUTPUT_FILTER_ID_EMPTY;
			status = apply_to_strata_in_basin(error, error_len, date, filter, s, id,
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
			case OUTPUT_FILTER_PATCH:
				status = output_patch(error, error_len, date, f);
				if (!status) return false;
				break;
			case OUTPUT_FILTER_CANOPY_STRATUM:
				status = output_stratum(error, error_len, date, f);
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

/*
 * TODO: Send accumulator lists into output_* functions instead of using global variables
 */
bool output_filter_output_monthly(char * const error, size_t error_len,
		struct date date, OutputFilter const * const filters) {
	fprintf(stderr, "output_filter_output_monthly(): Where filtered output will happen...\n");

	char *local_error;
	bool status = true;

	for (OutputFilter const * f = filters; f != NULL; f = f->next) {
		if (f->timestep == TIMESTEP_MONTHLY) {
			switch (f->type) {
			case OUTPUT_FILTER_PATCH:
				status = output_patch(error, error_len, date, f);
				if (!status) return false;
				break;
			case OUTPUT_FILTER_CANOPY_STRATUM:
				status = output_stratum(error, error_len, date, f);
				if (!status) return false;
				break;
			default:
				local_error = (char *)calloc(MAXSTR, sizeof(char));
				snprintf(local_error, MAXSTR, "output_filter_output_monthly: output filter type %d is unknown or not yet implemented.", f->type);
				return_with_error(error, error_len, local_error);
			}
		}
	}

	reset_accumulators();

	return status;
}

/*
 * TODO: Send accumulator lists into output_* functions instead of using global variables
 */
bool output_filter_output_yearly(char * const error, size_t error_len,
		struct date date, OutputFilter const * const filters) {
	fprintf(stderr, "output_filter_output_yearly(): Where filtered output will happen...\n");

	char *local_error;
	bool status = true;

	for (OutputFilter const * f = filters; f != NULL; f = f->next) {
		if (f->timestep == TIMESTEP_YEARLY) {
			switch (f->type) {
			case OUTPUT_FILTER_PATCH:
				status = output_patch(error, error_len, date, f);
				if (!status) return false;
				break;
			case OUTPUT_FILTER_CANOPY_STRATUM:
				status = output_stratum(error, error_len, date, f);
				if (!status) return false;
				break;
			default:
				local_error = (char *)calloc(MAXSTR, sizeof(char));
				snprintf(local_error, MAXSTR, "output_filter_output_yearly: output filter type %d is unknown or not yet implemented.", f->type);
				return_with_error(error, error_len, local_error);
			}
		}
	}

	reset_accumulators();

	return status;
}

void reset_accumulators() {
	if (accum_patch_obj_to_reset) {
		reset_accum_obj(accum_patch_obj_to_reset, sizeof(struct accumulate_patch_object));
		freePointerList(accum_patch_obj_to_reset);
		accum_patch_obj_to_reset = NULL;
	}

	if (accum_strata_obj_to_reset) {
		reset_accum_obj(accum_strata_obj_to_reset, sizeof(struct accumulate_strata_object));
		freePointerList(accum_strata_obj_to_reset);
		accum_strata_obj_to_reset = NULL;
	}
}
