import os
import sys
import re
import fileinput
import argparse

TYPES = {
	'bool': 'DATA_TYPE_BOOL',
	'char *': 'DATA_TYPE_CHAR_ARRAY',
	'int': 'DATA_TYPE_INT',
	'long': 'DATA_TYPE_LONG',
	'long *': 'DATA_TYPE_LONG_ARRAY',
	'float': 'DATA_TYPE_FLOAT',
	'double': 'DATA_TYPE_DOUBLE',
	'double *': 'DATA_TYPE_DOUBLE_ARRAY',
	'FILE *': 'DATA_TYPE_FILE_PTR'
}

STRUCT_NAMES = [
	'patch_object',
	'accumulate_patch_object',
	'patch_hourly_object',
	'patch_fire_water_object',
	'rooting_zone_object',
	'snowpack_object',
	'soil_c_object',
	'soil_n_object',
	'litter_object',
	'litter_c_object',
	'litter_n_object',
	'cdayflux_patch_struct',
	'ndayflux_patch_struct',
	'canopy_strata_object',
	'cstate_struct',
	'accumulate_strata_object',
	'cdayflux_struct',
	'cstate_struct',
	'target_object',
	'epvar_struct',
	'nstate_struct',
	'ndayflux_struct',
	'phenology_struct',
	'fire_effects_object',
	'mult_conduct_struct',
	'hillslope_object',
	'gw_object',
	'zone_object',
	'accumulate_zone_object',
	'metvar_struct'
]

COMMENT_PATT = re.compile('^\s*//.*')
START_STRUCT_PATT = re.compile('^\s*struct\s+(\S+)\s*{\s*$')
STRUCT_KW_PATT = re.compile('^\s*struct\s+(\S+)\s*$')
OPEN_BRACE_PATT = re.compile('^\s*{\s*$')
END_STRUCT_PATT = re.compile('^\s*}\s*;\s*$')
STRUCT_MEM_PATT = re.compile('^\s*(\S+\s+\**)(\S+)\s*;\s*.*$')
STRUCT_MEM_STRUCT_PATT = re.compile('^\s*(struct\s+\S+\s+\**)(\S+)\s*;\s*.*$')

parser = argparse.ArgumentParser(description='Create C source for indexing structs in header files for dynamic lookup')
parser.add_argument('--headers', nargs='+', type=str, help='Header files to read structs from')
parser.add_argument('--include_structs', nargs='+', type=str, help='Names of structs to include in index.')
parser.add_argument('--output', type=str, help='Path of .c output file')

args = parser.parse_args()
print(args.headers)
print(args.output)

# Make sure headers exist
for f in args.headers:
	if not os.path.exists(f):
		sys.exit("dynamic_field_lookup indexer was unable to find header file: {0}".format(f))
	
