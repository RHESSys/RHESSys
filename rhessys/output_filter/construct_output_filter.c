#include <string.h>

#include "rhessys.h"
#include "output_filter.h"
#include "index_struct_fields.h"
#include "output_filter/output_format_csv.h"

#define STRUCT_NAME_PATCH "patch_object"
#define STRUCT_NAME_ACCUM_PATCH "accumulate_patch_object"
#define STRUCT_NAME_STRATUM "canopy_strata_object"
#define STRUCT_NAME_ACCUM_STRATUM "accumulate_strata_object"


OutputFilter *parse(const char* input);
struct basin_object *find_basin(int basin_ID, struct world_object *world);
struct hillslope_object *find_hillslope_in_basin(int hillslope_ID, struct basin_object *basin);
struct zone_object *find_zone_in_hillslope(int zone_ID, struct hillslope_object *hillslope);
struct patch_object *find_patch(int patch_ID, int zone_ID, int hill_ID, struct basin_object *basin);
struct canopy_strata_object *find_stratum(int stratum_ID, int patch_ID, int zone_ID, int hill_ID, int basin_ID, struct world_object *world);


// TODO: add support for v->sub_struct_var_offset for fields of type DATA_TYPE_STRUCT
static bool init_variables_hourly_daily(OutputFilter *f, StructIndex_t *i, bool verbose) {
	if (f->variables == NULL) {
		fprintf(stderr, "init_variables_hourly_daily: no variables defined.");
				return false;
	}

	Dictionary_t *struct_index = NULL;
	char *struct_name = NULL;
	switch (f->type) {
	case OUTPUT_FILTER_PATCH:
		struct_index = i->patch_object;
		struct_name = STRUCT_NAME_PATCH;
		break;
	case OUTPUT_FILTER_CANOPY_STRATUM:
		struct_index = i->canopy_strata_object;
		struct_name = STRUCT_NAME_STRATUM;
		break;
	default:
		fprintf(stderr, "init_variables_hourly_daily: output filter type %d is unknown or not yet implemented.", f->type);
		return false;
	}

	OutputFilterVariable *v = f->variables;
	while (v != NULL) {
		if (v->variable_type == NAMED) {
			DictionaryValue_t *var_idx_entry = dictionaryGet(struct_index, v->name);
			if (var_idx_entry == NULL) {
				fprintf(stderr, "init_variables_hourly_daily: variable %s does not appear to be a member of struct %s.",
						v->name, struct_name);
				return false;
			}
			v->offset = var_idx_entry->offset;
			v->data_type = var_idx_entry->data_type;
			// TODO: Check for data_type == DATA_TYPE_STRUCT
			// if so, lookup sub_struct by name and get value of sub_struct_var_offset

			f->num_named_variables += 1;
		}
		v = v->next;
	}

	return true;
}

static bool init_variables_monthly_yearly(OutputFilter *f, StructIndex_t *i, bool verbose) {
	if (f->variables == NULL) {
		fprintf(stderr, "init_variables_monthly_yearly: no variables defined.");
				return false;
	}

	Dictionary_t *struct_index = NULL;
	char *struct_name = NULL;
	switch (f->type) {
	case OUTPUT_FILTER_PATCH:
		struct_index = i->accumulate_patch_object;
		struct_name = STRUCT_NAME_ACCUM_PATCH;
		break;
	case OUTPUT_FILTER_CANOPY_STRATUM:
		struct_index = i->accumulate_strata_object;
		struct_name = STRUCT_NAME_ACCUM_STRATUM;
		break;
	default:
		fprintf(stderr, "init_variables_monthly_yearly: output filter type %d is unknown or not yet implemented.", f->type);
		return false;
	}

	OutputFilterVariable *v = f->variables;
	while (v != NULL) {
		if (v->variable_type == NAMED) {
			DictionaryValue_t *var_idx_entry = dictionaryGet(struct_index, v->name);
			if (var_idx_entry == NULL) {
				fprintf(stderr, "init_variables_monthly_yearly: variable %s does not appear to be a member of struct %s.",
						v->name, struct_name);
				return false;
			}
			v->offset = var_idx_entry->offset;
			v->data_type = var_idx_entry->data_type;
			f->num_named_variables += 1;
		}
		v = v->next;
	}

	return true;
}

