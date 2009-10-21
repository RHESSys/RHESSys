#include "macaque.h"

#define MAXTEMPLATELINE 100
#define TEMPLATEFILENAME "template"
#define TEMPFILENAME "tmpg2m"
#define OUTFILENAME "worldfile"
#define MAXMAPNAME 50
#define MAXEXTENT 10 

/* The 't' prefix to these variable names stands for 'template'
   to distinguish them from the variables of similar name in
   the main macaque code.
*/


struct tableliststruct {
	struct tableliststruct *next;
	float value;
	int label;
	};

struct tvarliststruct {
  struct tvarliststruct *next;
  char varnum;
  int funcnum;
  float	mult[MAXEXTENT];
  float add[MAXEXTENT];
  float value[MAXEXTENT];
  char map[MAXEXTENT][MAXFILENAME];
  char map2[MAXEXTENT][MAXFILENAME];
  struct tableliststruct *table[MAXEXTENT];
  struct tableliststruct *table_ptr[MAXEXTENT];
}; 

struct tunitstruct {
  int lev;
  int numsubtunits;
  int label;
  struct tunitstruct *subtunit;
};


struct tlevelstruct {
  struct tvarliststruct *thetvarlist;
  char map[MAXFILENAME];
  int	extent;
  struct tableliststruct *table;
  struct tableliststruct *table_ptr;
};



