/* This is a header file which is writtern by chu */ 

int funcnumber(char *funcname);

void msgnoloop(char *msg);

int varnumber( char *vname, char **VARNAME);

void space_output(FILE *outfile, int lev);
		
void output_varlist (FILE *outfile, struct tvarliststruct *thetvarlist, int	lev, int ext) ;

void tallyunit(	struct	tunitstruct *tunit,	struct	tlevelstruct	*tlevel );

void readheader(FILE *templatefile , struct headerstruct  *header );


void outputheader(FILE *templatefile, struct headerstruct *header);

void exportunit(struct tunitstruct *tunit,  FILE *outfile, 
				struct tlevelstruct *tlevel, char **VARNAME);


void error(char *msg);
