/*-----------------------------------------------------------------------
@
@ 
@ NAME:                    FILEIO.C
@ 
@ VERSION:
@ 
@ 			1.0
@ 
@ PURPOSE:
@
@		contains all of the functions for handling file input
@		and output. 
@
@ revision: 6.0  29, April 2005
@ DEFINITIONS:
 
@ INCLUDE FILES:										*/

#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

#include "fileio.h"
#include "blender.h"
#include "glb.h"
#include "sub.h"
 
/*
@ CALLING SEQUENCE:
@ 
@ FUNCTIONS CALLED:
@ 
@
@ REMARKS:
@
@
@ REFERENCES:
@
@			Band, L.E. (1986) Topographic partition of watersheds with 
@				digital elevation models. Water Resources Research,
@				Vol. 22, no. 1, pp. 15-24.
@
@			Band, L.E.	(1989) A terrain Based watershed information system.
@				Hydrological Processes. Vol. 4, pp.151-162
@
@
@ COMPUTER:			O/S:				COMPILER:
@
@	SUN 3/60		SUN O/S 4.0 		Standard C compiler
@ 
@ PROPRIETARY NOTICE:
@
@ 
@ REVISION HISTORY:
@ 
@ 	DD-MMM-YY  VERSION:		NAME:		REASON:
@ 
-----------------------------------------------------------------------*/


/* define dynam. mem. alloc. function */

char    *W_malloc();


/*-------------------------------------------------------------------------
        output_char() - output a binary image, sizeof(char) using the
                        (row, col) coordinates maxr and maxc.

-------------------------------------------------------------------------*/

output_char(array, filename, maxr, maxc)
    char array[];
    int maxr, maxc;
    char      filename[];
    {
    FILE *out1,  *fopen();

    if ( (out1 = fopen(filename, "w")) == NULL)
        {
        printf("cannot open file :%s: for (char) output\n",filename);
        pause();
        }
    else
        {
        fwrite(array, sizeof(char), maxr*maxc, out1);
        fclose(out1);
        }
    }

/*-------------------------------------------------------------------------
        output_fptr() - output a binary image, sizeof(char*FPTR_SIZE) using the
                        (row, col) coordinates maxr and maxc.
 
-------------------------------------------------------------------------*/

output_fptr(array, filename, maxr, maxc)
    char array[];
	int maxr, maxc;
    char filename[];
    {
    FILE *out1,  *fopen();
 
    if ( (out1 = fopen(filename, "w")) == NULL)
        {
        printf("cannot open file :%s: for (char) output\n",filename);
       pause();  
        }
    else
        {
        fwrite(array, sizeof(char), maxr*maxc*FPTR_SIZE, out1);
        fclose(out1);
        }
    }



/*-------------------------------------------------------------------------
        output_short2() - output a binary image, sizeof(short) using the
                        (row, col) coordinates maxr and maxc.
		dynam alloc
-------------------------------------------------------------------------*/

output_short2(array, filename , maxr, maxc)
    short int array[];
	int maxr, maxc;
    char      filename[];
    {   
    FILE *out1,  *fopen();

    if ( (out1 = fopen(filename, "w")) == NULL)
        {
        printf("cannot open file :%s: for (short int) output\n",filename);
        pause();
        }
    else
        {
        fwrite(array, sizeof(short int), maxr*maxc, out1);
        fclose(out1);
        }
    } 


/*-------------------------------------------------------------------------
        output_float() - output a binary image, sizeof(float) using the
                        (row, col) coordinates maxr and maxc.
		dynam alloc
-------------------------------------------------------------------------*/

output_float(array, filename, maxr, maxc)
	int maxr, maxc;
    float  array[];
    char   filename[];
    {
    FILE   *out1,  *fopen();

    if ( (out1 = fopen(filename, "w")) == NULL)
        {
        printf("cannot open file :%s: for (float) output\n",filename);
        pause();
        }
    else 
        {
        fwrite(array, sizeof(float), maxr*maxc, out1);
        fclose(out1);
        }
    }    




/*-------------------------------------------------------------------------
        output_int2() - output a binary image, sizeof(int) using the
                        (row, col) coordinates maxr and maxc.
		for dynam alloc arrays 
-------------------------------------------------------------------------*/

