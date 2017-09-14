/*--------------------------------------------------------------*/
/* 																*/
/*					construct_surface_energy_defaults				*/
/*																*/
/*	construct_surface_energy_defaults.c - makes surface_energy default	*/
/*										objects.				*/
/*																*/
/*	NAME														*/
/*	construct_surface_energy_defaults.c - makes surface_energy default	*/
/*										objects.				*/
/*																*/
/*	SYNOPSIS													*/
/*	struct surface_energy_default *construct_surface_energy_defaults(     */
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
struct surface_energy_default *construct_surface_energy_defaults(
    int	num_default_files,
    char	**default_files,
    int grow_flag,
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
	double  ftmp, soil;
	FILE	*default_file;
	char	*newrecord;
        char	outFilename[filenameLen];
        char	strbuf[strbufLen];
	char	record[MAXSTR];
	struct	surface_energy_default	*default_object_list;
        param *paramPtr = NULL;
        int paramCnt = 0;
	
	/*--------------------------------------------------------------*/
	/*	Allocate an array of default objects.						*/
	/*-------------------------------------------------------------*/
	default_object_list   = (struct surface_energy_default *)
		alloc(num_default_files *
		sizeof(struct surface_energy_default),"default_object_list",
		"construct_surface_energy_defaults");
	
	/*--------------------------------------------------------------*/
	/*	Loop through the default files list.						*/
	/*--------------------------------------------------------------*/
	for (i=0 ; i<num_default_files; i++) {
		/*--------------------------------------------------------------*/
		/*		Try to open the ith default file.						*/
		/*--------------------------------------------------------------*/
                paramCnt = 0;
                printf("Reading %s\n", default_files[i]);
                if (paramPtr != NULL)
                    free(paramPtr);

                paramPtr = readParamFile(&paramCnt, default_files[i]);

		/*--------------------------------------------------------------*/
		/*		read the ith default file into the ith object.			*/
		/*--------------------------------------------------------------*/
		default_object_list[i].ID = getIntParam(&paramCnt, &paramPtr, "surface_energy_ID", "%d", 1, 1);
		/*--------------------------------------------------------------*/
		/*		assign parameters in  default */
		/*--------------------------------------------------------------*/
		default_object_list[i].N_thermal_nodes     = getIntParam(&paramCnt, &paramPtr, "N_thermal_nodes", "%d", 10, 1);
		default_object_list[i].exp_dist            = getIntParam(&paramCnt, &paramPtr, "exp_dist", "%d", 0, 1);
		default_object_list[i].damping_depth       = getDoubleParam(&paramCnt, &paramPtr, "damping_depth", "%d", 4, 1);
		default_object_list[i].iteration_threshold = getDoubleParam(&paramCnt, &paramPtr, "iteration_threshold", "%lf", 0.01, 1);

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
                    strcat(outFilename, "_surface_energy.params");
                } 
                else {
                    if (filename != NULL) {
                        strcat(outFilename, "_");
                        strcat(outFilename, filename);
                    }
                    strcat(outFilename, "surface_energy.params");
                }
    
                printParams(paramCnt, paramPtr, outFilename);

		/*--------------------------------------------------------------*/
		/*		Close the ith default file.								*/
		/*--------------------------------------------------------------*/
		fclose(default_file);
	} /*end for*/
	return(default_object_list);
} /*end construct_surface_energy_defaults*/
