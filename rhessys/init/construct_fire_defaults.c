/*--------------------------------------------------------------*/
/* 																*/
/*					construct_fire_defaults				*/
/*																*/
/*	construct_fire_defaults.c - makes fire default	*/
/*										objects.				*/
/*																*/
/*	NAME														*/
/*	construct_fire_defaults.c - makes fire default	*/
/*										objects.				*/
/*																*/
/*	SYNOPSIS													*/
/*	struct fire_default *construct_fire_defaults(     */
/*								  num_default_files,			*/
/*								  default_files,				*/
/*								  grow_flag,					*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*	Original code, January 15, 1996.							*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "rhessys.h"
#include "params.h"

struct fire_default *construct_fire_defaults(
        int	num_default_files,
        char	**default_files,
        struct command_line_object *command_line)
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.									*/
	/*--------------------------------------------------------------*/
	void	*alloc( size_t, char *, char *);
	
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	int 	i;
        int strbufLen = 256;
        int filenameLen = 1024;
        int paramCnt = 0;
        char	strbuf[strbufLen];
        char	outFilename[filenameLen];
	double  ftmp, soil;
	FILE	*default_file;
	char	*newrecord;
	char	record[MAXSTR];
	struct	fire_default	*default_object_list;
        param *paramPtr = NULL;
	
	/*--------------------------------------------------------------*/
	/*	Allocate an array of default objects.						*/
	/*-------------------------------------------------------------*/
	default_object_list   = (struct fire_default *)
		alloc(num_default_files *
		sizeof(struct fire_default),"default_object_list",
		"construct_fire_defaults");
	
	/*--------------------------------------------------------------*/
	/*	Loop through the default files list.						*/
	/*--------------------------------------------------------------*/
	for (i=0 ; i<num_default_files; i++){
		/*--------------------------------------------------------------*/
		/*		Try to open the ith default file.						*/
		/*--------------------------------------------------------------*/
		printf("\n Reading %s\n", default_files[i]);
                paramCnt = 0;
                if (paramPtr != NULL)
                    free(paramPtr);

                paramPtr = readParamFile(&paramCnt, default_files[i]);

		/*--------------------------------------------------------------*/
		/*		read the ith default file into the ith object.			*/
		/*--------------------------------------------------------------*/
		default_object_list[i].ID = getIntParam(&paramCnt, &paramPtr, "fire_default_ID", "%d", 1, 1); // new param name
		/*--------------------------------------------------------------*/
		/*		assign parameters in  default and read the      */
                /*   optional parameter specification                           */
		/*--------------------------------------------------------------*/
		default_object_list[i].veg_fuel_weighting = getDoubleParam(&paramCnt, &paramPtr, "veg_fuel_weighting", "%lf", 0.2, 1);
		default_object_list[i].ndays_average = getDoubleParam(&paramCnt, &paramPtr, "ndays_average", "%lf", 30, 1);
		default_object_list[i].load_k1=getDoubleParam(&paramCnt, &paramPtr, "load_k1", "%lf", 0.8, 1);
  		default_object_list[i].load_k2=getDoubleParam(&paramCnt, &paramPtr, "load_k2", "%lf", 10, 1);
  		default_object_list[i].slope_k1=getDoubleParam(&paramCnt, &paramPtr, "slope_k1", "%lf", 0.8, 1);
		default_object_list[i].slope_k2=getDoubleParam(&paramCnt, &paramPtr, "slope_k2", "%lf", 0.25, 1);
		default_object_list[i].moisture_k1=getDoubleParam(&paramCnt, &paramPtr, "moisture_k1", "%lf", 0.8, 1);
		default_object_list[i].moisture_k2=getDoubleParam(&paramCnt, &paramPtr, "moisture_k2", "%lf", 0.15, 1);
		default_object_list[i].winddir_k1=getDoubleParam(&paramCnt, &paramPtr, "winddir_k1", "%lf", 0.15, 1);
 		default_object_list[i].winddir_k2=getDoubleParam(&paramCnt, &paramPtr, "winddir_k2", "%lf", 1, 1);
		default_object_list[i].windmax=getDoubleParam(&paramCnt, &paramPtr, "windmax", "%lf", 100, 1);
		default_object_list[i].ignition_col=getDoubleParam(&paramCnt, &paramPtr, "ignition_col", "%lf", -1, 1);
		default_object_list[i].ignition_row=getDoubleParam(&paramCnt, &paramPtr, "ignition_row", "%lf", -1, 1);
		default_object_list[i].ignition_tmin=getDoubleParam(&paramCnt, &paramPtr, "ignition_tmin", "%lf", 10, 1);
		default_object_list[i].fire_write=getDoubleParam(&paramCnt, &paramPtr, "fire_write", "%lf", 0, 1);
		default_object_list[i].fire_in_buffer=getDoubleParam(&paramCnt, &paramPtr, "fire_in_buffer", "%lf", 0, 1); 
		default_object_list[i].n_rows=getDoubleParam(&paramCnt, &paramPtr, "n_rows", "%lf", -1, 1);
		default_object_list[i].n_cols=getDoubleParam(&paramCnt, &paramPtr, "n_cols", "%lf", -1, 1);
		default_object_list[i].spread_calc_type=getDoubleParam(&paramCnt, &paramPtr, "spread_calc_type", "%lf", 1, 1);
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
                // and "_stratum.params"
                if (command_line[0].output_prefix != NULL) {
                    strcat(outFilename, command_line[0].output_prefix);
                    if (filename != NULL) {
                        strcat(outFilename, "_");
                        strcat(outFilename, filename);
                    }
                    strcat(outFilename, "_fire.params");
                } 
                else {
                    if (filename != NULL) {
                        strcat(outFilename, "_");
                        strcat(outFilename, filename);
                    }
                    strcat(outFilename, "fire.params");
                }
        
            printParams(paramCnt, paramPtr, outFilename);
	} /*end for*/
	return(default_object_list);
} /*end construct_fire_defaults*/