output_int2(array, filename, maxr, maxc)
	int maxr, maxc;
    int    array[];
    char   filename[];
    {   
    FILE *out1,  *fopen();
   

    if ( (out1 = fopen(filename, "w")) == NULL)
        {
        printf("cannot open file :%s: for (int) output\n",filename);
        pause();
        }
    else
        {
        fwrite(array, sizeof(int), maxr*maxc, out1);
/*
        for (r = 0; r < maxr; r++)
            {
            for (c = 0; c < maxc; c++)
                {
                match[c] = array[r][c];
                }
            pmatch = &match[0];
            fwrite(pmatch, sizeof(int), maxc, out1);
            }
*/
        fclose(out1);
        }
    }    




/*-------------------------------------------------------------------------
        input_short2() - input a binary image, sizeof(short) using the
                        (row, col) coordinates maxr and maxc.
		(dynam. alloc.)
-------------------------------------------------------------------------*/
 
void input_short2(array, filename, maxr, maxc)
	int maxr, maxc;
    short int array[];
    char      filename[];
    {   
    FILE *in1;
 
 
    if ( (in1 = fopen(filename, "r")) == NULL)
        {
        printf("cannot open file %s\n",filename);
        pause();
        }
    else 
        {
	fread(array, sizeof(short int), maxc*maxr, in1);  

        fclose(in1);
        }
 
    return;
    }   

/*-------------------------------------------------------------------------
        input_ascii_int() - input an ascii image into an interger array using the
                        (row, col) coordinates maxr and maxc.
 
-------------------------------------------------------------------------*/
 
void input_ascii_int(array, filename, mc, mr, arc_flag)
    int    *array;
    int 	mc, mr, arc_flag;
    char   *filename;
    {
    FILE *in1, *fopen();
  int  r;
   int max; 

    max = 0;
	 


    if ( (in1 = fopen(filename, "r")) == NULL)
        {
        printf("cannot open file %s\n",filename);
		pause();
        }
    else 
        {
       
	/* skip header */
	if (arc_flag == 0)
		for (r=0; r < LEN_GRASSHEADER; r++)
			fscanf(in1,"%*s");
	else
		for (r=0; r < LEN_ARCHEADER; r++)
			fscanf(in1,"%*s");

	for (r=0; r < mr*mc; r++) {
		  fscanf(in1,"%d",&array[r]);
		  if (array[r] > max) 
					max = array[r];
		   }
		
		 printf("\n Max for %s is %d",filename,max);
         fclose(in1);
		
        }

    return;
    }
 

/*-------------------------------------------------------------------------
        input_ascii_sint() - input an ascii image into an interger array using the
                        (row, col) coordinates maxr and maxc.
 
-------------------------------------------------------------------------*/
 
void input_ascii_sint(array, filename, mc, mr, arc_flag)
    short int    *array;
    int 	mc, mr, arc_flag;
    char   *filename;
    {
    FILE *in1, *fopen();
	
  int  r;
  int value;
   short int max; 

    max = 0;
	 
    

    if ( (in1 = fopen(filename, "r")) == NULL)
        {
        printf("cannot open file %s\n",filename);
		pause();
        }
    else 
        {
      
	/* skip header */
	if (arc_flag == 0)
		for (r=0; r < LEN_GRASSHEADER; r++)
			fscanf(in1,"%*s");
	else
		for (r=0; r < LEN_ARCHEADER; r++)
			fscanf(in1,"%*s");

	/* read array */
	for (r=0; r < mr*mc; r++) {
		  fscanf(in1,"%d",&value);
		  array[r] = value;
		  if (array[r] > max) 
					max = array[r];
		   }
		
		 printf("\n Max for %s is %d",filename,max);
         fclose(in1);
		
        }

    return;
    }
 

/*-------------------------------------------------------------------------
       input_ascii_float() - input an ascii image into an float array using the
                        (row, col) coordinates maxr and maxc.
 
-------------------------------------------------------------------------*/
 
void input_ascii_float(float *array, char *filename, int mc, int mr, int arc_flag, float sc)
    

{
    FILE *in1, *fopen();
	
   int  r;
   float max; 


    max = 0;
	 


    if ( (in1 = fopen(filename, "r")) == NULL)
        {
        printf("cannot open file %s\n",filename);
		pause();
        }
    else 
        {
      
	/* skip header */
	if (arc_flag == 0)
		for (r=0; r < LEN_GRASSHEADER; r++)
			fscanf(in1,"%*s");
	else
		for (r=0; r < LEN_ARCHEADER; r++)
			fscanf(in1,"%*s");

	for (r=0; r < mr*mc; r++) {
		  fscanf(in1,"%f",&array[r]);
		  array[r] = (float)(array[r]*sc); 
		  if (array[r] > max) 
					max = array[r];
		   }
		
		 printf("\n Max for %s is %f",filename, max);
         fclose(in1);
		
        }

    return;
    }
 

