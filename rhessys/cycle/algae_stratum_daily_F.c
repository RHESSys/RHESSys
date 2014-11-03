
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
    double death_rate, death_temptheta;
    double growth, death, respire, settle;
    double growth_rate;
    double Kdown_direct, Kdown_diffuse;
    double PAR_direct, PAR_diffuse, total_incoming_PAR;
    double light_extinct;
    double nitconc, sminnconc, chlaconc; /* turn mass to mg/L as themselves - some empricial equations are a funciton of concentration, not total mass */
    double npref;
    double scm_photo_depth;
    
    
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
   
    
    
    // ONLY RUN THIS SURFACE WATER BGC IF THERE IS A DETENTION STORE
    
    if(patch[0].detention_store > 0){
    
         /*--------------------------------------------------------------*/
          /*	1 - Compute Growth                                          */
         /*      f(  1 - Temperature                                     */
         /*          2 - Light                                           */
         /*          3 - N and P conc )                                  */
          /*--------------------------------------------------------------*/
             
         /* Compute volume and convert some state variables to concentrations - some empricial equations a f(conc) rather than total mass */

         nitconc         = patch[0].surface_NO3 / patch[0].detention_store; //kg/m^3
         sminnconc       = patch[0].surface_NH4 / patch[0].detention_store;   //kg/m^3
         chlaconc        = stratum[0].algae.chla / patch[0].detention_store;  //kg/m^3
         npref           = ((nitconc * sminnconc) / ((stratum[0].defaults[0][0].algae.npref_coeff + sminnconc)*(stratum[0].defaults[0][0].algae.npref_coeff + nitconc)) + (sminnconc * stratum[0].defaults[0][0].algae.npref_coeff)/((nitconc + sminnconc)*(stratum[0].defaults[0][0].algae.npref_coeff + nitconc)));
         scm_photo_depth = min(patch[0].scm_ave_height, stratum[0].defaults[0][0].algae.scm_photo_depth);
         
         
         /* 1.1 - Temperature Effects:  Empirical equation from Stefan and Prudhomme 1993 - for streams, but best I found */
         patch[0].scm_temp = stratum[0].defaults[0][0].algae.temp_coef_A + stratum[0].defaults[0][0].algae.temp_coef_B * (zone[0].metv.tmax+zone[0].metv.tmin)/2;
         
         growth_rate = stratum[0].defaults[0][0].algae.growth_rate * pow(stratum[0].defaults[0][0].algae.growth_temp_theta,(patch[0].scm_temp - stratum[0].defaults[0][0].algae.growth_temp_baseT));
         
         
         /* 1.2 - Light Effects: */  
         
             /* 1.2.1 - Compute effects of self-shading on light extinction (Riley 1956) coefficient as a funciton of present chl-a concentration in mg/m3 - requires unit conversion hence multiply by 1000^2 */
             light_extinct = stratum[0].defaults[0][0].algae.light_extinct + stratum[0].defaults[0][0].algae.light_coef_A * (chlaconc*1000*1000) + stratum[0].defaults[0][0].algae.light_coef_B * pow((chlaconc*1000*1000),(2/3));
         
             /* 1.2.2 - alpha0 and alpha1 of the Steele (1962) Model */  
             alpha0      = ((patch[0].Kdown_diffuse + patch[0].Kdown_direct) / stratum[0].defaults[0][0].algae.light_optimum)*exp(-1*light_extinct*0);
             alpha1      = ((patch[0].Kdown_diffuse + patch[0].Kdown_direct) / stratum[0].defaults[0][0].algae.light_optimum)*exp(-1*light_extinct*scm_photo_depth);
             growth_rate = growth_rate * (2.718 * (basin[0].daylength / (24*60*60) / light_extinct / scm_photo_depth )*(exp(-1*alpha1)-exp(-1*alpha0)));
         
         
         /* 1.3 - Nutrient Limitation (Michaelis-Menten model): */     

         growth_rate = growth_rate * min((nitconc + sminnconc) / (stratum[0].defaults[0][0].algae.nitro_halfsat + (nitconc+sminnconc)), stratum[0].defaults[0][0].algae.phos_conc / (stratum[0].defaults[0][0].algae.phos_halfsat + stratum[0].defaults[0][0].algae.phos_conc));
         
         /* 1.4 - Gross Growth */
         
                                          
         /* If preday detention store = 0, and today > 0, seed chlA */ 
         if(patch[0].preday_detention_store <= 0){
             growth = growth_rate * (stratum[0].algae.chla+stratum[0].defaults[0][0].algae.chla_resprout_conc);
             //fprintf(stderr, "RESPROUTING ALGAE in Patch No. %d\n", patch[0].ID);
         } else {
             growth = growth_rate * stratum[0].algae.chla; // kg/m^2/d
         }
         
         
         /*--------------------------------------------------------------*/
         /*	2 - Compute Death                                         */
         /*--------------------------------------------------------------*/
         death = stratum[0].algae.chla * pow(stratum[0].defaults[0][0].algae.death_temp_theta, (patch[0].scm_temp-stratum[0].defaults[0][0].algae.death_temp_baseT)); // kg/m^2/d
             
         /*--------------------------------------------------------------*/
         /*	3 - Compute Respiration                                   */
         /*--------------------------------------------------------------*/
         respire = stratum[0].algae.chla * pow(stratum[0].defaults[0][0].algae.respire_temp_theta, (patch[0].scm_temp-stratum[0].defaults[0][0].algae.respire_temp_baseT));  // kg/m^2/d
         
         /*--------------------------------------------------------------*/
         /*	4 - Compute Settling                                      */
         /*--------------------------------------------------------------*/  
         settle = stratum[0].algae.chla * (stratum[0].defaults[0][0].algae.chla_settling_rate/patch[0].scm_ave_height); // kg/m2/d
                  
         /*--------------------------------------------------------------*/
         /*	5 - Calculate state variables/fluxes for N specieis and C   */
         /*--------------------------------------------------------------*/
         
         /* 5.1 - ALGAE  */
         stratum[0].algae.chla             += growth - death - settle - respire;
         stratum[0].algae.chla_settle      = settle;
         stratum[0].algae.chla_settle_cum += settle;
     
        
         /* 5.2 - CARBON  */
         stratum[0].algae.totalC       += (growth - death - settle - respire) / stratum[0].defaults[0][0].algae.chla_to_C;
         stratum[0].algae.Cfix_cum     += growth / stratum[0].defaults[0][0].algae.chla_to_C;
         stratum[0].algae.Cfix          = growth / stratum[0].defaults[0][0].algae.chla_to_C;
         stratum[0].algae.Crespire_cum += respire / stratum[0].defaults[0][0].algae.chla_to_C;
         stratum[0].algae.Crespire      = respire / stratum[0].defaults[0][0].algae.chla_to_C;
         stratum[0].algae.Crelease_cum += death / stratum[0].defaults[0][0].algae.chla_to_C * patch[0].detention_store;
         stratum[0].algae.Crelease     = death / stratum[0].defaults[0][0].algae.chla_to_C * patch[0].detention_store;
         patch[0].surface_DOC          += death / stratum[0].defaults[0][0].algae.chla_to_C * patch[0].detention_store;
         
          /* 5.3 - NITROGEN  */       
         stratum[0].algae.totalN       += (growth - death - settle - respire) / stratum[0].defaults[0][0].algae.chla_to_N;
         stratum[0].algae.Nuptake_cum  += growth / stratum[0].defaults[0][0].algae.chla_to_N; /* gross */
         stratum[0].algae.Nuptake       = growth / stratum[0].defaults[0][0].algae.chla_to_N; /* gross */
         stratum[0].algae.Nrelease_cum += death / stratum[0].defaults[0][0].algae.chla_to_N;
         stratum[0].algae.Nrelease      = death / stratum[0].defaults[0][0].algae.chla_to_N;
         stratum[0].algae.Nrespire_cum += respire / stratum[0].defaults[0][0].algae.chla_to_N;
         stratum[0].algae.Nrespire      = respire / stratum[0].defaults[0][0].algae.chla_to_N;
         patch[0].surface_DON          += stratum[0].algae.Nrelease;
         
         // Make sure that niether inorganic N pool is over-used.  Also, add respired algal-N to NH4 pool
         if (patch[0].surface_NH4 >= stratum[0].algae.Nuptake*npref && patch[0].surface_NO3 >= stratum[0].algae.Nuptake*(1-npref)){
             patch[0].surface_NH4 -= stratum[0].algae.Nuptake*npref;
             patch[0].surface_NH4 += stratum[0].algae.Nrespire;
             patch[0].surface_NO3 -= stratum[0].algae.Nuptake*(1-npref);
         }
         else if (patch[0].surface_NH4 >= stratum[0].algae.Nuptake*npref && patch[0].surface_NO3 < stratum[0].algae.Nuptake*(1-npref)){
             patch[0].surface_NH4 -= (stratum[0].algae.Nuptake - patch[0].surface_NO3);
             patch[0].surface_NH4 += stratum[0].algae.Nrespire;
             patch[0].surface_NO3 = 0;
             
         }
         else if (patch[0].surface_NH4 < stratum[0].algae.Nuptake*npref && patch[0].surface_NO3 >= stratum[0].algae.Nuptake*(1-npref)) {
             patch[0].surface_NO3 -= (stratum[0].algae.Nuptake - patch[0].surface_NH4);
             patch[0].surface_NH4 = 0;
             patch[0].surface_NH4 += stratum[0].algae.Nrespire;
         }
                                            

    
    } else {
         stratum[0].algae.chla     = 0;
         stratum[0].algae.Nuptake  = 0;
         stratum[0].algae.Nrelease = 0;
         stratum[0].algae.Cfix     = 0;
         stratum[0].algae.totalN   = 0;
         stratum[0].algae.totalC   = 0;
    }
     
    //  fprintf(stderr, "\nHour: %d, Day: %d, Month: %d, Year: %d, chla: %f, ave. dep: %f, detention: %f", current_date.hour, current_date.day, current_date.month, current_date.year, chla, scm_ave_height, detention_store);
     
    // fprintf(stderr, "\nMonth: %d | Date: %d | Hour: %d | P no: %d | TotalN: %f | Depth: %f", current_date.month, current_date.day, current_date.hour, patch[0].ID, totalN, detention_store);

	 /*--------------------------------------------------------------*/
	 /*	6 - Add settled algae to labile litter pool					 */
	 /*--------------------------------------------------------------*/
	 stratum[0].algae.chla_settle = settle;
	 stratum[0].algae.chla_settle_cum += settle;
	 
	 patch[0].litter_ns.litr1n += settle / stratum[0].defaults[0][0].algae.chla_to_N;
	 patch[0].litter_cs.litr1c += settle / stratum[0].defaults[0][0].algae.chla_to_C;
                                 
    /*--------------------------------------------------------------*/
    /*	7 - Do Light Balance (copied from canopy_stratum_daily_F)   */
    /*--------------------------------------------------------------*/

     /*--------------------------------------------------------------*/
     /*  7.1 - Intercept diffuse radiation.                          */
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
    /*  7.2 - Intercept direct radiation.                           */
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


	return;
} /*end algae_stratum_daily_F.c*/
