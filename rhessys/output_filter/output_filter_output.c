#include <stdlib.h>
#include <stdio.h>

#include "rhessys.h"
#include "output_filter/output_filter_output.h"


// TODO: Update to + v->sub_struct_var_offset to each address calculation
inline static MaterializedVariable materialize_variable(OutputFilterVariable const * const v, void * const p) {
	MaterializedVariable mat_var;
	// TODO:
	//void * p_offset = p + v->offset + v->sub_struct_var_offset
	switch (v->data_type) {
	case DATA_TYPE_BOOL:
		mat_var.data_type = v->data_type;
		mat_var.u.bool_val = *((bool *)(p + v->offset));
		fprintf(stderr, "\t\t\tvar: %s, value: %h\n", v->name, mat_var.u.bool_val);
		break;
	case DATA_TYPE_CHAR:
		mat_var.data_type = v->data_type;
		mat_var.u.char_val = *((char *)(p + v->offset));
		fprintf(stderr, "\t\t\tvar: %s, value: %c\n", v->name, mat_var.u.char_val);
		break;
	case DATA_TYPE_CHAR_ARRAY:
		mat_var.data_type = v->data_type;
		mat_var.u.char_array = (char *)(p + v->offset);
		fprintf(stderr, "\t\t\tvar: %s, value: %s\n", v->name, mat_var.u.char_array);
		break;
	case DATA_TYPE_INT:
		mat_var.data_type = v->data_type;
		mat_var.u.int_val = *((int *)(p + v->offset));
		fprintf(stderr, "\t\t\tvar: %s, value: %d\n", v->name, mat_var.u.int_val);
		break;
	case DATA_TYPE_LONG:
		mat_var.data_type = v->data_type;
		mat_var.u.long_val = *((long *)(p + v->offset));
		fprintf(stderr, "\t\t\tvar: %s, value: %l\n", v->name, mat_var.u.long_val);
		break;
	case DATA_TYPE_LONG_ARRAY:
		mat_var.data_type = v->data_type;
		mat_var.u.long_array = (long *)(p + v->offset);
		fprintf(stderr, "\t\t\tvar: %s, value: %p\n", v->name, mat_var.u.long_array);
		break;
	case DATA_TYPE_FLOAT:
		mat_var.data_type = v->data_type;
		mat_var.u.float_val = *((float *)(p + v->offset));
		fprintf(stderr, "\t\t\tvar: %s, value: %f\n", v->name, mat_var.u.float_val);
		break;
	case DATA_TYPE_DOUBLE:
		mat_var.data_type = v->data_type;
		mat_var.u.double_val = *((double *)(p + v->offset));
		fprintf(stderr, "\t\t\tvar: %s, value: %f\n", v->name, mat_var.u.double_val);
		break;
	case DATA_TYPE_DOUBLE_ARRAY:
		mat_var.data_type = v->data_type;
		mat_var.u.double_array = (double *)(p + v->offset);
		fprintf(stderr, "\t\t\tvar: %s, value: %p\n", v->name, mat_var.u.double_array);
		break;
	default:
		mat_var.data_type = DATA_TYPE_UNDEFINED;
	}
	return mat_var;
}

static bool apply_to_patches_in_zone(char * const error, size_t error_len,
		struct date date,
		OutputFilter const * const filter, OutputFilterPatch const * const p, EntityID id,
		bool (*output_fn)(char * const, size_t, struct date date, struct patch_object const * const, EntityID, OutputFilter const * const)) {
	for (size_t i = 0; i < p->zone->num_patches; i++) {
		struct patch_object *patch = p->zone->patches[i];
		id.patch_ID = patch->ID;
		bool status = (*output_fn)(error, error_len, date, patch, id, filter);
		if (!status) return false;
	}
	return true;
}

static bool apply_to_patches_in_hillslope(char * const error, size_t error_len,
		struct date date,
		OutputFilter const * const filter, OutputFilterPatch const * const p, EntityID id,
		bool (*output_fn)(char * const, size_t, struct date date, struct patch_object const * const, EntityID, OutputFilter const * const)) {
	for (size_t i = 0; i < p->hill->num_zones; i++) {
		struct zone_object *z = p->hill->zones[i];
		id.zone_ID = z->ID;
		for (size_t j = 0; j < z->num_patches; j++) {
			struct patch_object *patch = z->patches[j];
			id.patch_ID = patch->ID;
			bool status = (*output_fn)(error, error_len, date, patch, id, filter);
			if (!status) return false;
		}
	}
	return true;
}

