/*--------------------------------------------------------------*/
/* 																*/
/*					construct_stratum_defaults					*/
/*																*/
/*	construct_stratum_defaults.c - makes stratum default		*/
/*										objects.				*/
/*																*/
/*	NAME														*/
/*	construct_stratum_defaults.c - makes stratum default		*/
/*										objects.				*/
/*																*/
/*	SYNOPSIS													*/
/*	struct stratum_default *construct_stratum_defaults(         */
/*									num_default_files,			*/
/*								  default_files,				*/
/*								  grow_flag,					*/
/*								  default_object_list )			*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*	Original code, January 15, 1996.							*/
/*																*/
/* March 12, 1997   C. Tague								    */
/* added seasonal leafon/off ramp variable		  				*/
/*																*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"
#include "phys_constants.h"

struct stratum_default *construct_stratum_defaults(
												   int	num_default_files,
												   char	**default_files,
												   struct command_line_object *command_line)
												   
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.									*/
	/*--------------------------------------------------------------*/
	void	*alloc(	size_t, char *, char *);
	int	parse_veg_type( char *);
	int	parse_phenology_type( char *);
	int	parse_dyn_flag( char *);
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	int		i, itmp;
	char		record[MAXSTR];
	char		*newrecord;
	char		stmp[MAXSTR];
	double		fcel, ftmp, lig_cel_ratio;
	FILE	*default_file;
	struct stratum_default	*default_object_list;
	struct	epconst_struct	*epc;
	
	/*--------------------------------------------------------------*/
	/*	Allocate an array of default objects.						*/
	/*--------------------------------------------------------------*/
	default_object_list = (struct stratum_default *)
		alloc(num_default_files *
		sizeof(struct stratum_default),"default_object_list",
		"construct_stratum_defaults" );
	
	/*--------------------------------------------------------------*/
	/*	Loop through the default files list.						*/
	/*--------------------------------------------------------------*/
	for (i=0 ; i<num_default_files; i++) {
		epc = &(default_object_list[i].epc);
		/*--------------------------------------------------------------*/
		/*		Try to open the ith default file.						*/
		/*--------------------------------------------------------------*/
		if ( (default_file = fopen( default_files[i], "r")) == NULL ){
			fprintf(stderr,
				"FATAL ERROR:in construct_stratum_defaults,unable to open defaults file %d. - %s\n",
				i, default_files[i]);
			exit(0);
		} /*end if*/
	
		printf("\n Reading %s", default_files[i]);
		/*--------------------------------------------------------------*/
		/*		read the ith default file into the ith object.			*/
		/*--------------------------------------------------------------*/
		fscanf(default_file,"%d",&(default_object_list[i].ID));
		read_record(default_file, record);
		fscanf(default_file,"%s",stmp);
		read_record(default_file, record);
		default_object_list[i].epc.veg_type = parse_veg_type(stmp);
		fscanf(default_file,"%lf",&(default_object_list[i].K_absorptance));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].K_reflectance));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].K_transmittance));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].PAR_absorptance));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].PAR_reflectance));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].PAR_transmittance));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].epc.ext_coef));
		read_record(default_file, record);
		fscanf(default_file,"%lf",
			&(default_object_list[i].specific_rain_capacity));
		read_record(default_file, record);
		fscanf(default_file,"%lf",
			&(default_object_list[i].specific_snow_capacity));
		read_record(default_file, record);
		fscanf(default_file,"%lf",
			&(default_object_list[i].wind_attenuation_coeff));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].ustar_overu));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].mrc.q10));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].mrc.per_N));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].epc.gr_perc));
		read_record(default_file, record);
		fscanf(default_file,"%lf",
			&(default_object_list[i].lai_stomatal_fraction));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].epc.flnr));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].epc.ppfd_coef));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].epc.topt));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].epc.tmax));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].epc.tcoef));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].epc.psi_open));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].epc.psi_close));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].epc.vpd_open));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].epc.vpd_close));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].epc.gl_smax));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].epc.gl_c));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].gsurf_slope));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].gsurf_intercept));
		read_record(default_file, record);
		fscanf(default_file,"%s",stmp);
		read_record(default_file, record);
		default_object_list[i].epc.phenology_flag = parse_dyn_flag(stmp);
		fscanf(default_file,"%s",stmp);
		read_record(default_file, record);
		default_object_list[i].epc.phenology_type = parse_phenology_type(stmp);
		fscanf(default_file,"%lf",&(default_object_list[i].epc.max_lai));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].epc.proj_sla));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].epc.lai_ratio));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].epc.proj_swa));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].epc.leaf_turnover));
		read_record(default_file, record);
		fscanf(default_file,"%d",&(default_object_list[i].epc.day_leafon));
		read_record(default_file, record);
		fscanf(default_file,"%d",&(default_object_list[i].epc.day_leafoff));
		read_record(default_file, record);
		fscanf(default_file,"%d",&(default_object_list[i].epc.ndays_expand));
		read_record(default_file, record);
		fscanf(default_file,"%d",&(default_object_list[i].epc.ndays_litfall));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].epc.leaf_cn));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].epc.leaflitr_cn));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].min_heat_capacity));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].max_heat_capacity));
		read_record(default_file, record);
		fscanf(default_file,"%s",stmp);
		read_record(default_file, record);
		default_object_list[i].epc.allocation_flag = parse_dyn_flag(stmp);
		/*--------------------------------------------------------------*/
		/*          NOTE: PLACE ANY GROW READING HERE.                  */
		/*--------------------------------------------------------------*/
		if ( (default_object_list[i].epc.leaflitr_cn <
			default_object_list[i].epc.leaf_cn) && (command_line[0].grow_flag > 0) ){
			fprintf(stderr, "\nWARNING construct_stratum_defaults");
			fprintf(stderr, "\n  leaf litter C:N < leaf C:N");
		}
		fscanf(default_file,"%lf",
			&(default_object_list[i].epc.storage_transfer_prop));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].epc.froot_turnover));
		read_record(default_file, record);
		if  ((default_object_list[i].epc.veg_type == GRASS) || (default_object_list[i].epc.veg_type == C4GRASS)) {
			fscanf(default_file,"%lf",
				&(default_object_list[i].epc.deadleaf_turnover));
			read_record(default_file, record);
		}
		else {
			fscanf(default_file,"%lf",
				&(default_object_list[i].epc.livewood_turnover));
			read_record(default_file, record);
		}
		fscanf(default_file,"%lf",&(default_object_list[i].epc.kfrag_base));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(ftmp));
		read_record(default_file, record);
		default_object_list[i].epc.daily_mortality_turnover = ftmp/365;
		fscanf(default_file,"%lf",&(default_object_list[i].epc.froot_cn));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].epc.livewood_cn));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].epc.leaflitr_flab));

		// Skipping epc.leaflitr_fcel
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(fcel));

		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].epc.leaflitr_flig));
		read_record(default_file, record);
		if ( (float)(epc->leaflitr_flig + epc->leaflitr_flab + fcel) != 1.0 )	{
			fprintf(stderr,"\nFATAL ERROR construct_stratum_defaults");
			fprintf(stderr,"\n  litter proportions of labile, cell. and lignin must sum to 1.0");
			fprintf(stderr," \n for default ID %d \n", default_object_list[i].ID);
			exit(0);
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
		fscanf(default_file,"%lf",&(default_object_list[i].epc.frootlitr_flab));
		read_record(default_file, record);

		// Skipping the mortality parameter
		fscanf(default_file,"%lf",&(fcel));
		read_record(default_file, record);

		fscanf(default_file,"%lf",&(default_object_list[i].epc.frootlitr_flig));
		read_record(default_file, record);
		if ( (float)(epc->frootlitr_flab + epc->frootlitr_flig + fcel) != 1.0 ){
			fprintf(stderr,"\nFATAL ERROR construct_stratum_defaults");
			fprintf(stderr,"\n  froot litter proportions of labile, cell. and lignin must sum to 1.0");
			exit(0);
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
		fscanf(default_file,"%lf",&(fcel));
		read_record(default_file, record);
		fscanf(default_file,"%lf",&(default_object_list[i].epc.deadwood_flig));
		read_record(default_file, record);
		if (epc->veg_type == TREE) {
			if ( (float)(epc->deadwood_flig + fcel) != 1.0 ){
				fprintf(stderr,"\nFATAL ERROR construct_stratum_defaults");
				fprintf(stderr,"\n  dead wood proportions of labile, cell. and lignin must sum to 1.0");
				exit(0);
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
				exit(0); 
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
		fscanf(default_file,"%lf",
			&(default_object_list[i].epc.alloc_frootc_leafc));
		read_record(default_file, record);
		fscanf(default_file,"%lf",
			&(default_object_list[i].epc.alloc_crootc_stemc));
		read_record(default_file, record);
		fscanf(default_file,"%lf",
			&(default_object_list[i].epc.alloc_stemc_leafc));
		read_record(default_file, record);
		fscanf(default_file,"%lf",
			&(default_object_list[i].epc.alloc_livewoodc_woodc));
		read_record(default_file, record);
		if (epc->veg_type != TREE){
			epc->alloc_crootc_stemc = 0.0;
			epc->phloemcsa_per_alllai = 0.0;
		}
		fscanf(default_file,"%lf",&(default_object_list[i].epc.alloc_maxlgf));
		read_record(default_file, record);
		fscanf(default_file,"%lf",
			&(default_object_list[i].epc.alloc_prop_day_growth));
		read_record(default_file, record);
		fscanf(default_file,"%lf",
			&(default_object_list[i].epc.daily_fire_turnover));
		read_record(default_file, record);
		fscanf(default_file,"%lf",
			&(default_object_list[i].epc.height_to_stem_exp));
		read_record(default_file, record);
		fscanf(default_file,"%lf",
			&(default_object_list[i].epc.height_to_stem_coef));
		read_record(default_file, record);
		/*--------------------------------------------------------------*/
		/*	optionally read in parameters on re-sprouting		*/
		/* 	and other newly implemented vegetation routines		*/
		/*--------------------------------------------------------------*/
		default_object_list[i].epc.min_leaf_carbon = 0.0005;
		default_object_list[i].epc.max_years_resprout = 100;
		default_object_list[i].epc.resprout_leaf_carbon = 0.001;
		default_object_list[i].epc.litter_gsurf_slope = 0.0;
		default_object_list[i].epc.litter_gsurf_intercept = 100000000;
		default_object_list[i].epc.coef_CO2 = 1.0;
		default_object_list[i].epc.root_growth_direction = 0.8;
		default_object_list[i].epc.root_distrib_parm = 8.0;
		/*--------------------------------------------------------------*/
		/* default values for phenology (leaf onset/offset) model parameters */
	    /* are set based on Jolly et al., 2005, Global Change Biology   */
		/* who defined a globally uniform parameter set					*/
		/*--------------------------------------------------------------*/
		default_object_list[i].epc.gs_tmin = -2.0;
		default_object_list[i].epc.gs_tmax = 5.0;
		default_object_list[i].epc.gs_vpd_min = 900;
		default_object_list[i].epc.gs_vpd_max = 4100;

	        default_object_list[i].epc.gs_dayl_min = 36000;
		default_object_list[i].epc.gs_dayl_max = 39600;
		default_object_list[i].epc.max_storage_percent = 0.2;
		default_object_list[i].epc.min_percent_leafg = default_object_list[i].epc.leaf_turnover; 
		default_object_list[i].epc.dickenson_pa = 0.25;
		default_object_list[i].epc.waring_pa = 0.8;
		default_object_list[i].epc.waring_pb = 2.5;
	/*--------------------------------------------------------------*/
	/*	 litter is assumed to have a mositure capacity of 	*/
	/*	given by litter_moist_coef default assumes			*/
	/*	200% moisture content by weight following		*/
	/*	Helvey, 1964 (deciduous forest in Coweeta) and 		*/
	/*	Ogee and Brunet (2002) J of Hydrology, for a pine	*/
	/*	forest - 						*/
	/* 	so capacity in m is 2*litter carbon * 2(carbon to biomass */
	/*		 / density of water				*/
	/*--------------------------------------------------------------*/
		default_object_list[i].epc.litter_moist_coef = 2.0/1000.0;
		while (!feof(default_file)) {
			fscanf(default_file,"%lf", &(ftmp));
			read_record(default_file, record);
			// This strchr splitting method works since all tagged
			// entries are labled epc.*.  This must change before
			// we can make the whole file tagged
			newrecord = strchr(record,'e');
			if (newrecord != NULL) {
			if (strcasecmp(newrecord,"epc.dickenson_pa") == 0) {	
				default_object_list[i].epc.dickenson_pa = ftmp;
				printf("\n Using %lf for %s for veg default ID %d",
					ftmp, newrecord, default_object_list[i].ID);
				}
			if (strcasecmp(newrecord,"epc.waring_pb") == 0) {	
				default_object_list[i].epc.waring_pb = ftmp;
				printf("\n Using %lf for %s for veg default ID %d",
					ftmp, newrecord, default_object_list[i].ID);
				}
			if (strcasecmp(newrecord,"epc.waring_pa") == 0) {	
				default_object_list[i].epc.waring_pa = ftmp;
				printf("\n Using %lf for %s for veg default ID %d",
					ftmp, newrecord, default_object_list[i].ID);
				}
			if (strcasecmp(newrecord,"epc.min_percent_leafg") == 0) {	
				default_object_list[i].epc.min_percent_leafg = ftmp;
				printf("\n Using %lf for %s for veg default ID %d",
					ftmp, newrecord, default_object_list[i].ID);
				}
			if (strcasecmp(newrecord,"epc.max_storage_percent") == 0) {	
				default_object_list[i].epc.max_storage_percent = ftmp;
				printf("\n Using %lf for %s for veg default ID %d",
					ftmp, newrecord, default_object_list[i].ID);
				}
			if (strcasecmp(newrecord,"epc.min_leaf_carbon") == 0) {	
				default_object_list[i].epc.min_leaf_carbon = ftmp;
				printf("\n Using %lf for %s for veg default ID %d",
					ftmp, newrecord, default_object_list[i].ID);
				}
			if (strcasecmp(newrecord,"epc.max_years_resprout") == 0) {	
			        default_object_list[i].epc.max_years_resprout = (int) ftmp;
				printf("\n Using %lf for %s for veg default ID %d",
					ftmp, newrecord, default_object_list[i].ID);
				}

			if (strcasecmp(newrecord,"epc.resprout_leaf_carbon") == 0) { 	
				default_object_list[i].epc.resprout_leaf_carbon = ftmp;
				printf("\n Using %lf for %s for veg default ID %d",
					ftmp, newrecord, default_object_list[i].ID);
				}

			if (strcasecmp(newrecord,"epc.litter_gsurf_slope") == 0) {	
				default_object_list[i].epc.litter_gsurf_slope = ftmp;
				printf("\n Using %lf for %s for veg default ID %d",
					ftmp, newrecord, default_object_list[i].ID);
				}
			if (strcasecmp(newrecord,"epc.coef_CO2") == 0) { 
				default_object_list[i].epc.coef_CO2 = ftmp;
				printf("\n Using %lf for %s for veg default ID %d",
					ftmp, newrecord, default_object_list[i].ID);
				}
			if (strcasecmp(newrecord,"epc.litter_moist_coef") == 0) { 
				default_object_list[i].epc.litter_moist_coef = ftmp;
				printf("\n Using %lf for %s for veg default ID %d",
					ftmp, newrecord, default_object_list[i].ID);
				}
			if (strcasecmp(newrecord,"epc.litter_gsurf_intercept") == 0) { 
				default_object_list[i].epc.litter_gsurf_intercept = ftmp;
				printf("\n Using %lf for %s for veg default ID %d",
					ftmp, newrecord, default_object_list[i].ID);
				}
			if (strcasecmp(newrecord,"epc.root_growth_direction") == 0) { 
				default_object_list[i].epc.root_growth_direction = ftmp;
				printf("\n Using %lf for %s for veg default ID %d",
					ftmp, newrecord, default_object_list[i].ID);
				}
			if (strcasecmp(newrecord,"epc.root_distrib_parm") == 0) { 
				default_object_list[i].epc.root_distrib_parm = ftmp;
				printf("\n Using %lf for %s for veg default ID %d",
					ftmp, newrecord, default_object_list[i].ID);
				}
			if (strcasecmp(newrecord,"epc.gs_tmin") == 0) { 
				default_object_list[i].epc.gs_tmin = ftmp;
				printf("\n Using %lf for %s for veg default ID %d",
					ftmp, newrecord, default_object_list[i].ID);
				}
			if (strcasecmp(newrecord,"epc.gs_tmax") == 0) { 
				default_object_list[i].epc.gs_tmax = ftmp;
				printf("\n Using %lf for %s for veg default ID %d",
					ftmp, newrecord, default_object_list[i].ID);
				}
            if (strcasecmp(newrecord,"epc.gs_vpd_min") == 0) { 
				default_object_list[i].epc.gs_vpd_min = ftmp;
				printf("\n Using %lf for %s for veg default ID %d",
					ftmp, newrecord, default_object_list[i].ID);
				}
			if (strcasecmp(newrecord,"epc.gs_vpd_max") == 0) { 
				default_object_list[i].epc.gs_vpd_max = ftmp;
				printf("\n Using %lf for %s for veg default ID %d",
					ftmp, newrecord, default_object_list[i].ID);
				}
			if (strcasecmp(newrecord,"epc.gs_dayl_min") == 0) { 
				default_object_list[i].epc.gs_dayl_min = ftmp;
				printf("\n Using %lf for %s for veg default ID %d",
					ftmp, newrecord, default_object_list[i].ID);
				}
			if (strcasecmp(newrecord,"epc.gs_dayl_max") == 0) { 
				default_object_list[i].epc.gs_dayl_max = ftmp;
				printf("\n Using %lf for %s for veg default ID %d",
					ftmp, newrecord, default_object_list[i].ID);
				}


			}
			}
		
		default_object_list[i].epc.gs_dayl_range = default_object_list[i].epc.gs_dayl_max-default_object_list[i].epc.gs_dayl_min;
		default_object_list[i].epc.gs_vpd_range = default_object_list[i].epc.gs_vpd_max-default_object_list[i].epc.gs_vpd_min;
		default_object_list[i].epc.gs_trange = default_object_list[i].epc.gs_tmax-default_object_list[i].epc.gs_tmin;


		/*
		if (default_object_list[i].epc.min_percent_leafg > default_object_list[i].epc.leaf_turnover) {
			printf("\n In veg default file %s", default_files[i]);
			printf("\n min percent leafg is greater than leaf turnover you probably don't want that");
			printf("\n Resetting min % gleaf to leaf turnover to avoid instability", default_object_list[i].epc.min_percent_leafg);
			default_object_list[i].epc.min_percent_leafg = default_object_list[i].epc.leaf_turnover;
			}
		*/
		/*--------------------------------------------------------------*/
		/* set sunlit sla multiplier	this should be an input		*/
		/*--------------------------------------------------------------*/
		default_object_list[i].epc.shade_sla_mult = 1;

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
		fclose(default_file);
		} /*end for*/
		return(default_object_list);
} /*end construct_stratum_defaults*/