/*-------------------------------------------------------------------------
       output_ascii_float() - output an ascii image into an float array using the
                        (row, col) coordinates maxr and maxc.
 
-------------------------------------------------------------------------*/
 
void output_ascii_float(float *array, char *filename, int mc, int mr)
    

{
    FILE *in1, *fopen();
	
   int  r;
   float max, new;

    max=0.0;
    if ( (in1 = fopen(filename, "w")) == NULL)
        {
        printf("cannot open file %s\n",filename);
		pause();
        }
    else 
        {
	for (r=0; r < mr*mc; r++) {
		  new = array[r];
		  if (new > max) max = new;
		  fprintf(in1,"%f ",array[r]);
		}

	printf("\n Max for ew horizon is %lf", max);
         fclose(in1);
		
        }

    return;
    }

void output_ascii_double(double *array, char *filename, int mc, int mr)
{
    FILE *in1, *fopen();
	
   int  r;
   double max, new;

    max=0.0;
    if ( (in1 = fopen(filename, "w")) == NULL)
        {
        printf("cannot open file %s\n",filename);
		pause();
        }
    else 
        {
	for (r=0; r < mr*mc; r++) {
		  new = array[r];
		  if (new > max) max = new;
		  fprintf(in1,"%f ", array[r]);
		}

	printf("\n Max for ew horizon is %lf", max);
         fclose(in1);
		
        }

    return;
}
    
/*-------------------------------------------------------------------------
        input_int() - input a binary image, sizeof(int) using the
                        (row, col) coordinates maxr and maxc.
 
-------------------------------------------------------------------------*/
 
void input_int2(array, filename, mc, mr)
    int    *array;
    int 	mc, mr;
    char   *filename;
    {
    FILE *in1, *fopen();
	
  int  r;
   int max; 
	int *array2;

	array2 = (int *)malloc(sizeof(int)*(mr*mc));

	printf(" Maxr %d Maxc %d \n", mr, mc);
    max = 0;
	 


    if ( (in1 = fopen(filename, "r")) == NULL)
        {
        printf("cannot open file %s\n",filename);
		pause();
        }
    else 
        {
       /* fread(array, sizeof(int), mc*mr, in1); */

	for (r=0; r < mr*mc; r++) {
		  fread(&(array2[r]),sizeof(int), 1, in1);
		  if (array2[r] > max) 
					max = array2[r];
		   }
		
		 printf("\n Final Max is %d",max);
         fclose(in1);
		
        }

    return;
    }
 
/*-------------------------------------------------------------------------
        max_int() - find coordinates of max value in a matrix sizeof(int)

-------------------------------------------------------------------------*/

int max_int( dat, maxr, maxc)
	int maxr, maxc;
    int        dat[];
    {
    int  r, c, tr, tc, tempmax = -10000;

    for (r = 0; r < maxr; ++r)
        {
        for (c = 0; c < maxc; ++c)
            {
            if (tempmax < dat[r*maxc + c])
				{
            	tempmax = dat[r*maxc + c];
				tr = r;
				tc = c;
				}
            }
        }

    startr = tr;
    startc = tc;

    return(tempmax);
    }


/*-------------------------------------------------------------------------
        input_float() - input a binary image, sizeof(float) using the
                        (row, col) coordinates maxr and maxc.
 
-------------------------------------------------------------------------*/
 
void input_float2(array, filename, mr, mc)
    float   *array;
    int		mc, mr;
    char    filename[];

    {
    FILE   *in1,  *fopen();
    int    r;
	float max;
 
	max = 0.0;
    if ( (in1 = fopen(filename, "r")) == NULL)
        {
        printf("cannot open file %s\n",filename);
       pause();
        }
    else 
        {
        fread(array, sizeof(float), mc*mr, in1);
		
        fclose(in1);
        }

	for (r=0; r<mc*mr; r++) {
			if (array[r] > max)
					max = array[r];
	}

	printf("\n Max Float %f", max);
    return;
    }
 



/*-------------------------------------------------------------------------
	input_short_ASCII() - input an ASCII image, into an array
			sizeof(short) using the (row, col) coordinates 
			maxr and maxc. 
  -------------------------------------------------------------------------*/

