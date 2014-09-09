
/*--------------------------------------------------------------*/
/* 								*/
/*			algae_stratum_daily_F			*/
/*								*/
/*	NAME							*/
/*	algae_stratum_daily_F				*/
/*								*/
/*								*/
/*	SYNOPSIS						*/
/*	void algae_stratum_daily_F 				*/
/*								*/
/*	OPTIONS							*/
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*								*/
/*								*/
/*	PROGRAMMER NOTES					*/
/*								*/
/*								*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "rhessys.h"

void	algae_stratum_daily_F(
							   struct	world_object	     	*world,
							   struct	basin_object	     	*basin,
							   struct	hillslope_object	    *hillslope, 
							   struct	zone_object	     	    *zone,
							   struct	patch_object		    *patch,
							   struct   layer_object		    *layer,
							   struct 	canopy_strata_object 	*stratum,
							   struct 	command_line_object     *command_line,
							   struct	tec_entry               *event,
							   struct 	date                    current_date)
{
	/*--------------------------------------------------------------*/
	/*	Local function declaration				*/
	/*--------------------------------------------------------------*/
	long	julday(struct date calendar_date);

double	compute_direct_radiative_fluxes(
		int	,
		double	*,
		double	,
		double	,
		double	,
		double	,
		double	,
		double	);

	double	compute_diffuse_radiative_fluxes(
		int,
		double *,
		double,
		double,
		double,
		double,
		double,
		double);
	
	double	compute_diffuse_radiative_PAR_fluxes(
		int	,
		double	*,
		double	,
		double	,
		double	,
		double	,
		double	,
		double	);
	
	
	/*--------------------------------------------------------------*/
	/*  Local variable definition.                                  */
	/*--------------------------------------------------------------*/

    double alpha0, alpha1;
    double chla, Nuptake, Nrelease, Cfixed, totalN, totalC;
    double chla_to_N, chla_to_C;
    double detention_store, preday_detention_store;
    double daylength;
    double death_rate, death_temptheta;
    double growth, death, flushing; 
    double growth_rate, growth_temptheta;
    double Kdown_direct, Kdown_diffuse;
    double PAR_direct, PAR_diffuse, total_incoming_PAR;
    double light_extinct, light_optimum;
    double nitrate, sminn, DON, DOC;
    double nitconc, sminnconc, phos_conc, chlaconc; /* turn mass to mg/L as themselves - some empricial equations are a funciton of concentration, not total mass */
    double nitro_halfsat, phos_halfsat;
    double npref, npref_coeff;
    double scm_temp, scm_volume, scm_area, scm_photo_depth, scm_ave_height;
    double tmax,tmin;
    
    
	if ( command_line[0].verbose_flag > 1 )
		printf("\n%8d -444.1 ",julday(current_date)-2449000);
	if ( command_line[0].verbose_flag > 1 )
		printf("%8.2f %8.2f %8.2f %8.2f ",
		patch[0].Kdown_direct,
		patch[0].Kdown_diffuse,
		patch[0].PAR_direct/1000,
		patch[0].PAR_diffuse/1000);
	
     /*--------------------------------------------------------------*/
	/*	Initialize temporary variables for transmitted fluxes.	*/
	/*--------------------------------------------------------------*/
	Kdown_diffuse = patch[0].Kdown_diffuse ;
	PAR_diffuse = patch[0].PAR_diffuse;
	Kdown_direct = patch[0].Kdown_direct;
	PAR_direct = patch[0].PAR_direct;
	total_incoming_PAR = PAR_diffuse + PAR_direct;
   
 
    /*--------------------------------------------------------------*/
	/*	Initialize temporary variables                              */
	/*--------------------------------------------------------------*/

    daylength              = basin[0].daylength; /* s */
    
    tmax                   = zone[0].metv.tmax; /* degC */
    tmin                   = zone[0].metv.tmin; /* degC */
    
    Kdown_direct           = patch[0].Kdown_direct;     /* kJ/(m^s*day) */
    Kdown_diffuse          = patch[0].Kdown_diffuse;   /* kJ/(m^s*day) */
    detention_store        = patch[0].detention_store; /* m */
    preday_detention_store = patch[0].preday_detention_store;   /* m */
    scm_temp               = patch[0].scm_temp;       /* degC */
    scm_area               = patch[0].area;            /* m^2 */       
    nitrate                = patch[0].surface_NO3;     /* kg/m^2 */
    sminn                  = patch[0].surface_NH4;     /* kg/m^2 */
    DON                    = patch[0].surface_DON;     /* kg/m^2 */    
    DOC                    = patch[0].surface_DOC;     /* kg/m^2 */
    scm_ave_height         = patch[0].scm_ave_height;  /* m */
 
    
    chla                   = stratum[0].algae.chla;     /* kg/m^2 */
    Nuptake                = stratum[0].algae.Nuptake;  /* kg/m^2 */
    Nrelease               = stratum[0].algae.Nrelease; /* kg/m^2 */
    Cfixed                 = stratum[0].algae.Cfixed;   /* kg/m^2 */
    totalN                 = stratum[0].algae.totalN;   /* kg/m^2 */
    totalC                 = stratum[0].algae.totalC;   /* kg/m^2 */
    
    growth_rate            = stratum[0].defaults[0][0].algae.growth_rate;      /* d^-1 */
    growth_temptheta       = stratum[0].defaults[0][0].algae.growth_temptheta; /* unitless */
    light_extinct          = stratum[0].defaults[0][0].algae.light_extinct;    /* kJ / (m^2 * day) */
    light_optimum          = stratum[0].defaults[0][0].algae.light_optimum;    /* kJ / (m^2 * day) */
    nitro_halfsat          = stratum[0].defaults[0][0].algae.nitro_halfsat;    /* kg/m^3 */
    phos_halfsat           = stratum[0].defaults[0][0].algae.phos_halfsat;     /* kg/m^3 */
    phos_conc              = stratum[0].defaults[0][0].algae.phos_conc;   /* kg/m^3 -- Ambient P conc in scm - static Conc, brought in algae def file */
    death_rate             = stratum[0].defaults[0][0].algae.death_rate;       /* d^-1 */
    death_temptheta        = stratum[0].defaults[0][0].algae.death_temptheta;  /* unitless */
    scm_photo_depth        = stratum[0].defaults[0][0].algae.scm_photo_depth; /* m */
    npref_coeff            = stratum[0].defaults[0][0].algae.npref_coeff;      /* kg/m^3 */
    chla_to_C              = stratum[0].defaults[0][0].algae.chla_to_C;        /* kg/m^2 Chla : kg/m^2 C */
    chla_to_N              = stratum[0].defaults[0][0].algae.chla_to_N;        /* kg/m^2 Chla : kg/m^2 N */
  
    	/*--------------------------------------------------------------*/
	/*	Initialize temporary variables for transmitted fluxes.	*/
	/*--------------------------------------------------------------*/
	Kdown_diffuse = patch[0].Kdown_diffuse ;
	PAR_diffuse = patch[0].PAR_diffuse;
	Kdown_direct = patch[0].Kdown_direct;
	PAR_direct = patch[0].PAR_direct;
	total_incoming_PAR = PAR_diffuse + PAR_direct;
    
    
    // ONLY RUN THIS SURFACE_WATER BGC IF THERE IS A DETENTION STORE
    
    if(detention_store > 0){
         /*--------------------------------------------------------------*/
          /*	1 - Compute Growth                                          */
         /*      f(  1 - Temperature                                     */
         /*          2 - Light                                           */
         /*          3 - N and P conc )                                  */
          /*--------------------------------------------------------------*/
             
         /* Compute volume and convert some state variables to concentrations - some empricial equations a f(conc) rather than total mass */

         
         scm_volume      = scm_area * detention_store;
         nitconc         = nitrate / detention_store; //kg/m^3
         sminnconc       = sminn / detention_store;   //kg/m^3
         chlaconc        = stratum[0].algae.chla / detention_store;  //kg/m^3
         npref           = ((nitconc * sminnconc) / ((npref_coeff + sminnconc)*(npref_coeff + nitconc)) + (sminnconc * npref_coeff)/((nitconc + sminnconc)*(npref_coeff + nitconc)));
         scm_photo_depth = min(scm_ave_height, scm_photo_depth);
         
         
         /* 1.1 - Temperature Effects: */
         scm_temp = 5 + 0.75 * (tmax-tmin)/2;
         
         /* Empirical equation from Stefan and Prudhomme 1993 - for streams, but best I found */
         growth_rate = growth_rate * pow(growth_temptheta,(scm_temp - 20));
         
         
         /* 1.2 - Light Effects: */  
         
             /* 1.2.1 - Compute effects of self-shading on light extinction (Riley 1956) coefficient as a funciton of present chl-a concentration in mg/m3 - requires unit conversion hence multiply by 1000^2 */
             light_extinct = light_extinct + 0.0088 * (chlaconc*1000*1000) + 0.54 * pow((chlaconc*1000*1000),(2/3));
         
             /* 1.2.2 - alpha0 and alpha1 of the Steele (1962) Model */  
             alpha0      = ((Kdown_diffuse + Kdown_direct) / light_optimum)*exp(-1*light_extinct*0);
             alpha1      = ((Kdown_diffuse + Kdown_direct) / light_optimum)*exp(-1*light_extinct*scm_photo_depth);
             growth_rate = growth_rate * (2.718 * (daylength / (24*60*60) / light_extinct / scm_photo_depth )*(exp(-1*alpha1)-exp(-1*alpha0)));
         
         
         /* 1.3 - Nutrient Limitation (Michaelis-Menten model): */     

         growth_rate = growth_rate * min((nitconc + sminnconc) / (nitro_halfsat + (nitconc+sminnconc)), phos_conc / (phos_halfsat + phos_conc));
         
         /* 1.4 - Gross Growth */
         
                                          
         /* If preday detention store = 0, and today > 0, seed chlA w/ 0.000001 kg/m^3 (0.001 mg/L) (*/
         if(preday_detention_store <= 0){
             growth = growth_rate * (chlaconc+0.000001);
             //fprintf(stderr, "Resprouting Algae in Patch No. %d", patch[0].ID);
         } else {
             growth = growth_rate * chlaconc;
         }
         
         
         /*--------------------------------------------------------------*/
         /*	2 - Compute Death                                           */
         /*--------------------------------------------------------------*/

         death = chlaconc * pow(death_temptheta, (scm_temp-20));
             
         
         /*--------------------------------------------------------------*/
         /*	4 - Calculate state variables/fluxes for N specieis and C   */
         /*--------------------------------------------------------------*/
         chla    += ((growth - death))*detention_store;
         Cfixed   = ((growth - death) / chla_to_C)*detention_store;
         totalC  += ((growth - death - flushing) / chla_to_C)*detention_store;
         DOC     += death / chla_to_C * detention_store;
         Nuptake  = growth / chla_to_N * detention_store; /* gross */
         Nrelease = death / chla_to_N * detention_store; /* gross */
         totalN  += (Nuptake - Nrelease) * detention_store;
         DON     += Nrelease;
         
         if (sminn >= Nuptake*npref || nitrate >= Nuptake*(1-npref)){
             sminn   -= Nuptake*npref;
             nitrate -= Nuptake*(1-npref);
         }
         else if (sminn >= Nuptake*npref || nitrate < Nuptake*(1-npref)){
             sminn -= (Nuptake - nitrate);
             nitrate = 0;
             
         }
         else if (sminn < Nuptake*npref || nitrate >= Nuptake*(1-npref)) {
             nitrate -= (Nuptake-sminn);
             sminn = 0;       
         }
                                            

         /*--------------------------------------------------------------*/
          /*	5 - Update patch/stratum varibales....                      */
          /*--------------------------------------------------------------*/     
         
         patch[0].scm_temp    = scm_temp;
         patch[0].surface_NO3 = nitrate;
         patch[0].surface_NH4 = sminn;
         patch[0].surface_DON = DON;
         patch[0].surface_DOC = DOC;
                                          
         stratum[0].algae.chla     = chla;
         stratum[0].algae.Nuptake  = Nuptake;
         stratum[0].algae.Nrelease = Nrelease;
         stratum[0].algae.Cfixed   = Cfixed;
         stratum[0].algae.totalN   = totalN;
         stratum[0].algae.totalC   = totalC;
    
    } else {
         stratum[0].algae.chla     = 0;
         stratum[0].algae.Nuptake  = 0;
         stratum[0].algae.Nrelease = 0;
         stratum[0].algae.Cfixed   = 0;
         stratum[0].algae.totalN   = 0;
         stratum[0].algae.totalC   = 0;
    }
     
    //  fprintf(stderr, "\nHour: %d, Day: %d, Month: %d, Year: %d, chla: %f, ave. dep: %f, detention: %f", current_date.hour, current_date.day, current_date.month, current_date.year, chla, scm_ave_height, detention_store);
     
    // fprintf(stderr, "\nMonth: %d | Date: %d | Hour: %d | P no: %d | TotalN: %f | Depth: %f", current_date.month, current_date.day, current_date.hour, patch[0].ID, totalN, detention_store);
                                     
    /*--------------------------------------------------------------*/
    /*	6 - Do Light Balance (copied from canopy_stratum_daily_F)   */
    /*--------------------------------------------------------------*/

     /*--------------------------------------------------------------*/
     /*  6.1 - Intercept diffuse radiation.                          */
     /*  We assume that the zone slope == patch slope.               */
     /*  We also assume that radiation reflected into the upper      */
     /*      hemisphere is lost.                                     */
     /*  We do not make adjustements for chaanging gap fraction over */
     /*      the diurnal cycle - using a suitable mean gap fraction  */
     /*      instead.                                                */
     /*  We do take into account the patch level horizon which will  */
     /*      allow simulation of clear-cuts/clearings with low sza's */
     /*  Note that for zone level temperature and radiation          */
     /*      computation we took into account only zone level horizon*/
     /*      since we figured that climate above the zone was well   */
     /*      mixed.                                                  */
     /*--------------------------------------------------------------*/
     
     if ( command_line[0].verbose_flag > 2 ){
         printf("\n%4d %4d %4d -444.4 \n",
                current_date.day, current_date.month, current_date.year);
         printf("\n %f %f %f %f %f %f %f",
                Kdown_diffuse,
                Kdown_direct,
                -1*light_extinct,
                stratum[0].gap_fraction,
                1,
                basin[0].theta_noon,
                stratum[0].defaults[0][0].algae.K_reflectance);
         printf("\n%4d %4d %4d -444.4b \n",
                current_date.day, current_date.month, current_date.year);
     }
                                     
     stratum[0].Kstar_diffuse = compute_diffuse_radiative_fluxes(
         command_line[0].verbose_flag,
         &(Kdown_diffuse),
         Kdown_direct,
         -1*light_extinct,
         stratum[0].gap_fraction,
         1,
         basin[0].theta_noon,
         stratum[0].defaults[0][0].algae.K_reflectance);
     if ( command_line[0].verbose_flag > 2 )
         printf("\n%8d -444.5 ",julday(current_date)-2449000);
     
                                     
     stratum[0].APAR_diffuse = compute_diffuse_radiative_PAR_fluxes(
        command_line[0].verbose_flag,
        &(PAR_diffuse),
        PAR_direct,
        -1*light_extinct,
        stratum[0].gap_fraction,
        1,
        basin[0].theta_noon,
        stratum[0].defaults[0][0].algae.K_reflectance);
                                     
                                     
    /*--------------------------------------------------------------*/
    /*  6.2 - Intercept direct radiation.                           */
    /*      hard to measure for each strata.  We could use top      */
    /*      of canopy albedo but this integrates the effect of the  */
    /*      entire canopy.  Furthermore, it requires in general     */
    /*      knowledge of the canopy BRDF - which we want to avoid.  */
    /*      Instead we assume a certain reflectance and             */
    /*      transmittance for each strata's canopy elements.        */
    /*  We assume that the zone slope == patch slope.               */
    /*  We also assume that radiation reflected into the upper      */
    /*      hemisphere is lost.                                     */
    /*  We do not make adjustements for chaanging gap fraction over */
    /*      the diurnal cycle - using a suitable mean gap fraction  */
    /*      instead.                                                */
    /*  We do take into account the patch level horizon which will  */
    /*      allow simulation of clear-cuts/clearings with low sza's */
    /*  Note that for zone level temperature and radiation          */
    /*      computation we took into account only zone level horizon*/
    /*      since we figured that climate above the zone was well   */
    /*      mixed.                                                  */
    /*--------------------------------------------------------------*/
     if ( command_line[0].verbose_flag > 2 )
     printf("\n%8d -444.2 ",julday(current_date)-2449000);
     stratum[0].Kstar_direct = compute_direct_radiative_fluxes(
           command_line[0].verbose_flag,
           &(Kdown_direct),
           -1*light_extinct,
           stratum[0].gap_fraction,
           1,
           basin[0].theta_noon,
           stratum[0].defaults[0][0].algae.K_reflectance,
           stratum[0].defaults[0][0].algae.K_reflectance);
     
     
     if ( command_line[0].verbose_flag >2  )
     printf("\n%d %d %d  -444.3 ",
            current_date.year, current_date.month, current_date.day);
     stratum[0].APAR_direct = compute_direct_radiative_fluxes(
          command_line[0].verbose_flag,
          &(PAR_direct),
          -1*light_extinct,
          stratum[0].gap_fraction,
          1,
          basin[0].theta_noon,
          stratum[0].defaults[0][0].algae.K_reflectance,
          stratum[0].defaults[0][0].algae.K_reflectance);



