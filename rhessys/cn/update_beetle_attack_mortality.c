
/*--------------------------------------------------------------*/
/*                                                              */
/*		update_beetle_attack_mortality									*/
/*                                                              */
/*  NAME                                                        */
/*		update_beetle_attack_mortality									*/
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/* 	void update_beetle_attack_mortality(
/*                      struct epconst_struct,			*/
/*                      struct phenology_struct *,		*/
/*                      struct cstate_struct *,			*/
/*                      struct cdayflux_struct *,		*/
/*                      struct cdayflux_patch_struct *,		*/
/*                      struct nstate_struct *,			*/
/*                      struct ndayflux_struct *,		*/
/*                      struct ndayflux_patch_struct *,		*/
/*                      struct litter_c_object *,		*/
/*                      struct litter_n_object *,		*/
/*                      double);				*/
/*								*/
/*  OPTIONS                                                     */
/*                                                              */
/*  DESCRIPTION                                                 */
/*                                                              */
/*                                                              */
/*	calculated daily mortality losses and updates 		*/
/*	carbon and nitrogen pools				*/
/*                                                              */
/*  PROGRAMMER NOTES                                            */
/*                                                              */
/*	Copy from update mortality to make the work flow more		*/
/*  clear. By N.Ren 2018-4-25                                                    */
/*                                                              */
/*--------------------------------------------------------------*/

#include <stdio.h>
#include "rhessys.h"
#include "phys_constants.h"
void update_beetle_attack_mortality(
					  struct epconst_struct	epc	,
					  struct cstate_struct *cs,
					  struct cdayflux_struct	*cdf,
					  struct cdayflux_patch_struct *cdf_patch,
					  struct nstate_struct *ns,
					  struct ndayflux_struct	*ndf,
					  struct ndayflux_patch_struct *ndf_patch,
					  struct litter_c_object *cs_litr,
					  struct litter_n_object *ns_litr,
					  struct snag_sequence_object *snag_sequence, //NREN 20180630
					  struct snag_sequence_object *redneedle_sequence,
					  int inx,
					  int thintyp,
					  int veg_parm_ID,
					  //int harvest_dead_root,
					  struct mortality_struct mort,
					  struct beetle_default *beetle)
{
	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/

	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	double m_cpool, m_npool;
	double m_leafc_to_litr1c = 0.0 , m_leafc_to_litr2c = 0.0;
	double m_leafc_to_litr3c = 0.0, m_leafc_to_litr4c = 0.0;
	double m_deadleafc_to_litr1c = 0.0, m_deadleafc_to_litr2c = 0.0;
	double m_deadleafc_to_litr3c = 0.0, m_deadleafc_to_litr4c =0.0;
	double m_frootc_to_litr1c = 0.0, m_frootc_to_litr2c =0 ;
	double m_frootc_to_litr3c = 0.0, m_frootc_to_litr4c = 0.0;
	double m_leafc_store_to_snagc = 0.0;
	double m_frootc_store_to_litr1c = 0.0;
	double m_livestemc_store_to_litr1c = 0.0;
	double m_deadstemc_store_to_litr1c = 0.0;
	double m_livecrootc_store_to_litr1c = 0.0;
	double m_deadcrootc_store_to_litr1c = 0.0;
	double m_leafc_transfer_to_snagc = 0.0;
	double m_frootc_transfer_to_litr1c = 0.0;
	double m_livestemc_transfer_to_litr1c = 0.0;
	double m_deadstemc_transfer_to_litr1c = 0.0;
	double m_livecrootc_transfer_to_litr1c = 0.0;
	double m_deadcrootc_transfer_to_litr1c = 0.0;
	double m_gresp_store_to_litr1c = 0.0;
	double m_gresp_transfer_to_litr1c = 0.0;
	double m_leafn_to_litr1n, m_leafn_to_litr2n;
	double m_leafn_to_litr3n, m_leafn_to_litr4n;
	double m_deadleafn_to_litr1n, m_deadleafn_to_litr2n;
	double m_deadleafn_to_litr3n, m_deadleafn_to_litr4n;
	double m_frootn_to_litr1n, m_frootn_to_litr2n;
	double m_frootn_to_litr3n, m_frootn_to_litr4n;
	double m_livestemn_to_litr1n, m_livecrootn_to_litr1n;
	double m_leafn_store_to_snagn;
	double m_frootn_store_to_litr1n;
	double m_livestemn_store_to_litr1n;
	double m_deadstemn_store_to_litr1n;
	double m_livecrootn_store_to_litr1n;
	double m_deadcrootn_store_to_litr1n;
	double m_leafn_transfer_to_snagn;
	double m_frootn_transfer_to_litr1n;
	double m_livestemn_transfer_to_litr1n;
	double m_deadstemn_transfer_to_litr1n;
	double m_livecrootn_transfer_to_litr1n;
	double m_deadcrootn_transfer_to_litr1n;
	double m_retransn_to_litr1n;
	double m_livestemc_to_cwdc = 0.0;
	double m_deadstemc_to_cwdc = 0.0;
	double m_livecrootc_to_cwdc = 0.0;
	double m_deadcrootc_to_cwdc = 0.0;
	double m_livestemn_to_cwdn;
	double m_deadstemn_to_cwdn;
	double m_livecrootn_to_cwdn;
	double m_deadcrootn_to_cwdn;



