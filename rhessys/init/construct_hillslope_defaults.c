/*--------------------------------------------------------------*/
/* 								*/
/*	construct_hillslope_defaults				*/
/*								*/
/*	construct_hillslope_defaults.c - makes hillslope default*/
/*			objects.				*/
/*								*/
/*	NAME							*/
/*	construct_hillslope_defaults.c - makes hillslope default*/
/*			objects.				*/
/*								*/
/*	SYNOPSIS						*/
/*	struct hillslope_default *construct_hillslope_defaults( */
/*			num_default_files,			*/
/*			default_files,				*/
/*			command_line_object) 			*/
/*								*/
/*	OPTIONS							*/
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*	PROGRAMMER NOTES					*/
/*								*/
/*	March 4/97 - C. Tague					*/
/*	added soil parameters for baseflow routine		*/
/*								*/
/*	Original code, January 15, 1996.			*/
/*								*/
/*	Sep 15 97 - RAF						*/
/*	Removed soil parameters due to new baseflow routine	*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "rhessys.h"
#include "params.h"

struct hillslope_default *construct_hillslope_defaults(
	int	num_default_files,
	char	**default_files,
	struct command_line_object *command_line)
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.				*/
	/*--------------------------------------------------------------*/
	void	*alloc(	size_t, char *, char *);
	
	/*--------------------------------------------------------------*/
	/*	Local variable definition.				*/
	/*--------------------------------------------------------------*/
	int 	i;
        int strbufLen = 256;
        int filenameLen = 1024;
	FILE	*default_file;
        char	strbuf[strbufLen];
        char	outFilename[filenameLen];
	char	record[MAXSTR];
	char	*newrecord;
	double	ftmp;
	struct	hillslope_default	*default_object_list;
        param *paramPtr = NULL;
        int paramCnt = 0;
	
	/*-------------------------------------------------------------*/
	/*	Allocate an array of default objects.		       */
	/*-------------------------------------------------------------*/
	default_object_list   = (struct hillslope_default *)
		alloc(num_default_files * sizeof(struct hillslope_default),
		"default_object_list", "construct_hillslope_defaults");
	
	/*--------------------------------------------------------------*/
	/*	Loop through the default files list.			*/
	/*--------------------------------------------------------------*/
	for (i=0 ; i<num_default_files; i++) {
		/*--------------------------------------------------------------*/
		/*		read the ith default file into the ith object.	*/
		/*--------------------------------------------------------------*/
		printf("Reading %s\n", default_files[i]);
                paramCnt = 0;
                if (paramPtr != NULL)
                    free(paramPtr);

                paramPtr = readParamFile(&paramCnt, default_files[i]);

		default_object_list[i].ID = getIntParam(&paramCnt, &paramPtr, "hillslope_default_ID", "%d", 1, 1);

		/*--------------------------------------------------------------*/
		/* 	read in optional paramters				*/
		/*--------------------------------------------------------------*/
		default_object_list[i].gw_loss_coeff = 		getDoubleParam(&paramCnt, &paramPtr, "gw_loss_coeff", "%lf", 1.0, 1);
		default_object_list[i].n_routing_timesteps = 	getIntParam(&paramCnt, &paramPtr, "n_routing_timesteps", "%d", 24, 1);
		default_object_list[i].gw_loss_fast_threshold = 	getDoubleParam(&paramCnt, &paramPtr, "gw_loss_fast_threshold", "%lf", -1.0, 1);
		default_object_list[i].gw_loss_fast_coeff = 	getDoubleParam(&paramCnt, &paramPtr, "gw_loss_fast_coeff", "%lf", 3.0*default_object_list[i].gw_loss_coeff, 1);

		if (default_object_list[i].n_routing_timesteps < 1)
			default_object_list[i].n_routing_timesteps = 1;
		
		if (command_line[0].gw_flag > 0) {
			default_object_list[i].gw_loss_coeff *= command_line[0].gw_loss_coeff_mult;
			default_object_list[i].gw_loss_fast_coeff *= command_line[0].gw_loss_coeff_mult;
			}

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
            // and "_hillslope.params"
            if (command_line[0].output_prefix != NULL) {
                strcat(outFilename, command_line[0].output_prefix);
                if (filename != NULL) {
                    strcat(outFilename, "_");
                    strcat(outFilename, filename);
                }
                strcat(outFilename, "_hillslope.params");
            } 
            else {
                if (filename != NULL) {
                    strcat(outFilename, "_");
                    strcat(outFilename, filename);
                }
                strcat(outFilename, "hillslope.params");
            }
    
                printParams(paramCnt, paramPtr, outFilename);
	} /*end for*/

                if (paramPtr != NULL)
                    free(paramPtr);
		    
	return(default_object_list);
} /*end construct_hillslope_defaults*/
