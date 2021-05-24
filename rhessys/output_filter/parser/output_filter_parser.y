%{
#include <stdio.h>
#include <string.h>

#include "output_filter_parser.tab.h"
#include "output_filter.h"
#include "strings.h"

void yyerror(char *s);
int yylex(void);
int set_input_file(char * const file_path);

// Control variables
bool verbose_output = false;

// State machine
bool in_filter = false;
bool in_output = false;
bool in_basin = false;
bool in_zone = false;
bool in_patch = false;
bool in_stratum = false;
bool syntax_error = false;

// Setup data structures for capturing state
OutputFilter *filters = NULL;
OutputFilter *curr_filter = NULL;

%}

%error-verbose

%union {
	char *string;
	int integer;
	struct of_var_expr_ast *ast;
        double d;
}

/* tokens */
/* keywords */
%token FILTER
%token OUTPUT
%token TIMESTEP
%token FORMAT
%token PATH
%token FILENAME
%token BASIN_TOK
%token ZONE_TOK
%token PATCH_TOK
%token STRATUM_TOK
%token IDS
%token VARS
/* output format components */
%token <string> PATH_SPEC
%token <string> FILENAME_SPEC
/* id_spec components */
%token <string> IDENTIFIER
%token <string> VAR_DEF
%token <string> LEVEL_HILLSLOPE
%token <string> LEVEL_ZONE
%token <string> LEVEL_PATCH
%token <string> LEVEL_STRATUM
%token <integer> NUMBER
%token <d> FLOAT
%token DELIM
%token COMMA
%token DOT
/* whitespace */
%token INDENT
%token EOL

%right '='
%left '+' '-'
%left '*' '/'
%nonassoc UMINUS

%type <ast> exp

%start filter_list

%%

filter_list: 
  | filter_list filter EOL { /* do nothing, allow filter to be evaluated below */ }
  | filter_list output EOL {}
  | filter_list timestep EOL {}
  | filter_list format EOL {}
  | filter_list path EOL {}
  | filter_list filename EOL {}
  | filter_list basin EOL {}
  | filter_list zone EOL {}
  | filter_list patch EOL {}
  | filter_list stratum EOL {}
  | filter_list ids {}
  | filter_list variables {}
  ;

filter: FILTER {
		// Update state machine
		in_filter = true;
		in_output = false;
		in_basin = false;
		in_zone = false;
		in_patch = false;
		in_stratum = false;
		
		curr_filter = create_new_output_filter();
		if (filters == NULL) {
			filters = curr_filter;
		} else {
			add_to_output_filter_list(filters, curr_filter);
		}
		
		if (verbose_output) fprintf(stderr, "BEGIN FILTER\n");
	}
	;

output: OUTPUT {
		if (!in_filter) {
			syntax_error = true;
			yyerror("output definition must be nested within filter definition");
		} else {
			in_output = true;
			in_basin = false;
			in_zone = false;
			in_patch = false;
			in_stratum = false;
			curr_filter->output = create_new_output_filter_output();
			if (verbose_output) fprintf(stderr, "\tBEGIN OUTPUT\n");
		}
	}
	;

timestep: TIMESTEP IDENTIFIER {
		if (!in_filter) {
			syntax_error = true;
			yyerror("timestep definition must be nested within filter definition");
		} else {
			if (strcmp($2, OUTPUT_TIMESTEP_HOURLY) == 0) {
				curr_filter->timestep = TIMESTEP_HOURLY;
				if (verbose_output) fprintf(stderr, "\tTIMESTEP IS: %s\n", $2);
			} else if (strcmp($2, OUTPUT_TIMESTEP_DAILY) == 0) {
				curr_filter->timestep = TIMESTEP_DAILY;
				if (verbose_output) fprintf(stderr, "\tTIMESTEP IS: %s\n", $2);
			} else if (strcmp($2, OUTPUT_TIMESTEP_MONTHLY) == 0) {
				curr_filter->timestep = TIMESTEP_MONTHLY;
				if (verbose_output) fprintf(stderr, "\tTIMESTEP IS: %s\n", $2);
			} else if (strcmp($2, OUTPUT_TIMESTEP_YEARLY) == 0) {
				curr_filter->timestep = TIMESTEP_YEARLY;
				if (verbose_output) fprintf(stderr, "\tTIMESTEP IS: %s\n", $2);
			} else {
				syntax_error = true;
				yyerror("unknown timestamp definition");
			}
		}
	}
	;