	/* beetle stem to snag pool */
    double m_livestemc_to_snagc = 0.0;
	double m_deadstemc_to_snagc = 0.0;
	double m_livestemn_to_snagn = 0.0;
	double m_deadstemn_to_snagn = 0.0;

	//double m_snagc_to_cwdc; // exponential decay
	//double m_snagn_to_cwdn; // exponential decay


    /* beetle leaf to foliage pool */

    double m_leafc_to_redneedlec;
    double m_deadleafc_to_redneedlec;
    double m_leafn_to_redneedlen;
    double m_deadleafn_to_redneedlen;

	/* does the store and transfer also go to the snag pool do I need?? */
	/* and does the cpool also go to the snag pool */
/*    double m_livestemc_store_to_litr1c;
	double m_deadstemc_store_to_litr1c;
	double m_livestemc_transfer_to_litr1c;
	double m_deadstemc_transfer_to_litr1c;

    double m_livestemn_store_to_litr1n;
	double m_deadstemn_store_to_litr1n;
    double m_livestemn_transfer_to_litr1n;
	double m_deadstemn_transfer_to_litr1n; */



   if ((epc.veg_type==TREE && thintyp ==5 && epc.max_lai>=beetle[0].max_lai_th && epc.phenology_type == EVERGREEN ) || 
	   (veg_parm_ID == beetle[0].veg_ID_attack))
	//make sure the evergreen tree is attacked, the understory is decidous, the shrub has max_lai is 7
  // if (epc.veg_type==TREE && thintyp ==5 && epc.phenology_type ==EVERGREEN  )// this can isolate the understory but can not isolate the shrub due to shrub is evergreen and tree 20181126
   { // if it is the beetle attack and trees
	   if ((int)(cs->live_stemc) % 10 == 1)
	   {
		   printf("\n updating the beetle mortality  [veg_parm_ID %d],  [veg_ID_attack %d], [max_lai %lf], [max_lai_th %lf]\n", veg_parm_ID, beetle[0].veg_ID_attack, epc.max_lai, beetle[0].max_lai_th);
	   } // the index is the time series of beetle attack mortality 0 is the first one 24 is the second

	/******************************************************************/
	/* beetle attack mortality: stem go to snag pool then cwd pool and leaf go to dead doliage pool then litter pool */
	/******************************************************************/
	/* daily carbon fluxes due to mortality */
	/* mortality fluxes out of leaf and fine root pools */
	/* carbon depts in cpool have to die with the plant - could result in a carbon balance issue */
	/*******************************************************************/
	/* the  daily carbon fluxes leaf and fine root                     */
	/*******************************************************************/

	m_cpool = mort.mort_cpool * cs->cpool; // so how to calculate the mort.mort_cpool???
	m_npool = mort.mort_cpool * ns->npool;

    /* if outbreak, leafc go to the red needle pool instead of litter pool */
    /* here we suppose only the live leaf go the red needle pool, while the dead leaf and */
    /* leaf store and transfer go to the litter pool immediately */
    m_leafc_to_redneedlec = mort.mort_leafc * cs->leafc;

	/*m_leafc_to_litr1c = mort.mort_leafc * cs->leafc * epc.leaflitr_flab; // leaf mainly to litter pool flab is fraction to labile litter pool
	m_leafc_to_litr2c = mort.mort_leafc * cs->leafc * epc.leaflitr_fucel;
	m_leafc_to_litr3c = mort.mort_leafc * cs->leafc * epc.leaflitr_fscel;
	m_leafc_to_litr4c = mort.mort_leafc * cs->leafc * epc.leaflitr_flig; */
	m_deadleafc_to_litr1c = mort.mort_deadleafc * cs->dead_leafc * epc.leaflitr_flab; // dead leaf to litter pool
	m_deadleafc_to_litr2c = mort.mort_deadleafc * cs->dead_leafc * epc.leaflitr_fucel;
	m_deadleafc_to_litr3c = mort.mort_deadleafc * cs->dead_leafc * epc.leaflitr_fscel;
	m_deadleafc_to_litr4c = mort.mort_deadleafc * cs->dead_leafc * epc.leaflitr_flig;
	m_frootc_to_litr1c = mort.mort_frootc * cs->frootc * epc.frootlitr_flab;  // fine root to litter pool
	m_frootc_to_litr2c = mort.mort_frootc * cs->frootc * epc.frootlitr_fucel;
	m_frootc_to_litr3c = mort.mort_frootc * cs->frootc * epc.frootlitr_fscel;
	m_frootc_to_litr4c = mort.mort_frootc * cs->frootc * epc.frootlitr_flig;
	/* mortality fluxes out of storage and transfer pools */
	/* Assumes cpool mortality fraction applies to all non-structural stores and transfers */
	m_leafc_store_to_snagc  = mort.mort_cpool * cs->leafc_store; // what is differences between store and fluxes, and
	m_frootc_store_to_litr1c  = mort.mort_cpool * cs->frootc_store; // so the stores goes to the labile pool only? why
	m_leafc_transfer_to_snagc = mort.mort_cpool * cs->leafc_transfer;
	m_frootc_transfer_to_litr1c = mort.mort_cpool * cs->frootc_transfer;
	m_gresp_store_to_litr1c = mort.mort_cpool * cs->gresp_store;
	m_gresp_transfer_to_litr1c = mort.mort_cpool * cs->gresp_transfer;

    /*****************************************************************************/
	/* TREE-specific carbon fluxes the stem c and coarse root pool */
    /*****************************************************************************/
        m_livestemc_to_snagc = mort.mort_livestemc * cs->live_stemc;  //
		m_deadstemc_to_snagc = mort.mort_deadstemc * cs->dead_stemc; // stem go to the snag pool first
		m_livecrootc_to_cwdc = mort.mort_livecrootc * cs->live_crootc; // the coarse root go to cwd immediately
		m_deadcrootc_to_cwdc = mort.mort_deadcrootc * cs->dead_crootc;

		/* Assumes cpool mortality fraction applies to all non-structural stores and transfers */
		/* should the stem store and transfer go to the litter pool or also go to the snag pool */
		m_livestemc_store_to_litr1c  = mort.mort_cpool * cs->livestemc_store; // why livestem_to_cwdc is different with livestem_store_to_litr1c
		m_deadstemc_store_to_litr1c  = mort.mort_cpool * cs->deadstemc_store; // all the stem and fine root stores go to the litter pool not the cwd pool
		m_livecrootc_store_to_litr1c  = mort.mort_cpool * cs->livecrootc_store;
		m_deadcrootc_store_to_litr1c  = mort.mort_cpool * cs->deadcrootc_store; // the croot store and transfer go to litter

		m_livestemc_transfer_to_litr1c = mort.mort_cpool * cs->livestemc_transfer;
		m_deadstemc_transfer_to_litr1c = mort.mort_cpool * cs->deadstemc_transfer;
		m_livecrootc_transfer_to_litr1c = mort.mort_cpool * cs->livecrootc_transfer;
		m_deadcrootc_transfer_to_litr1c = mort.mort_cpool * cs->deadcrootc_transfer;
		 // not the snag pool thin

     /*************************************************************************/
	/* daily nitrogen fluxes due to mortality */
	/*  leaf and fine root pools */
	/***************************************************************************/

	if (epc.leaflitr_cn > ZERO) {  // leaf litter leaflitr_cn 93

	   /* treat the red needle pool as the dead leaf */
      m_leafn_to_redneedlen = m_leafc_to_redneedlec / epc.leaflitr_cn; // should i use leaf cn ratio, epc.leaflitr_cn or cel cn??
	  //or m_leafn_to_redneedlen = mort.mort_leafc*ns->leafn ??
	  m_leafn_to_litr1n = mort.mort_leafc * ns->leafn - m_leafn_to_redneedlen; // should it go to litter 1 or 2??
	  m_leafn_to_litr1n = max(m_leafn_to_litr1n, 0.0);



	   /*below is dead leaf not go to the red needle pool*/
		m_deadleafn_to_litr2n = m_deadleafc_to_litr2c / CEL_CN;
		m_deadleafn_to_litr3n = m_deadleafc_to_litr3c / CEL_CN;
		m_deadleafn_to_litr4n = m_deadleafc_to_litr4c / LIG_CN;
		m_deadleafn_to_litr1n = mort.mort_deadleafc*ns->dead_leafn - (m_deadleafn_to_litr2n+m_deadleafn_to_litr3n+m_deadleafn_to_litr4n);
		m_deadleafn_to_litr1n = max(m_deadleafn_to_litr1n, 0.0);
	}
	else {
		m_deadleafn_to_litr1n = 0.0;
		m_deadleafn_to_litr2n = 0.0;
		m_deadleafn_to_litr3n = 0.0;
		m_deadleafn_to_litr4n = 0.0;
	}
	if (epc.froot_cn > ZERO) { // fine root carbon and nitrogen
		m_frootn_to_litr2n = m_frootc_to_litr2c / CEL_CN;
		m_frootn_to_litr3n = m_frootc_to_litr3c / CEL_CN;
		m_frootn_to_litr4n = m_frootc_to_litr4c / LIG_CN;
		m_frootn_to_litr1n = mort.mort_frootc*ns->frootn - (m_frootn_to_litr2n+m_frootn_to_litr3n+m_frootn_to_litr4n);
		m_frootn_to_litr1n = max(m_frootn_to_litr1n, 0.0);
		}
	else {
		m_frootn_to_litr1n = 0.0;
		m_frootn_to_litr2n = 0.0;
		m_frootn_to_litr3n = 0.0;
		m_frootn_to_litr4n = 0.0;
		}

	/* mortality fluxes out of storage and transfer pools */
	/* Assumes same mortality fractions as for c pools */
	/* Assumes cpool mortality fraction applies to all non-structural stores and transfers */
	m_leafn_store_to_snagn  = mort.mort_cpool * ns->leafn_store; // leaf and fine root n store and transfer go into
	m_frootn_store_to_litr1n  = mort.mort_cpool * ns->frootn_store; // the litter
	m_leafn_transfer_to_snagn = mort.mort_cpool * ns->leafn_transfer;
	m_frootn_transfer_to_litr1n = mort.mort_cpool * ns->frootn_transfer;
	m_retransn_to_litr1n = mort.mort_cpool * ns->retransn; // the retranslocation flux

    /**********************************************************************/
	/* TREE-specific nitrogen fluxes */
    /* the stem go to snag and coarse root go to litter pool  */
    /*********************************************************************/
        m_livestemn_to_snagn = m_livestemc_to_snagc / epc.deadwood_cn; // here is n tfrom live stem and root o the snag pool
		m_livecrootn_to_cwdn = m_livecrootc_to_cwdc / epc.deadwood_cn; //root no change
		/* the left N go to the normal routine go to the CWD pool or go to the litter pool????*/
		m_livestemn_to_cwdn = (mort.mort_livestemc * ns->live_stemn) - m_livestemn_to_snagn;  // live stem n left go to the cwd or litter1?
		m_livecrootn_to_litr1n = (mort.mort_livecrootc * ns->live_crootn) - m_livecrootn_to_cwdn; // croot no change

		m_deadstemn_to_snagn = mort.mort_deadstemc * ns->dead_stemn;  // here is n from the dead stem ato snag pool
		m_deadcrootn_to_cwdn = mort.mort_deadcrootc * ns->dead_crootn;  // deat croot no change


		/* Assumes cpool mortality fraction applies to all non-structural stores and transfers */
		/* should the livestemn and deadstem n store and transfer go to the litter or go to snag pool */
		m_livestemn_store_to_litr1n  = mort.mort_cpool * ns->livestemn_store;
		m_deadstemn_store_to_litr1n  = mort.mort_cpool * ns->deadstemn_store;
		m_livecrootn_store_to_litr1n  = mort.mort_cpool * ns->livecrootn_store;
		m_deadcrootn_store_to_litr1n  = mort.mort_cpool * ns->deadcrootn_store;
		m_livestemn_transfer_to_litr1n = mort.mort_cpool * ns->livestemn_transfer;
		m_deadstemn_transfer_to_litr1n = mort.mort_cpool * ns->deadstemn_transfer;
		m_livecrootn_transfer_to_litr1n = mort.mort_cpool * ns->livecrootn_transfer;
		m_deadcrootn_transfer_to_litr1n = mort.mort_cpool * ns->deadcrootn_transfer;


   /************************************************************/
	/* update state variables */
   /************************************************************/
	/* ---------------------------------------- */
	/* CARBON mortality state variable update   */
	/* ---------------------------------------- */

    /***********************************************************/
	/* ABOVEGROUND C POOLS */
    /***********************************************************/
	/* add the stem c to the snag pool    */
	/* is not 2 (harvest case). If thintyp is 2, harvest aboveground c.   */
    // not harvest means these wood still there
	//	cs_litr->litr1c    += m_cpool;  // what does this mean, nsc storage go to litter, if noe harvest the abc, this should go to snag pool
		/*    Leaf mortality */

		//cs->redneedlec += m_leafc_to_redneedlec;

		redneedle_sequence->seq[inx].Cvalue +=m_leafc_to_redneedlec;  //NREN 20180630 the sequence [inx] is for tracking the removed C&N for each attack
		cs->delay_redneedlec +=m_leafc_to_redneedlec; // NREN 20180727 here is delayed dead leaf, stay there, not decay


		cs_litr->litr1c    += m_deadleafc_to_litr1c; // add the dead leaf to litter
		cs_litr->litr2c    += m_deadleafc_to_litr2c;
		cs_litr->litr3c    += m_deadleafc_to_litr3c;
		cs_litr->litr4c    += m_deadleafc_to_litr4c;
		/*cs_litr->litr1c    += m_leafc_store_to_litr1c; // add the storage to litter here still all the store and transfer go to the litter only, not go to 2,3,4
		cs_litr->litr1c    += m_leafc_transfer_to_litr1c; // addt the leaf transfer flux to litter */ //NREN test 20190908


			/*    Stem wood mortality */
			/*	  Transfer to snag pool if standing dead */

			/*	  Transfer to CWD otherwise */
			/* if it is beetle snag pool thintype=5 then should I update here or in the update patches to update daily*/


			    /*cs->snagc += m_livestemc_to_snagc; // this should be the ready to decay pool
                cs->snagc += m_deadstemc_to_snagc; */
                snag_sequence->seq[inx].Cvalue += m_livestemc_to_snagc; //for tracking the attack c amount
                snag_sequence->seq[inx].Cvalue += m_deadstemc_to_snagc;
                snag_sequence->seq[inx].Cvalue += (m_leafc_store_to_snagc + m_leafc_transfer_to_snagc + m_cpool); //assume all these extra carbon is in snag NREN20190908

                cs->delay_snagc += m_livestemc_to_snagc; // NREN 20180727 for the snag staying on the tree not decay
                cs->delay_snagc += m_deadstemc_to_snagc;
                cs->delay_snagc += (m_leafc_store_to_snagc + m_leafc_transfer_to_snagc + m_cpool); //NREN 20190908


               /* cs->cwdc       += m_livestemc_to_cwdc; // if here the snag not go to the cwd, means it is just stored in the snag pool for the next 5 years delay
				cs->cwdc       += m_deadstemc_to_cwdc; */ // NREN bug, here stemc should not go to cwd pool 20190908



			cs_litr->litr1c    += m_livestemc_store_to_litr1c; // why the stem store and transfer go to litter
			cs_litr->litr1c    += m_deadstemc_store_to_litr1c;
			cs_litr->litr1c    += m_livestemc_transfer_to_litr1c;
			cs_litr->litr1c    += m_deadstemc_transfer_to_litr1c;

		/* gresp... group in with aboveground? */
		cs_litr->litr1c         += m_gresp_store_to_litr1c;
		cs_litr->litr1c         += m_gresp_transfer_to_litr1c;

	/* Remove aboveground dead c from carbon stores in all cases. */
	cs->cpool -= m_cpool;
	/*    Leaf mortality */

	cs->leafc -= m_leafc_to_redneedlec;

	cs->dead_leafc          -= m_deadleafc_to_litr1c;
	cs->dead_leafc          -= m_deadleafc_to_litr2c;
	cs->dead_leafc          -= m_deadleafc_to_litr3c;
	cs->dead_leafc          -= m_deadleafc_to_litr4c;
	cs->leafc_store       -= m_leafc_store_to_snagc;
	cs->leafc_transfer      -= m_leafc_transfer_to_snagc;

		/*    Stem wood mortality */





        cs->live_stemc  -= m_livestemc_to_snagc;  //stem go to the snag pool
		cs->dead_stemc  -= m_deadstemc_to_snagc;





	/*	cs->live_stemc  -= m_livestemc_to_cwdc;  //stem go to the dead stem and store and transfer go to litter
		cs->dead_stemc  -= m_deadstemc_to_cwdc; */ //NREN here is the bug, livestem and dead stem carbon all go to snag pool
		cs->livestemc_store   -= m_livestemc_store_to_litr1c;
		cs->deadstemc_store   -= m_deadstemc_store_to_litr1c;
		cs->livestemc_transfer  -= m_livestemc_transfer_to_litr1c;
		cs->deadstemc_transfer  -= m_deadstemc_transfer_to_litr1c;

	/* gresp... group in with aboveground? */
	cs->gresp_store       -= m_gresp_store_to_litr1c;
	cs->gresp_transfer      -= m_gresp_transfer_to_litr1c;

    /***************************************************************/
	/* BELOWGROUND C POOLS */
	/* Belowground dead c goes to litter and cwd in all cases. */
	/*   Fine root mortality */
	/* based on ryan fire effect model, if the root go to litter pool immedialty after outbreak, */
	/*there will be an sudden increase of fire which is not realistic, to solve this problem, using defs to control the root*/
	/*mainly there are three possiblities:1. after beetle attack, assume the root is still alive, so nothing happened to root*/
	/*2. after attack, the root is dead, and go to litter pool, so this is the traditional mortality routine (but causing fire pulse problem*/
	/*3. after attack, the root is dead and harvest, which means the carbon is removed from root pool, but not going to the litter pool*/
	/*two parameters control it, root_alive, and harvest_dead_root, in beetles.def By ning ren 20190908 */
	/*root_alive =1 is alive =0 is root dead, root_alive ==3 is fine root is dead too, but move to dead_root_beetle pool and then slowly decay to litter pool*/
	/***************************************************************/
	if (beetle[0].root_alive == 0.0) { //if the root is dead after attack
        if(beetle[0].harvest_dead_root == 0.0) {//if harvest the dead root (1), then the carbon not go to litter pool, if not harvest, litter goto litter pool
	cs_litr->litr1c    += m_frootc_to_litr1c;
	cs_litr->litr2c    += m_frootc_to_litr2c;
	cs_litr->litr3c    += m_frootc_to_litr3c;
	cs_litr->litr4c    += m_frootc_to_litr4c;
	cs_litr->litr1c    += m_frootc_store_to_litr1c;
	cs_litr->litr1c    += m_frootc_transfer_to_litr1c;
		/* Coarse root wood mortality */
		cs->cwdc       += m_livecrootc_to_cwdc; //live coarse root
		cs->cwdc       += m_deadcrootc_to_cwdc; // dead coarse root
		cs_litr->litr1c       += m_livecrootc_store_to_litr1c; //root storage
		cs_litr->litr1c       += m_deadcrootc_store_to_litr1c; // dead root storage
		cs_litr->litr1c         += m_livecrootc_transfer_to_litr1c; //live root transfer
		cs_litr->litr1c         += m_deadcrootc_transfer_to_litr1c; // dead root transfer
        } // end if harvest_dead_root

	cs->frootc         -= m_frootc_to_litr1c;
	cs->frootc         -= m_frootc_to_litr2c;
	cs->frootc         -= m_frootc_to_litr3c;
	cs->frootc         -= m_frootc_to_litr4c;
	cs->frootc_store   -= m_frootc_store_to_litr1c;
	cs->frootc_transfer    -= m_frootc_transfer_to_litr1c;




		cs->live_crootc -= m_livecrootc_to_cwdc;
		cs->dead_crootc -= m_deadcrootc_to_cwdc;
		cs->livecrootc_store  -= m_livecrootc_store_to_litr1c;
		cs->deadcrootc_store  -= m_deadcrootc_store_to_litr1c;
		cs->livecrootc_transfer -= m_livecrootc_transfer_to_litr1c;
		cs->deadcrootc_transfer -= m_deadcrootc_transfer_to_litr1c;
      /*  printf("updating beetle attack mortality, the index is %d", inx);*/
        } // end if (root_alive)

    else if (beetle[0].root_alive ==2 ) {//if root_alive option 2 means the root is dead but moved to a dead_root_beetle pool and slowly decay to the litter pool NREN 20190910

    cs->dead_rootc_beetle += (m_frootc_to_litr1c + m_frootc_to_litr2c + m_frootc_to_litr3c + m_frootc_to_litr4c + m_frootc_store_to_litr1c + m_frootc_transfer_to_litr1c);
		/* Coarse root wood mortality */
		cs->cwdc       += m_livecrootc_to_cwdc; //live coarse root coarse root still go to the cwdc pool
		cs->cwdc       += m_deadcrootc_to_cwdc; // dead coarse root

		cs->dead_rootc_beetle       += m_livecrootc_store_to_litr1c; //root storage
		cs->dead_rootc_beetle       += m_deadcrootc_store_to_litr1c; // dead root storage
		cs->dead_rootc_beetle         += m_livecrootc_transfer_to_litr1c; //live root transfer
		cs->dead_rootc_beetle         += m_deadcrootc_transfer_to_litr1c; // dead root transfer


	cs->frootc         -= m_frootc_to_litr1c;
	cs->frootc         -= m_frootc_to_litr2c;
	cs->frootc         -= m_frootc_to_litr3c;
	cs->frootc         -= m_frootc_to_litr4c;
	cs->frootc_store   -= m_frootc_store_to_litr1c;
	cs->frootc_transfer    -= m_frootc_transfer_to_litr1c;




		cs->live_crootc -= m_livecrootc_to_cwdc;
		cs->dead_crootc -= m_deadcrootc_to_cwdc;
		cs->livecrootc_store  -= m_livecrootc_store_to_litr1c;
		cs->deadcrootc_store  -= m_deadcrootc_store_to_litr1c;
		cs->livecrootc_transfer -= m_livecrootc_transfer_to_litr1c;
		cs->deadcrootc_transfer -= m_deadcrootc_transfer_to_litr1c;

        } //end else if root_alive ==2
	/* ---------------------------------------- */
	/* NITROGEN mortality state variable update */
	/* ---------------------------------------- */
    /**************************************************/
	/* ABOVEGROUND N POOLS */
    /**************************************************/
	/* Only add dead leaf and stem n to litter and cwd pools if thintyp   */
	/* is not 2 (harvest case). If thintyp is 2, harvest aboveground n.   */

		//ns_litr->litr1n         += m_npool; //NREN test 20190908 here is the main reason causing the litter n pool have a spike npool and cpool should go to snag pool
		/*    Leaf mortality */
		//ns->redneedlen     += m_leafn_to_redneedlen;
		redneedle_sequence->seq[inx].Nvalue += m_leafn_to_redneedlen; // this is for tracking
		ns->delay_redneedlen += m_leafn_to_redneedlen; // for delayed dead leaf pool stay on tree not falling NREN 20180727

        ns_litr->litr1n    += m_leafn_to_litr1n; //NREN debug 20190908 11:30pm extra leafn is another main reason litrn increase pulse

		/*
		ns_litr->litr2n    += m_leafn_to_litr2n;
		ns_litr->litr3n    += m_leafn_to_litr3n;
		ns_litr->litr4n    += m_leafn_to_litr4n; */
		ns_litr->litr1n    += m_deadleafn_to_litr1n;
		ns_litr->litr2n    += m_deadleafn_to_litr2n;
		ns_litr->litr3n    += m_deadleafn_to_litr3n;
		ns_litr->litr4n    += m_deadleafn_to_litr4n;  //NREN debug 20190908 11:29
		/*ns_litr->litr1n    += m_leafn_store_to_litr1n;
		ns_litr->litr1n    += m_leafn_transfer_to_litr1n; */ //NREN test 20190908 11:29
		ns_litr->litr1n    += m_retransn_to_litr1n;

		/*    Stem wood mortality */
      //  ns_litr->litr1n     += m_livestemn_to_litr1n; //Here is the bug...NREN 20190908
			/*	  Transfer to CWD if there is leftwover after go to snagn */

        ns->cwdn       += m_livestemn_to_cwdn;
       /* ns->cwdn       += m_deadstemn_to_cwdn; //NREN here is the bug 20190908 all deadstemn go to snagn pool

              // beetle caused snag pool


      /*  ns->snagn       += m_livestemn_to_snagn; // the nitrogen stored in the snag pool not go the cwd pool
        ns->snagn       += m_deadstemn_to_snagn; */

        snag_sequence->seq[inx].Nvalue += m_livestemn_to_snagn; // for tracking each attack
        snag_sequence->seq[inx].Nvalue += m_deadstemn_to_snagn;
        snag_sequence->seq[inx].Nvalue += (m_leafn_store_to_snagn + m_leafn_transfer_to_snagn + m_npool); //assume extra nitrogen is in snag 20190908 NREN


        ns->delay_snagn += m_livestemn_to_snagn; // the nitrogen stored in the snag pool not go the cwd pool
        ns->delay_snagn += m_deadstemn_to_snagn; // NREN 20180828
        ns->delay_snagn += (m_leafn_store_to_snagn + m_leafn_transfer_to_snagn + m_npool); //assume extra nitrogen is in snag 20190908 NREN




       // ns->dead_stemn       += m_livestemn_to_cwdn; // the nitrogen stored in the snag pool not go the cwd pool
       // ns->dead_stemn       += m_deadstemn_to_cwdn;


        ns_litr->litr1n    += m_livestemn_store_to_litr1n;
        ns_litr->litr1n    += m_deadstemn_store_to_litr1n;
        ns_litr->litr1n    += m_livestemn_transfer_to_litr1n;
        ns_litr->litr1n    += m_deadstemn_transfer_to_litr1n;//NREN debug 20190908 11:29

	/* Remove aboveground dead n from n stores in all cases. */
        ns->npool -= m_npool;
	/*    Leaf mortality */

	   ns->leafn -= m_leafn_to_redneedlen;

	    ns->leafn -= m_leafn_to_litr1n;

        ns->dead_leafn          -= m_deadleafn_to_litr1n;
        ns->dead_leafn          -= m_deadleafn_to_litr2n;
        ns->dead_leafn          -= m_deadleafn_to_litr3n;
        ns->dead_leafn          -= m_deadleafn_to_litr4n;
        ns->leafn_store       -= m_leafn_store_to_snagn;
        ns->leafn_transfer      -= m_leafn_transfer_to_snagn;
        ns->retransn            -= m_retransn_to_litr1n;

		/*    Stem wood mortality */

		ns->live_stemn -= m_livestemn_to_snagn;
		ns->dead_stemn -= m_deadstemn_to_snagn;





		//ns->live_stemn  -= m_livestemn_to_litr1n; here is the bug stemn leftover should go to cwd pool instead of litter pool
		ns->live_stemn  -= m_livestemn_to_cwdn;
		//ns->dead_stemn  -= m_deadstemn_to_cwdn; here is the bug all dead stemn go to snag pool
		ns->livestemn_store   -= m_livestemn_store_to_litr1n;
		ns->deadstemn_store   -= m_deadstemn_store_to_litr1n;
		ns->livestemn_transfer  -= m_livestemn_transfer_to_litr1n;
		ns->deadstemn_transfer  -= m_deadstemn_transfer_to_litr1n;

/****************************************************************/
	/* BELOWGROUND N POOLS */
/****************************************************************/
	/* Belowground dead n goes to litter and cwd in all cases. */
	/*   Fine root mortality */
	if (beetle[0].root_alive == 0.0) {
        if(beetle[0].harvest_dead_root ==0){

	ns_litr->litr1n    += m_frootn_to_litr1n;
	ns_litr->litr2n    += m_frootn_to_litr2n;
	ns_litr->litr3n    += m_frootn_to_litr3n;
	ns_litr->litr4n    += m_frootn_to_litr4n;
	ns_litr->litr1n         += m_frootn_store_to_litr1n;
	ns_litr->litr1n         += m_frootn_transfer_to_litr1n;

		/* Coarse root mortality */
		ns_litr->litr1n     += m_livecrootn_to_litr1n;
		ns->cwdn       += m_livecrootn_to_cwdn;
		ns->cwdn       += m_deadcrootn_to_cwdn;
		ns_litr->litr1n         += m_livecrootn_store_to_litr1n;
		ns_litr->litr1n         += m_deadcrootn_store_to_litr1n;
		ns_litr->litr1n         += m_livecrootn_transfer_to_litr1n;
		ns_litr->litr1n         += m_deadcrootn_transfer_to_litr1n;
        } // end if harvest dead root


	ns->frootn         -= m_frootn_to_litr1n;
	ns->frootn         -= m_frootn_to_litr2n;
	ns->frootn         -= m_frootn_to_litr3n;
	ns->frootn         -= m_frootn_to_litr4n;
	ns->frootn_store      -= m_frootn_store_to_litr1n;
	ns->frootn_transfer     -= m_frootn_transfer_to_litr1n;


		ns->live_crootn -= m_livecrootn_to_litr1n; //line 289
		ns->live_crootn -= m_livecrootn_to_cwdn;
		ns->dead_crootn -= m_deadcrootn_to_cwdn;
		ns->livecrootn_store  -= m_livecrootn_store_to_litr1n;
		ns->deadcrootn_store  -= m_deadcrootn_store_to_litr1n;
		ns->livecrootn_transfer -= m_livecrootn_transfer_to_litr1n;
		ns->deadcrootn_transfer -= m_deadcrootn_transfer_to_litr1n;
            } // end of the if root_alive


    else if(beetle[0].root_alive == 2) { // condition 2 fine root go to dead_rootn_beetle pool, then slowly decay to litter pool

    ns->dead_rootn_beetle += (m_frootn_to_litr1n + m_frootn_to_litr2n + m_frootn_to_litr3n + m_frootn_to_litr4n + m_frootn_store_to_litr1n + m_frootn_transfer_to_litr1n);

		/* Coarse root mortality */
		ns->dead_rootn_beetle     += m_livecrootn_to_litr1n;
		ns->cwdn       += m_livecrootn_to_cwdn;
		ns->cwdn       += m_deadcrootn_to_cwdn;
		ns ->dead_rootn_beetle         += m_livecrootn_store_to_litr1n;
		ns ->dead_rootn_beetle         += m_deadcrootn_store_to_litr1n;
		ns ->dead_rootn_beetle         += m_livecrootn_transfer_to_litr1n;
		ns ->dead_rootn_beetle         += m_deadcrootn_transfer_to_litr1n;



	ns->frootn         -= m_frootn_to_litr1n;
	ns->frootn         -= m_frootn_to_litr2n;
	ns->frootn         -= m_frootn_to_litr3n;
	ns->frootn         -= m_frootn_to_litr4n;
	ns->frootn_store      -= m_frootn_store_to_litr1n;
	ns->frootn_transfer     -= m_frootn_transfer_to_litr1n;


		ns->live_crootn -= m_livecrootn_to_litr1n; //line 289
		ns->live_crootn -= m_livecrootn_to_cwdn;
		ns->dead_crootn -= m_deadcrootn_to_cwdn;
		ns->livecrootn_store  -= m_livecrootn_store_to_litr1n;
		ns->deadcrootn_store  -= m_deadcrootn_store_to_litr1n;
		ns->livecrootn_transfer -= m_livecrootn_transfer_to_litr1n;
		ns->deadcrootn_transfer -= m_deadcrootn_transfer_to_litr1n;




    }



    /* calculate the above ground litter part NREN 20190926*/
   cdf_patch->leafc_to_litr1c += m_leafc_to_litr1c;
   cdf_patch->leafc_to_litr1c += m_deadleafc_to_litr1c;
   cdf_patch->leafc_to_litr1c += m_gresp_store_to_litr1c;
   cdf_patch->leafc_to_litr1c += m_gresp_transfer_to_litr1c;

   cdf_patch->leafc_to_litr2c += m_leafc_to_litr2c;
   cdf_patch->leafc_to_litr2c += m_deadleafc_to_litr2c;

   cdf_patch->leafc_to_litr3c += m_leafc_to_litr3c;
   cdf_patch->leafc_to_litr3c += m_deadleafc_to_litr3c;

   cdf_patch->leafc_to_litr4c += m_leafc_to_litr4c;
   cdf_patch->leafc_to_litr4c += m_deadleafc_to_litr4c;

    //stem
   cdf_patch->stemc_to_litr1c += m_livestemc_store_to_litr1c;
   cdf_patch->stemc_to_litr1c += m_livestemc_transfer_to_litr1c;
   cdf_patch->stemc_to_litr1c += m_deadstemc_store_to_litr1c;
   cdf_patch->stemc_to_litr1c += m_deadstemc_transfer_to_litr1c;

    /* calculate the below ground litter part NREN 20190926 */
   cdf_patch->frootc_to_litr1c += m_frootc_to_litr1c;
   cdf_patch->frootc_to_litr1c += m_frootc_store_to_litr1c;
   cdf_patch->frootc_to_litr1c += m_livecrootc_store_to_litr1c;
   cdf_patch->frootc_to_litr1c += m_deadcrootc_store_to_litr1c;

   cdf_patch->frootc_to_litr2c += m_frootc_to_litr2c;

   cdf_patch->frootc_to_litr3c += m_frootc_to_litr3c;

   cdf_patch->frootc_to_litr4c += m_frootc_to_litr4c;

    /* calculate the stemc to cwdc pool */

   cdf_patch->stemc_to_cwdc += m_livestemc_to_cwdc;
   cdf_patch->stemc_to_cwdc += m_deadstemc_to_cwdc;

    //root to cwdc pool
   cdf_patch->rootc_to_cwdc += m_livecrootc_to_cwdc;
   cdf_patch->rootc_to_cwdc += m_deadcrootc_to_cwdc;

      } // end of the if at the beginning
	return;

}/*end update_beetle_attack_mortality*/