static bool apply_to_patches_in_basin(char * const error, size_t error_len,
		struct date date,
		OutputFilter const * const filter, OutputFilterPatch const * const p, EntityID id,
		bool (*output_fn)(char * const, size_t, struct date date, struct patch_object const * const, EntityID, OutputFilter const * const)) {
	for (size_t i = 0; i < p->basin->num_hillslopes; i++) {
		struct hillslope_object *h = p->basin->hillslopes[i];
		id.hillslope_ID = h->ID;
		for (size_t j = 0; j < h->num_zones; j++) {
			struct zone_object *z = h->zones[j];
			id.zone_ID = z->ID;
			for (size_t k = 0; k < z->num_patches; k++) {
				struct patch_object *patch = z->patches[k];
				id.patch_ID = patch->ID;
				bool status = (*output_fn)(error, error_len, date, patch, id, filter);
				if (!status) return false;
			}
		}
	}
	return true;
}

static bool output_patch_daily_variables(char * const error, size_t error_len,
		struct date date, struct patch_object const * const patch, EntityID id, OutputFilter const * const f) {
	fprintf(stderr, "\t\toutput_patch_daily_variables(num_named_variables: %hu)...\n", f->num_named_variables);

	char *local_error;
	bool status;
	MaterializedVariable mat_var;
	MaterializedVariable *mat_vars = f->output->materialized_variables;
	void *p = (void *) patch;
	num_elements_t curr_var = 0;

	for (OutputFilterVariable *v = f->variables; v != NULL; v = v->next) {
		switch (v->variable_type) {
		case NAMED:
			// Materialize variable and add it to array
			mat_var = materialize_variable(v, p);
			if (mat_var.data_type == DATA_TYPE_UNDEFINED) {
				local_error = (char *)calloc(MAXSTR, sizeof(char));
				snprintf(local_error, MAXSTR, "output_patch_daily_variables:  data type %d of variable %s is unknown or not yet implemented.",
						 v->data_type, v->name);
				return_with_error(error, error_len, local_error);
			}
			mat_vars[curr_var++] = mat_var;
			break;
		case ANY_VAR:
		default:
			local_error = (char *)calloc(MAXSTR, sizeof(char));
			snprintf(local_error, MAXSTR, "output_patch_daily_variables: variable type %d is unknown or not yet implemented.",
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
	default:
		fprintf(stderr, "output_patch_daily_variables: output format type %d is unknown or not yet implemented.",
				f->output->format);
		return false;
	}

	return true;
}

static bool output_patch_daily(char * const error, size_t error_len,
		struct date date, OutputFilter const * const filter) {
	fprintf(stderr, "\toutput_patch_daily()...\n");

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
			status = output_patch_daily_variables(error, error_len, date, p->patch, id, filter);
			break;
		case PATCH_TYPE_ZONE:
			id.basin_ID = p->basinID;
			id.hillslope_ID = p->hillslopeID;
			id.zone_ID = p->zoneID;
			id.patch_ID = OUTPUT_FILTER_ID_EMPTY;
			id.canopy_strata_ID = OUTPUT_FILTER_ID_EMPTY;
			status = apply_to_patches_in_zone(error, error_len, date, filter, p, id,
					*output_patch_daily_variables);
			break;
		case PATCH_TYPE_HILLSLOPE:
			id.basin_ID = p->basinID;
			id.hillslope_ID = p->hillslopeID;
			id.zone_ID = OUTPUT_FILTER_ID_EMPTY;
			id.patch_ID = OUTPUT_FILTER_ID_EMPTY;
			id.canopy_strata_ID = OUTPUT_FILTER_ID_EMPTY;
			status = apply_to_patches_in_hillslope(error, error_len, date, filter, p, id,
					*output_patch_daily_variables);
			break;
		case PATCH_TYPE_BASIN:
			id.basin_ID = p->basinID;
			id.hillslope_ID = OUTPUT_FILTER_ID_EMPTY;
			id.zone_ID = OUTPUT_FILTER_ID_EMPTY;
			id.patch_ID = OUTPUT_FILTER_ID_EMPTY;
			id.canopy_strata_ID = OUTPUT_FILTER_ID_EMPTY;
			status = apply_to_patches_in_basin(error, error_len, date, filter, p, id,
					*output_patch_daily_variables);
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

bool output_filter_output_daily(char * const error, size_t error_len,
		struct date date, OutputFilter const * const filters) {
	fprintf(stderr, "output_filter_output_daily(): Where filtered output will happen...\n");

	char *local_error;

	for (OutputFilter const * f = filters; f != NULL; f = f->next) {
		if (f->timestep == TIMESTEP_DAILY) {
			switch (f->type) {
			case OUTPUT_FILTER_PATCH:
				return output_patch_daily(error, error_len, date, f);
			case OUTPUT_FILTER_CANOPY_STRATUM:
			default:
				local_error = (char *)calloc(MAXSTR, sizeof(char));
				snprintf(local_error, MAXSTR, "output_filter_output_daily: output filter type %d is unknown or not yet implemented.", f->type);
				return_with_error(error, error_len, local_error);
			}
		}
	}

	return true;
}