format: FORMAT IDENTIFIER {
		if (!in_output) {
			syntax_error = true;
			yyerror("format definition must be nested within output definition");
		} else {
			if (strcmp($2, OUTPUT_FORMAT_CSV) == 0) {
				curr_filter->output->format = OUTPUT_TYPE_CSV;
				if (verbose_output) fprintf(stderr, "\t\tOUTPUT FORMAT IS: %s\n", $2);
			} else if (strcmp($2, OUTPUT_FORMAT_NETCDF) == 0) {
				curr_filter->output->format = OUTPUT_TYPE_NETCDF;
				if (verbose_output) fprintf(stderr, "\t\tOUTPUT FORMAT IS: %s\n", $2);
			} else {
				syntax_error = true;
				yyerror("unkown format definition");
			}
		}
	}
	;

path: PATH PATH_SPEC {
		if (!in_output) {
			syntax_error = true;
			yyerror("path definition must be nested within output definition");
		} else {
			curr_filter->output->path = strip($2);
			if (verbose_output) fprintf(stderr, "\t\tOUTPUT PATH IS: %s\n", $2);
		}
	}
	;

filename: FILENAME FILENAME_SPEC {
		if (!in_output) {
			syntax_error = true;
			yyerror("filename definition must be nested within output definition");
		} else {
			curr_filter->output->filename = strip($2);
			if (verbose_output) fprintf(stderr, "\t\tOUTPUT FILENAME IS: %s\n", $2);
		}
	}
	;

basin: BASIN_TOK {
		if (!in_filter) {
			syntax_error = true;
			yyerror("basin definition must be nested within filter definition");
		} else if (curr_filter->patches != NULL || curr_filter->zones != NULL || curr_filter->strata != NULL) {
			syntax_error = true;
			yyerror("only one of basin, zone, patch, or stratum is allowed in a filter, but more than one was encountered.");
		} else if (curr_filter->basins != NULL) {
			syntax_error = true;
			yyerror("only one basin definition is allowed in a filter, but more than one was encountered.");
		} else if (curr_filter->type != OUTPUT_FILTER_UNDEFINED) {
		  	syntax_error = true;
		  	yyerror("current output filter already has a type assigned and can have only one type, so failing to re-assign type to basin.");
		} else {
			in_basin = true;
			in_patch = false;
			in_stratum = false;
			in_output = false;
			curr_filter->type = OUTPUT_FILTER_BASIN;
			if (verbose_output) fprintf(stderr, "\tBEGIN BASIN\n");
		}
	}
	;

zone: ZONE_TOK {
		if (!in_filter) {
			syntax_error = true;
			yyerror("zone definition must be nested within filter definition");
		} else if (curr_filter->basins != NULL || curr_filter->patches != NULL || curr_filter->strata != NULL) {
			syntax_error = true;
			yyerror("only one of basin, zone, patch, or stratum is allowed in a filter, but more than one was encountered.");
		} else if (curr_filter->zones != NULL) {
			syntax_error = true;
			yyerror("only one zone definition is allowed in a filter, but more than one was encountered.");
		} else if (curr_filter->type != OUTPUT_FILTER_UNDEFINED) {
		  	syntax_error = true;
		  	yyerror("current output filter already has a type assigned and can have only one type, so failing to re-assign type to zone.");
		} else {
			in_basin = false;
			in_zone = true;
			in_patch = false;
			in_stratum = false;
			in_output = false;
			curr_filter->type = OUTPUT_FILTER_ZONE;
			if (verbose_output) fprintf(stderr, "\tBEGIN ZONE\n");
		}
	}
	;

patch: PATCH_TOK {
		if (!in_filter) {
			syntax_error = true;
			yyerror("patch definition must be nested within filter definition");
		} else if (curr_filter->basins != NULL || curr_filter->zones != NULL || curr_filter->strata != NULL) {
			syntax_error = true;
			yyerror("only one of basin, zone, patch, or stratum is allowed in a filter, but more than one was encountered.");
		} else if (curr_filter->patches != NULL) {
			syntax_error = true;
			yyerror("only one patch definition is allowed in a filter, but more than one was encountered.");
		} else if (curr_filter->type != OUTPUT_FILTER_UNDEFINED) {
		  	syntax_error = true;
		  	yyerror("current output filter already has a type assigned and can have only one type, so failing to re-assign type to patch.");
		} else {
			in_basin = false;
			in_patch = true;
			in_stratum = false;
			in_output = false;
			curr_filter->type = OUTPUT_FILTER_PATCH;
			if (verbose_output) fprintf(stderr, "\tBEGIN PATCH\n");
		}
	}
	;

