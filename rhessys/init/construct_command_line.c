/*--------------------------------------------------------------*/
/* 																*/
/*					construct_command_line						*/
/*																*/
/*	construct_command_line.c - creates command line object		*/
/*																*/
/*	NAME														*/
/*	construct_command_line.c - creates command line object		*/
/*																*/
/*	SYNOPSIS													*/
/*	struct	command_line_object	*construct_command_line(		*/
/*								argc, argv, command_line)		*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	Creates a command_line object which consists of flags		*/
/*	entered on the command line during execution of rhessys.	*/
/*	Some error checking is performed but error checking must	*/
/*	wait until the world object has been specified.				*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*	Original code, January 15, 1996.							*/
/*	valid_option to be written still - determines if the next	*/
/*			arguement is a valid option.						*/
/*	added routing option - May 7, 1997, C.Tague					*/
/*																*/
/*																*/
/*	Sep 1997	RAF												*/
/*	Removed extended output option flag as all output is		*/
/*	now of a single format specified by output routines.		*/
/*								*/
/*	Sept 1998	C.Tague					*/
/* 	added comma separated output option			*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rhessys.h"
struct	command_line_object	*construct_command_line(
													int main_argc,
													char **main_argv)
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.									*/
	/*--------------------------------------------------------------*/
	int	valid_option( char * );
	void	*alloc( size_t, char *, char * );
	void	output_template_structure();
	
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	int		i;
	struct	command_line_object	*command_line;
	
	/*--------------------------------------------------------------*/
	/*	Allocate a command line object.								*/
	/*--------------------------------------------------------------*/
	command_line = (struct command_line_object *)
		alloc(1 * sizeof(struct command_line_object),
		"command_line","construct_command_line");
	
	/*--------------------------------------------------------------*/
	/*	Initialize the options as null				*/
	/*--------------------------------------------------------------*/
	command_line[0].gridded_ascii_flag = 0;
	command_line[0].gridded_netcdf_flag = 0;
	command_line[0].grow_flag = 0;	
	command_line[0].std_scale = 0;
	command_line[0].stdev_flag = 0;
	command_line[0].road_flag = 1;
	command_line[0].prefix_flag = 0;
	command_line[0].verbose_flag = 0;
	command_line[0].routing_flag = 0;
	command_line[0].surface_routing_flag = 0;
	command_line[0].stream_routing_flag = 0;
	command_line[0].reservoir_operation_flag = 0;
	command_line[0].clim_repeat_flag = 0;
	command_line[0].dclim_flag = 0;
	command_line[0].ddn_routing_flag = 0;
	command_line[0].tec_flag = 0;
	command_line[0].world_flag = 0;
	command_line[0].world_header_flag = 0;
	command_line[0].start_flag = 0;
	command_line[0].end_flag = 0;
	command_line[0].sen_flag = 0;
	command_line[0].vsen_flag = 0;
	command_line[0].vsen_alt_flag = 0;
	command_line[0].precip_scale_flag = 0;
	command_line[0].snow_scale_flag = 0;
	command_line[0].noredist_flag = 0;
	command_line[0].surface_energy_flag = 0;
	command_line[0].firespread_flag = 0;
	command_line[0].vegspinup_flag = 0;		
	command_line[0].vgsen_flag = 0;
	command_line[0].FillSpill_flag=0;	
	command_line[0].evap_use_longwave_flag = 0;
	command_line[0].veg_sen1 = 1.0;
	command_line[0].veg_sen2 = 1.0;
	command_line[0].veg_sen3 = 1.0;
	command_line[0].vmort_flag = 0;
	command_line[0].version_flag = 0;
	command_line[0].vsen[M] = 1.0;
	command_line[0].vsen[K] = 1.0;
	command_line[0].sen[M] = 1.0;
	command_line[0].sen[K] = 1.0;
	command_line[0].sen[SOIL_DEPTH] = 1.0;
	command_line[0].prev_flag = 0;
	command_line[0].gw_flag = 0;
	command_line[0].gwtoriparian_flag = 0;
	command_line[0].tchange_flag = 0;
	command_line[0].tmax_add = 0.0;
	command_line[0].tmin_add = 0.0;
	command_line[0].output_prefix = NULL;
	command_line[0].output_flags.yearly = 0;
	command_line[0].output_flags.monthly = 0;
	command_line[0].output_flags.daily = 0;
	command_line[0].output_flags.hourly = 0;
	command_line[0].stro = NULL;
	command_line[0].b = NULL;
	command_line[0].h = NULL;
	command_line[0].z = NULL;
	command_line[0].p = NULL;
	command_line[0].c = NULL;
	command_line[0].tmp_value = 1.0;
	command_line[0].thresholds[SATDEF] = 0.0;
	command_line[0].thresholds[STREAMFLOW] = 0.0;
	command_line[0].snow_scale_tol = 999999999;
	
	/*-------------------------------------------------*/
	/* Loop through each arguement in the command line.*/
	/*-------------------------------------------------*/
	i = 1;
	while  ( i < main_argc){
		/*------------------------------------------*/
		/* Check for the print version flag         */
		/*------------------------------------------*/
		if ( strcmp(main_argv[i], "-version") == 0) {
			command_line[0].version_flag = 1;
			++i;
		}
		/*------------------------------------------*/
		/*	Check if the verbose flag is next.  */
		/*------------------------------------------*/
		if ( i< main_argc ){
			if ( strcmp(main_argv[i],"-v") == 0 ){
				/*-----------------------------------------------*/
				/*			Check if "-v" was the last agruement.   */
				/*-----------------------------------------------*/
				i++;
				if ( i == main_argc ){
					/*------------------------------------------*/
					/* assume we want verbose level 1			  */
					/*------------------------------------------*/
					command_line[0].verbose_flag= 1;
				}
				else if ( valid_option(main_argv[i]) == 1 ){
					/*----------------------------------------------*/
					/*	check if the next arguement is an option.		*/
					/*----------------------------------------------*/
					command_line[0].verbose_flag= 1;
				}
				else{
					/*-------------------------------------------------*/
					/*	read in the value of the verbose level.			*/
					/*-------------------------------------------------*/
					command_line[0].verbose_flag = (int)atoi(main_argv[i]);
					i++;
				}/*end if*/
			}/*end if*/
			/*------------------------------------------*/
			/* output_template file - this will terminate RHESSys RUN */
			/*------------------------------------------*/
			else if (strcmp(main_argv[i],"-template") == 0) {
				printf("\n Outputting template file structure and Exiting\n");
				output_template_structure();
				exit(EXIT_FAILURE);
			}
			/*------------------------------------------*/
			/*Check if the no redistribution flag is next.           */
			/*------------------------------------------*/
			else if ( strcmp(main_argv[i],"-noredist") == 0 ){
				printf("\n Running with no lateral redistribution - water balance not maintained ");
				command_line[0].noredist_flag = 1;
				i++;
			}
			/*------------------------------------------*/
			/*Check if the variable mortality flag is next.           */
			/*------------------------------------------*/
			else if ( strcmp(main_argv[i],"-vmort") == 0 ){
				command_line[0].vmort_flag = 1;
				i++;
				if ((i == main_argc) || (valid_option(main_argv[i])==1)){
					fprintf(stderr,"FATAL ERROR: Value for vmort flag not specified\n");
					exit(EXIT_FAILURE);
				} /*end if*/
				/*-------------------------------*/
				/*Read in the tmp value		*/
				/*-------------------------------*/
				command_line[0].cpool_mort_fract = (double)atof(main_argv[i]);
				i++;
			}
			/*------------------------------------------*/
			/*Check if the distributed climate flag is next.           */
			/*------------------------------------------*/
			else if ( strcmp(main_argv[i],"-climrepeat") == 0 ){
				command_line[0].clim_repeat_flag = 1;
				i++;
			}
			/*------------------------------------------*/
			/*Check if the distributed climate flag is next.           */
			/*------------------------------------------*/
			else if ( strcmp(main_argv[i],"-dclim") == 0 ){
				command_line[0].dclim_flag = 1;
				i++;
			}
			/*------------------------------------------*/
			/*Check if the grow flag is next.           */
			/*------------------------------------------*/
			else if ( strcmp(main_argv[i],"-g") == 0 ){
				command_line[0].grow_flag = 1;
				i++;
			}
			/*-------------------------------------------------*/
			/*Check tmp value option is next.				*/
			/* Currently tmp is used for sensitivity analysis of rooting_depth  */ 
			/*-------------------------------------------------*/
			else if ( strcmp(main_argv[i],"-tmp") == 0 ){
				i++;
				if ((i == main_argc) || (valid_option(main_argv[i])==1)){
					fprintf(stderr,"FATAL ERROR: Value for Tmp variable not specified\n");
					exit(EXIT_FAILURE);
				} /*end if*/
				/*-------------------------------*/
				/*Read in the tmp value		*/
				/*-------------------------------*/
				command_line[0].tmp_value = (double)atof(main_argv[i]);
				i++;
			}/* end if */


			/*-------------------------------------------------------*/
			/*Check if the snow distribution flag is next.           */
			/*-------------------------------------------------------*/
			else if ( strcmp(main_argv[i],"-snowdistb") == 0 ){
				printf("\n Running wiith snow redistribution");
				command_line[0].snow_scale_flag = 1;
				i++;
				/*--------------------------------------------------------------*/
				/*	check to see if there is a tolerance parameter 		*/
				/*--------------------------------------------------------------*/
				if (  (i != main_argc) && (valid_option(main_argv[i])==0) ){
					command_line[0].snow_scale_tol = (double)atof(main_argv[i]);
					i++;
				}/*end if*/
			}/* end if */
			/*-------------------------------------------------*/
			/*	fire spread option and coeffcients	  */
			/*-------------------------------------------------*/
			else if ( strcmp(main_argv[i],"-firespread") == 0 ){
				printf("\n Running with FIRE SPREAD turned on");
				command_line[0].firespread_flag = 1;
				i++;
				command_line[0].fire_grid_res = 30;
				/*-------------------------------*/
				/*Read in the fire spread grid parameters		*/
				/*-------------------------------*/
				if (  (i != main_argc) && (valid_option(main_argv[i])==0) ){
					command_line[0].fire_grid_res = (double)atof(main_argv[i]);
					i++;
				}/*end if*/
			}/* end if */

			/*-------------------------------------------------*/
			/*	surface energy option */
			/*-------------------------------------------------*/
			else if ( strcmp(main_argv[i],"-surfaceenergy") == 0 ){
				i++;
				printf("\n Running with SURFACE ENERGY CALC turned on");
				command_line[0].surface_energy_flag = 1;
				if ((i == main_argc-1) || (valid_option(main_argv[i])==1)){
					fprintf(stderr,"FATAL ERROR: Values for fire grid parameters not specified\n");
					exit(EXIT_FAILURE);
				} /*end if*/
			}/* end if */

			/*------------------------------------------*/
			/*Check if spinup flag next.                */
			/*------------------------------------------*/
			else if ( strcmp(main_argv[i],"-vegspinup") == 0 ){
				printf("\n Running with SPINUP turned on \n");
				command_line[0].vegspinup_flag = 1;
				i++;
     
      	/*--------------------------------------------------------------*/
				/*			Read in the vegspinup file name.						          	*/
				/*--------------------------------------------------------------*/
				strncpy(command_line[0].vegspinup_filename, main_argv[i], FILEPATH_LEN);
				i++;
      }

			/*-------------------------------------------------*/
			/*	routing gw to riparian option */
			/*-------------------------------------------------*/
			else if ( strcmp(main_argv[i],"-gwtoriparian") == 0 ){
				i++;
				printf("\n Running with hillslope gw routed to riparian areas\n ");
				command_line[0].gwtoriparian_flag = 1;
			}/* end if */
			/*-------------------------------------------------*/
			/*	groundwater flag and coeffcients	  */
			/*-------------------------------------------------*/
			else if ( strcmp(main_argv[i],"-gw") == 0 ){
				i++;
				command_line[0].gw_flag = 1;
				if ((i == main_argc-1) || (valid_option(main_argv[i])==1)){
					fprintf(stderr,"FATAL ERROR: Values for gw coefficients not specified\n");
					exit(EXIT_FAILURE);
				} /*end if*/
				/*-------------------------------*/
				/*Read in the loss to gw rate multiplier values		*/
				/*-------------------------------*/
				command_line[0].sat_to_gw_coeff_mult = (double)atof(main_argv[i]);
				i++;
				command_line[0].gw_loss_coeff_mult = (double)atof(main_argv[i]);
				i++;
			}/* end if */


			/*-------------------------------------------------*/
			/* simple addition of temperature increases 		*/
			/*-------------------------------------------------*/
			else if ( strcmp(main_argv[i],"-tchange") == 0 ){
				i++;
				command_line[0].tchange_flag = 1;
				if ((i == main_argc-1) || (valid_option(main_argv[i])==1)){
					fprintf(stderr,"FATAL ERROR: Values for tchange not specified\n");
					exit(EXIT_FAILURE);
				} /*end if*/
				/*-------------------------------*/
				/*Read in the loss to gw rate multiplier values		*/
				/*-------------------------------*/
				command_line[0].tmax_add = (double)atof(main_argv[i]);
				i++;
				command_line[0].tmin_add = (double)atof(main_argv[i]);
				i++;
			}/* end if */
			/*-------------------------------------------------*/
			/* 	soil moisture standard deviation	-std */
			/*	if this flag is set there must be an extra variable in the worldfile */
			/*	at the patch level which inputs a std for that patch 	*/	
			/*-------------------------------------------------*/
			else if ( strcmp(main_argv[i],"-stdev") == 0 ){
				i++;
				command_line[0].stdev_flag = 1;
				if ((i == main_argc) || (valid_option(main_argv[i])==1)){
					fprintf(stderr,"FATAL ERROR: Value for soil moisture std not specified\n");
					exit(EXIT_FAILURE);
				} /*end if*/
				command_line[0].std_scale = (double)atof(main_argv[i]);
				i++;
			}/* end if */
			/*-------------------------------------------------*/
			/*Check if the threshold option is next.				*/
			/*-------------------------------------------------*/
			else if ( strcmp(main_argv[i],"-th") == 0 ){
				i++;
				if ((i == main_argc) || (valid_option(main_argv[i])==1)){
					fprintf(stderr,"FATAL ERROR: Thresholds not specified\n");
					exit(EXIT_FAILURE);
				} /*end if*/
				/*-------------------------------*/
				/*Read in the thresholds 	*/
				/* 	at present there are up to two values - threshold */
				/*	sat_deficit - used for outputting water stress days 	*/
				/*	and streamflow - used for outputting low flow days	*/
				/*-------------------------------*/
				command_line[0].thresholds[SATDEF] = (double)atof(main_argv[i]);
				i++;
				/*--------------------------------------------------------------*/
				/*	check to see if there is a second threshold parameter 	*/
				/*--------------------------------------------------------------*/
				if (  (i != main_argc) && (valid_option(main_argv[i])==0) ){
					command_line[0].thresholds[STREAMFLOW] = (double)atof(main_argv[i]);
					i++;
				}/*end if*/
			}/* end if */
			/*-----------------------------------------------------------*/
			/*Check if the sensitivity analysis option is next.				*/
			/*-----------------------------------------------------------*/
			else if ( strcmp(main_argv[i],"-s") == 0 ){
				i++;
				command_line[0].sen_flag = 1;
				if (  (i == main_argc) || (valid_option(main_argv[i])==1) ){
					fprintf(stderr,
						"FATAL ERROR: Sensitivity perturbation not specified\n");
					exit(EXIT_FAILURE);
				}/*end if*/
				/*--------------------------------------------------------------*/
				/*			Read in the sensitivity parameter values. */
				/*--------------------------------------------------------------*/
				command_line[0].sen[M] = (double)atof(main_argv[i]);
				i++;
				if (  (i == main_argc) || (valid_option(main_argv[i])==1) ){
					fprintf(stderr,
						"FATAL ERROR: Sensitivity perturbation not specified\n");
					exit(EXIT_FAILURE);
				}/*end if*/
				command_line[0].sen[K] = (double)atof(main_argv[i]);
				i++;
				/*--------------------------------------------------------------*/
				/*	check to see if there is a 3rd sensitivity parameter 	*/
				/*	if not set to 1.0					*/
				/*--------------------------------------------------------------*/
				if (  (i != main_argc) && (valid_option(main_argv[i]) == 0) ){
					command_line[0].sen[SOIL_DEPTH] = (double)atof(main_argv[i]);
					i++;
				}  /*end if*/
			} /* end if */


			/*-----------------------------------------------------------*/
			/*Check if the vertical  sensitivity analysis option is next.				*/
			/*-----------------------------------------------------------*/
			else if ( strcmp(main_argv[i],"-sv") == 0 ){
				i++;
				command_line[0].vsen_flag = 1;
				if (  (i == main_argc) || (valid_option(main_argv[i])==1) ){
					fprintf(stderr,
						"FATAL ERROR: Sensitivity perturbation not specified\n");
					exit(EXIT_FAILURE);
				}/*end if*/
				/*--------------------------------------------------------------*/
				/*			Read in the sensitivity parameter values. */
				/*--------------------------------------------------------------*/
				command_line[0].vsen[M] = (double)atof(main_argv[i]);
				i++;
				if (  (i == main_argc) || (valid_option(main_argv[i])==1) ){
					fprintf(stderr,
						"FATAL ERROR: Sensitivity perturbation not specified\n");
					exit(EXIT_FAILURE);
				}/*end if*/
				command_line[0].vsen[K] = (double)atof(main_argv[i]);
				i++;
			} /* end if */


			/*-------------------------------------------------------*/
			/*Check if the precip scaling (using random dist) flag is next.           */
			/*-------------------------------------------------------*/
			else if ( strcmp(main_argv[i],"-precip") == 0 ){
				printf("\n Running wiith stochastic precipitation scaling ");
				command_line[0].precip_scale_flag = 1;
				i++;

			} /* end if */

			/*-----------------------------------------------------------*/
			/*alternatively use pore size inidex and psi air entry				*/
			/*-----------------------------------------------------------*/
			else if ( strcmp(main_argv[i],"-svalt") == 0 ){
				i++;
				command_line[0].vsen_alt_flag = 1;
				if (  (i == main_argc) || (valid_option(main_argv[i])==1) ){
					fprintf(stderr,
						"FATAL ERROR: Sensitivity perturbation not specified\n");
					exit(EXIT_FAILURE);
				}/*end if*/
				/*--------------------------------------------------------------*/
				/*			Read in the sensitivity parameter values. */
				/*--------------------------------------------------------------*/
				command_line[0].vsen_alt[PA] = (double)atof(main_argv[i]);
				i++;
				if (  (i == main_argc) || (valid_option(main_argv[i])==1) ){
					fprintf(stderr,
						"FATAL ERROR: Sensitivity perturbation not specified\n");
					exit(EXIT_FAILURE);
				}/*end if*/
				command_line[0].vsen_alt[PO] = (double)atof(main_argv[i]);
				i++;
			} /* end if */

			/*-----------------------------------------------------------*/
			/*Check if the vegetation  sensitivity analysis option is next.				*/
			/*-----------------------------------------------------------*/
			else if ( strcmp(main_argv[i],"-vgsen") == 0 ){
				i++;
				if (  (i == main_argc) || (valid_option(main_argv[i])==1) ){
					fprintf(stderr,
						"FATAL ERROR: Sensitivity perturbation not specified\n");
					exit(EXIT_FAILURE);
				}/*end if*/
				/*--------------------------------------------------------------*/
				/*			Read in the sensitivity parameter name. */
				/*--------------------------------------------------------------*/
				command_line[0].vgsen_flag = 1;
				command_line[0].veg_sen1 = (double)atof(main_argv[i]);
				i++;
				if (  (i == main_argc) || (valid_option(main_argv[i])==1) ){
					fprintf(stderr,
						"FATAL ERROR: Sensitivity perturbation not specified\n");
					exit(EXIT_FAILURE);
				}/*end if*/
				command_line[0].veg_sen2 = (double)atof(main_argv[i]);
				i++;
				if (  (i == main_argc) || (valid_option(main_argv[i])==1) ){
					fprintf(stderr,
						"FATAL ERROR: 3rd Vegetation Sensitivity perturbation not specified\n");
					exit(EXIT_FAILURE);
				}/*end if*/
				command_line[0].veg_sen3 = (double)atof(main_argv[i]);
				i++;
			} /* end if */
			/*--------------------------------------------------------------*/
			/* check for start and end dates				*/
			/*--------------------------------------------------------------*/
			else if ( strcmp(main_argv[i],"-st") == 0 ){
				command_line[0].start_flag = 1;
				/*-------------------------------------------------------*/
				/*			Check that the next arguement exists.				*/
				/*-------------------------------------------------------*/
				i++;
				if ((i == main_argc) || (valid_option(main_argv[i])==1)){
					fprintf(stderr,"FATAL ERROR: Start date year not specified\n");
					exit(EXIT_FAILURE);
				} /*end if*/
				/*-------------------------------------------------*/
				/*			Read in the start year.							*/
				/*-------------------------------------------------*/
				command_line[0].start_date.year = (int)atoi(main_argv[i]);
				/*--------------------------------------------------------------*/
				/*			Check that the next arguement exists.				*/
				/*--------------------------------------------------------------*/
				i++;
				if ((i == main_argc) || (valid_option(main_argv[i])==1)){
					fprintf(stderr,"FATAL ERROR: Start date month not specified\n");
					exit(EXIT_FAILURE);
				} /*end if*/
				/*--------------------------------------------------------------*/
				/*			Read in the start month.							*/
				/*--------------------------------------------------------------*/
				command_line[0].start_date.month = (int)atoi(main_argv[i]);
				/*--------------------------------------------------------------*/
				/*			Check that the next arguement exists.				*/
				/*--------------------------------------------------------------*/
				i++;
				if (  (i == main_argc) || (valid_option(main_argv[i])==1)){
					fprintf(stderr,"FATAL ERROR: Start date day not specified\n");
					exit(EXIT_FAILURE);
				} /*end if*/
				/*--------------------------------------------------------------*/
				/*			Read in the start day.							*/
				/*--------------------------------------------------------------*/
				command_line[0].start_date.day = (int)atoi(main_argv[i]);
				/*--------------------------------------------------------------*/
				/*			Check that the next arguement exists.				*/
				/*--------------------------------------------------------------*/
				i++;
				if ((i == main_argc) || (valid_option(main_argv[i])==1)){
					fprintf(stderr,"FATAL ERROR: Start date hour not specified\n");
					exit(EXIT_FAILURE);
				} /*end if*/
				/*--------------------------------------------------------------*/
				/*			Read in the hour year.							*/
				/*--------------------------------------------------------------*/
				command_line[0].start_date.hour = (int)atoi(main_argv[i]);
				i++;
			}/*end if*/
			/*--------------------------------------------------------------*/
			/* check for start and end dates				*/
			/*--------------------------------------------------------------*/
			else if ( strcmp(main_argv[i],"-ed") == 0 ){
				command_line[0].end_flag = 1;
				/*--------------------------------------------------------------*/
				/*			Check that the next arguement exists.				*/
				/*--------------------------------------------------------------*/
				i++;
				if ((i == main_argc) || (valid_option(main_argv[i])==1)){
					fprintf(stderr,"FATAL ERROR: Start date year not specified\n");
					exit(EXIT_FAILURE);
				} /*end if*/
				/*--------------------------------------------------------------*/
				/*			Read in the end year.							*/
				/*--------------------------------------------------------------*/
				command_line[0].end_date.year = (int)atoi(main_argv[i]);
				/*--------------------------------------------------------------*/
				/*			Check that the next arguement exists.				*/
				/*--------------------------------------------------------------*/
				i++;
				if ((i == main_argc) || (valid_option(main_argv[i])==1)){
					fprintf(stderr,"FATAL ERROR: Start date month not specified\n");
					exit(EXIT_FAILURE);
				} /*end if*/
				/*--------------------------------------------------------------*/
				/*			Read in the end month.							*/
				/*--------------------------------------------------------------*/
				command_line[0].end_date.month = (int)atoi(main_argv[i]);
				/*--------------------------------------------------------------*/
				/*			Check that the next arguement exists.				*/
				/*--------------------------------------------------------------*/
				i++;
				if ((i == main_argc) || (valid_option(main_argv[i])==1)){
					fprintf(stderr,"FATAL ERROR: Start date day not specified\n");
					exit(EXIT_FAILURE);
				} /*end if*/
				/*--------------------------------------------------------------*/
				/*			Read in the end day.							*/
				/*--------------------------------------------------------------*/
				command_line[0].end_date.day = (int)atoi(main_argv[i]);
				/*--------------------------------------------------------------*/
				/*			Check that the next arguement exists.				*/
				/*--------------------------------------------------------------*/
				i++;
				if ((i == main_argc) || (valid_option(main_argv[i])==1)){
					fprintf(stderr,"FATAL ERROR: Start date hour not specified\n");
					exit(EXIT_FAILURE);
				} /*end if*/
				/*--------------------------------------------------------------*/
				/*			Read in the hour year.							*/
				/*--------------------------------------------------------------*/
				command_line[0].end_date.hour = (int)atoi(main_argv[i]);
				i++;
			}/*end if*/
			/*--------------------------------------------------------------*/
			/*		Check if the routing option file is next.				*/
			/*--------------------------------------------------------------*/
			else if ( strcmp(main_argv[i],"-r") == 0 ){
				/*--------------------------------------------------------------*/
				/*			Check that the next arguement exists.				*/
				/*--------------------------------------------------------------*/
				i++;
				if ((i == main_argc) || (valid_option(main_argv[i])==1) ){
					fprintf(stderr,"FATAL ERROR: Routing file name not specified\n");
					exit(EXIT_FAILURE);
				} /*end if*/
				/*--------------------------------------------------------------*/
				/*			Read in the routing file name.						*/
				/*--------------------------------------------------------------*/
				command_line[0].routing_flag = 1;
				strncpy(command_line[0].routing_filename, main_argv[i], FILEPATH_LEN);
				i++;
				/*--------------------------------------------------------------*/
				/*	Attempt to read in surface routing file name.				*/
				/*--------------------------------------------------------------*/
				if ( (i < main_argc) && !valid_option(main_argv[i]) ) {
					command_line[0].surface_routing_flag = 1;
					strncpy(command_line[0].surface_routing_filename, main_argv[i], FILEPATH_LEN);
					i++;
				}
			} /*end if*/


			/*--------------------------------------------------------------*/
			/*		Check if the stream routing option file is next.				*/
			/*--------------------------------------------------------------*/
			else if ( strcmp(main_argv[i],"-str") == 0 ){
				/*--------------------------------------------------------------*/
				/*			Check that the next arguement exists.				*/
				/*--------------------------------------------------------------*/
				i++;
				if ((i == main_argc) || (valid_option(main_argv[i])==1) ){
					fprintf(stderr,"FATAL ERROR: Routing file name not specified\n");
					exit(EXIT_FAILURE);
				} /*end if*/
				/*--------------------------------------------------------------*/
				/*			Read in the routing file name.						*/
				/*--------------------------------------------------------------*/
				command_line[0].stream_routing_flag = 1;
				strcpy(command_line[0].stream_routing_filename,main_argv[i]);
				i++;
			} /*end if*/


			/*--------------------------------------------------------------*/
			/*		Check if the reservoir option file is next.				*/
			/*--------------------------------------------------------------*/
			else if ( strcmp(main_argv[i],"-res") == 0 ){
				/*--------------------------------------------------------------*/
				/*			Check that the next arguement exists.				*/
				/*--------------------------------------------------------------*/
				i++;
				if ((i == main_argc) || (valid_option(main_argv[i])==1) ){
					fprintf(stderr,"FATAL ERROR: Routing file name not specified\n");
					exit(EXIT_FAILURE);
				} /*end if*/
				/*--------------------------------------------------------------*/
				/*			Read in the reservoir file name.						*/
				/*--------------------------------------------------------------*/
				command_line[0].reservoir_operation_flag = 1;
				strcpy(command_line[0].reservoir_operation_filename,main_argv[i]);
				i++;
			} /*end if*/


			/*--------------------------------------------------------------*/
			/*		Check if the ddn routing option file is next.				*/
			/*--------------------------------------------------------------*/
			else if ( strcmp(main_argv[i],"-rddn") == 0 ){
				/*--------------------------------------------------------------*/
				/*			Check that the next arguement exists.				*/
				/*--------------------------------------------------------------*/
				i++;
				if ((i == main_argc) || (valid_option(main_argv[i])==1) ){
					fprintf(stderr,"FATAL ERROR: Routing file name not specified\n");
					exit(EXIT_FAILURE);
				} /*end if*/
				/*--------------------------------------------------------------*/
				/*			Read in the routing file name.						*/
				/*--------------------------------------------------------------*/
				command_line[0].ddn_routing_flag = 1;
				command_line[0].routing_flag = 1;
				strcpy(command_line[0].routing_filename,main_argv[i]);
				i++;
			} /*end if*/

			/*--------------------------------------------------------------*/
			/*		Check if the world file is next.						*/
			/*--------------------------------------------------------------*/
			else if ( strcmp(main_argv[i],"-w") == 0 ){
				/*--------------------------------------------------------------*/
				/*			Check that the next arguement exists.				*/
				/*--------------------------------------------------------------*/
				i++;
				if ((i == main_argc) || (valid_option(main_argv[i])==1) ){
					fprintf(stderr,"FATAL ERROR: World file name not specified\n");
					exit(EXIT_FAILURE);
				} /*end if*/
				/*--------------------------------------------------------------*/
				/*			Read in the world file name.						*/
				/*--------------------------------------------------------------*/
				command_line[0].world_flag = 1;
				strcpy(command_line[0].world_filename,main_argv[i]);
				i++;
			} /*end if*/
			/*--------------------------------------------------------------*/
			/*		Check if the world header file is next.						*/
			/*--------------------------------------------------------------*/
			else if ( strcmp(main_argv[i],"-whdr") == 0 ){
				/*--------------------------------------------------------------*/
				/*			Check that the next argument exists.				*/
				/*--------------------------------------------------------------*/
				i++;
				if ((i == main_argc) || (valid_option(main_argv[i])==1) ){
					fprintf(stderr,"FATAL ERROR: World file header name not specified\n");
					exit(EXIT_FAILURE);
				} /*end if*/
				/*--------------------------------------------------------------*/
				/*			Read in the world file name.						*/
				/*--------------------------------------------------------------*/
				command_line[0].world_header_flag = 1;
				strcpy(command_line->world_header_filename,main_argv[i]);
				i++;
			} /*end if*/
			/*--------------------------------------------------------------*/
			/*		Check if the tec file is next.							*/
			/*--------------------------------------------------------------*/
			else if( strcmp(main_argv[i],"-t") == 0 ){
				/*--------------------------------------------------------------*/
				/*			Check that the next arguement exists.				*/
				/*--------------------------------------------------------------*/
				i++;
				if ((i == main_argc) || (valid_option(main_argv[i])==1) ){
					fprintf(stderr,"FATAL ERROR: TEC file name not specified\n");
					exit(EXIT_FAILURE);
				} /*end if*/
				/*--------------------------------------------------------------*/
				/*			Read in the tec file name.							*/
				/*--------------------------------------------------------------*/
				command_line[0].tec_flag = 1;
				strcpy(command_line[0].tec_filename,main_argv[i]);
				i++;
			} /*end if*/
			/*--------------------------------------------------------------*/
			/*		Check if the output prefix is next.						*/
			/*--------------------------------------------------------------*/
			else if( strcmp(main_argv[i],"-pre") == 0 ){
				/*--------------------------------------------------------------*/
				/*			Check that the next arguement exists.				*/
				/*--------------------------------------------------------------*/
				i++;
				if ((i == main_argc) || (valid_option(main_argv[i])==1)){
					fprintf(stderr,"FATAL ERROR: Output prefix not specified\n");
					exit(EXIT_FAILURE);
				} /*end if*/
				/*--------------------------------------------------------------*/
				/*			Allocate an array for the output prefix and			*/
				/*			Read in the output prefix .							*/
				/*--------------------------------------------------------------*/
				command_line[0].output_prefix =
					(char *) alloc((1+strlen(main_argv[i]))*sizeof(char),
					"output_prefix","construct_command_line");
				strcpy(command_line[0].output_prefix,main_argv[i]);
				command_line[0].prefix_flag = 1;
				i++;
			}/*end if*/
                        /*--------------------------------------------------------------*/
			/*		Check if the stream_routing output flag is next.    				*/
			/*--------------------------------------------------------------*/
			else if( strcmp(main_argv[i],"-stro") == 0 ){
				/*--------------------------------------------------------------*/
				/*			Allocate the stream_routing output specifier.				*/
				/*--------------------------------------------------------------*/
				command_line[0].stro = (struct stro_option *)
					alloc(sizeof(struct stro_option), "stro","construct_command_line" );
				command_line[0].stro[0].reachID 	= -999;
				
				/*--------------------------------------------------------------*/
				/*			Check that the next arguement exists.				*/
				/*--------------------------------------------------------------*/
				i++;
				if (i < main_argc){
					/*----------------------------------------------*/
					/*Check that the next arguement is a reachID		*/
					/*----------------------------------------------*/
					if ( valid_option(main_argv[i]) == 0){
						command_line[0].stro[0].reachID = (int)atoi(main_argv[i]);
						i++;
					}/*end if*/
				} /*end if*/
			} /*end if*/
			
                        /*--------------------------------------------------------------*/
			/*		Check if the basin output flag is next.    				*/
			/*--------------------------------------------------------------*/
			else if( strcmp(main_argv[i],"-b") == 0 ){
				/*--------------------------------------------------------------*/
				/*			Allocate the basin output specifier.				*/
				/*--------------------------------------------------------------*/
				command_line[0].b = (struct b_option *)
					alloc(sizeof(struct b_option), "b","construct_command_line" );
				command_line[0].b[0].basinID 	= -999;
                               
				/*--------------------------------------------------------------*/
				/*			Check that the next arguement exists.				*/
				/*--------------------------------------------------------------*/
				i++;
				if (i < main_argc){
					/*----------------------------------------------*/
					/*Check that the next arguement is a basinID		*/
					/*----------------------------------------------*/
					if ( valid_option(main_argv[i]) == 0){
						command_line[0].b[0].basinID = (int)atoi(main_argv[i]);
						i++;
					}/*end if*/
				} /*end if*/
			} /*end if*/
			/*----------------------------------------------------------*/
			/*		Check if the hillslope output flag is next.  			*/
			/*----------------------------------------------------------*/
			else if( strcmp(main_argv[i],"-h") == 0 ){
				/*-------------------------------------------------------*/
				/*			Allocate the hillslope output specifier.			*/
				/*-------------------------------------------------------*/
				command_line[0].h = (struct h_option *)
					alloc(sizeof(struct h_option), "h", "construct_command_line" );
				command_line[0].h[0].basinID 	= -999;
				command_line[0].h[0].hillID 	= -999;
				/*-------------------------------------------------------*/
				/*			Check that the next arguement exists.				*/
				/*-------------------------------------------------------*/
				i++;
				if ( i < main_argc ){
					/*--------------------------------------------------------------*/
					/*				Check that the next arguement is a basinID		*/
					/*--------------------------------------------------------------*/
					if ( valid_option(main_argv[i]) == 0 ){
						command_line[0].h[0].basinID = (int)atoi(main_argv[i]);
						i++;
						/*-------------------------------------------------------*/
						/*		Check that the next arguement exists.		*/
						/*-------------------------------------------------------*/
						if (  i < main_argc ){
							/*-------------------------------------------*/
							/*	Check that the next arguement is hillID	*/
							/*-------------------------------------------*/
							if ( valid_option(main_argv[i]) == 0 ){
								command_line[0].h[0].hillID = (int)atoi(main_argv[i]);
								i++;
							}/*end if*/
						}/*end if*/
					} /*end if*/
				} /*end if*/
			} /*end if*/
			/*-------------------------------------------------------*/
			/*		Check if the zone output flag is next.  				*/
			/*-------------------------------------------------------*/
			else if( strcmp(main_argv[i],"-z") == 0 ){
				/*----------------------------------------------------*/
				/*			Allocate the zone output specifier.				*/
				/*----------------------------------------------------*/
				command_line[0].z = (struct z_option *)
					alloc(sizeof(struct z_option), "z", "construct_command_line");
				command_line[0].z[0].basinID 	= -999;
				command_line[0].z[0].hillID 	= -999;
				command_line[0].z[0].zoneID 	= -999;
				/*-------------------------------------------------------*/
				/*			Check that the next arguement exists.				*/
				/*-------------------------------------------------------*/
				i++;
				if (  i < main_argc ){
					/*----------------------------------------------------------*/
					/*				Check that the next arguement is a basinID		*/
					/*----------------------------------------------------------*/
					if ( valid_option(main_argv[i]) == 0 ){
						command_line[0].z[0].basinID = (int)atoi(main_argv[i]);
						i++;
						/*-------------------------------------------------------*/
						/*					Check that the next arguement exists.		*/
						/*-------------------------------------------------------*/
						if (  i < main_argc ){
							/*----------------------------------------------------*/
							/*  			Check that the next arguement is hillID	*/
							/*----------------------------------------------------*/
							if ( valid_option(main_argv[i]) == 0 ){
								command_line[0].z[0].hillID = (int)atoi(main_argv[i]);
								i++;
								/*----------------------------------------------------*/
								/*			Check that the next arguement exists.	*/
								/*----------------------------------------------------*/
								if (  i < main_argc ){
									/*-------------------------------------------------*/
									/*				Check that the next arg is a zoneID	*/
									/*-------------------------------------------------*/
									if ( valid_option(main_argv[i]) == 0 ){
										command_line[0].z[0].zoneID=(int)atoi(main_argv[i]);
										i++;
									}/*end if*/
								}/*end if*/
							} /*end if*/
						} /*end  if*/
					} /*end  if*/
				} /*end if*/
			} /*end if*/
			/*--------------------------------------------------------------*/
			/*		Check if the patch output flag is next.  			*/
			/*--------------------------------------------------------------*/
			else if( strcmp(main_argv[i],"-p") == 0 ){
				/*--------------------------------------------------------------*/
				/*			Allocate the patch output specifier.				*/
				/*--------------------------------------------------------------*/
				command_line[0].p = (struct p_option *)
					alloc(sizeof(struct p_option),"p","construct_command_line" );
				command_line[0].p[0].basinID 	= -999;
				command_line[0].p[0].hillID 	= -999;
				command_line[0].p[0].zoneID 	= -999;
				command_line[0].p[0].patchID 	= -999;
				/*--------------------------------------------------------------*/
				/*			Check that the next arguement exists.				*/
				/*--------------------------------------------------------------*/
				i++;
				if (  i < main_argc ){
					/*--------------------------------------------------------------*/
					/*				Check that the next arguement is a basinID		*/
					/*--------------------------------------------------------------*/
					if ( valid_option(main_argv[i]) == 0 ){
						command_line[0].p[0].basinID = (int)atoi(main_argv[i]);
						i++;
						/*-------------------------------------------------------*/
						/*					Check that the next arguement exists.		*/
						/*-------------------------------------------------------*/
						if (  i < main_argc ){
							/*-------------------------------------------------------*/
							/*		  			Check that the next arguement is hillID	*/
							/*-------------------------------------------------------*/
							if ( valid_option(main_argv[i]) == 0){
								command_line[0].p[0].hillID = (int)atoi(main_argv[i]);
								i++;
								/*----------------------------------------------------*/
								/*	  Check that the next arguement exists.				*/
								/*----------------------------------------------------*/
								if (  i < main_argc ){
									/*----------------------------------------------*/
									/* 				Check that next arg is a zoneID		*/
									/*-----------------------------------------------*/
									if ( valid_option(main_argv[i]) == 0  ){
										command_line[0].p[0].zoneID = (int)atoi(main_argv[i]);
										i++;
										/*--------------------------------------------*/
										/*			Check that next arguement exists.	*/
										/*-------------------------------------------*/
										if (  i < main_argc ){
											/*------------------------------------------*/
											/*			Check next arg is a patchID		*/
											/*----------------------------------------*/
											if ( valid_option(main_argv[i]) == 0 ){
												command_line[0].p[0].patchID =
													(int)atoi(main_argv[i]);
												i++;
											}/*end if*/
										} /*end if*/
									} /*end if*/
								} /*end if*/
							} /*end if*/
						} /*end if*/
					} /*end if*/
				} /*end if*/
			} /*end if*/
			/*--------------------------------------------------------------*/
			/*		Check if the canopy stratum output flag is next.		*/
			/*--------------------------------------------------------------*/
			else if ( strcmp(main_argv[i],"-c") == 0 ){
				/*--------------------------------------------------------------*/
				/*			Allocate the patch output specifier.				*/
				/*--------------------------------------------------------------*/
				command_line[0].c = (struct c_option *)
					alloc(sizeof(struct c_option),"c","construct_command_line" );
				command_line[0].c[0].basinID 	= -999;
				command_line[0].c[0].hillID 	= -999;
				command_line[0].c[0].zoneID 	= -999;
				command_line[0].c[0].patchID 	= -999;
				command_line[0].c[0].stratumID 	= -999;
				/*--------------------------------------------------------------*/
				/*			Check that the next arguement exists.				*/
				/*--------------------------------------------------------------*/
				i++;
				if (  i < main_argc ){
					/*--------------------------------------------------------------*/
					/*				Check that the next arguement is a basinID		*/
					/*--------------------------------------------------------------*/
					if ( valid_option(main_argv[i]) == 0 ){
						command_line[0].c[0].basinID = (int)atoi(main_argv[i]);
						i++;
						/*--------------------------------------------------------------*/
						/*					Check that the next arguement exists.		*/
						/*--------------------------------------------------------------*/
						if (  i < main_argc ){
							/*----------------------------------------------------------*/
							/*						Check that the next arguement is hillID	*/
							/*----------------------------------------------------------*/
							if ( valid_option(main_argv[i]) == 0){
								command_line[0].c[0].hillID = (int)atoi(main_argv[i]);
								i++;
								/*-------------------------------------------------------*/
								/*			Check that the next arguement exists.				*/
								/*-------------------------------------------------------*/
								if (  i < main_argc ){
									/*----------------------------------------------------*/
									/*					Check that next arg is a zoneID		*/
									/*----------------------------------------------------*/
									if ( valid_option(main_argv[i]) == 0  ){
										command_line[0].c[0].zoneID =	(int)atoi(main_argv[i]);
										i++;
										/*-------------------------------------------------*/
										/*				Check that next arguement exists.	*/
										/*-------------------------------------------------*/
										if (  i < main_argc ){
											/*---------------------------------------------*/
											/*				Check next arg is a patchID		*/
											/*---------------------------------------------*/
											if ( valid_option(main_argv[i]) == 0 ){
												command_line[0].c[0].patchID =
													(int)atoi(main_argv[i]);
												i++;
												/*------------------------------------------*/
												/*		Check that next arguement exists.	*/
												/*-----------------------------------------*/
												if (  i < main_argc ){
													/*----------------------------------------*/
													/*	Check next arg is a stratumID	*/
													/*----------------------------------------*/
													if ( valid_option(main_argv[i]) == 0 ){
														command_line[0].c[0].stratumID =
															(int)atoi(main_argv[i]);
														i++;
													}/*end if*/
												} /*end if*/
											}/*end if*/
										} /*end if*/
									} /*end if*/
								} /*end if*/
							} /*end if*/
						   } /*end if*/
						} /*end if*/
					} /*end if*/
				} /*end if*/
			/*--------------------------------------------------------------*/
			/*		Check if the gridded climate input flag is next		    */
			/*--------------------------------------------------------------*/
			else if ( strcmp(main_argv[i],"-asciigrid") == 0 ){
				printf("Setting climate mode to gridded ascii\n");
				command_line[0].gridded_ascii_flag = 1;
				i++;
				} /*end if*/
			else if (strcmp(main_argv[i],"-netcdfgrid") == 0 ){
				command_line[0].gridded_netcdf_flag = 1;
				i++;
			}
			else if (strcmp(main_argv[i], "-longwaveevap") == 0) {
				command_line[0].evap_use_longwave_flag = 1;
				i++;
			}
			/*--------------------------------------------------------------*/
			/*	NOTE:  ADD MORE OPTION PARSING HERE.						*/
			/*--------------------------------------------------------------*/
			/*--------------------------------------------------------------*/
			/*		the option must be invalid.								*/
			/*--------------------------------------------------------------*/
			else{
				fprintf(stderr,
					"FATAL ERROR: in construct_command_line option #%d is invalid.\n",i);
				fprintf(stderr,"for argument %s\n", main_argv[i]);
				exit(EXIT_FAILURE);
			} /*end if*/
		} /*end if*/
	} /*end while*/

	return(command_line);
} /*end construct_command_line*/
