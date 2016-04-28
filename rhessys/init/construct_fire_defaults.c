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
		printf("ID: %d\n",default_object_list[i].ID);
		/*--------------------------------------------------------------*/
		/*		assign parameters in  default and read the      */
                /*   optional parameter specification                           */
		/*--------------------------------------------------------------*/
		default_object_list[i].veg_fuel_weighting = getDoubleParam(&paramCnt, &paramPtr, "veg_fuel_weighting", "%lf", 0.2, 1);
		printf("veg_fuel_weighting: %lf\n",default_object_list[i].veg_fuel_weighting);
		default_object_list[i].ndays_average = getDoubleParam(&paramCnt, &paramPtr, "ndays_average", "%lf", 30, 1);
		printf("ndays_average: %lf\n",default_object_list[i].ndays_average);
		default_object_list[i].load_k1=getDoubleParam(&paramCnt, &paramPtr, "load_k1", "%lf", 0.8, 1);
  		printf("load_k1: %lf\n",default_object_list[i].load_k1);
		default_object_list[i].load_k2=getDoubleParam(&paramCnt, &paramPtr, "load_k2", "%lf", 10, 1);
		printf("load_k2: %lf\n",default_object_list[i].load_k2);
  		default_object_list[i].slope_k1=getDoubleParam(&paramCnt, &paramPtr, "slope_k1", "%lf", 0.8, 1);
		printf("slope_k1: %lf\n",default_object_list[i].slope_k1);
		default_object_list[i].slope_k2=getDoubleParam(&paramCnt, &paramPtr, "slope_k2", "%lf", 0.25, 1);
		printf("slope_k2: %lf\n",default_object_list[i].slope_k2);
		default_object_list[i].moisture_k1=getDoubleParam(&paramCnt, &paramPtr, "moisture_k1", "%lf", 0.8, 1);
		printf("moisture_k1: %lf\n",default_object_list[i].moisture_k1);
		default_object_list[i].moisture_k2=getDoubleParam(&paramCnt, &paramPtr, "moisture_k2", "%lf", 0.15, 1);
		printf("moisture_k2: %lf\n",default_object_list[i].moisture_k2);
		default_object_list[i].winddir_k1=getDoubleParam(&paramCnt, &paramPtr, "winddir_k1", "%lf", 0.15, 1);
		printf("winddir_k1: %lf\n",default_object_list[i].winddir_k1);
 		default_object_list[i].winddir_k2=getDoubleParam(&paramCnt, &paramPtr, "winddir_k2", "%lf", 1, 1);
		printf("winddir_k2: %lf\n",default_object_list[i].winddir_k2);
		default_object_list[i].windmax=getDoubleParam(&paramCnt, &paramPtr, "windmax", "%lf", 100, 1);
		printf("windmax: %lf\n",default_object_list[i].windmax);
		default_object_list[i].ignition_col=getDoubleParam(&paramCnt, &paramPtr, "ignition_col", "%lf", -1, 1);
		printf("ignition_col: %lf\n",default_object_list[i].ignition_col);
		default_object_list[i].ignition_row=getDoubleParam(&paramCnt, &paramPtr, "ignition_row", "%lf", -1, 1);
		printf("ignition_row: %lf\n",default_object_list[i].ignition_row);
		default_object_list[i].ignition_tmin=getDoubleParam(&paramCnt, &paramPtr, "ignition_tmin", "%lf", 10, 1);
		printf("ignition_tmin: %lf\n",default_object_list[i].ignition_tmin);
		default_object_list[i].fire_verbose=getIntParam(&paramCnt, &paramPtr, "fire_verbose", "%d", 0, 1);
		printf("fire_verbose: %d\n",default_object_list[i].fire_verbose);
		default_object_list[i].fire_write=getIntParam(&paramCnt, &paramPtr, "fire_write", "%d", 0, 1);
		printf("fire_write: %d\n",default_object_list[i].fire_write);
		default_object_list[i].fire_in_buffer=getIntParam(&paramCnt, &paramPtr, "fire_in_buffer", "%d", 0, 1); 
		printf("fire_in_buffer: %d\n",default_object_list[i].fire_in_buffer);
		default_object_list[i].n_rows=getIntParam(&paramCnt, &paramPtr, "n_rows", "%d", -1, 1);
		printf("n_rows: %d\n",default_object_list[i].n_rows);
		default_object_list[i].n_cols=getIntParam(&paramCnt, &paramPtr, "n_cols", "%d", -1, 1);
		printf("n_cols: %d\n",default_object_list[i].n_cols);
		default_object_list[i].spread_calc_type=getIntParam(&paramCnt, &paramPtr, "spread_calc_type", "%d", 1, 1);
		printf("spread_calc_type: %d\n",default_object_list[i].spread_calc_type);
		default_object_list[i].mean_log_wind=getDoubleParam(&paramCnt, &paramPtr, "mean_log_wind", "%lf", 1, 1);
		printf("mean_log_wind: %lf\n",default_object_list[i].mean_log_wind);
		default_object_list[i].sd_log_wind=getDoubleParam(&paramCnt, &paramPtr, "sd_log_wind", "%lf", 1, 1);
		printf("sd_log_wind: %lf\n",default_object_list[i].sd_log_wind);
		default_object_list[i].mean1_rvm=getDoubleParam(&paramCnt, &paramPtr, "mean1_rvm", "%lf", 1, 1);
		printf("mean1_rvm: %lf\n",default_object_list[i].mean1_rvm);
		default_object_list[i].mean2_rvm=getDoubleParam(&paramCnt, &paramPtr, "mean2_rvm", "%lf", 1, 1);
		printf("mean2_rvm: %lf\n",default_object_list[i].mean2_rvm);
		default_object_list[i].kappa1_rvm=getDoubleParam(&paramCnt, &paramPtr, "kappa1_rvm", "%lf", 1, 1);
		printf("kappa1_rvm: %lf\n",default_object_list[i].kappa1_rvm);
		default_object_list[i].kappa2_rvm=getDoubleParam(&paramCnt, &paramPtr, "kappa2_rvm", "%lf", 1, 1);
		printf("kappa2_rvm: %lf\n",default_object_list[i].kappa2_rvm);
		default_object_list[i].p_rvm=getDoubleParam(&paramCnt, &paramPtr, "p_rvm", "%lf", 1 , 1);
		printf("p_rvm: %lf\n",default_object_list[i].p_rvm);
		default_object_list[i].ign_def_mod=getDoubleParam(&paramCnt, &paramPtr, "ign_def_mod", "%lf", 1 , 1);
		printf("ign_def_mod: %lf\n",default_object_list[i].ign_def_mod);
		default_object_list[i].veg_k1=getDoubleParam(&paramCnt, &paramPtr, "veg_k1", "%lf", 0.8, 1);
 		default_object_list[i].veg_ign=getIntParam(&paramCnt, &paramPtr, "veg_ign", "%d", 0, 1);
		printf("veg_ign: %d\n",default_object_list[i].veg_ign);
  		printf("veg_k1: %lf\n",default_object_list[i].veg_k1);
		default_object_list[i].veg_k2=getDoubleParam(&paramCnt, &paramPtr, "veg_k2", "%lf", 10, 1);
		printf("veg_k2: %lf\n",default_object_list[i].veg_k2);
		default_object_list[i].mean_ign=getDoubleParam(&paramCnt, &paramPtr, "mean_ign", "%lf", 1, 1);
		printf("mean_ign: %lf\n",default_object_list[i].mean_ign);
		default_object_list[i].ran_seed=getIntParam(&paramCnt, &paramPtr, "ran_seed", "%d", 0, 1);
		printf("ran_seed: %d\n",default_object_list[i].ran_seed);

 
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