static bool init_variables(OutputFilter *f, StructIndex_t *i, bool verbose) {
	switch (f->timestep) {
	case TIMESTEP_HOURLY:
	case TIMESTEP_DAILY:
		return init_variables_hourly_daily(f, i, verbose);
	case TIMESTEP_MONTHLY:
	case TIMESTEP_YEARLY:
		return init_variables_monthly_yearly(f, i, verbose);
	case TIMESTEP_UNDEFINED:
	default:
		fprintf(stderr, "init_variable: timestep %d is unknown or not yet implemented.", f->timestep);
		return false;
	}

	return true;
}

static bool init_spatial_hierarchy_patch(OutputFilter *f,
		struct world_object * const w,
		bool verbose) {

	struct basin_object *b;

	if (f->patches == NULL) {
		fprintf(stderr, "init_spatial_hierarchy_patch: no patches defined but patch type was specified.");
		return false;
	}

	if (verbose) fprintf(stderr, "BEGIN init_spatial_hierarchy_patch\n");

	OutputFilterPatch *p = f->patches;
	while (p != NULL) {
		switch (p->output_patch_type) {
		case PATCH_TYPE_BASIN:
			if (verbose) {
				fprintf(stderr, "\tbasinID: %d\n", p->basinID);
			}
			p->basin = find_basin(p->basinID, w);
			if (p->basin == NULL) {
				fprintf(stderr, "init_spatial_hierarchy_patch: no basin with ID %d could be found.",
						p->basinID);
				return false;
			}
			break;
		case PATCH_TYPE_HILLSLOPE:
			if (verbose) {
				fprintf(stderr, "\tbasinID: %d, hillslopeID: %d\n",
						p->basinID, p->hillslopeID);
			}
			b = find_basin(p->basinID, w);
			if (b == NULL) {
				fprintf(stderr, "init_spatial_hierarchy_patch: basin %d could not be found, so could not locate hillslope %d.",
						p->basinID, p->hillslopeID);
				return false;
			}
			p->hill = find_hillslope_in_basin(p->hillslopeID, b);
			if (p->hill == NULL) {
				fprintf(stderr, "init_spatial_hierarchy_patch: hillslope %d could not be found in basin %d.",
						p->hillslopeID, p->basinID);
				return false;
			}
			break;
		case PATCH_TYPE_ZONE:
			if (verbose) {
				fprintf(stderr, "\tbasinID: %d, hillslopeID: %d, zoneID: %d\n",
						p->basinID, p->hillslopeID, p->zoneID);
			}
			b = find_basin(p->basinID, w);
			if (b == NULL) {
				fprintf(stderr, "init_spatial_hierarchy_patch: basin %d could not be found, so could not locate zone %d in hillslope %d.",
						p->basinID, p->zoneID, p->hillslopeID);
				return false;
			}
			struct hillslope_object *h = find_hillslope_in_basin(p->hillslopeID, b);
			if (h == NULL) {
				fprintf(stderr, "init_spatial_hierarchy_patch: hillslope %d could not be found in basin %d, so could not locate zone %d.",
						p->hillslopeID, p->basinID, p->zoneID);
				return false;
			}
			p->zone = find_zone_in_hillslope(p->zoneID, h);
			if (p->zone == NULL) {
				fprintf(stderr, "init_spatial_hierarchy_patch: zone %d could not be found in hillslope %d, basin %d.",
						p->zoneID, p->hillslopeID, p->basinID);
				return false;
			}
			break;
		case PATCH_TYPE_PATCH:
			if (verbose) {
				fprintf(stderr, "\tbasinID: %d, hillslopeID: %d, zoneID: %d, patchID: %d\n",
						p->basinID, p->hillslopeID, p->zoneID, p->patchID);
			}
			b = find_basin(p->basinID, w);
			if (b == NULL) {
				fprintf(stderr, "init_spatial_hierarchy_patch: basin %d could not be found, so could not locate patch %d in hillslope %d, patch %d.",
						p->basinID, p->patchID, p->hillslopeID, p->zoneID);
				return false;
			}
			p->patch = find_patch(p->patchID, p->zoneID, p->hillslopeID, b);
			if (p->patch == NULL) {
				fprintf(stderr, "init_spatial_hierarchy_patch: patch %d could not be found in zone %d, hillslope %d, basin %d.",
						p->patchID, p->zoneID, p->hillslopeID, p->basinID);
				return false;
			}
			break;
		}
		p = p->next;
	}

	if (verbose) fprintf(stderr, "END init_spatial_hierarchy_patch\n");

	return true;
}

