/*--------------------------------------------------------------*/
/* 								*/
/*	construct_zone_defaults					*/
/*								*/
/*	construct_zone_defaults.c - makes zone default		*/
/*			objects.				*/
/*								*/
/*	NAME							*/
/*	construct_zone_defaults.c - makes zone default		*/
/*			objects.				*/
/*								*/
/*	SYNOPSIS						*/
/*	struct zone_default *construct_zone_defaults(           */
/*		num_default_files,				*/
/*		default_files,					*/
/*		command_line)					*/
/*								*/
/*	OPTIONS							*/
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*	PROGRAMMER NOTES					*/
/*								*/
/*	Original code, January 15, 1996.			*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "rhessys.h"
#include "params.h"

struct zone_default *construct_zone_defaults(
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
	int	i;
        int strbufLen = 256;
        int filenameLen = 1024;
	FILE	*default_file;
        char	outFilename[filenameLen];
        char	strbuf[strbufLen];
	char	record[MAXSTR];
	struct	zone_default	*default_object_list ;
	char	*newrecord;
	double	ftmp;
        param *paramPtr = NULL;
        int paramCnt = 0;
	
	/*--------------------------------------------------------------*/
	/*	Allocate an array of default objects.			*/
	/*--------------------------------------------------------------*/
	default_object_list = (struct zone_default *)
		alloc(num_default_files * sizeof(struct zone_default),
		"default_object_list","construct_zone_defaults");
	
	/*--------------------------------------------------------------*/
	/*	Loop through the default files list.			*/
	/*--------------------------------------------------------------*/
	for (i=0 ; i<num_default_files; i++) {
		/*--------------------------------------------------------------*/
		/*		Try to open the ith default file.		*/
		/*--------------------------------------------------------------*/
		//if ( (default_file = fopen( default_files[i], "r")) == NULL ){
		//	fprintf(stderr,
		//		"FATAL ERROR:in construct_zone_defaults unable to open defaults file %d.\n",i);
		//	exit(EXIT_FAILURE);
		//} /*end if*/
                paramCnt = 0;
                printf("Reading %s\n", default_files[i]);
                if (paramPtr != NULL)
                    free(paramPtr);

                paramPtr = readParamFile(&paramCnt, default_files[i]);
		/*--------------------------------------------------------------*/
		/*		read the ith default file into the ith object.	*/
		/*--------------------------------------------------------------*/
		default_object_list[i].ID = 			getIntParam(&paramCnt, &paramPtr, "zone_default_ID", "%d", 1, 1);
		default_object_list[i].atm_trans_lapse_rate =	getDoubleParam(&paramCnt, &paramPtr, "atm_trans_lapse_rate", "%lf", 0.000029, 1);
		default_object_list[i].dewpoint_lapse_rate =	getDoubleParam(&paramCnt, &paramPtr, "dewpoint_lapse_rate", "%lf", 0.0015, 1);
		default_object_list[i].max_effective_lai =	getDoubleParam(&paramCnt, &paramPtr, "max_effective_lai", "%lf", 10.0, 1);
		default_object_list[i].lapse_rate =		getDoubleParam(&paramCnt, &paramPtr, "lapse_rate", "%lf", 0.0064, 1);
		default_object_list[i].pptmin =			getDoubleParam(&paramCnt, &paramPtr, "pptmin", "%lf", 0.0254, 1);
		default_object_list[i].sea_level_clear_sky_trans = getDoubleParam(&paramCnt, &paramPtr, "sea_level_clear_sky_trans", "%lf", 0.75, 1);
		default_object_list[i].temcf = 			getDoubleParam(&paramCnt, &paramPtr, "temcf", "%lf", 0.4, 1);
		default_object_list[i].trans_coeff1 = 		getDoubleParam(&paramCnt, &paramPtr, "trans_coeff1", "%lf", -999.0, 1);
		default_object_list[i].trans_coeff2 =		getDoubleParam(&paramCnt, &paramPtr, "trans_coeff2", "%lf", 2.2, 1);
		default_object_list[i].trans_coeff1_sum = 		getDoubleParam(&paramCnt, &paramPtr, "trans_coeff1_sum", "%lf", -999.0, 1);
		default_object_list[i].trans_coeff2_sum =		getDoubleParam(&paramCnt, &paramPtr, "trans_coeff2_sum", "%lf", -999.0, 1);
		default_object_list[i].trans_startmonth_sum =		getDoubleParam(&paramCnt, &paramPtr, "trans_startmonth_sum", "%lf", 5, 1);
			if ((default_object_list[i].trans_startmonth_sum < 1) || (default_object_list[i].trans_startmonth_sum > 12)) {
				fprintf(stderr,"\nSummer start month for zone atmospheric transmissivity is not valid (1-12). Defaulting to 5 (May).");
				default_object_list[i].trans_startmonth_sum = 5;
			}
		default_object_list[i].trans_coeff1_win = 		getDoubleParam(&paramCnt, &paramPtr, "trans_coeff1_win", "%lf", -999.0, 1);
		default_object_list[i].trans_coeff2_win =		getDoubleParam(&paramCnt, &paramPtr, "trans_coeff2_win", "%lf", -999.0, 1);
		default_object_list[i].trans_startmonth_win =		getDoubleParam(&paramCnt, &paramPtr, "trans_startmonth_win", "%lf", 11, 1);
			if ((default_object_list[i].trans_startmonth_win < 1) || (default_object_list[i].trans_startmonth_win > 12)) {
				fprintf(stderr,"\nWinter start month for zone atmospheric transmissivity is not valid (1-12). Defaulting to 11 (November).");
				default_object_list[i].trans_startmonth_win = 11;
			}
		default_object_list[i].wind =			getDoubleParam(&paramCnt, &paramPtr, "wind", "%lf", 1.0, 1);
		default_object_list[i].max_snow_temp =		getDoubleParam(&paramCnt, &paramPtr, "max_snow_temp", "%lf", 1.0, 1);
		default_object_list[i].min_rain_temp =		getDoubleParam(&paramCnt, &paramPtr, "min_rain_temp", "%lf", -1.0, 1);
		default_object_list[i].ndep_NO3 =		getDoubleParam(&paramCnt, &paramPtr, "n_deposition", "%lf", 0.001, 1) / 365.0; // variable name different than parameter name


		/*--------------------------------------------------------------*/
		/*	convert from annual to daily				*/
		/*	(old)							*/
		/*	currently a concentration				*/
		/*--------------------------------------------------------------*/
		default_object_list[i].wind_direction = getDoubleParam(&paramCnt, &paramPtr, "wind_direction", "%lf", 180.0, 1); // parameter name is "wind" in param file
		default_object_list[i].lapse_rate_tmin = getDoubleParam(&paramCnt, &paramPtr, "lapse_rate_tmax", "%lf", default_object_list[i].lapse_rate, 1);
		default_object_list[i].lapse_rate_tmax = getDoubleParam(&paramCnt, &paramPtr, "lapse_rate_tmin", "%lf", default_object_list[i].lapse_rate, 1);
		default_object_list[i].wet_lapse_rate = 	getDoubleParam(&paramCnt, &paramPtr, "wet_lapse_rate", "%lf", 0.0049, 1);
		default_object_list[i].lapse_rate_precip_default = getDoubleParam(&paramCnt, &paramPtr, "lapse_rate_precip_default", "%lf", -999.0, 1);
		default_object_list[i].psen[PTHRESH] = 		getDoubleParam(&paramCnt, &paramPtr, "psen.pthresh", "%lf", 0.0, 1);
		default_object_list[i].psen[PTYPELOW] = 	getDoubleParam(&paramCnt, &paramPtr, "psen.ptypelow", "%lf", 1.0, 1);
		default_object_list[i].psen[P1LOW] = 		getDoubleParam(&paramCnt, &paramPtr, "psen.p1low", "%lf", 0.0, 1);
		default_object_list[i].psen[P2LOW] = 		getDoubleParam(&paramCnt, &paramPtr, "psen.p2low", "%lf", 0.0, 1);
		default_object_list[i].psen[PTYPEHIGH] =        getDoubleParam(&paramCnt, &paramPtr, "psen.ptypehigh", "%lf", 1.0, 1);
		default_object_list[i].psen[P1HIGH] = 		getDoubleParam(&paramCnt, &paramPtr, "psen.p1high", "%lf", 0.0, 1);
		default_object_list[i].psen[P2HIGH] = 		getDoubleParam(&paramCnt, &paramPtr, "psen.p2high", "%lf", 0.0, 1);
		default_object_list[i].ravg_days= 		getDoubleParam(&paramCnt, &paramPtr, "ravg_days", "%lf", 6.0, 1);


		/*--------------------------------------------------------------*/
		/* read any optional (tagged) defaults here			*/
		/*--------------------------------------------------------------*/
		/*	CO2 is initialized/default to 322 ppm			*/
		/*--------------------------------------------------------------*/
		default_object_list[i].atm_CO2 = 		getDoubleParam(&paramCnt, &paramPtr, "atm_CO2 ", "%lf", 322.0, 1);
		/*--------------------------------------------------------------*/
		/*      if grow flag allocate a grow default structure for the  */
		/*      ith object.                                             */
		/*--------------------------------------------------------------*/
		if ( command_line[0].grow_flag ){
			default_object_list[i].grow_defaults = (struct
				zone_grow_default *)
				alloc(1 * sizeof(struct zone_grow_default),
				"grow_defaults","construct_zone_defaults");
			/*--------------------------------------------------------------*/
			/*          NOTE: PLACE ANY GROW READING HERE.                  */
			/*--------------------------------------------------------------*/
		} /*end if*/

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
            // and "_zone.params"
            if (command_line[0].output_prefix != NULL) {
                strcat(outFilename, command_line[0].output_prefix);
                if (filename != NULL) {
                    strcat(outFilename, "_");
                    strcat(outFilename, filename);
                }
                strcat(outFilename, "_zone.params");
            } 
            else {
                if (filename != NULL) {
                    strcat(outFilename, "_");
                    strcat(outFilename, filename);
                }
                strcat(outFilename, "zone.params");
            }
    
                printParams(paramCnt, paramPtr, outFilename);
	} /*end for*/

                if (paramPtr != NULL)
                    free(paramPtr);
		    
	return(default_object_list);
} /*end construct_zone_defaults*/
