/*--------------------------------------------------------------*/
/* 																*/
/*					output_growth_basin						*/
/*																*/
/*	output_growth_basin - creates output files objects.		*/
/*																*/
/*	NAME														*/
/*	output_growth_basin - outputs current contents of a basin.			*/
/*																*/
/*	SYNOPSIS													*/
/*	void	output_growth_basin(										*/
/*					struct	basin_object	*basin,				*/
/*					struct	date	date,  						*/
/*					FILE 	*outfile)							*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	outputs spatial structure according to commandline			*/
/*	specifications to specific files							*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*	We only permit one fileset per spatial modelling level.     */
/*	Each fileset has one file for each timestep.  				*/
/*																*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"

void	output_growth_basin(
							struct	basin_object	*basin,
							struct	date	current_date,
							FILE *outfile)
{
	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	int h,z,p,c;
	int  layer;
	double agpsn, aresp;
	double alai;
	double aleafc, afrootc, awoodc;
	double aleafn, afrootn, awoodn;
	double acpool;
	double anpool;
	double alitrc;
	double asoilhr;
	double acloss;
	double asoilc, asminn, anitrate, asurfaceN;
	double alitrn, asoiln, anfix, anuptake;
	double aarea, hill_area, basin_area;
	double acarbon_balance, anitrogen_balance;
	double atotaln, adenitrif;
	double astreamflow_NO3, astreamflow_NH4, astreamflow_DON, astreamflow_DOC;
	double anitrif, aDOC, aDON, arootdepth;
	double aninput, afertilizer_NO3, afertilizer_NH4;
	double hstreamflow_NO3, hstreamflow_NH4, hstreamflow_DON, hstreamflow_DOC;
	double streamNO3_from_surface;
	double streamNO3_from_sub;
	double hgwNO3, hgwDON, hgwDOC, hgwNH4;
	double hgwNO3out, hgwDONout, hgwDOCout, hgwNH4out;

	struct	patch_object  *patch;
	struct	zone_object	*zone;
	struct hillslope_object *hillslope;
	struct  canopy_strata_object    *strata;

	/*--------------------------------------------------------------*/
	/*	Initialize Accumlating variables.								*/
	/*--------------------------------------------------------------*/
	alai = 0.0; acpool=0.0; anpool = 0.0;
	aleafc = 0.0; afrootc=0.0; awoodc=0.0;
	aleafn = 0.0; afrootn=0.0; awoodn=0.0;
	agpsn = 0.0; aresp=0.0; anfix=0.0; anuptake=0.0;
	aarea =  0.0 ;
	asoilhr = 0.0;
	alitrc = 0.0;
	alitrn = 0.0; asoiln = 0.0;
	anitrate = 0.0;
	asurfaceN = 0.0;
	asoilc = 0.0; asminn=0.0;
	acarbon_balance = 0.0;
	anitrogen_balance = 0.0;
	astreamflow_DOC = 0.0;
	hstreamflow_DOC = 0.0;
	hgwDOC = 0.0;
	hgwDOCout = 0.0;
	astreamflow_DON = 0.0;
	hstreamflow_DON = 0.0;
	hgwDON = 0.0;
	hgwDONout = 0.0;
	astreamflow_NH4 = 0.0;
	hstreamflow_NH4 = 0.0;
	hgwNH4 = 0.0;
	hgwNH4out = 0.0;
	astreamflow_NO3 = 0.0;
	hstreamflow_NO3 = 0.0;
	hgwNO3 = 0.0;
	hgwNO3out = 0.0;
	atotaln = 0.0;
	adenitrif = 0.0;
	anitrif = 0.0;
	aDOC = 0.0; aDON = 0.0;
	arootdepth = 0.0;
	basin_area = 0.0;
	acloss = 0.0;
	streamNO3_from_surface = 0.0;
	streamNO3_from_sub = 0.0;
	aninput = 0.0;
	afertilizer_NO3 = 0.0;
	afertilizer_NH4 = 0.0;
	for (h=0; h < basin[0].num_hillslopes; h++){
		hillslope = basin[0].hillslopes[h];
		hill_area = 0.0;
		for (z=0; z< hillslope[0].num_zones; z++){
			zone = hillslope[0].zones[z];
				aninput = (zone[0].ndep_NO3+zone[0].ndep_NH4)*zone[0].area;
			for (p=0; p< zone[0].num_patches; p++){
				patch = zone[0].patches[p];
				alitrn += (patch[0].litter_ns.litr1n + patch[0].litter_ns.litr2n
					+ patch[0].litter_ns.litr3n + patch[0].litter_ns.litr4n)
					* patch[0].area;
				asoiln += (patch[0].soil_ns.soil1n + patch[0].soil_ns.soil2n
					+ patch[0].soil_ns.soil3n + patch[0].soil_ns.soil4n)
					* patch[0].area;
				alitrc += (patch[0].litter_cs.litr1c + patch[0].litter_cs.litr2c
					+ patch[0].litter_cs.litr3c + patch[0].litter_cs.litr4c)
					* patch[0].area;
				asoilc += (patch[0].soil_cs.soil1c + patch[0].soil_cs.soil2c
					+ patch[0].soil_cs.soil3c + patch[0].soil_cs.soil4c)
					* patch[0].area;
				asminn += (patch[0].soil_ns.sminn) * patch[0].area;
				anitrate += (patch[0].soil_ns.nitrate) * patch[0].area;
				asurfaceN += (patch[0].surface_DON+patch[0].surface_NO3+patch[0].surface_NH4) * patch[0].area;
				atotaln += (patch[0].totaln) * patch[0].area;
				astreamflow_NH4 += patch[0].streamflow_NH4 * patch[0].area;
				astreamflow_NO3 += patch[0].streamflow_NO3 * patch[0].area;
				astreamflow_DON += patch[0].streamflow_DON * patch[0].area;
				astreamflow_DOC += patch[0].streamflow_DOC * patch[0].area;
				streamNO3_from_surface += patch[0].streamNO3_from_surface * patch[0].area;
				streamNO3_from_sub += patch[0].streamNO3_from_sub * patch[0].area;
				acarbon_balance += (patch[0].carbon_balance) * patch[0].area;
				anitrogen_balance += (patch[0].nitrogen_balance) * patch[0].area;
				adenitrif += (patch[0].ndf.denitrif) * patch[0].area;	
				anitrif += (patch[0].ndf.sminn_to_nitrate) * patch[0].area;
				afertilizer_NO3 += (patch[0].fertilizer_NO3) * patch[0].area;
				afertilizer_NH4 += (patch[0].fertilizer_NH4) * patch[0].area;
				aDON += (patch[0].soil_ns.DON) * patch[0].area;
				aDOC += (patch[0].soil_cs.DOC) * patch[0].area;
				anfix += (patch[0].ndf.nfix_to_sminn) * patch[0].area;
				acloss += (patch[0].grazing_Closs) * patch[0].area;
				anuptake += (patch[0].ndf.sminn_to_npool) * patch[0].area,

				asoilhr += (
					patch[0].cdf.litr1c_hr + 
					patch[0].cdf.litr2c_hr + 
					patch[0].cdf.litr4c_hr + 
					patch[0].cdf.soil1c_hr + 
					patch[0].cdf.soil2c_hr + 
					patch[0].cdf.soil3c_hr + 
					patch[0].cdf.soil4c_hr) * patch[0].area;

				for ( layer=0 ; layer<patch[0].num_layers; layer++ ){
					for ( c=0 ; c<patch[0].layers[layer].count; c++ ){
						strata = patch[0].canopy_strata[(patch[0].layers[layer].strata[c])];
						agpsn += strata->cover_fraction * strata->cdf.psn_to_cpool
							* patch[0].area;
						/*---------------------------
						agpsn += strata->cover_fraction
							* strata->cs.net_psn
							* patch[0].area;
						------------------------------*/
						anpool += strata->cover_fraction * (strata->ns.npool);
						aresp += strata->cover_fraction
							* (strata->cdf.leaf_day_mr + strata->cdf.cpool_leaf_gr
							+ strata->cdf.leaf_night_mr +	strata->cdf.livestem_mr
							+ strata->cdf.cpool_livestem_gr + strata->cdf.livecroot_mr
							+ strata->cdf.cpool_livecroot_gr
							+ strata->cdf.cpool_deadcroot_gr
							+ strata->cdf.froot_mr + strata->cdf.cpool_froot_gr
							+ strata->cdf.cpool_to_gresp_store)	* patch[0].area;
						aleafn += strata->cover_fraction	* (strata->ns.leafn
							+ strata->ns.leafn_store + strata->ns.leafn_transfer)
							* patch[0].area;
						afrootn += strata->cover_fraction * (strata->ns.frootn
							+ strata->ns.frootn_store + strata->ns.frootn_transfer)
							* patch[0].area;
						awoodn += strata->cover_fraction	* (strata->ns.live_crootn
							+ strata->ns.live_stemn + strata->ns.dead_crootn
							+ strata->ns.dead_stemn + strata->ns.livecrootn_store
							+ strata->ns.livestemn_store + strata->ns.deadcrootn_store
							+ strata->ns.deadstemn_store
							+ strata->ns.livecrootn_transfer
							+ strata->ns.livestemn_transfer
							+ strata->ns.deadcrootn_transfer
							+ strata->ns.deadstemn_transfer
							+ strata->ns.cwdn + strata->ns.retransn + strata->ns.npool ) * patch[0].area;
						aleafc += strata->cover_fraction	* (strata->cs.leafc
							+ strata->cs.leafc_store + strata->cs.leafc_transfer )
							* patch[0].area;
						afrootc += strata->cover_fraction * (strata->cs.frootc
							+ strata->cs.frootc_store + strata->cs.frootc_transfer)
							* patch[0].area;
						awoodc += strata->cover_fraction	* (strata->cs.live_crootc
							+ strata->cs.live_stemc + strata->cs.dead_crootc
							+ strata->cs.dead_stemc + strata->cs.livecrootc_store
							+ strata->cs.livestemc_store + strata->cs.deadcrootc_store
							+ strata->cs.deadstemc_store
							+ strata->cs.livecrootc_transfer
							+ strata->cs.livestemc_transfer
							+ strata->cs.deadcrootc_transfer
							+ strata->cs.deadstemc_transfer
							+ strata->cs.cwdc + strata->cs.cpool)* patch[0].area;
						arootdepth += strata->cover_fraction * (strata->rootzone.depth)
							* patch[0].area;
						alai += strata->cover_fraction * (strata->epv.proj_lai)
							* patch[0].area;
						acpool += strata->cover_fraction*strata->cs.cpool*patch[0].area;
						anpool += strata->cover_fraction*strata->ns.npool*patch[0].area;
					}
				}
				aarea +=  patch[0].area;
				hill_area += patch[0].area;
			}

		}
		hgwNO3 += hillslope[0].gw.NO3 * hill_area;
		hgwNH4 += hillslope[0].gw.NH4 * hill_area;
		hgwDOC += hillslope[0].gw.DOC * hill_area;
		hgwDON += hillslope[0].gw.DON * hill_area;
		hgwDONout += hillslope[0].gw.DONout * hill_area;
		hgwDOCout += hillslope[0].gw.DOCout * hill_area;
		hgwNO3out += hillslope[0].gw.NO3out * hill_area;
		hgwNH4out += hillslope[0].gw.NH4out * hill_area;
		hstreamflow_NH4 += hillslope[0].streamflow_NH4 * hillslope[0].area;
		hstreamflow_NO3 += hillslope[0].streamflow_NO3 * hillslope[0].area;
		hstreamflow_DON += hillslope[0].streamflow_DON * hillslope[0].area;
		hstreamflow_DOC += hillslope[0].streamflow_DOC * hillslope[0].area;
		basin_area += hill_area;
		
		
	}
	agpsn /= aarea ;
	aresp /= aarea ;
	alai /= aarea ;
	anitrate /= aarea;
	asurfaceN /= aarea;
	acpool /= aarea ;
	anpool /= aarea ;
	aleafc /= aarea ;
	aleafn /= aarea ;
	afrootc /= aarea;
	afrootn /= aarea;
	awoodc /= aarea;
	awoodn /= aarea;
	alitrc /= aarea;
	asoilc /= aarea;
	asoilhr /= aarea;	
	alitrn /= aarea;
	asoiln /= aarea;
	asminn /= aarea;
	atotaln /= aarea;
	acarbon_balance /= aarea;
	anitrogen_balance /= aarea;
	astreamflow_NH4 /= aarea;
	astreamflow_NO3 /= aarea;
	astreamflow_DON /= aarea;
	astreamflow_DOC /= aarea;
	streamNO3_from_surface /=aarea;
	streamNO3_from_sub /=aarea;
	adenitrif /= aarea;
	anitrif /= aarea;
	aDON /= aarea;
	aDOC /= aarea;
	arootdepth /= aarea;
	anfix /= aarea;
	acloss /= aarea;
	anuptake /= aarea;
	aninput /= aarea;
	afertilizer_NO3 /= aarea;
	afertilizer_NH4 /= aarea;
	astreamflow_NH4 += (hstreamflow_NH4/ basin_area);
	astreamflow_NO3 += (hstreamflow_NO3/ basin_area);
	astreamflow_DON += (hstreamflow_DON/ basin_area);
	astreamflow_DOC += (hstreamflow_DOC/ basin_area);
	hgwNH4 = hgwNH4 / basin_area;
	hgwNO3 = hgwNO3 / basin_area;
	hgwDON = hgwDON / basin_area;
	hgwDOC = hgwDOC / basin_area;
	hgwNH4out = hgwNH4out / basin_area;
	hgwNO3out = hgwNO3out / basin_area;
	hgwDONout = hgwDONout / basin_area;
	hgwDOCout = hgwDOCout / basin_area;


	fprintf(outfile,"%d %d %d %d %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %11.9lf %11.9lf %11.9lf %11.9lf %lf %lf %lf %lf %11.9lf %11.9lf %11.9lf %11.9lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf \n",
		current_date.day,
		current_date.month,
		current_date.year,
		basin[0].ID,
		alai,
		agpsn * 1000,
		aresp * 1000,
		asoilhr * 1000,
		anitrate * 1000,
		asminn * 1000,
		asurfaceN * 1000,
		(aleafc + awoodc + afrootc),
		(aleafn + awoodn + afrootn),
		acpool,
		anpool,
		alitrc,
		alitrn,
		asoilc,
		asoiln,
		hgwNO3,
		hgwNH4,
		hgwDON,
		hgwDOC,
		astreamflow_NO3*1000.0,
		astreamflow_NH4*1000.0,
		astreamflow_DON*1000.0,
		astreamflow_DOC*1000.0,
		hgwNO3out*1000.0,
		hgwNH4out*1000.0,
		hgwDONout*1000.0,
		hgwDOCout*1000.0,
		adenitrif*1000.0,
		anitrif*1000.0,
		aDOC,
		aDON,
		arootdepth*1000.0,
		anfix * 1000.0,
		anuptake * 1000.0,
		acloss * 1000.0,
		streamNO3_from_surface * 1000.0,
		streamNO3_from_sub * 1000.0,
		aninput * 1000.0,
		(afertilizer_NO3 + afertilizer_NH4)*1000.0
		);
	/*------------------------------------------*/
	/*printf("\n Basin %d Output %4d %3d %3d \n",*/ 
	/*	basin[0].ID, date.year, date.month, date.day);*/
	/*------------------------------------------*/
	return;
} /*end output_daily_growth_basin*/