input_short_ASCII(array, filename, maxr, maxc)
	int maxr, maxc;
    short int array[][MAXCOL]; 
    char      filename[];
    {
    FILE *in1,  *fopen();
    int  r, c;
    short int elev;

    if ( (in1 = fopen(filename, "r")) == NULL)
        {
        printf("cannot open file %s\n",filename);
       pause();
        }
    else
        {
        for (r = 0; r < maxr; r++)
            {
            for (c = 0; c < maxc; c++)
                {
                fscanf(in1,"%d",&elev);
		array[r][c] = elev;
                }
	    }
        fclose(in1);
        }

   
    }


/*-------------------------------------------------------------------------
	input_header() - input information (row, col) from [root].header
  -------------------------------------------------------------------------*/

input_header(maxr, maxc, fndem, arc_flag)
	int *maxr, *maxc;
	int arc_flag;
	char *fndem;
    {
    FILE  *in1,  *fopen();
	char tmp[100];
	int i;

    if ( (in1 = fopen(fndem, "r")) == NULL)
        {
        fprintf(stderr,"cannot open file %s\n\n",fndem);
		header_help(maxr, maxc, fndem);
        }
    else
	{
		if (arc_flag == 0) {
		for (i=0; i< 9; i++)
			fscanf(in1,"%s",tmp);
		
		fscanf(in1,"%d %*s %d",maxr, maxc);
		}
	else
		fscanf(in1, "%*s %d %*s %d", maxc, maxr);
	
	fclose(in1);
	}

   
    }


/*-------------------------------------------------------------------------
	header_help() - inform user they need a [root].header file
  -------------------------------------------------------------------------*/

void header_help(maxr, maxc, fnhdr)
	int *maxr, *maxc;
	char* fnhdr;
    {
    FILE  *in1,  *fopen();

    fprintf(stderr,"\n\n I cannot open the .hdr file to retreive the row and\n");
    fprintf(stderr,  " column coordinates. Please enter them now, they will\n");
    fprintf(stderr,  " be stored in a new header file called %s.\n\n",fnhdr);
    fprintf(stderr,  " Coordinates are (row col) : ");
	scanf("%d %d",maxr, maxc);

    if ( (in1 = fopen(fnhdr, "w")) == NULL) 
        {
        fprintf(stderr,"I still cannot open file %s, exiting program...\n",fnhdr); 
        exit(1);  
	}
    else
	{
	fprintf(in1, "%6d %6d", *maxr, *maxc);
        fclose(in1);
	}

    return;
    }




/*-------------------------------------------------------------------------
	input_prompt() - input root filename, create full filenames
  -------------------------------------------------------------------------*/

void	input_prompt(maxr, maxc,filename, fndem,fnslope, fnK, fnflna, fnpatch,fnzone,fnhill,
			fnstream, fnroads, fnsewers, fnmpar,fnpartition,
			fntable,fnroot, fnehr, fnwhr, f_flag, sewer_flag, arc_flag)
	int *maxr, *maxc;
	char *filename;
	char *fndem, *fnK, *fnpatch, *fnzone, *fnhill, *fnstream, *fnroads, *fnmpar, *fnpartition,
		*fntable, *fnroot, *fnflna, *fnslope, *fnsewers, *fnehr, *fnwhr;
	int f_flag, arc_flag, sewer_flag;
	
    {


/* copy the root filename into the specific filenames */

   
    strcpy(fndem,filename);
    strcpy(fnslope,filename);
    strcpy(fnK,filename);
    strcpy(fnpatch,filename);
    strcpy(fnzone,filename);
    strcpy(fnhill,filename);
    strcpy(fnstream,filename);
    strcpy(fnroads,filename);
    strcpy(fnmpar,filename);
    strcpy(fnpartition,filename);
    strcpy(fntable,filename);
    strcpy(fnehr,filename);
    strcpy(fnwhr,filename);
    strcpy(fnroot,filename);	/* store root filename as global variable */

    
/* append '.' extensions to each filename (these should be generalized) */

  
    strcat(fndem,".dem");
    strcat(fnslope,".slope");
    strcat(fnK,".K");
    strcat(fnmpar,".m");
    strcat(fnpatch,".patch");
    strcat(fnzone,".zone");
    strcat(fnhill,".hill");
    strcat(fnstream,".stream");
    strcat(fnroads,".road");
    strcat(fntable,".table");
    strcat(fnwhr,".whr");
    strcat(fnehr,".ehr");

/* add flna image if requested */
	if (f_flag) {
		strcpy(fnflna, filename);
		strcat(fnflna, ".flna");
		}

	if (sewer_flag) {
		strcpy(fnsewers, filename);
		strcat(fnflna, ".sewer");
		}



    input_header(maxr, maxc, fndem, arc_flag);


    return;
    }