stratum: STRATUM_TOK {
		if (!in_filter) {
			syntax_error = true;
			yyerror("stratum definition must be nested within filter definition");
		} else if (curr_filter->basins != NULL || curr_filter->zones != NULL || curr_filter->patches != NULL) {
			syntax_error = true;
			yyerror("only one of basin, zone, patch, or stratum is allowed in a filter, but more than one was encountered.");
		} else if (curr_filter->strata != NULL) {
			syntax_error = true;
			yyerror("only one stratum definition is allowed in a filter, but more than one was encountered.");
		} else if (curr_filter->type != OUTPUT_FILTER_UNDEFINED) {
		  	syntax_error = true;
		  	yyerror("current output filter already has a type assigned and can have only one type, so failing to re-assign type to stratum.");
		} else {
			in_basin = false;
			in_stratum = true;
			in_patch = false;
			in_output = false;
			curr_filter->type = OUTPUT_FILTER_CANOPY_STRATUM;
			if (verbose_output) fprintf(stderr, "\tBEGIN STRATUM\n");
		}
	}
 	;

ids: IDS id_spec {
		if (!in_basin && !in_zone && !in_patch && !in_stratum) {
			syntax_error = true;
			yyerror("ids definition must be nested within basin, zone, patch, or stratum definition");
		} 
	}
	;