static bool init_spatial_hierarchy_stratum(OutputFilter *f,
		struct world_object * const w,
		bool verbose) {

	struct basin_object *b;

	if (f->strata == NULL) {
		fprintf(stderr, "init_spatial_hierarchy_stratum: no strata defined but stratum type was specified.");
		return false;
	}

	if (verbose) fprintf(stderr, "BEGIN init_spatial_hierarchy_strata\n");

	OutputFilterStratum *s = f->strata;
	while (s != NULL) {
		switch (s->output_stratum_type) {
		case STRATUM_TYPE_BASIN:
			if (verbose) {
				fprintf(stderr, "\tbasinID: %d\n", s->basinID);
			}
			s->basin = find_basin(s->basinID, w);
			if (s->basin == NULL) {
				fprintf(stderr, "init_spatial_hierarchy_stratum: no basin with ID %d could be found.",
						s->basinID);
				return false;
			}
			break;
		case STRATUM_TYPE_HILLSLOPE:
			if (verbose) {
				fprintf(stderr, "\tbasinID: %d, hillslopeID: %d\n",
						s->basinID, s->hillslopeID);
			}
			b = find_basin(s->basinID, w);
			if (b == NULL) {
				fprintf(stderr, "init_spatial_hierarchy_stratum: basin %d could not be found, so could not locate hillslope %d.",
						s->basinID, s->hillslopeID);
				return false;
			}
			s->hill = find_hillslope_in_basin(s->hillslopeID, b);
			if (s->hill == NULL) {
				fprintf(stderr, "init_spatial_hierarchy_stratum: hillslope %d could not be found in basin %d.",
						s->hillslopeID, s->basinID);
				return false;
			}
			break;
		case STRATUM_TYPE_ZONE:
			if (verbose) {
				fprintf(stderr, "\tbasinID: %d, hillslopeID: %d, zoneID: %d\n",
						s->basinID, s->hillslopeID, s->zoneID);
			}
			b = find_basin(s->basinID, w);
			if (b == NULL) {
				fprintf(stderr, "init_spatial_hierarchy_stratum: basin %d could not be found, so could not locate zone %d in hillslope %d.",
						s->basinID, s->zoneID, s->hillslopeID);
				return false;
			}
			struct hillslope_object *h = find_hillslope_in_basin(s->hillslopeID, b);
			if (h == NULL) {
				fprintf(stderr, "init_spatial_hierarchy_stratum: hillslope %d could not be found in basin %d, so could not locate zone %d.",
						s->hillslopeID, s->basinID, s->zoneID);
				return false;
			}
			s->zone = find_zone_in_hillslope(s->zoneID, h);
			if (s->zone == NULL) {
				fprintf(stderr, "init_spatial_hierarchy_stratum: zone %d could not be found in hillslope %d, basin %d.",
						s->zoneID, s->hillslopeID, s->basinID);
				return false;
			}
			break;
		case STRATUM_TYPE_PATCH:
			if (verbose) {
				fprintf(stderr, "\tbasinID: %d, hillslopeID: %d, zoneID: %d, patchID: %d\n",
						s->basinID, s->hillslopeID, s->zoneID, s->patchID);
			}
			b = find_basin(s->basinID, w);
			if (b == NULL) {
				fprintf(stderr, "init_spatial_hierarchy_stratum: basin %d could not be found, so could not locate patch %d in hillslope %d, patch %d.",
						s->basinID, s->patchID, s->hillslopeID, s->zoneID);
				return false;
			}
			s->patch = find_patch(s->patchID, s->zoneID, s->hillslopeID, b);
			if (s->patch == NULL) {
				fprintf(stderr, "init_spatial_hierarchy_stratum: patch %d could not be found in zone %d, hillslope %d, basin %d.",
						s->patchID, s->zoneID, s->hillslopeID, s->basinID);
				return false;
			}
			break;
		case STRATUM_TYPE_STRATUM:
			if (verbose) {
				fprintf(stderr, "\tbasinID: %d, hillslopeID: %d, zoneID: %d, patchID: %d, stratumID: %d\n",
						s->basinID, s->hillslopeID, s->zoneID, s->patchID, s->stratumID);
			}
			s->stratum = find_stratum(s->stratumID, s->patchID, s->zoneID, s->hillslopeID, s->basinID, w);
			if (s->stratum == NULL) {
				fprintf(stderr, "init_spatial_hierarchy_stratum: stratum %d could not be found in patch %d, zone %d, hillslope %d, basin %d.",
						s->stratumID, s->patchID, s->zoneID, s->hillslopeID, s->basinID);
				return false;
			}
			break;
		}
		s = s->next;
	}

	if (verbose) fprintf(stderr, "END init_spatial_hierarchy_stratum\n");

	return true;
}

