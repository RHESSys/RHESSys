#include "rhessys.h"
#include "output_filter/output_filter_output.h"


static bool output_patch_daily_variables(char * const error, size_t error_len,
		OutputFilterPatch * const p, OutputFilterVariable * const variables,
		OutputFilterOutput * const output) {
	fprintf(stderr, "\t\toutput_patch_daily_variables()...\n");

	char *local_error;

	for (OutputFilterVariable *v = variables; v != NULL; v = v->next) {
		switch (v->variable_type) {
		case NAMED:
			// TODO: Add to list of variables to output...
			fprintf(stderr, "\t\t\tNAMED variable encountered...\n");
			break;
		case ANY_VAR:
		default:
			local_error = (char *)calloc(MAXSTR, sizeof(char));
			snprintf(local_error, MAXSTR, "output_patch_daily_variables: variable type %d is unknown or not yet implemented.",
					v->variable_type);
			return_with_error(error, error_len, local_error);
		}
	}

	return true;
}

static bool output_patch_daily(char * const error, size_t error_len,
		OutputFilter * const filter) {
	fprintf(stderr, "\toutput_patch_daily()...\n");

	char *local_error;

	for (OutputFilterPatch *p = filter->patches; p != NULL; p = p->next) {
		switch (p->output_patch_type) {
		case PATCH:
			return output_patch_daily_variables(error, error_len, p,
					filter->variables, filter->output);
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
		OutputFilter * const filters) {
	fprintf(stderr, "output_filter_output_daily(): Where filtered output will happen...\n");

	char *local_error;

	for (OutputFilter *f = filters; f != NULL; f = f->next) {
		if (f->timestep == TIMESTEP_DAILY) {
			switch (f->type) {
			case OUTPUT_FILTER_PATCH:
				return output_patch_daily(error, error_len, f);
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