id_spec: | NUMBER {
		if (verbose_output) fprintf(stderr, "\t\tIDS: basinID: %d\n", $1);
		
		if (curr_filter->type == OUTPUT_FILTER_BASIN) {
			OutputFilterBasin *new_basin = create_new_output_filter_basin();
			new_basin->basinID = $1;
			
			if (curr_filter->basins == NULL) {
				curr_filter->basins = new_basin;
			} else {
				add_to_output_filter_basin_list(curr_filter->basins, new_basin);
			}
		} else if (curr_filter->type == OUTPUT_FILTER_ZONE) {
			OutputFilterZone *new_zone = create_new_output_filter_zone();
			new_zone->output_zone_type = ZONE_TYPE_BASIN;
			new_zone->basinID = $1;

			if (curr_filter->zones == NULL) {
				curr_filter->zones = new_zone;
			} else {
				add_to_output_filter_zone_list(curr_filter->zones, new_zone);
			}
		} else if (curr_filter->type == OUTPUT_FILTER_PATCH) {
			OutputFilterPatch *new_patch = create_new_output_filter_patch();
			new_patch->output_patch_type = PATCH_TYPE_BASIN;
			new_patch->basinID = $1;
			
			if (curr_filter->patches == NULL) {
				curr_filter->patches = new_patch;
			} else {
				add_to_output_filter_patch_list(curr_filter->patches, new_patch);
			}
		} else if (curr_filter->type == OUTPUT_FILTER_CANOPY_STRATUM) {
			OutputFilterStratum *new_stratum = create_new_output_filter_stratum();
			new_stratum->output_stratum_type = STRATUM_TYPE_BASIN;
			new_stratum->basinID = $1;
			
			if (curr_filter->strata == NULL) {
				curr_filter->strata = new_stratum;
			} else {
				add_to_output_filter_stratum_list(curr_filter->strata, new_stratum);
			}
		} else {
			syntax_error = true;
			yyerror("Filter type unkown but should not be.");
		}
	}
	| NUMBER DELIM NUMBER { 
		if (verbose_output) fprintf(stderr, "\t\tIDS: basinID: %d, hillID: %d\n", $1, $3); 
	
		if (curr_filter->type == OUTPUT_FILTER_BASIN) {
			syntax_error = true;
			yyerror("Hillslope ID specified for basin ID.");
		} else if (curr_filter->type == OUTPUT_FILTER_ZONE) {
			OutputFilterZone *new_zone = create_new_output_filter_zone();
			new_zone->output_zone_type = ZONE_TYPE_HILLSLOPE;
			new_zone->basinID = $1;
			new_zone->hillslopeID = $3;

			if (curr_filter->zones == NULL) {
				curr_filter->zones = new_zone;
			} else {
				add_to_output_filter_zone_list(curr_filter->zones, new_zone);
			}
		}else if (curr_filter->type == OUTPUT_FILTER_PATCH) {
			OutputFilterPatch *new_patch = create_new_output_filter_patch();
			new_patch->output_patch_type = PATCH_TYPE_HILLSLOPE;
			new_patch->basinID = $1;
			new_patch->hillslopeID = $3;
			
			if (curr_filter->patches == NULL) {
				curr_filter->patches = new_patch;
			} else {
				add_to_output_filter_patch_list(curr_filter->patches, new_patch);
			}
		} else if (curr_filter->type == OUTPUT_FILTER_CANOPY_STRATUM) {
			OutputFilterStratum *new_stratum = create_new_output_filter_stratum();
			new_stratum->output_stratum_type = STRATUM_TYPE_HILLSLOPE;
			new_stratum->basinID = $1;
			new_stratum->hillslopeID = $3;
			
			if (curr_filter->strata == NULL) {
				curr_filter->strata = new_stratum;
			} else {
				add_to_output_filter_stratum_list(curr_filter->strata, new_stratum);
			}
		} else {
			syntax_error = true;
			yyerror("Filter type unkown but should not be.");
		}
	}
	| NUMBER DELIM NUMBER DELIM NUMBER { 
		if (verbose_output) fprintf(stderr, "\t\tIDS: basinID: %d, hillID: %d, zoneID: %d\n", $1, $3, $5);
		
		if (curr_filter->type == OUTPUT_FILTER_BASIN) {
			syntax_error = true;
			yyerror("Zone ID specified for basin ID.");
		} else if (curr_filter->type == OUTPUT_FILTER_ZONE) {
                  	OutputFilterZone *new_zone = create_new_output_filter_zone();
			new_zone->output_zone_type = ZONE_TYPE_ZONE;
			new_zone->basinID = $1;
			new_zone->hillslopeID = $3;
			new_zone->zoneID = $5;

			if (curr_filter->zones == NULL) {
				curr_filter->zones = new_zone;
			} else {
				add_to_output_filter_zone_list(curr_filter->zones, new_zone);
			}
                } else if (curr_filter->type == OUTPUT_FILTER_PATCH) {
			OutputFilterPatch *new_patch = create_new_output_filter_patch();
			new_patch->output_patch_type = PATCH_TYPE_ZONE;
			new_patch->basinID = $1;
			new_patch->hillslopeID = $3;
			new_patch->zoneID = $5;
			
			if (curr_filter->patches == NULL) {
				curr_filter->patches = new_patch;
			} else {
				add_to_output_filter_patch_list(curr_filter->patches, new_patch);
			}
		} else if (curr_filter->type == OUTPUT_FILTER_CANOPY_STRATUM) {
			OutputFilterStratum *new_stratum = create_new_output_filter_stratum();
			new_stratum->output_stratum_type = STRATUM_TYPE_ZONE;
			new_stratum->basinID = $1;
			new_stratum->hillslopeID = $3;
			new_stratum->zoneID = $5;
			
			if (curr_filter->strata == NULL) {
				curr_filter->strata = new_stratum;
			} else {
				add_to_output_filter_stratum_list(curr_filter->strata, new_stratum);
			}
		} else {
			syntax_error = true;
			yyerror("Filter type unkown but should not be.");
		}
	}
	| NUMBER DELIM NUMBER DELIM NUMBER DELIM NUMBER { 
		if (verbose_output) fprintf(stderr, "\t\tIDS: basinID: %d, hillID: %d, zoneID: %d, patchID: %d\n", $1, $3, $5, $7); 
	
		if (curr_filter->type == OUTPUT_FILTER_BASIN) {
			syntax_error = true;
			yyerror("Patch-level ID specified for basin ID.");
		} else if (curr_filter->type == OUTPUT_FILTER_ZONE) {
                  	syntax_error = true;
                        yyerror("Patch-level ID specified for zone ID.");
                } else if (curr_filter->type == OUTPUT_FILTER_PATCH) {
			OutputFilterPatch *new_patch = create_new_output_filter_patch();
			new_patch->output_patch_type = PATCH_TYPE_PATCH;
			new_patch->basinID = $1;
			new_patch->hillslopeID = $3;
			new_patch->zoneID = $5;
			new_patch->patchID = $7;
			
			if (curr_filter->patches == NULL) {
				curr_filter->patches = new_patch;
			} else {
				add_to_output_filter_patch_list(curr_filter->patches, new_patch);
			}
		} else if (curr_filter->type == OUTPUT_FILTER_CANOPY_STRATUM) {
			OutputFilterStratum *new_stratum = create_new_output_filter_stratum();
			new_stratum->output_stratum_type = STRATUM_TYPE_PATCH;
			new_stratum->basinID = $1;
			new_stratum->hillslopeID = $3;
			new_stratum->zoneID = $5;
			new_stratum->patchID = $7;
			
			if (curr_filter->strata == NULL) {
				curr_filter->strata = new_stratum;
			} else {
				add_to_output_filter_stratum_list(curr_filter->strata, new_stratum);
			}
		} else {
			syntax_error = true;
			yyerror("Filter type unkown but should not be.");
		}
	}
	| NUMBER DELIM NUMBER DELIM NUMBER DELIM NUMBER DELIM NUMBER { 
		if (verbose_output) fprintf(stderr, "\t\tIDS: basinID: %d, hillID: %d, zoneID: %d, patchID: %d, stratumID: %d\n", $1, $3, $5, $7, $9); 
	
		if (curr_filter->type == OUTPUT_FILTER_BASIN) {
			syntax_error = true;
			yyerror("Stratum-level ID specified for basin ID.");
		} else if (curr_filter->type == OUTPUT_FILTER_ZONE) {
			syntax_error = true;
			yyerror("Stratum-level ID specified for zone ID.");
		} else if (curr_filter->type == OUTPUT_FILTER_PATCH) {
			syntax_error = true;
			yyerror("Stratum ID specified by filter type is patch.");
		} else if (curr_filter->type == OUTPUT_FILTER_CANOPY_STRATUM) {
			OutputFilterStratum *new_stratum = create_new_output_filter_stratum();
			new_stratum->output_stratum_type = STRATUM_TYPE_STRATUM;
			new_stratum->basinID = $1;
			new_stratum->hillslopeID = $3;
			new_stratum->zoneID = $5;
			new_stratum->patchID = $7;
			new_stratum->stratumID = $9;
			
			if (curr_filter->strata == NULL) {
				curr_filter->strata = new_stratum;
			} else {
				add_to_output_filter_stratum_list(curr_filter->strata, new_stratum);
			}
		} else {
			syntax_error = true;
			yyerror("Filter type unkown but should not be.");
		}
	}
	| id_spec COMMA id_spec { /* do nothing, allow individual id_spec to be evaluated by above rules */ }
	| id_spec EOL id_spec { }
	;

