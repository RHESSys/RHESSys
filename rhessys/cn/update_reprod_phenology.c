/*--------------------------------------------------------------*/
/*                                                              */ 
/*		update_reprod_phenology				*/
/*                                                              */
/*  NAME                                                        */
/*		update_reprod_phenology				*/
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/* 	void update_reprod_phenology(										*/
/*			struct epv_struct *,		                        */
/*			struct epconst_struct,   		*/
/*                      struct phenology_struct *,              */
/*                      struct cstate_struct *,                 */
/*                      struct cdayflux_struct *,               */
/*                      struct cdayflux_patch_struct *,         */
/*                      struct nstate_struct *,                 */
/*                      struct ndayflux_struct *,               */
/*                      struct ndayflux_patch_struct *,         */
/*                      struct litter_c_object *,               */
/*                      struct litter_n_object *,               */
/*                      double,                                 */
/*			struct date current_date,		*/
/*			struct date command_line)		*/
/*                                  				*/
/*                                                              */
/*  OPTIONS                                                     */
/*                                                              */
/*  DESCRIPTION                                                 */
/*                                                              */
/*	performs seasonal leaf drop and budding   		*/
/*	updates annnual allocates during leaf out		*/
/*	computes leaf and fine root litfall			*/
/*                                                              */
/*                                                              */
/*  PROGRAMMER NOTES                                            */
/*                                                              */
/*	modifed from phenology and prephenology in		*/
/*	P.Thornton (1997) version of 1d_bgc			*/
/*                                                              */
/*                                                              */
/*--------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include "rhessys.h"

void update_reprod_phenology(
					  struct epvar_struct	*epv ,
					  struct epconst_struct	epc	,
					  struct reproduction_phenology_struct *phen,	
					  struct cstate_struct *cs,
					  struct cdayflux_struct	*cdf,
					  struct cdayflux_patch_struct *cdf_patch,
					  struct nstate_struct *ns,
					  struct ndayflux_struct	*ndf,
					  struct ndayflux_patch_struct *ndf_patch,
					  struct litter_c_object *cs_litr,
					  struct litter_n_object *ns_litr,
					  struct litter_object *litter,
					  double	cover_fraction,
					  struct date current_date,
					  struct command_line_object *command_line)
{
	/*--------------------------------------------------------------*/
	/*  Local function declaration                                  */
	/*--------------------------------------------------------------*/
	long	yearday( struct date);
	int	compute_annual_seedfall(
		struct epconst_struct,
		struct phenology_struct *,
		struct cstate_struct *);
	int     compute_seed_decay(	struct epconst_struct *,
		double,
		struct cstate_struct *,
		struct nstate_struct *,
		struct litter_c_object *,
		struct litter_n_object *,
		struct cdayflux_patch_struct *,
		struct ndayflux_patch_struct *);
	int	compute_reprod_litfall(
		struct epconst_struct,
		double ,
		double ,
		struct cstate_struct *,
		struct nstate_struct *,
		struct cdayflux_struct *,
		struct ndayflux_struct *);
		
	/*--------------------------------------------------------------*/
	/*  Local variable definition.                                  */
	/*--------------------------------------------------------------*/

	int ok=1;
	long day;
	double perc_sunlit, reprodlitfallc;
	int remdays_transfer;
	int expand_flag, litfall_flag;


	expand_flag = 0; 
	reprodlitfallc = 0.0;
	litfall_flag = 0;
	day = yearday(current_date);
	
	/*--------------------------------------------------------------*/
	/*  for now we only have STATIC phenology for reproduction.     */
	/* 	see regular update-phenology for how dynamic phenology might */
	/* be implemented						*/
	/*--------------------------------------------------------------*/
	if (phen->expand_startday < phen->expand_stopday) {
		if ((day >= phen->expand_startday) && (day < phen->expand_stopday))
			expand_flag = 1;
		}
	else {
		if ((day >= phen->expand_startday) || (day < phen->expand_stopday))
			expand_flag = 1;
	}

		if (phen->litfall_startday < phen->litfall_stopday) {
		if ((day >= phen->litfall_startday) && (day < phen->litfall_stopday))
			litfall_flag = 1;
	}
	else if (phen->litfall_startday == phen->litfall_stopday) {
		if (day != phen->litfall_stopday)
			litfall_flag = 1;
	}
	else if (phen->litfall_startday > phen->litfall_stopday) {
		if ((day >= phen->litfall_startday) || (day < phen->litfall_stopday))
			litfall_flag = 1;
	}
	

	phen->daily_allocation = epc.alloc_prop_day_growth;
	phen->annual_allocation = 0;
	/*--------------------------------------------------------------*/
	/*	Leaf expansion - spring leaf out			*/
	/*--------------------------------------------------------------*/

	if (expand_flag == 1) {
		if (day <= phen->expand_stopday) 
			remdays_transfer = max(1.0,(phen->expand_stopday - day));
		else
			remdays_transfer = max(1.0,(phen->expand_stopday + 365 - day));

	
		cdf->reprodc_transfer_to_reprodc = 2.0*cs->reprodc_transfer / remdays_transfer;
		ndf->reprodn_transfer_to_reprodn = 2.0*ns->reprodn_transfer / remdays_transfer;
	}
	/*--------------------------------------------------------------*/
	/*	seed drop - fall litter fall				*/
	/*--------------------------------------------------------------*/

	/*--------------------------------------------------------------*/
	/* at beginning of litter fall figure out how much to drop */
	/* what percentage of reproductive tissue to drop */
	/*--------------------------------------------------------------*/
	if (day == phen->litfall_startday)  {
		ok = compute_annual_seedfall(epc, phen, cs);
	}

	/*--------------------------------------------------------------*/
	/*	compute daily litter fall				*/
	/*--------------------------------------------------------------*/
	if (litfall_flag == 1) {
		if (day <= phen->litfall_stopday) 
			remdays_transfer = (phen->litfall_stopday - day);
		else
			remdays_transfer = phen->litfall_stopday + 365 - day;
		reprodlitfallc = 2.0*phen->reprodlitfallc / remdays_transfer;
		if (reprodlitfallc > cs->reprodc)
			reprodlitfallc = cs->reprodc;
	}
	/*--------------------------------------------------------------*/
	/*	on the last day of litterfall make sure that deciduous no longer */
	/*	have any reproduction material left					*/
	/*	this is also considered to be the end of the growing season */
	/*	so do annual allcation					*/
	/*--------------------------------------------------------------*/
	 if (( day == phen->litfall_stopday) ){
		if (epc.phenology_type == DECID) {
			reprodlitfallc = cs->reprodc;
			phen->daily_allocation = 0;
			}
		phen->annual_allocation = 1;
	}


	/*--------------------------------------------------------------*/
	/*	update growth variables					*/
	/*		this is expression of allocation from 		*/
	/*		last seasons stored photosynthesis		*/
	/*	note all cdf and ndf variables are zero'd at the 	*/
	/*	start of the day, so only values set above are used	*/
	/*--------------------------------------------------------------*/
	 
	/* reproduction transfers from last years storage */
		cs->reprodc            += cdf->reprodc_transfer_to_reprodc;
		cs->reprodc_transfer   -= cdf->reprodc_transfer_to_reprodc;
		ns->reprodn            += ndf->reprodn_transfer_to_reprodn;
		ns->reprodn_transfer   -= ndf->reprodn_transfer_to_reprodn;
			
	/*--------------------------------------------------------------*/
	/* check for leaf and fine root litfall for this day */
	/*--------------------------------------------------------------*/
	if ((reprodlitfallc > ZERO )  && (cs->reprodc > ZERO) && (ns->reprodn > ZERO)){
		/*--------------------------------------------------------------*/
		/* set daily flux variables */
		/*--------------------------------------------------------------*/
		/*	compute reprod litter fall				*/
		/*--------------------------------------------------------------*/
		if (ok && compute_reprod_seedfall(epc,
			reprodlitfallc,cover_fraction,
			cs,ns, cdf,ndf)){
			fprintf(stderr,
				"FATAL ERROR: in seed_litfall() from update_reprod_phenology()\n");
			exit(EXIT_FAILURE);
		}
		phen->reprodlitfallc -= reprodlitfallc;
		if (phen->reprodlitfallc < 0)
			phen->reprodlitfallc = 0;
	}


		/*--------------------------------------------------------------*/
		/*	compute seed debris fragmentation		*/
		/*--------------------------------------------------------------*/
		if ((cs->seedc > ZERO) && (cover_fraction > ZERO)) {
			if (ok && compute_seed_decay(&(epc),cover_fraction, cs,ns,cs_litr,
				ns_litr,cdf_patch,ndf_patch)){
				fprintf(stderr,
					"FATAL ERROR: in seed_decay() from update_phenology()\n");
				exit(EXIT_FAILURE);
			}
		}


	/*--------------------------------------------------------------*/
	/* check for rounding errors on end of litfall season */
	/*--------------------------------------------------------------*/
	if (fabs(cs->reprodc) <= 1e-13){
		cs->reprodc = 0.0;
		ns->reprodn = 0.0;
	}



	return;

}/*end update reprod phenology*/