// The following was originally copied over form canopy_straum_daily_F
// thinking that I needed to do the ET, snow and heat balance, etc.
// However, I tihnk patch_daily_F. does all this to "ponds"
// Just need to specifiy that algae height is relaly low (shallower than pond height)



     /*--------------------------------------------------------------*/
     /*	7 - Compute conductance aerodynamic.			*/
     /*--------------------------------------------------------------*/
                             
  //   stratum[0].ga = 0;

    
     /*--------------------------------------------------------------*/
     /* 8 - Determine non-vascular condductance to evaporation.      */
     /*	This conductance represnets the inverse of an additional   */
     /*	resistance to vapour flux from the stratum rian storage		*/
     /*	surface over and above aerodynamic resistances that also	*/
     /*	affect turbulent heat transfer.  						 	*/
     /*	                            								*/
     /*	A linear relationship is currently assumed with the amount	*/
     /*	of relative rain stored - or with the patch unsat zone storage  */
     /*	relative to water equiv depth top water table if roots are present */
     /*	.  Parameters for the relationship	*/
     /*	are supplied via the stratum default file.					*/
     /*--------------------------------------------------------------*/
    
     /* stratum[0].gsurf = 0; // assume no conductance by algae
     
     if ( command_line[0].verbose_flag > 1 )
		printf("%8.6f ",stratum[0].gs);
	if ( command_line[0].verbose_flag > 1 )
		printf("\n%4d %4d %4d -444.9 \n ",
		current_date.day, current_date.month, current_date.year);
	if ( command_line[0].verbose_flag > 1 )
		printf("%8.4f %8.4f %8.4f %8.4f \n ",
		stratum[0].Kstar_direct,stratum[0].Kstar_diffuse,
		stratum[0].APAR_direct,stratum[0].APAR_diffuse);
	if ( command_line[0].verbose_flag > 1 ) {
		printf("\n%4d %4d %4d -444.9.1 \n ",current_date.day, current_date.month,
			current_date.year);
		printf(" %f\n", stratum[0].snow_stored);
	} */
     
     /*--------------------------------------------------------------*/
	/*	9 - Update snow storage ( this also updates the patch level	*/
	/*	snow throughfall and the stratum level Kstar )	 	    */
	/*	Snow on the canopy is first sublimated before any ET	*/
	/*	can take place.  This may seem like we are hobbling	*/
	/*	the energy budget since the sublimation reduces the	*/
	/*	energy used for stomatal conductance etc. however we	*/
	/*	suggest that when it snows it is likely ET is small.	*/
	/*--------------------------------------------------------------*/
	
     /* stratum[0].snow_stored = compute_snow_stored(
		command_line[0].verbose_flag,
		zone[0].metv.tday,
		1000.0,
		0, // set to zero
		&(rain_throughfall)+&(snow_throughfall), // set rain to rain+snow (basically assume all snow melts upon contact w. water)
		stratum);
	if ( command_line[0].verbose_flag > 1 )
		printf("\n%8.4f %f ",stratum[0].snow_stored, stratum[0].sublimation);
	if ( command_line[0].verbose_flag > 1 )
		printf("\n%8d -444.10 ",julday(current_date)-2449000);
	if ( command_line[0].verbose_flag > 1 )
		printf("%8.4f %8.4f %8.4f %8.4f ",stratum[0].Kstar_direct,
		stratum[0].Kstar_diffuse,stratum[0].APAR_direct,
		stratum[0].APAR_diffuse); */