# Open output
output = open(args.output, 'w')
output.write('''#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>

#include "types.h"
#include "rhessys.h"
#include "dictionary.h"
#include "index_struct_fields.h"

StructIndex_t *new_struct_index() {
	StructIndex_t *i = (StructIndex_t *) malloc(sizeof(StructIndex_t));
	assert(i);
	i->patch_object = NULL;
	i->accumulate_patch_object = NULL;
	i->patch_hourly_object = NULL;
	i->patch_fire_water_object = NULL;
	i->rooting_zone_object = NULL;
	i->snowpack_object = NULL;
	i->soil_c_object = NULL;
	i->soil_n_object = NULL;
	i->litter_object = NULL;
	i->litter_c_object = NULL;
	i->litter_n_object = NULL;
	i->cdayflux_patch_struct = NULL;
	i->ndayflux_patch_struct = NULL;
	i->canopy_strata_object = NULL;
	i->accumulate_strata_object = NULL;
	i->cdayflux_struct = NULL;
	i->cstate_struct = NULL;
	i->target_object = NULL;
	i->epvar_struct = NULL;
	i->nstate_struct = NULL;
	i->ndayflux_struct = NULL;
	i->phenology_struct = NULL;
	i->fire_effects_object = NULL;
	i->mult_conduct_struct = NULL;
	i->hillslope_object == NULL;
	i->gw_object == NULL;
	i->zone_object == NULL;
	i->accumulate_zone_object == NULL;
	i->metvar_struct == NULL;
	
	return i;
}

void free_struct_index(StructIndex_t *i) {
	if (i == NULL) return;
	if (i->patch_object) {
		freeDictionary(i->patch_object);
	}
	
	if (i->accumulate_patch_object) {
		freeDictionary(i->accumulate_patch_object);
	}
	if (i->patch_hourly_object) {
		freeDictionary(i->patch_hourly_object);
	}
	if (i->patch_fire_water_object) {
		freeDictionary(i->patch_fire_water_object);
	}
	if (i->rooting_zone_object) {
		freeDictionary(i->rooting_zone_object);
	}
	if (i->snowpack_object) {
		freeDictionary(i->snowpack_object);
	}
	if (i->soil_c_object) {
		freeDictionary(i->soil_c_object);
	}
	if (i->soil_n_object) {
		freeDictionary(i->soil_n_object);
	}
	if (i->litter_object) {
		freeDictionary(i->litter_object);
	}
	if (i->litter_c_object) {
		freeDictionary(i->litter_c_object);
	}
	if (i->litter_n_object) {
		freeDictionary(i->litter_n_object);
	}
	if (i->cdayflux_patch_struct) {
		freeDictionary(i->cdayflux_patch_struct);
	}
	if (i->ndayflux_patch_struct) {
		freeDictionary(i->ndayflux_patch_struct);
	}
	
	
	if (i->canopy_strata_object) {
		freeDictionary(i->canopy_strata_object);
	}
	
	if (i->accumulate_strata_object) {
		freeDictionary(i->accumulate_strata_object);
	}
	if (i->cdayflux_struct) {
		freeDictionary(i->cdayflux_struct);
	}
	if (i->cstate_struct) {
		freeDictionary(i->cstate_struct);
	}
	if (i->target_object) {
		freeDictionary(i->target_object);
	}
	if (i->epvar_struct) {
		freeDictionary(i->epvar_struct);
	}
	if (i->nstate_struct) {
		freeDictionary(i->nstate_struct);
	}
	if (i->ndayflux_struct) {
		freeDictionary(i->ndayflux_struct);
	}
	if (i->phenology_struct) {
		freeDictionary(i->phenology_struct);
	}
	if (i->fire_effects_object) {
		freeDictionary(i->fire_effects_object);
	}
	if (i->mult_conduct_struct) {
		freeDictionary(i->mult_conduct_struct);
	}
	
	if (i->hillslope_object) {
		freeDictionary(i->hillslope_object);
	}
	if (i->gw_object) {
		freeDictionary(i->gw_object);
	}
	
	if (i->zone_object) {
		freeDictionary(i->zone_object);
	}
	if (i->accumulate_zone_object) {
		freeDictionary(i->accumulate_zone_object);
	}
	if (i->metvar_struct) {
		freeDictionary(i->metvar_struct);
	}
	
	free(i);
}

StructIndex_t *index_struct_fields() {
	StructIndex_t *i = new_struct_index();
	i->patch_object = newDictionary(DICTIONARY_DEFAULT_SIZE);
	i->accumulate_patch_object = newDictionary(DICTIONARY_SIZE_MEDIUM);
	i->patch_hourly_object = newDictionary(DICTIONARY_SIZE_SMALL);
	
	i->patch_fire_water_object = newDictionary(DICTIONARY_SIZE_SMALL);
	i->rooting_zone_object = newDictionary(DICTIONARY_SIZE_SMALL);
	i->snowpack_object = newDictionary(DICTIONARY_SIZE_SMALL);
	i->soil_c_object = newDictionary(DICTIONARY_SIZE_SMALL);
	i->soil_n_object = newDictionary(DICTIONARY_SIZE_MEDIUM);
	i->litter_object = newDictionary(DICTIONARY_SIZE_SMALL);
	i->litter_c_object = newDictionary(DICTIONARY_SIZE_SMALL);
	i->litter_n_object = newDictionary(DICTIONARY_SIZE_SMALL);
	i->cdayflux_patch_struct = newDictionary(DICTIONARY_SIZE_LARGE);
	i->ndayflux_patch_struct = newDictionary(DICTIONARY_SIZE_LARGE);
	
	
	i->canopy_strata_object = newDictionary(DICTIONARY_SIZE_MEDIUM);
	
	i->accumulate_strata_object = newDictionary(DICTIONARY_SIZE_SMALL);
	i->cdayflux_struct = newDictionary(DICTIONARY_SIZE_LARGE);
	i->cstate_struct = newDictionary(DICTIONARY_SIZE_MEDIUM);
	i->target_object = newDictionary(DICTIONARY_SIZE_SMALL);
	i->epvar_struct = newDictionary(DICTIONARY_SIZE_SMALL);
	i->nstate_struct = newDictionary(DICTIONARY_SIZE_SMALL);
	i->ndayflux_struct = newDictionary(DICTIONARY_SIZE_MEDIUM);
	i->phenology_struct = newDictionary(DICTIONARY_SIZE_SMALL);
	i->fire_effects_object = newDictionary(DICTIONARY_SIZE_SMALL);
	i->mult_conduct_struct = newDictionary(DICTIONARY_SIZE_SMALL);
	
	
	i->hillslope_object = newDictionary(DICTIONARY_SIZE_MEDIUM);
	
	i->gw_object = newDictionary(DICTIONARY_SIZE_SMALL);
	
	
	i->zone_object == newDictionary(DICTIONARY_SIZE_LARGE);
	i->accumulate_zone_object == newDictionary(DICTIONARY_SIZE_TINY);
	i->metvar_struct == newDictionary(DICTIONARY_SIZE_SMALL);
''')

