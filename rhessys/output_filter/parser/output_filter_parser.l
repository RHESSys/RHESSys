%{
#include <stdio.h>

#include "output_filter_parser.tab.h"

#define ERR_MESG_LEN 2048

%}

%%

"+" { return yytext[0]; }
"-" { return yytext[0]; }
"*" { return yytext[0]; }
"/" { return yytext[0]; }
"(" { return yytext[0]; }
")" { return yytext[0]; }
[0-9]+"."[0-9]*|"."[0-9]+ { yylval.d = atof(yytext); return FLOAT; }

^"filter:" { return FILTER; }
^([ ]{2}+|[\t]+)"timestep:" { return TIMESTEP; }
^([ ]{2}+|[\t]+)"output:" { return OUTPUT; }
^([ ]{4}+|[\t]{2}+)"format:" { return FORMAT; }
^([ ]{4}+|[\t]{2}+)"path:" { return PATH; }
^([ ]{4}+|[\t]{2}+)"filename:" { return FILENAME; }
^([ ]{2}+|[\t]+)"basin:" { return BASIN_TOK; }
^([ ]{2}+|[\t]+)"zone:" { return ZONE_TOK; }
^([ ]{2}+|[\t]+)"patch:" { return PATCH_TOK; }
^([ ]{2}+|[\t]+)"stratum:" { return STRATUM_TOK; }
^([ ]{4}+|[\t]{2}+)"ids:" { return IDS; }
^([ ]{4}+|[\t]{2}+)"variables:" { return VARS; }

[0-9]+ { yylval.integer = atoi(yytext); return NUMBER; }
[a-zA-Z_][a-zA-Z0-9_]* { yylval.string = strdup(yytext); return IDENTIFIER; }
[a-zA-Z_][a-zA-Z0-9_]*[ ]*= {
    size_t len = 0;
    // First look for ' ' to find the end of the variable name
    char *sep = strchr(yytext, ' ');
    if (sep == NULL) {
        // No space, so find '=', which must exist otherwise this pattern would not
        // have been matched.
        sep = strchr(yytext, '=');
    }
    len = (size_t) (sep - yytext);
    yylval.string = (char *) calloc(len + 1, sizeof(char));
    strncpy(yylval.string, yytext, len);
    return VAR_DEF;
}
\"[a-zA-Z0-9_\-\(\)\[\]\.]+\" { yylval.string = strdup(yytext); return FILENAME_SPEC; }
\"(\.{1,2}\/){0,1}(\/*[a-zA-Z0-9_\-\(\)\[\]])+\/*\" { yylval.string = strdup(yytext); return PATH_SPEC; }

"hill." { return LEVEL_HILLSLOPE; }
"zone." { return LEVEL_ZONE; }
"patch." { return LEVEL_PATCH; }
"stratum." { return LEVEL_STRATUM; }

":" { return DELIM; }
"," { return COMMA; }
"." { return DOT; }

[ \t] { /* ignore whitespace that is not at the start of a line */ }

\n { return EOL; }

<<EOF>> { yyterminate(); }

%%

/**
 * Use to set a string as input to a parser. 
 * Should be called before yyparse() is called.
 * After yyparse() returns, call end_scanner() to de-allocate the input.
 */
void set_input(char * const input) {
  yy_scan_string(input);
}

void end_scanner(void) {
  yy_delete_buffer(YY_CURRENT_BUFFER);
}

/**
 * Use to specify a file to use as input to the parse.
 * Should be called before yyparse() is called.
 */
int set_input_file(char * const file_path) {
	if (!(yyin = fopen(file_path, "r"))) {
		char *error_mesg = (char *) malloc(sizeof(char) * ERR_MESG_LEN);
		snprintf(error_mesg, ERR_MESG_LEN, "Unable to open filter file %s", file_path);
		perror(error_mesg);
		free(error_mesg);
		return 0;
	}
	return 1;
}