/* CB: Not sure what to do about this... Set perc_sunlit = 1, ppfd_shade=0, let it compute 

	if (stratum[0].epv.proj_lai > ZERO)
		perc_sunlit = (stratum[0].epv.proj_lai_sunlit)/(stratum[0].epv.proj_lai);
	else
		perc_sunlit = 1.0;

	if (stratum[0].epv.proj_lai_shade > ZERO && zone[0].metv.dayl > ZERO)
		stratum[0].ppfd_shade = (stratum[0].APAR_diffuse * (1-perc_sunlit)) / 
					zone[0].metv.dayl /stratum[0].epv.proj_lai_shade;
	else
		stratum[0].ppfd_shade = 0.0;

	if (stratum[0].epv.proj_lai_sunlit > ZERO && zone[0].metv.dayl > ZERO)
		stratum[0].ppfd_sunlit = (stratum[0].APAR_direct + stratum[0].APAR_diffuse * perc_sunlit) / 
					zone[0].metv.dayl /stratum[0].epv.proj_lai_sunlit;
	else
		stratum[0].ppfd_sunlit = 0.0;          
          */
                                    
                                     
	/*--------------------------------------------------------------*/
	/*	Increment the transmitted fluxes from this patch layer	*/
	/*	by weighting the fluxes in this stratum by its cover	*/
	/*	fraction - we have check cover fractions sum to 1 in 	*/
	/*	a layer when constructing the patch.			*/
	/* 	we will still need to do this - as the alge later will */
	/*  need to deal with it - I'll work it out later 		*/
	/*--------------------------------------------------------------*/
/*	patch[0].Kdown_direct_final += Kdown_direct * stratum[0].cover_fraction;
	patch[0].PAR_direct_final += PAR_direct * stratum[0].cover_fraction;
	patch[0].Kdown_diffuse_final += Kdown_diffuse * stratum[0].cover_fraction;
	patch[0].PAR_diffuse_final += PAR_diffuse * stratum[0].cover_fraction;
	patch[0].rain_throughfall_final += rain_throughfall
		* stratum[0].cover_fraction;
	patch[0].snow_throughfall_final += snow_throughfall
		* stratum[0].cover_fraction;
	patch[0].ga_final += ga * stratum[0].cover_fraction;
	patch[0].wind_final += wind * stratum[0].cover_fraction;
	} */
	return;
} /*end algae_stratum_daily_F.c*/
