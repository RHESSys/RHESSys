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
	int	i, k;
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
    double *orifice_D_local;
    double *orifice_coef_local;
    double *orifice_H_local;
    char D_name[18];
    char coef_name[18];
    char H_name[18];
    
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
		default_object_list[i].ID  		    =  getIntParam(&paramCnt, &paramPtr, "scm_default_ID", "%d", 1, 1);
        default_object_list[i].num_discrete    =  getIntParam(&paramCnt, &paramPtr, "num_discrete", "%d", 1000, 1);
		default_object_list[i].maxH  	         =  getDoubleParam(&paramCnt, &paramPtr, "maxH", "%lf", 2.0, 1); //default is 2.0m
		default_object_list[i].LtoW  	         =  getDoubleParam(&paramCnt, &paramPtr, "LtoW", "%lf", 3.0, 1); //default is 3:1 L:W
		default_object_list[i].SS  		    =  getDoubleParam(&paramCnt, &paramPtr, "SS", "%lf", 5.0, 1);  //Default is 5:1 side slopes
        default_object_list[i].infil_rate   =  getDoubleParam(&paramCnt, &paramPtr, "infil_rate", "%lf", 0.006096, 1);  //Default is 0.006096 m/d (0.01 in/hr)
		default_object_list[i].riser_L  	    =  getDoubleParam(&paramCnt, &paramPtr, "riser_L", "%lf", 1.0, 1); //default length/perimeter of riser is 1.0m
		default_object_list[i].riser_weir_coef =  getDoubleParam(&paramCnt, &paramPtr, "riser_weir_coef", "%lf", 3.0, 1); //default riser discharge C as a weir is 3.0
        default_object_list[i].riser_ori_coef  =  getDoubleParam(&paramCnt, &paramPtr, "riser_ori_coef", "%lf", 0.6, 1); //default riser discharge C as an orifice 0.6
		default_object_list[i].riser_H         = getDoubleParam(&paramCnt, &paramPtr, "riser_H", "%lf", 1.4, 1); //default riser height is 1.4 m
		default_object_list[i].spillway_L      = getDoubleParam(&paramCnt, &paramPtr, "spillway_L", "%lf", 2.0, 1); //default spillway width is 2.0m
		default_object_list[i].spillway_coef   = getDoubleParam(&paramCnt, &paramPtr, "spillway_coef", "%lf", 3.33, 1); //default spillway Cd is 3.33
		default_object_list[i].spillway_H      = getDoubleParam(&paramCnt, &paramPtr, "spillway_H", "%lf", 1.75, 1); //default spillway height is 1.75m
		default_object_list[i].orifice_n       = getIntParam(&paramCnt, &paramPtr, "orifice_n", "%d", 1, 1); // Default is one orifice

          if((default_object_list[i].orifice_n+2) >= default_object_list[i].num_discrete/2){
               fprintf(stderr,
			"FATAL ERROR: In construct_scm_defaults - scm default ID %d - the number of SCM discretizations must be > than 2 * number of (orifices + spillway + riser )\n",
			default_object_list[i].ID);
		exit(EXIT_FAILURE);
          }

        /*--------------------------------------------------------------*/
		/*	Read in params orifice_n orifices, all named orifice_PARAMNUM where number varies from 1 to orifice_n	*/
		/*--------------------------------------------------------------*/
         
        // Initialize pointers locally
        orifice_D_local = (double *) alloc((default_object_list[i].orifice_n ) * sizeof(double),"orifice_D","construct_scm_defaults");
        orifice_coef_local = (double *) alloc((default_object_list[i].orifice_n ) * sizeof(double),"orifice_coef","construct_scm_defaults");
        orifice_H_local = (double *) alloc((default_object_list[i].orifice_n ) * sizeof(double),"orifice_H","construct_scm_defaults");

        //  Loop thorugh file and read in parameters to local array/pointers
        for(k=0; k<(default_object_list[i].orifice_n); k++){
              snprintf(D_name, sizeof D_name, "%s%d","orifice_D_", (k+1));
              snprintf(coef_name, sizeof coef_name, "%s%d","orifice_coef_", (k+1));
              snprintf(H_name, sizeof H_name, "%s%d","orifice_H_", (k+1));
              orifice_D_local[k]    = getDoubleParam(&paramCnt, &paramPtr, D_name, "%lf", 0.20, 1); // Deafult is 20cm diameter (about 8")0           
              orifice_coef_local[k] = getDoubleParam(&paramCnt, &paramPtr, coef_name, "%lf", 0.6, 1); // Default is 0.6
              orifice_H_local[k]    = getDoubleParam(&paramCnt, &paramPtr, H_name, "%lf", 0.5*(k+1), 1); //default height is one every 0.5m
        }      

        // Assign local pointers to scm_defaults object
		default_object_list[i].orifice_D = orifice_D_local;
        default_object_list[i].orifice_coef = orifice_coef_local;
        default_object_list[i].orifice_H = orifice_H_local;
        		
		default_object_list[i].DON_settling_rate = getDoubleParam(&paramCnt, &paramPtr, "DON_settling_rate", "%lf", 0.15, 1); //default settling is 0.15 m/d
		default_object_list[i].DOC_settling_rate = getDoubleParam(&paramCnt, &paramPtr, "DOC_settling_rate", "%lf", 0.2, 1); //default settling is 0.2 m/d
        
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
