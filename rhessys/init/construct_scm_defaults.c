/*--------------------------------------------------------------*/
/* 								*/
/*	construct_scm_defaults					*/
/*								*/
/*	construct_scm_defaults.c - makes patch default		*/
/*			objects.				*/
/*								*/
/*	NAME							*/
/*	construct_scm_defaults.c - makes patch default		*/
/*			objects.				*/
/*								*/
/*	SYNOPSIS						*/
/*	struct scm_default *construct_scm_defaults(           */
/*			num_default_files,			*/
/*			default_files,				*/
/*			grow_flag,				*/
/*			default_object_list )			*/
/*								*/
/*	OPTIONS							*/
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*	PROGRAMMER NOTES					*/
/*								*/
/*	Original code, May 1, 2014.			*/
/*	C.Bell					*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "rhessys.h"
#include "params.h"

struct scm_default *construct_scm_defaults(
			int	num_default_files,
			char	**default_files,
			struct command_line_object *command_line) 
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.				*/
	/*--------------------------------------------------------------*/
	void	*alloc(	size_t,
		char	*,
		char	*);
	
	//double compute_delta_water(int, double, double,	double, double, double);
	
	/*--------------------------------------------------------------*/
	/*	Local variable definition.				*/
	/*--------------------------------------------------------------*/
        int strbufLen = 256;
        int filenameLen = 1024;
	int	i;
	double 	ftmp,scm;
	FILE	*default_file;
        char	strbuf[strbufLen];
        char	outFilename[filenameLen];
	char	*newrecord;
	char	record[MAXSTR];
	struct 	scm_default *default_object_list;
	void	*alloc(	size_t, char *, char *);
        param *paramPtr = NULL;
        int paramCnt = 0;
    

	/*--------------------------------------------------------------*/
	/*	Allocate an array of default objects.			*/
	/*--------------------------------------------------------------*/
	default_object_list = (struct scm_default *)
		alloc(num_default_files * sizeof(struct scm_default),"default_object_list",
		"construct_scm_defaults");
	
	/*--------------------------------------------------------------*/
	/*	Loop through the default files list.			*/
	/*--------------------------------------------------------------*/
	//printf("\n number of scm default files is %d \n", num_default_files);
     for (i=0 ; i<num_default_files; i++){
    // printf("\n on scm def number %d \n", i+1);
		/*--------------------------------------------------------------*/
		/*		Try to open the ith default file.		*/
		/*--------------------------------------------------------------*/
		//if ( (default_file = fopen( default_files[i], "r")) == NULL ){
		//	fprintf(stderr,"FATAL ERROR:in construct_scm_defaults",
		//		"unable to open defaults file %d.\n",i);
		//	exit(EXIT_FAILURE);
		//} /*end if*/

		printf("Reading %s\n", default_files[i]);
                paramCnt = 0;
                if (paramPtr != NULL)
                    free(paramPtr);

                paramPtr = readParamFile(&paramCnt, default_files[i]);

		/*--------------------------------------------------------------*/
		/*		read the ith default file into the ith object.	*/
		/*--------------------------------------------------------------*/
		default_object_list[i].ID = 		   getIntParam(&paramCnt, &paramPtr, "scm_default_ID", "%d", 1, 1);
		default_object_list[i].maxH = 	       getDoubleParam(&paramCnt, &paramPtr, "maxH", "%lf", 2.0, 1); //default is 2.0m
		default_object_list[i].LtoW = 		   getDoubleParam(&paramCnt, &paramPtr, "LtoW", "%lf", 3.0, 1); //default is 3:1 L:W
		default_object_list[i].SS = 		   getDoubleParam(&paramCnt, &paramPtr, "SS", "%lf", 5.0, 1);  //Default is 5:1 side slopes
		default_object_list[i].riser_L = 	   getDoubleParam(&paramCnt, &paramPtr, "riser_L", "%lf", 0.75, 1); //default length of riser is 0.75m
		default_object_list[i].riser_coef =    getDoubleParam(&paramCnt, &paramPtr, "riser_coef", "%lf", 0.057, 1); //default riser discharge C is 0.057
		default_object_list[i].riser_H =       getDoubleParam(&paramCnt, &paramPtr, "riser_H", "%lf", 1.4, 1); //default riser height is 1.4 m
		default_object_list[i].spillway_L =   getDoubleParam(&paramCnt, &paramPtr, "spillway_L", "%lf", 2.0, 1); //default spillway width is 2.0m
		default_object_list[i].spillway_coef = getDoubleParam(&paramCnt, &paramPtr, "spillway_coef", "%lf", 0.38, 1); //default spillway Cd is 0.38
		default_object_list[i].spillway_H =    getDoubleParam(&paramCnt, &paramPtr, "spillway_H", "%lf", 1.75, 1); //default spillway height is 1.75m
		default_object_list[i].orifice_n = 	   getIntParam(&paramCnt, &paramPtr, "orifice_n", "%d", 1, 1); // Default is one orifice
          default_object_list[i].orifice_D = 	   getDoubleParam(&paramCnt, &paramPtr, "orifice_D", "%lf", 0.20, 1); //Deafult is 20cm diameter (about 8")
		default_object_list[i].orifice_coef =  getDoubleParam(&paramCnt, &paramPtr, "orifice_coef", "%lf", 0.6, 1); //Default is 0.6
		default_object_list[i].orifice_H = 	   getDoubleParam(&paramCnt, &paramPtr, "orifice_H", "%lf", 1.0, 1); //default height of orifice is 1.0m
        
        /*--------------------------------------------------------------*/
		/* initialization of optional default file params	        	*/
		/*--------------------------------------------------------------*/
        default_object_list[i].orifice_D_2 = 	getDoubleParam(&paramCnt, &paramPtr, "orifice_D_2", "%lf", 0.20, 1); //Deafult is 20cm diameter (about 8")
		default_object_list[i].orifice_coef_2 = getDoubleParam(&paramCnt, &paramPtr, "orifice_coef_2", "%lf", 0.6, 1); //Default is 0.6
        default_object_list[i].orifice_H_2 = 	getDoubleParam(&paramCnt, &paramPtr, "orifice_H_2", "%lf", 1.1, 1); //default height of orifice is 1.1m
        default_object_list[i].orifice_D_3 = 	getDoubleParam(&paramCnt, &paramPtr, "orifice_D_3", "%lf", 0.20, 1); //Deafult is 20cm diameter (about 8")
		default_object_list[i].orifice_coef_3 = getDoubleParam(&paramCnt, &paramPtr, "orifice_coef_3", "%lf", 0.6, 1); //Default is 0.6
		default_object_list[i].orifice_H_3 = 	getDoubleParam(&paramCnt, &paramPtr, "orifice_H_3", "%lf", 1.2, 1); //default height of orifice is 1.2m
        default_object_list[i].orifice_D_4 = 	getDoubleParam(&paramCnt, &paramPtr, "orifice_D_4", "%lf", 0.20, 1); //Deafult is 20cm diameter (about 8")
		default_object_list[i].orifice_coef_4 = getDoubleParam(&paramCnt, &paramPtr, "orifice_coef_4", "%lf", 0.6, 1); //Default is 0.6
		default_object_list[i].orifice_H_4 = 	getDoubleParam(&paramCnt, &paramPtr, "orifice_H_4", "%lf", 1.3, 1); //default height of orifice is 1.2m
        
        //printf("\n default object list ID %d", default_object_list[i].ID);
		/*--------------------------------------------------------------*/
		/*		Close the ith default file.								*/
		/*--------------------------------------------------------------*/

                memset(strbuf, '\0', strbufLen);
                strcpy(strbuf, default_files[i]);
                char *s = strbuf;
                char *y = NULL;
                char *token = NULL;
                char filename[256];
    
                // Store filename portion of path in 't'
                while ((token = strtok(s, "/")) != NULL) {
                    // Save the latest component of the filename
                    strcpy(filename, token);
                    s = NULL;
                } 
    
                // Remove the file extension, if one exists
                memset(strbuf, '\0', strbufLen);
                strcpy(strbuf, filename);
                free(s);
                s = strbuf;
                token = strtok(s, ".");
                if (token != NULL) {
                    strcpy(filename, token);
                }
        
                memset(outFilename, '\0', filenameLen);
            

    
            // Concatenate the output prefix with the filename of the input .def file
            // and "_scm.params"
            if (command_line[0].output_prefix != NULL) {
                strcat(outFilename, command_line[0].output_prefix);
                if (filename != NULL) {
                    strcat(outFilename, "_");
                    strcat(outFilename, filename);
                }
                strcat(outFilename, "_scm.params");
            } 
            else {
                if (filename != NULL) {
                    strcat(outFilename, "_");
                    strcat(outFilename, filename);
                }
                strcat(outFilename, "scm.params");
            }
    
        printParams(paramCnt, paramPtr, outFilename);
	} /*end for*/

	return(default_object_list);
} /*end construct_scm_defaults*/