variables: VARS variable_spec {
		if (!in_basin && !in_zone && !in_patch && !in_stratum) {
			syntax_error = true;
			yyerror("variables definition must be nested within basin, patch, or stratum definition");
		} 
	}
	;

variable_spec: | VAR_DEF exp {
		if (verbose_output) {
			fprintf(stderr, "\t\tVARIABLE name : %s, AST:\n", $1);
			print_of_expr_ast($2, 1);
		}

		HierarchyLevel level;
		if (in_basin) {
			syntax_error = true;
			yyerror("Variable in basin definitions can only be named variables, not expression variables.");
		} else if (in_patch) {
			level = OF_HIERARCHY_LEVEL_PATCH;
		} else if (in_stratum) {
			level = OF_HIERARCHY_LEVEL_STRATUM;
		} else if (in_zone) {
			level = OF_HIERARCHY_LEVEL_ZONE;
		}
		OutputFilterVariable *new_var = create_new_output_filter_expr_variable(level, $1, $2);

		if (curr_filter->variables == NULL) {
			curr_filter->variables = new_var;
		} else {
			add_to_output_filter_variable_list(curr_filter->variables, new_var);
		}

      	}
	| IDENTIFIER {
		if (verbose_output) fprintf(stderr, "\t\tVARIABLE: %s\n", $1);
		
		HierarchyLevel level;
		if (in_basin) {
			syntax_error = true;
			yyerror("Variable names in basin definitions must include hierarchy level (e.g. patch.foo).");
		} else if (in_patch) {
			level = OF_HIERARCHY_LEVEL_PATCH;
		} else if (in_stratum) {
			level = OF_HIERARCHY_LEVEL_STRATUM;
		} else if (in_zone) {
			level = OF_HIERARCHY_LEVEL_ZONE;
		}
		OutputFilterVariable *new_var = create_new_output_filter_variable(level, $1);
		
		if (curr_filter->variables == NULL) {
			curr_filter->variables = new_var;
		} else {
			add_to_output_filter_variable_list(curr_filter->variables, new_var);
		}
	}
	| IDENTIFIER DOT IDENTIFIER {
		if (verbose_output) fprintf(stderr, "\t\tVARIABLE: %s.%s\n", $1, $3);
		
		HierarchyLevel level;
		if (in_basin) {
			syntax_error = true;
			yyerror("Variable names in basin definitions must include hierarchy level (e.g. patch.foo).");
		} else if (in_patch) {
			level = OF_HIERARCHY_LEVEL_PATCH;
		} else if (in_stratum) {
			level = OF_HIERARCHY_LEVEL_STRATUM;
		} else if (in_zone) {
			level = OF_HIERARCHY_LEVEL_ZONE;
		}
		OutputFilterVariable *new_var = create_new_output_filter_sub_struct_variable(level, $1, $3);
		
		if (curr_filter->variables == NULL) {
			curr_filter->variables = new_var;
		} else {
			add_to_output_filter_variable_list(curr_filter->variables, new_var);
		}
	}
	| LEVEL_HILLSLOPE IDENTIFIER {
		if (verbose_output) fprintf(stderr, "\t\tVARIABLE: hill.%s\n", $2);
		
		OutputFilterVariable *new_var = create_new_output_filter_variable(OF_HIERARCHY_LEVEL_HILLSLOPE, $2);
		
		if (curr_filter->variables == NULL) {
			curr_filter->variables = new_var;
		} else {
			add_to_output_filter_variable_list(curr_filter->variables, new_var);
		}
	}
	| LEVEL_HILLSLOPE IDENTIFIER DOT IDENTIFIER {
		if (verbose_output) fprintf(stderr, "\t\tVARIABLE: hill.%s.%s\n", $2, $4);
		
		OutputFilterVariable *new_var = create_new_output_filter_sub_struct_variable(OF_HIERARCHY_LEVEL_HILLSLOPE, $2, $4);
		
		if (curr_filter->variables == NULL) {
			curr_filter->variables = new_var;
		} else {
			add_to_output_filter_variable_list(curr_filter->variables, new_var);
		}
	}
	| LEVEL_PATCH IDENTIFIER {
		if (verbose_output) fprintf(stderr, "\t\tVARIABLE: patch.%s\n", $2);
		
		OutputFilterVariable *new_var = create_new_output_filter_variable(OF_HIERARCHY_LEVEL_PATCH, $2);
		
		if (curr_filter->variables == NULL) {
			curr_filter->variables = new_var;
		} else {
			add_to_output_filter_variable_list(curr_filter->variables, new_var);
		}
	}
	| LEVEL_PATCH IDENTIFIER DOT IDENTIFIER {
		if (verbose_output) fprintf(stderr, "\t\tVARIABLE: patch.%s.%s\n", $2, $4);
		
		OutputFilterVariable *new_var = create_new_output_filter_sub_struct_variable(OF_HIERARCHY_LEVEL_PATCH, $2, $4);
		
		if (curr_filter->variables == NULL) {
			curr_filter->variables = new_var;
		} else {
			add_to_output_filter_variable_list(curr_filter->variables, new_var);
		}
	}
	| LEVEL_STRATUM IDENTIFIER {
		if (verbose_output) fprintf(stderr, "\t\tVARIABLE: stratum.%s\n", $2);
		
		OutputFilterVariable *new_var = create_new_output_filter_variable(OF_HIERARCHY_LEVEL_STRATUM, $2);
		
		if (curr_filter->variables == NULL) {
			curr_filter->variables = new_var;
		} else {
			add_to_output_filter_variable_list(curr_filter->variables, new_var);
		}
	}
	| LEVEL_STRATUM IDENTIFIER DOT IDENTIFIER {
		if (verbose_output) fprintf(stderr, "\t\tVARIABLE: stratum.%s.%s\n", $2, $4);
		
		OutputFilterVariable *new_var = create_new_output_filter_sub_struct_variable(OF_HIERARCHY_LEVEL_STRATUM, $2, $4);
		
		if (curr_filter->variables == NULL) {
			curr_filter->variables = new_var;
		} else {
			add_to_output_filter_variable_list(curr_filter->variables, new_var);
		}
	}
	| LEVEL_ZONE IDENTIFIER {
		if (verbose_output) fprintf(stderr, "\t\tVARIABLE: zone.%s\n", $2);

		OutputFilterVariable *new_var = create_new_output_filter_variable(OF_HIERARCHY_LEVEL_ZONE, $2);

		if (curr_filter->variables == NULL) {
			curr_filter->variables = new_var;
		} else {
			add_to_output_filter_variable_list(curr_filter->variables, new_var);
		}
	}
	| LEVEL_ZONE IDENTIFIER DOT IDENTIFIER {
		if (verbose_output) fprintf(stderr, "\t\tVARIABLE: zone.%s.%s\n", $2, $4);

		OutputFilterVariable *new_var = create_new_output_filter_sub_struct_variable(OF_HIERARCHY_LEVEL_ZONE, $2, $4);

		if (curr_filter->variables == NULL) {
			curr_filter->variables = new_var;
		} else {
			add_to_output_filter_variable_list(curr_filter->variables, new_var);
		}
	}
	| variable_spec COMMA variable_spec { /* do nothing, allow individual variable_spec to be evaluated by above rules */ }
	| variable_spec EOL variable_spec { }
	;

