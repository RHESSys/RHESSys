/*--------------------------------------------------------------*/
/* 								*/
/*	construct_stratum_defaults				*/
/*								*/
/*	construct_stratum_defaults.c - makes stratum default	*/
/*					objects.		*/
/*								*/
/*	NAME							*/
/*	construct_stratum_defaults.c - makes stratum default	*/
/*					objects.		*/
/*								*/
/*	SYNOPSIS						*/
/*	struct stratum_default *construct_stratum_defaults(    	*/
/*					num_default_files,	*/
/*					default_files,		*/
/*					grow_flag,		*/
/*					default_object_list )	*/
/*								*/
/*	OPTIONS							*/
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*	PROGRAMMER NOTES					*/
/*								*/
/*	Original code, January 15, 1996.			*/
/*								*/
/* March 12, 1997   C. Tague					*/
/* added seasonal leafon/off ramp variable		  	*/
/*								*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "rhessys.h"
#include "phys_constants.h"
#include "params.h"

struct stratum_default *construct_stratum_defaults(
		int	num_default_files,
		char	**default_files,
		struct command_line_object *command_line)
												   
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.				*/
	/*--------------------------------------------------------------*/
	void	*alloc(	size_t, char *, char *);
	int	parse_veg_type( char *);
	int	parse_phenology_type( char *);
	int	parse_dyn_flag( char *);
	int	parse_alloc_flag( char *);
	/*--------------------------------------------------------------*/
	/*	Local variable definition.				*/
	/*--------------------------------------------------------------*/
	int		i, itmp;
        int strbufLen = 256;
        int filenameLen = 1024;
        int paramCnt = 0;
	char		record[MAXSTR];
	char		*newrecord;
	char		stmp[MAXSTR];
        char	strbuf[strbufLen];
        char	outFilename[filenameLen];
	double		fcel, ftmp, lig_cel_ratio;
	//FILE	*default_file;
        param *paramPtr = NULL;
	struct stratum_default	*default_object_list;
	struct	epconst_struct	*epc;

	/*--------------------------------------------------------------*/
	/*	Allocate an array of default objects.			*/
	/*--------------------------------------------------------------*/
	default_object_list = (struct stratum_default *)
		alloc(num_default_files *
		sizeof(struct stratum_default),"default_object_list",
		"construct_stratum_defaults" );

	/*--------------------------------------------------------------*/
	/*	Loop through the default files list.			*/
	/*--------------------------------------------------------------*/
	for (i=0 ; i<num_default_files; i++) {
		epc = &(default_object_list[i].epc);

		/*--------------------------------------------------------------*/
		/*		Try to open the ith default file.		*/
		/*--------------------------------------------------------------*/
		printf("Reading %s\n", default_files[i]);
                paramCnt = 0;
                if (paramPtr != NULL)
                    free(paramPtr);

                paramPtr = readParamFile(&paramCnt, default_files[i]);
		/*--------------------------------------------------------------*/
		/*		read the ith default file into the ith object.			*/
		/*--------------------------------------------------------------*/
		default_object_list[i].ID = getIntParam(&paramCnt, &paramPtr, "stratum_default_ID", "%d", 7, 1); // new param name
		default_object_list[i].epc.veg_type = 		parse_veg_type(getStrParam(&paramCnt, &paramPtr, "epc.veg.type", "%s", "TREE", 1)); // param name is "epc.veg.type" in param file
		default_object_list[i].K_absorptance = 		getDoubleParam(&paramCnt, &paramPtr, "K_absorptance", "%lf", 0.8, 1); // parameter misspelled in file as "K_apsorbtance"
		default_object_list[i].K_reflectance = 		getDoubleParam(&paramCnt, &paramPtr, "K_reflectance", "%lf", 0.1, 1);
		default_object_list[i].K_transmittance = 	getDoubleParam(&paramCnt, &paramPtr, "K_transmittance", "%lf", 0.1, 1); 
		default_object_list[i].PAR_absorptance = 	getDoubleParam(&paramCnt, &paramPtr, "PAR_absorptance", "%lf", 1.0, 1); // param misspelled in file "PAR_absrptance" 
		default_object_list[i].PAR_reflectance =  	getDoubleParam(&paramCnt, &paramPtr, "PAR_reflectance", "%lf", 0.0, 1);
		default_object_list[i].PAR_transmittance = 	getDoubleParam(&paramCnt, &paramPtr, "PAR_transmittance", "%lf", 0.0, 1);
		default_object_list[i].epc.ext_coef = 		getDoubleParam(&paramCnt, &paramPtr, "epc.ext_coef", "%lf", 0.5, 1);
		default_object_list[i].specific_rain_capacity = getDoubleParam(&paramCnt, &paramPtr, "specific_rain_capacity", "%lf", 0.00024, 1);
		default_object_list[i].specific_snow_capacity = getDoubleParam(&paramCnt, &paramPtr, "specific_snow_capacity", "%lf", 0.00024, 1);
		default_object_list[i].wind_attenuation_coeff = getDoubleParam(&paramCnt, &paramPtr, "wind_attenuation_coef", "%lf", 0.4, 1); // param name is "wind_attenuation_coef" in param file
		default_object_list[i].ustar_overu = 		getDoubleParam(&paramCnt, &paramPtr, "ustar_overu", "%lf", -999.9, 1);
		default_object_list[i].mrc.q10 = 		getDoubleParam(&paramCnt, &paramPtr, "mrc.q10", "%lf", 1.5, 1);
		default_object_list[i].mrc.per_N = 		getDoubleParam(&paramCnt, &paramPtr, "mrc.per_N", "%lf", 0.21, 1);
		default_object_list[i].epc.gr_perc = 		getDoubleParam(&paramCnt, &paramPtr, "epc.gr_perc", "%lf", 0.2, 1);
		default_object_list[i].lai_stomatal_fraction = 	getDoubleParam(&paramCnt, &paramPtr, "lai_stomatal_fraction", "%lf", 1.0, 1);
		default_object_list[i].epc.flnr = 		getDoubleParam(&paramCnt, &paramPtr, "epc.flnr", "%lf", 0.1, 1);
		default_object_list[i].epc.ppfd_coef = 		getDoubleParam(&paramCnt, &paramPtr, "epc.ppfd_coef", "%lf", 0.03, 1);
		default_object_list[i].epc.topt = 		getDoubleParam(&paramCnt, &paramPtr, "epc.topt", "%lf", 15.0, 1);
		default_object_list[i].epc.tmax = 		getDoubleParam(&paramCnt, &paramPtr, "epc.tmax", "%lf", 40.0, 1);
		default_object_list[i].epc.tcoef = 		getDoubleParam(&paramCnt, &paramPtr, "epc.tcoef", "%lf", 0.2, 1);
		default_object_list[i].epc.psi_open = 		getDoubleParam(&paramCnt, &paramPtr, "epc.psi_open", "%lf", -0.65, 1);
		default_object_list[i].epc.psi_close = 		getDoubleParam(&paramCnt, &paramPtr, "epc.psi_close", "%lf", -2.5, 1);
		default_object_list[i].epc.vpd_open = 		getDoubleParam(&paramCnt, &paramPtr, "epc.vpd_open", "%lf", 0.0, 1);
		default_object_list[i].epc.vpd_close = 		getDoubleParam(&paramCnt, &paramPtr, "epc.vpd_close", "%lf", 3500.0, 1);
		default_object_list[i].epc.gl_smax = 		getDoubleParam(&paramCnt, &paramPtr, "epc.gl_smax", "%lf", 0.006, 1);
		default_object_list[i].epc.gl_c = 		getDoubleParam(&paramCnt, &paramPtr, "epc.gl_c", "%lf", 0.00006, 1);
		default_object_list[i].gsurf_slope = 		getDoubleParam(&paramCnt, &paramPtr, "gsurf_slope", "%lf", 0.0, 1);
		default_object_list[i].gsurf_intercept = 	getDoubleParam(&paramCnt, &paramPtr, "gsurf_intercept", "%lf", 1000000.0, 1);
		default_object_list[i].epc.phenology_flag = 	parse_dyn_flag(getStrParam(&paramCnt, &paramPtr, "epc.phenology_flag", "%s", "static", 1));
		default_object_list[i].epc.phenology_type = 	parse_phenology_type(getStrParam(&paramCnt, &paramPtr, "epc.phenology.type", "%s", "EVERGREEN", 1));
		default_object_list[i].epc.max_lai = 		getDoubleParam(&paramCnt, &paramPtr, "epc.max_lai", "%lf", 12.0, 1);
		default_object_list[i].epc.proj_sla = 		getDoubleParam(&paramCnt, &paramPtr, "epc.proj_sla", "%lf", 9.0, 1);
		default_object_list[i].epc.lai_ratio = 		getDoubleParam(&paramCnt, &paramPtr, "epc.lai_ratio", "%lf", 2.6, 1);
		default_object_list[i].epc.proj_swa = 		getDoubleParam(&paramCnt, &paramPtr, "epc.proj_swa", "%lf", 1.4, 1);
		default_object_list[i].epc.leaf_turnover = 	getDoubleParam(&paramCnt, &paramPtr, "epc.leaf_turnover", "%lf", 0.27, 1);
		default_object_list[i].epc.day_leafon = 	getIntParam(&paramCnt, &paramPtr, "epc.day_leafon", "%d", 91, 1);
		default_object_list[i].epc.day_leafoff = 	getIntParam(&paramCnt, &paramPtr, "epc.day_leafoff", "%d", 260, 1);
		default_object_list[i].epc.ndays_expand = 	getIntParam(&paramCnt, &paramPtr, "epc.ndays_expand", "%d", 30, 1);
		default_object_list[i].epc.ndays_litfall = 	getIntParam(&paramCnt, &paramPtr, "epc.ndays_litfall", "%d", 30, 1);
		default_object_list[i].epc.leaf_cn = 		getDoubleParam(&paramCnt, &paramPtr, "epc.leaf_cn", "%lf", 45.0, 1);
		default_object_list[i].epc.leaflitr_cn = 	getDoubleParam(&paramCnt, &paramPtr, "epc.leaflitr_cn", "%lf", 70.0, 1);
		default_object_list[i].min_heat_capacity = 	getDoubleParam(&paramCnt, &paramPtr, "min_heat_capacity", "%lf", 0.0, 1);
		default_object_list[i].max_heat_capacity = 	getDoubleParam(&paramCnt, &paramPtr, "max_heat_capacity", "%lf", 0.0, 1);
		default_object_list[i].epc.allocation_flag = 	parse_alloc_flag(getStrParam(&paramCnt, &paramPtr, "epc.allocation_flag", "%s", "waring", 1));
		/*--------------------------------------------------------------*/
		/*          NOTE: PLACE ANY GROW READING HERE.                  */
		/*--------------------------------------------------------------*/
		if ( (default_object_list[i].epc.leaflitr_cn <
			default_object_list[i].epc.leaf_cn) && (command_line[0].grow_flag > 0) ){
			fprintf(stderr, "\nWARNING construct_stratum_defaults");
			fprintf(stderr, "\n  leaf litter C:N < leaf C:N");
		}
		default_object_list[i].epc.storage_transfer_prop = getDoubleParam(&paramCnt, &paramPtr, "epc.storage_transfer_prop", "%lf", 1.0, 1);
		default_object_list[i].epc.froot_turnover = getDoubleParam(&paramCnt, &paramPtr, "epc.froot_turnover", "%lf", 0.27, 1);

		if  ((default_object_list[i].epc.veg_type == GRASS) || (default_object_list[i].epc.veg_type == C4GRASS)) {
			default_object_list[i].epc.deadleaf_turnover = getDoubleParam(&paramCnt, &paramPtr, "epc.deadleaf_turnover", "%lf", 1.0, 1);
		}
		else {
			default_object_list[i].epc.livewood_turnover = getDoubleParam(&paramCnt, &paramPtr, "epc.livewood_turnover", "%lf", 0.7, 1);
		}

		default_object_list[i].epc.kfrag_base = 		getDoubleParam(&paramCnt, &paramPtr, "epc.kfrag_base", "%lf", 0.01, 1);

		default_object_list[i].epc.max_daily_mortality = getDoubleParam(&paramCnt, &paramPtr, "epc.max_daily_mortality", "%lf", 0.005, 1) / 365;
		default_object_list[i].epc.min_daily_mortality = getDoubleParam(&paramCnt, &paramPtr, "epc.min_daily_mortality", "%lf", 0.005, 1) / 365;
		default_object_list[i].epc.daily_mortality_threshold = getDoubleParam(&paramCnt, &paramPtr, "epc.daily_mortality_threshold", "%lf",0.0,1);
		default_object_list[i].epc.froot_cn = 			getDoubleParam(&paramCnt, &paramPtr, "epc.froot_cn", "%lf", 139.7, 1);
		default_object_list[i].epc.livewood_cn = getDoubleParam(&paramCnt, &paramPtr, "epc.livewood_cn", "%lf", 200.0, 1);
		default_object_list[i].epc.leaflitr_flab = getDoubleParam(&paramCnt, &paramPtr, "epc.leaflitr_flab", "%lf", 0.31, 1);

		// Skipping epc.leaflitr_fcel (pcs 20130117 : comment or var name wrong in orig code: defs/veg_westhemlock.def has 'leaflitr_fcel')
		fcel = getDoubleParam(&paramCnt, &paramPtr, "epc.leaflitr_fcel", "%lf", 0.45, 1); // param name in file is "leaflitr_fcel"

		default_object_list[i].epc.leaflitr_flig = getDoubleParam(&paramCnt, &paramPtr, "epc.leaflitr_flig", "%lf", 0.24, 1);

		if ( (float)(epc->leaflitr_flig + epc->leaflitr_flab + fcel) != 1.0 )	{
			fprintf(stderr,"\nFATAL ERROR construct_stratum_defaults");
			fprintf(stderr,"\n  litter proportions of labile, cell. and lignin must sum to 1.0");
			fprintf(stderr," \n for default ID %d \n", default_object_list[i].ID);
			exit(EXIT_FAILURE);
		}
		lig_cel_ratio = epc->leaflitr_flig/fcel;
		/* calculate shielded and unshielded cellulose fraction */
		if (lig_cel_ratio < 0.45){
			epc->leaflitr_fscel = 0.0;
			epc->leaflitr_fucel = fcel;
		}
		else{
			if ((lig_cel_ratio > 0.45) && (lig_cel_ratio < 0.7)){
				epc->leaflitr_fscel= (lig_cel_ratio - 0.45)*3.2*fcel;
				epc->leaflitr_fucel = (1.0 - 3.2*(lig_cel_ratio - 0.45))*fcel;
			}
			else {
				epc->leaflitr_fscel = 0.8*fcel;
				epc->leaflitr_fucel = 0.2*fcel;
			}
		}
		default_object_list[i].epc.frootlitr_flab = getDoubleParam(&paramCnt, &paramPtr, "epc.frootlitr_flab", "%lf", 0.23, 1);

		// Skipping the mortality parameter
		fcel = getDoubleParam(&paramCnt, &paramPtr, "epc.frootlitr_fcel", "%lf", 0.41, 1);
		default_object_list[i].epc.frootlitr_flig = getDoubleParam(&paramCnt, &paramPtr, "epc.frootlitr_flig", "%lf", 0.36, 1);

		if ( (float)(epc->frootlitr_flab + epc->frootlitr_flig + fcel) != 1.0 ){
			fprintf(stderr,"\nFATAL ERROR construct_stratum_defaults");
			fprintf(stderr,"\n  froot litter proportions of labile, cell. and lignin must sum to 1.0\n");
			exit(EXIT_FAILURE);
		}
		lig_cel_ratio = epc->frootlitr_flig/fcel;
		/* calculate shielded and unshielded cellulose fraction */
		if (lig_cel_ratio < 0.45) {
			epc->frootlitr_fscel = 0.0;
			epc->frootlitr_fucel = fcel;
		}
		else{
			if ((lig_cel_ratio > 0.45) && (lig_cel_ratio < 0.7)){
				epc->frootlitr_fscel= (lig_cel_ratio - 0.45)*3.2*fcel;
				epc->frootlitr_fucel = (1.0 - 3.2*(lig_cel_ratio - 0.45))*fcel;
			}
			else{
				epc->frootlitr_fscel = 0.8*fcel;
				epc->frootlitr_fucel = 0.2*fcel;
			}
		}
		fcel = getDoubleParam(&paramCnt, &paramPtr, "epc.deadwood_fcel", "%lf", 0.52, 1);
		default_object_list[i].epc.deadwood_flig = getDoubleParam(&paramCnt, &paramPtr, "epc.deadwood_flig", "%lf", 0.48, 1);
		if (epc->veg_type == TREE) {
			if ( (float)(epc->deadwood_flig + fcel) != 1.0 ){
				fprintf(stderr,"\nFATAL ERROR construct_stratum_defaults");
				fprintf(stderr,"\n  dead wood proportions of labile, cell. and lignin must sum to 1.0\n");
				exit(EXIT_FAILURE);
			}
			lig_cel_ratio = epc->deadwood_flig/fcel;
			/* calculate shielded and unshielded cellulose fraction */
			if (lig_cel_ratio < 0.45) {
				epc->deadwood_fscel = 0.0;
				epc->deadwood_fucel = fcel;
			}
			else{
				if ((lig_cel_ratio > 0.45) && (lig_cel_ratio < 0.7)){
					epc->deadwood_fscel= (lig_cel_ratio - 0.45)*3.2*fcel;
					epc->deadwood_fucel = (1.0 - 3.2*(lig_cel_ratio - 0.45))*fcel;
				}
				else{
					epc->deadwood_fscel = 0.8*fcel;
					epc->deadwood_fucel = 0.2*fcel;
				}
			}
				/*
			epc->deadwood_cn = ((epc->deadwood_fucel + epc->deadwood_fscel
				+ epc->deadwood_flig) * CEL_CN * LIG_CN )
				/ (LIG_CN * (epc->deadwood_fucel + epc->deadwood_fscel)
				+ CEL_CN * epc->deadwood_flig);
			if (epc->deadwood_cn < epc->livewood_cn){
				fprintf(stderr,"\nFATAL ERROR: construct_canopy_stratum");
				fprintf(stderr,"\ndeadwood C:N must be > livewood C:N");
				exit(EXIT_FAILURE); 
			}
				*/
			epc->deadwood_cn = (epc->deadwood_fucel + epc->deadwood_fscel) * CEL_CN
					+ (epc->deadwood_flig) * LIG_CN;

		} /* end if tree */
		else {
			epc->deadwood_flig = 0.0;
			epc->deadwood_fucel = 0.0;
			epc->deadwood_fscel = 0.0;
			epc->deadwood_cn = 0.0;
		}
		default_object_list[i].epc.alloc_frootc_leafc 	 = getDoubleParam(&paramCnt, &paramPtr, "epc.alloc_frootc_leafc", "%lf", 1.325, 1);
		default_object_list[i].epc.alloc_crootc_stemc 	 = getDoubleParam(&paramCnt, &paramPtr, "epc.alloc_crootc_stemc", "%lf", 0.3, 1);
		default_object_list[i].epc.alloc_stemc_leafc 	 = getDoubleParam(&paramCnt, &paramPtr, "epc.alloc_stemc_leafc", "%lf", 1.62, 1);
		default_object_list[i].epc.alloc_livewoodc_woodc = getDoubleParam(&paramCnt, &paramPtr, "epc.alloc_livewoodc_woodc", "%lf", 0.073, 1);
		if (epc->veg_type != TREE){
			epc->alloc_crootc_stemc = 0.0;
			epc->phloemcsa_per_alllai = 0.0;
		}
		default_object_list[i].epc.alloc_maxlgf 	 = getDoubleParam(&paramCnt, &paramPtr, "epc.maxlgf", "%lf", 0.05, 1); // param named 'epc.maxlgf' in parameter file
		default_object_list[i].epc.alloc_prop_day_growth = getDoubleParam(&paramCnt, &paramPtr, "epc.alloc_prop_day_growth", "%lf", 0.5, 1);
		default_object_list[i].epc.dynamic_alloc_prop_day_growth = getIntParam(&paramCnt, &paramPtr, "epc.dyn_alloc_prop_day_growth", "%d", 0, 1);
		default_object_list[i].epc.daily_fire_turnover 	 = getDoubleParam(&paramCnt, &paramPtr, "epc.daily_fire_turnover", "%lf", 0.0, 1);
		default_object_list[i].epc.height_to_stem_exp 	 = getDoubleParam(&paramCnt, &paramPtr, "epc.height_to_stem_exp", "%lf", 0.57, 1);
		default_object_list[i].epc.height_to_stem_coef 	 = getDoubleParam(&paramCnt, &paramPtr, "epc.height_to_stem_coef", "%lf", 11.39, 1);
		/*--------------------------------------------------------------*/
		/*	optionally read in parameters on re-sprouting		*/
		/* 	and other newly implemented vegetation routines		*/
		/*--------------------------------------------------------------*/
		default_object_list[i].epc.min_leaf_carbon = getDoubleParam(&paramCnt, &paramPtr, "epc.min_leaf_carbon", "%lf", 0.0005, 1);
		default_object_list[i].epc.max_years_resprout = getIntParam(&paramCnt, &paramPtr, "epc.max_years_resprout", "%d", 100, 1);
		default_object_list[i].epc.resprout_leaf_carbon = getDoubleParam(&paramCnt, &paramPtr, "epc.resprout_leaf_carbon", "%lf", 0.001, 1);
		default_object_list[i].epc.litter_gsurf_slope = getDoubleParam(&paramCnt, &paramPtr, "epc.litter_gsurf_slope", "%lf", 0.0, 1);
		default_object_list[i].epc.litter_gsurf_intercept = getDoubleParam(&paramCnt, &paramPtr, "epc.litter_gsurf_intercept", "%lf", 100000000, 1);
		default_object_list[i].epc.coef_CO2 = getDoubleParam(&paramCnt, &paramPtr, "epc.coef_CO2", "%lf", 1.0, 1);
		default_object_list[i].epc.root_growth_direction = getDoubleParam(&paramCnt, &paramPtr, "epc.root_growth_direction", "%lf", 0.8, 1);
		default_object_list[i].epc.root_distrib_parm = getDoubleParam(&paramCnt, &paramPtr, "epc.root_distrib_parm", "%lf", 8.0, 1);
		default_object_list[i].epc.crown_ratio = getDoubleParam(&paramCnt, &paramPtr, "epc.crown_ratio", "%lf", 0.6, 1);
		if (epc->veg_type != TREE)
			default_object_list[i].epc.crown_ratio = 1.0;
		
		/*--------------------------------------------------------------*/
		/* default values for phenology (leaf onset/offset) model parameters */
		/* are set based on Jolly et al., 2005, Global Change Biology   */
		/* who defined a globally uniform parameter set					*/
		/*--------------------------------------------------------------*/
		default_object_list[i].epc.gs_tmin = getDoubleParam(&paramCnt, &paramPtr, "epc.gs_tmin", "%lf", -2.0, 1);
		default_object_list[i].epc.gs_tmax = getDoubleParam(&paramCnt, &paramPtr, "epc.gs_tmax", "%lf", 5.0, 1);
		default_object_list[i].epc.gs_vpd_min = getDoubleParam(&paramCnt, &paramPtr, "epc.gs_vpd_min", "%lf", 900, 1);
		default_object_list[i].epc.gs_vpd_max = getDoubleParam(&paramCnt, &paramPtr, "epc.gs_vpd_max", "%lf", 4100, 1);

	        default_object_list[i].epc.gs_dayl_min = getDoubleParam(&paramCnt, &paramPtr, "epc.gs_dayl_min", "%lf", 36000, 1);
		default_object_list[i].epc.gs_dayl_max = getDoubleParam(&paramCnt, &paramPtr, "epc.gs_dayl_max", "%lf", 39600, 1);
	        default_object_list[i].epc.gs_psi_min = getDoubleParam(&paramCnt, &paramPtr, "epc.gs_psi_min", "%lf", -15.0, 1);
		default_object_list[i].epc.gs_psi_max = getDoubleParam(&paramCnt, &paramPtr, "epc.gs_psi_max", "%lf", -14.0, 1);
		default_object_list[i].epc.gs_ravg_days = getDoubleParam(&paramCnt, &paramPtr, "epc.gs_ravg_days", "%lf", 6, 1);
		default_object_list[i].epc.max_storage_percent = getDoubleParam(&paramCnt, &paramPtr, "epc.max_storage_percent", "%lf", 0.2, 1);
		default_object_list[i].epc.min_percent_leafg = getDoubleParam(&paramCnt, &paramPtr, "epc.min_percent_leafg", "%lf", default_object_list[i].epc.leaf_turnover, 1);
		default_object_list[i].epc.dickenson_pa = getDoubleParam(&paramCnt, &paramPtr, "epc.dickenson_pa", "%lf", 0.25, 1);
		default_object_list[i].epc.waring_pa = getDoubleParam(&paramCnt, &paramPtr, "epc.waring_pa", "%lf", 0.8, 1);
		default_object_list[i].epc.waring_pb = getDoubleParam(&paramCnt, &paramPtr, "epc.waring_pb", "%lf", 2.5, 1);
		default_object_list[i].epc.branch_turnover = getDoubleParam(&paramCnt, &paramPtr, "epc.branch_turnover", "%lf", 0.0, 1) / 365.0;
		default_object_list[i].epc.Tacclim = getIntParam(&paramCnt, &paramPtr, "epc.Tacclim", "%d", 0, 1);
		default_object_list[i].epc.Tacclim_intercpt = getDoubleParam(&paramCnt, &paramPtr, "epc.Tacclim_intercpt", "%lf",3.22, 1);
		default_object_list[i].epc.Tacclim_slp = getDoubleParam(&paramCnt, &paramPtr, "epc.Tacclim_slp", "%lf",0.046, 1);
		default_object_list[i].epc.Tacclim_days = getIntParam(&paramCnt, &paramPtr, "epc.Tacclim_days", "%d", 30, 1);
	/*--------------------------------------------------------------*/
	/*	 litter is assumed to have a mositure capacity of 	*/
	/*	given by litter_moist_coef default assumes			*/
	/*	200% moisture content by weight following		*/
	/*	Helvey, 1964 (deciduous forest in Coweeta) and 		*/
	/*	Ogee and Brunet (2002) J of Hydrology, for a pine	*/
	/*	forest - 						*/
	/* 	so capacity in m is 2*litter carbon * 2(carbon to biomass */
	/*		 / density of water				*/
	/* 	similarly for litter depth but we assume an organic 	*/
	/* 	matter density						*/
	/*--------------------------------------------------------------*/
		default_object_list[i].epc.litter_moist_coef = getDoubleParam(&paramCnt, &paramPtr, "epc.litter_moist_coef", "%lf", 2.0/1000.0, 1);
		default_object_list[i].epc.litter_density = getDoubleParam(&paramCnt, &paramPtr, "epc.litter_density", "%lf", 100.0/2.0, 1);
		default_object_list[i].epc.gs_psi_range = default_object_list[i].epc.gs_psi_max-default_object_list[i].epc.gs_psi_min;
		default_object_list[i].epc.gs_dayl_range = default_object_list[i].epc.gs_dayl_max-default_object_list[i].epc.gs_dayl_min;
		default_object_list[i].epc.gs_vpd_range = default_object_list[i].epc.gs_vpd_max-default_object_list[i].epc.gs_vpd_min;
		default_object_list[i].epc.gs_trange = default_object_list[i].epc.gs_tmax-default_object_list[i].epc.gs_tmin;

		/*--------------------------------------------------------------*/
		/* plant type defaults - are you an nfixer - are you edible 	*/
		/*--------------------------------------------------------------*/
		default_object_list[i].epc.nfix = 	getIntParam(&paramCnt, &paramPtr, "epc.nfix", "%d", 0, 1);
		default_object_list[i].epc.edible = 	getIntParam(&paramCnt, &paramPtr, "epc.edible", "%d", 1, 1);
		default_object_list[i].epc.psi_curve  = 	getIntParam(&paramCnt, &paramPtr, "epc.psi_curve", "%d", 0, 1);
		default_object_list[i].epc.psi_threshold =	getDoubleParam(&paramCnt, &paramPtr, "epc.psi_threshold", "%lf", -1, 1);
		default_object_list[i].epc.psi_slp =	getDoubleParam(&paramCnt, &paramPtr, "epc.psi_slp", "%lf", 0.2, 1);
		default_object_list[i].epc.psi_intercpt =	getDoubleParam(&paramCnt, &paramPtr, "epc.psi_intercpt", "%lf", 1.0, 1);

		/*--------------------------------------------------------------*/
		/* set sunlit sla multiplier	this should be an input		*/
		/*--------------------------------------------------------------*/
		default_object_list[i].epc.shade_sla_mult = 1.0;

		/*--------------------------------------------------------------*/
		/*	Apply sensitivity analysis if appropriate		*/
		/*--------------------------------------------------------------*/
		
		if (command_line[0].vgsen_flag == 1) {
			default_object_list[i].epc.proj_sla *= command_line[0].veg_sen1;
			default_object_list[i].epc.shade_sla_mult *= command_line[0].veg_sen2;
		}

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
                strcat(outFilename, "_stratum.params");
            } 
            else {
                if (filename != NULL) {
                    strcat(outFilename, "_");
                    strcat(outFilename, filename);
                }
                strcat(outFilename, "stratum.params");
            }
    
            printParams(paramCnt, paramPtr, outFilename);
	} /*end for*/

                if (paramPtr != NULL)
                    free(paramPtr);
		    
	return(default_object_list);
} /*end construct_stratum_defaults*/
