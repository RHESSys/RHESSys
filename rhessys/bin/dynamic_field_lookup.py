import os
import sys
import re
import fileinput
import argparse

COMMENT_PATT = re.compile('^\s*//.*')
START_STRUCT_PATT = re.compile('^\s*struct\s+(\S+)\s*{\s*$')
STRUCT_KW_PATT = re.compile('^\s*struct\s+(\S+)\s*$')
OPEN_BRACE_PATT = re.compile('^\s*{\s*$')
END_STRUCT_PATT = re.compile('^\s*};\s*$')
STRUCT_MEM_PATT = re.compile('^\s*\S+\s+\**(\S+)\s*;\s*.*$')
STRUCT_MEM_STRUCT_PATT = re.compile('^\s*struct\s+(\S+)\s+\**(\S+)\s*;\s*.*$')

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

#include "rhessys.h"
#include "dictionary.h"
#include "index_struct_fields.h"

StructIndex_t *newStructIndex() {
	StructIndex_t *i = (StructIndex_t *) malloc(sizeof(StructIndex_t));
	assert(i);
	i->patch_object = NULL;
	i->accumulate_patch_object = NULL;
	i->patch_hourly_object = NULL;
	return i;
}

void freeStructIndex(StructIndex_t *i) {
	if (i == NULL) return;
	if (i->patch_object != NULL) {
		freeDictionary(i->patch_object);
	}
	if (i->accumulate_patch_object != NULL) {
		freeDictionary(i->accumulate_patch_object);
	}
	if (i->patch_hourly_object != NULL) {
		freeDictionary(i->patch_hourly_object);
	}
	free(i);
}

StructIndex_t *index_struct_fields() {
	StructIndex_t *i = newStructIndex();
	i->patch_object = newDictionary(DICTIONARY_DEFAULT_SIZE);
	i->accumulate_patch_object = newDictionary(DICTIONARY_SIZE_MEDIUM);
	i->patch_hourly_object = newDictionary(DICTIONARY_SIZE_SMALL);
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
				member = m.group(1)
				print(f"\t\tMember {member} encountered.")
				output.write(f'''
	dictionaryInsert(i->{curr_struct}, "{member}", offsetof(struct {curr_struct}, {member}));''')
			else:
				# Check for struct members that are structs
				m = STRUCT_MEM_STRUCT_PATT.match(l)
				if m:
					struct_name = m.group(1)
					member = m.group(2)
					print(f"\t\tMember struct {struct_name} {member} encountered.")
					output.write(f'''
	dictionaryInsert(i->{curr_struct}, "{member}", offsetof(struct {curr_struct}, {member}));''')
output.write('''
	
	return i;
}
''')
output.close()