static bool init_spatial_hierarchy(OutputFilter *f,
		struct world_object * const w,
		bool verbose) {
	switch (f->type) {
	case OUTPUT_FILTER_PATCH:
		return init_spatial_hierarchy_patch(f, w, verbose);
	case OUTPUT_FILTER_CANOPY_STRATUM:
		return init_spatial_hierarchy_stratum(f, w, verbose);
	default:
		fprintf(stderr, "init_spatial_hierarchy: output filter type %d is unknown or not yet implemented.", f->type);
		return false;
	}
	return true;
}

static bool init_output(OutputFilter *f) {
	switch (f->output->format) {
	case OUTPUT_TYPE_CSV:
		return output_format_csv_init(f);
	case OUTPUT_TYPE_NETCDF:
	default:
		fprintf(stderr, "init_output: output format type %d is unknown or not yet implemented.", f->output->format);
		return false;
	}
	return true;
}

static bool write_headers(OutputFilter *f) {
	switch (f->output->format) {
	case OUTPUT_TYPE_CSV:
		return output_format_csv_write_headers(f);
	case OUTPUT_TYPE_NETCDF:
	default:
		fprintf(stderr, "write_headers: output format type %d is unknown or not yet implemented.", f->output->format);
		return false;
	}
	return true;
}

bool construct_output_filter(char * const error, size_t error_len,
		struct command_line_object * const cmd,
		struct world_object * const world) {
	if (!cmd->output_filter_flag) {
		return return_with_error(error, error_len, "output_filter_flag is fall, not constructing output filter.");
	}

	// Parse output filter
	OutputFilter *filters = parse(cmd->output_filter_filename);
	if (filters == NULL) {
		return return_with_error(error, error_len, "unable to parse output filter.");
	}
	if (filters->parse_error) {
		return return_with_error(error, error_len, "output_filter_parser returned with an error.");
	}

	StructIndex_t *idx = index_struct_fields();

	bool status;
	// Iterate over all filters and initialize them...
	for (OutputFilter *f = filters; f != NULL; f = f->next) {
		// Initialize spatial hierarchy objects referenced in filter
		status = init_spatial_hierarchy(f, world, cmd->verbose_flag);
		if (!status) {
			char *init_error = (char *)calloc(MAXSTR, sizeof(char));
			snprintf(init_error, MAXSTR, "unable to initialize spatial hierarchy for output filter with path %s and filename %s.",
					f->output->path, f->output->filename);
			return return_with_error(error, error_len, init_error);
		}

		// Validate variables and write offsets and data types to filter
		status = init_variables(f, idx, cmd->verbose_flag);
		if (!status) {
			char *init_error = (char *)calloc(MAXSTR, sizeof(char));
			snprintf(init_error, MAXSTR, "unable to initialize variables for output filter with path %s and filename %s.",
					f->output->path, f->output->filename);
			return return_with_error(error, error_len, init_error);
		}

		// Initialize output for filter
		if (f->output == NULL) {
			return return_with_error(error, error_len,
					"Output filter did not specify an output section.");
		}
		if (f->output->path == NULL) {
			return return_with_error(error, error_len,
					"Output filter did not specify output path.");
		}
		if (f->output->filename == NULL) {
			return return_with_error(error, error_len,
					"Output filter did not specify output filename.");
		}
		status = init_output(f);
		if (!status) {
			char *init_error = (char *)calloc(MAXSTR, sizeof(char));
			snprintf(init_error, MAXSTR, "unable to initialize output %s/%s for output filter.",
					f->output->path, f->output->filename);
			return return_with_error(error, error_len, init_error);
		}

		// Allocate array for num_named_variables materialized variables.
		// This will be used to temporarily store materialized variables before they are
		// output each time step.
		f->output->materialized_variables = calloc(f->num_named_variables, sizeof(MaterializedVariable));
		if (f->output->materialized_variables == NULL) {
			perror("construct_output_filter: Unable to allocate materialized variable array");
			return false;
		}

		// Write header information for each output file
		status = write_headers(f);
		if (!status) {
			char *init_error = (char *)calloc(MAXSTR, sizeof(char));
			snprintf(init_error, MAXSTR, "unable to write headers for output %s/%s.",
					f->output->path, f->output->filename);
			return return_with_error(error, error_len, init_error);
		}

		if (cmd->verbose_flag) print_output_filter(f);
	}

	free_struct_index(idx);
	cmd->output_filter = filters;
	return true;
}