exp: exp '+' exp {
		if (verbose_output) fprintf(stderr, "\t\tEXPR OP: +\n");
		$$ = new_of_expr_ast('+', $1, $3);
	}
	| exp '-' exp {
		if (verbose_output) fprintf(stderr, "\t\tEXPR OP: %s - %s\n", $1, $3);
		$$ = new_of_expr_ast('-', $1, $3);
	}
	| exp '*' exp { $$ = new_of_expr_ast('*', $1, $3); }
        | exp '/' exp { $$ = new_of_expr_ast('/', $1, $3); }
        | '(' exp ')' { $$ = $2; }
        | '-' exp %prec UMINUS { $$ = new_of_expr_ast('M', $2, NULL); }
        | FLOAT { $$ = new_of_expr_const($1); }
        | NUMBER { $$ = new_of_expr_const((double) $1); }
	| IDENTIFIER {
		if (verbose_output) fprintf(stderr, "\t\tEXPR IDENTIFIER: %s\n", $1);

		HierarchyLevel level;
		if (in_basin) {
			syntax_error = true;
			yyerror("Variable names in basin definitions must include hierarchy level (e.g. patch.foo).");
		} else if (in_patch) {
			level = OF_HIERARCHY_LEVEL_PATCH;
		} else if (in_stratum) {
			level = OF_HIERARCHY_LEVEL_STRATUM;
		}
		OutputFilterVariable *new_var = create_new_output_filter_variable(level, $1);

		$$ = (struct of_var_expr_ast *) new_of_expr_name(new_var);
	}
	| IDENTIFIER DOT IDENTIFIER {
		if (verbose_output) fprintf(stderr, "\t\tEXPR IDENTIFIER: %s.%s\n", $1, $3);

		HierarchyLevel level;
		if (in_basin) {
			syntax_error = true;
			yyerror("Variable names in basin definitions must include hierarchy level (e.g. patch.foo).");
		} else if (in_patch) {
			level = OF_HIERARCHY_LEVEL_PATCH;
		} else if (in_stratum) {
			level = OF_HIERARCHY_LEVEL_STRATUM;
		}
		OutputFilterVariable *new_var = create_new_output_filter_sub_struct_variable(level, $1, $3);

		$$ = (struct of_var_expr_ast *) new_of_expr_name(new_var);
	}
	;

%%

/**
  * Note: This is not a re-entrant parser. Do not call more than once in a given
  * process.
  */ 
OutputFilter *parse(char * const input, bool verbose) {
	verbose_output = verbose;
	int status = set_input_file(input);
	if (!status) {
		return NULL;
	} else {
		// Don't look at the return value from yyparse because a non-zero value will be
		// returned at end of file. Instead, rely on our own syntax_error flag...
	  yyparse();
	  filters->parse_error = syntax_error;
	  return filters;
	}
}

void yyerror(char *s) {
	extern char *yytext;
	if (*yytext == '\0') {
		// End of file, do not report the error.
	} else {
  	fprintf(stderr, "Error: %s, for token: |%s|\n", s, yytext);
  }
}