in_struct = False
curr_struct = None
with fileinput.input(files=args.headers) as f:
	for line in f:
		l = line.strip()
# 		print(l)
		if COMMENT_PATT.match(l):
			# Comment, ingore line
			continue
		elif not in_struct:
			if curr_struct:
				# Struct keyword has been seen, but an open brace hasn't yet, check for open brace
				m = OPEN_BRACE_PATT.match(l)
				if m:
					in_struct = True
					print("\tBeginning of struct {0}...".format(curr_struct))
					continue
			# Find "struct foo {" on a single line
			m = START_STRUCT_PATT.match(l)
			if m:
				curr_struct = m.group(1)
				if curr_struct in args.include_structs:
					in_struct = True
					print("\tBeginning of struct {0}...".format(curr_struct))
				else:
					curr_struct = None
				continue
			# Find "struct foo" on a single line (i.e. open "{" is on a subsequent line)
			m = STRUCT_KW_PATT.match(l)
			if m:
				curr_struct = m.group(1)
				if curr_struct not in args.include_structs:
					curr_struct = None
# 				print("\tFound keyword struct {0}...".format(curr_struct))
				continue
		else:
			# We are in a struct, try to match ...
			# end of struct
			m = END_STRUCT_PATT.match(l)
			if m:
# 				print("\tEnd of struct {0}.".format(curr_struct))
				curr_struct = None
				in_struct = False
				continue
			# member name
			m = STRUCT_MEM_PATT.match(l)
			if m:
				data_type_all = m.group(1)
				data_type_components = data_type_all.split()
				data_type_str = ' '.join(data_type_components)
				data_type = 'DATA_TYPE_UNDEFINED'
				try:
					data_type = TYPES[data_type_str]
				except KeyError:
					pass
				member = m.group(2)
				print(f"\t\tMember {member} of type {data_type} encountered.")
				output.write(f'''
	dictionaryInsert(i->{curr_struct}, "{member}", (DictionaryValue_t) {{{data_type}, offsetof(struct {curr_struct}, {member}), NULL}});''')
			else:
				# Check for struct members that are structs
				m = STRUCT_MEM_STRUCT_PATT.match(l)
				if m:
					data_type_all = m.group(1)
					data_type_components = data_type_all.split()
					struct_name = data_type_components[1]
					# Determine data type
					data_type = 'DATA_TYPE_STRUCT'
					if data_type_components[-1] == '*':
						data_type = 'DATA_TYPE_STRUCT_PTR'
					elif data_type_components[-1] == '**':
						data_type = 'DATA_TYPE_STRUCT_PTR_ARRAY'
			
					member = m.group(2)
					print(f"\t\tMember struct {struct_name} {member} of type {data_type} encountered.")
					sub_struct_index = 'NULL'
					if struct_name in STRUCT_NAMES:
						sub_struct_index = f"i->{struct_name}"
					output.write(f'''
	dictionaryInsert(i->{curr_struct}, "{member}", (DictionaryValue_t) {{{data_type}, offsetof(struct {curr_struct}, {member}), {sub_struct_index}}});''')
output.write('''
	
	return i;
}
''')
output.close()
