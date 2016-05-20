/*--------------------------------------------------------------*/
/* 								*/
/*	construct_landuse_defaults				*/
/*								*/
/*	construct_landuse_defaults.c - makes patch default	*/
/*			objects.				*/
/*								*/
/*	NAME							*/
/*	construct_landuse_defaults.c - makes patch default	*/
/*			objects.				*/
/*								*/
/*	SYNOPSIS						*/
/*	struct patch_default *construct_landuse_defaults(       */
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
/*	Original code, January 15, 1996.			*/
/*	July 28, 1997	C.Tague					*/
/*	removed capillary rise landuse variables 		*/
/*	i.e rooting depth 					*/		
/*	pore size index and suction				*/
/*								*/
/*	Sep 15 97 RAF						*/
/*	added cap rise variables back as we now do a cap rise	*/
/*	routine but removed wilting point which was never used.	*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "rhessys.h"
#include "params.h"

struct landuse_default *construct_landuse_defaults(
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
	
	
	/*--------------------------------------------------------------*/
	/*	Local variable definition.				*/
	/*--------------------------------------------------------------*/
	int	i;
        int strbufLen = 256;
        int filenameLen = 1024;
	double 	landuse, ftmp;
	FILE	*default_file;
        char	strbuf[strbufLen];
        char	outFilename[filenameLen];
	char	record[MAXSTR];
	char	*newrecord;
	struct 	landuse_default *default_object_list;
        param *paramPtr = NULL;
        int paramCnt = 0;
	
	/*--------------------------------------------------------------*/
	/*	Allocate an array of default objects.			*/
	/*--------------------------------------------------------------*/
	default_object_list = (struct landuse_default *)
		alloc(num_default_files *
		sizeof(struct landuse_default),"default_object_list",
		"construct_landuse_defaults");
	
	/*--------------------------------------------------------------*/
	/*	Loop through the default files list.			*/
	/*--------------------------------------------------------------*/
	for (i=0 ; i<num_default_files; i++){
		/*--------------------------------------------------------------*/
		/*		Try to open the ith default file.		*/
		/*--------------------------------------------------------------*/
		printf("Reading %s\n", default_files[i]);
                paramCnt = 0;
                if (paramPtr != NULL)
                    free(paramPtr);

                paramPtr = readParamFile(&paramCnt, default_files[i]);

		/*--------------------------------------------------------------*/
		/*		read the ith default file into the ith object.	*/
		/*--------------------------------------------------------------*/

		default_object_list[i].ID = 			getIntParam(&paramCnt, &paramPtr, "landuse_default_ID", "%d", 1, 0);
		default_object_list[i].irrigation = 		getDoubleParam(&paramCnt, &paramPtr, "irrigation", "%lf", 0.0, 1) / 365;
		default_object_list[i].fertilizer_NO3 = 	getDoubleParam(&paramCnt, &paramPtr, "fertilizer_NO3", "%lf", 0.0, 1) / 365;
		default_object_list[i].fertilizer_NH4 = 	getDoubleParam(&paramCnt, &paramPtr, "fertilizer_NH4", "%lf", 0.0, 1) / 365;
		default_object_list[i].septic_NO3_load = 	getDoubleParam(&paramCnt, &paramPtr, "septic_NO3_load", "%lf", 0.0, 1) / 365;
		default_object_list[i].septic_water_load = 	getDoubleParam(&paramCnt, &paramPtr, "septic_water_load", "%lf", 0.0, 1) / 365;
		default_object_list[i].detention_store_size = 	getDoubleParam(&paramCnt, &paramPtr, "detention_store_size", "%lf", 0.0, 1);
		default_object_list[i].PH = 			getDoubleParam(&paramCnt, &paramPtr, "PH", "%lf", 7.0, 1);
		default_object_list[i].percent_impervious = 	getDoubleParam(&paramCnt, &paramPtr, "landuse.percent_impervious", "%lf", 0.0, 1);
		default_object_list[i].grazing_Closs = 	getDoubleParam(&paramCnt, &paramPtr, "grazing_Closs", "%lf", 0.0, 1) / 365;

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
            // and "_landuse.params"
            if (command_line[0].output_prefix != NULL) {
                strcat(outFilename, command_line[0].output_prefix);
                if (filename != NULL) {
                    strcat(outFilename, "_");
                    strcat(outFilename, filename);
                }
                strcat(outFilename, "_landuse.params");
            } 
            else {
                if (filename != NULL) {
                    strcat(outFilename, "_");
                    strcat(outFilename, filename);
                }
                strcat(outFilename, "landuse.params");
            }
    
                printParams(paramCnt, paramPtr, outFilename);
	} /*end for*/
        free(paramPtr);
	return(default_object_list);
} /*end construct_landuse_defaults*/
