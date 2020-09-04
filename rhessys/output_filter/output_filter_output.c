#include <stdlib.h>
#include <stdio.h>

#include "rhessys.h"
#include "output_filter/output_filter_output.h"


MaterializedVariable *alloc_materialized_variable_array(size_t num_elements) {
	return calloc(num_elements, sizeof(MaterializedVariable));
}

inline static MaterializedVariable materialize_variable(OutputFilterVariable const * const v, void * const p) {
	MaterializedVariable mat_var;
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

static bool output_patch_daily_variables(char * const error, size_t error_len,
		struct date date, struct patch_object * const patch, OutputFilter * const f) {
	fprintf(stderr, "\t\toutput_patch_daily_variables(num_named_variables: %hu)...\n", f->num_named_variables);

	char *local_error;
	bool status;
	MaterializedVariable mat_var;

	// Allocate array for num_named_variables materialized variables
	MaterializedVariable *mat_vars = alloc_materialized_variable_array(f->num_named_variables);
	if (mat_vars == NULL) {
		perror("output_patch_daily_variables: Unable to allocate materialize variable array");
		return false;
	}

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
		// TODO: Output timestamp
		status = output_format_csv_write_data(error, error_len,
				date, f, mat_vars, true);
		break;
	case OUTPUT_TYPE_NETCDF:
	default:
		fprintf(stderr, "output_patch_daily_variables: output format type %d is unknown or not yet implemented.",
				f->output->format);
		return false;
	}

	// Free materialize variable array
	free(mat_vars);

	return true;
}

static bool output_patch_daily(char * const error, size_t error_len,
		struct date date, OutputFilter * const filter) {
	fprintf(stderr, "\toutput_patch_daily()...\n");

	char *local_error;

	for (OutputFilterPatch *p = filter->patches; p != NULL; p = p->next) {
		switch (p->output_patch_type) {
		case PATCH:
			return output_patch_daily_variables(error, error_len, date, p->patch, filter);
		case ZONE:
		case HILLSLOPE:
		case BASIN:
		case ALL_PATCHES:
		default:
			local_error = (char *)calloc(MAXSTR, sizeof(char));
			snprintf(local_error, MAXSTR, "output_patch_daily: patch type %d is unknown or not yet implemented.",
					p->output_patch_type);
			return_with_error(error, error_len, local_error);
		}
	}

	return false;
}

bool output_filter_output_daily(char * const error, size_t error_len,
		struct date date, OutputFilter * const filters) {
	fprintf(stderr, "output_filter_output_daily(): Where filtered output will happen...\n");

	char *local_error;

	for (OutputFilter *f = filters; f != NULL; f = f->next) {
		if (f->timestep == TIMESTEP_DAILY) {
			switch (f->type) {
			case OUTPUT_FILTER_PATCH:
				return output_patch_daily(error, error_len, date, f);
			case OUTPUT_FILTER_CANOPY_STRATA:
			default:
				local_error = (char *)calloc(MAXSTR, sizeof(char));
				snprintf(local_error, MAXSTR, "output_filter_output_daily: output filter type %d is unknown or not yet implemented.", f->type);
				return_with_error(error, error_len, local_error);
			}
		}
	}

	return false;
}