/* -------------------------------------------------------------
        pause() - stop program until user presses enter (return)
                        note: this should be in raw() mode for
                              ease of use (see <curses.h>)
   ------------------------------------------------------------- */

void pause()
    {  
    /* clear buffer first */

    while (getchar() != '\n')
        ;

    printf("\nPress enter to continue...");
    getchar();
    }


/*------------------------------------------------------------------
        copy one matrix (m1) to another (m2)  both sizeof(short int)
	dynam alloc
  ------------------------------------------------------------------*/

void copy_short_m2(m1, num,  m2, maxr, maxc)
	int maxr, maxc;
    short int m1[], num,  m2[];
    {   
    int r, c;

    if (num != 2)
        {
        printf("3 parameters, 'm1 2 m2'\n");
        pause();
        }

    for (r = 0; r < maxr; ++r)
        {
        for (c = 0; c < maxc; ++c)
            {
            m2[r*maxc + c] = m1[r*maxc + c];
            }
        }
    return;
    }    

/*------------------------------------------------------------------
        copy one matrix (m1) to another (m2)  both sizeof(int)
        dynam alloc
  ------------------------------------------------------------------*/

copy_int_m2(m1, num,  m2, maxr, maxc)
	int maxr, maxc;
    int m1[], num,  m2[];
    {
    int r, c;

    if (num != 2)
        {
        printf("3 parameters, 'm1 2 m2'\n");
        pause();
        }

    for (r = 0; r < maxr; ++r)
        {
        for (c = 0; c < maxc; ++c)
            {
            m2[r*maxc + c] = m1[r*maxc + c];
            }
        }
   
    }



/*--------------------------------------------------
        copy one matrix (m1) sizeof(int) to another
        (m2) sizeof(char)
	dynam alloc array
  -------------------------------------------------*/
 
copy_int_m_char2(m1, num,  m2, maxr, maxc)
	int maxr, maxc;
    int  m1[], num;
    char m2[];
    {
    int r, c;
     
    if (num != 2)
        {
        printf("3 parameters, 'm1 2 m2'\n");
        pause();
        }
 
    for (r = 0; r < maxr; ++r)
        {
        for (c = 0; c < maxc; ++c)
            {
            m2[r*maxc + c] = (char) m1[r*maxc + c];
            }
        }
  
    }



/*--------------------------------------------------
        copy one matrix (m1) sizeof(short) to another
        (m2) sizeof(char)
	dynam alloc
  -------------------------------------------------*/
 
copy_short_m_char2(m1, num,  m2, maxr, maxc)
	int maxr, maxc;
    short  m1[], num;
    char   m2[];
    {   
    int r, c;
 
    if (num != 2)
        {
        printf("3 parameters, 'm1 2 m2'\n");
        pause();
        }
 
    for (r = 0; r < maxr; ++r)
        {
        for (c = 0; c < maxc; ++c)
            {
            m2[r*maxc + c] = (char) m1[r*maxc + c];
            }
        }
  
    }   


 
 

/*___________________________________________________________

   W_malloc  from scott
   dynamic memory allocation
  ____________________________________________________________ */

char *W_malloc(n)
	int n;
    {
    char *buffer;

    if (buffer = malloc(n))
        {
        return buffer;
        }
    printf("**** Fatal error in assigning memory...halting process.\n");
    exit(100);
    }


init_int(array, maxr, maxc)
    int array[];
    int maxr, maxc;
    {
    int r, c;

    for (r = 0; r < maxr; r++)
        {
        for (c = 0; c < maxc; c++)
            {
            array[r*maxc + c] = 0;
            }
        }

    }


init_short(array, maxr, maxc)
    short array[];
    int maxr, maxc;
    {
    int r, c; 
 
    for (r = 0; r < maxr; r++)
        {
        for (c = 0; c < maxc; c++)
            {
            array[r*maxc + c] = 0;
            }
        }
    
    } 
 
init_float(array, maxr, maxc)
    float array[];
    int maxr, maxc;
    {
    int r, c;

    for (r = 0; r < maxr; r++)
        {
        for (c = 0; c < maxc; c++)
            {
            array[r*maxc + c] = 0.0;
            }
        }

    }



