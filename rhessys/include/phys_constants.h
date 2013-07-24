#ifndef _PHYS_CONSTANTS_H_
#define _PHYS_CONSTANTS_H_


/* holds physical constants */
/* atmospheric constants */
/* from the definition of the standard atmosphere, as established
by the International CIvil Aviation Organization , and referenced in 

Iribane, J.V. and W.L. Godson, 1981.  Atmospheric Thermodynamics. 2nd
	Edition. D. Reidel Publishing Company, Dordrecht.  The Netherlands.
	(pp. 10,167-168,245)
*/

#define	G_STD	9.80665		/* (m2/s) standard gravitational accel.		*/
#define	P_STD	101325.0	/* (Pa)	standard pressure at 0.0 m elevation	*/
#define T_STD	288.15		/* (K) 	standard temp at 0.0 m elevation	*/
#define	MA	28.9644e-3	/* (kg/mol) molecular weight of air		*/
#define	MW	18.0148e-3	/* (kg/mol) molecular weight of water		*/
#define	CP	1010.0		/* (J/kg*K) specific heat of air		*/
#define	LR_STD	0.0065		/* (-K/m) standard temperature lapse rate	*/
#define	R	8.3143		/* (m3 PA / mol K) gas law constant		*/
#define	SBC	5.67e-8		/* (W/m2*K4) Stefan Boltzmann COnstant		*/
#define	EPS	0.6219		/* (MW/MA) unitless ratio of molecular weights	*/

#define HVAP 42.7       /* Heat of vaporization, kJ/mol */
#define KELVIN 273.16
#define PI	3.14159265359
#define seconds_per_day 86400
#define ICE_DENSITY 917.0 	/* (kg/m3) density of ice */

/*physiological constants	*/
#define	RAD2PAR	0.5		/* (DIM) ratio PAR/ SWtotal			*/
#define	EPAR	4.55		/* (umol/J) PAR photon energy ratio		*/
#define SECPERRAD 13750.9871            /* seconds per radian of hour angle */

#define LITTER_ALBEDO 0.15 /* changed from 0.02 to 0.15 based on Oke 1987 */
#define WATER_ALBEDO 0.05	/* average liquid water albedo for solar angle of 60 (Dingman) */

#define LIVELAB_CN  50           /* C:N for labile fraction of live wood */
#define LIG_CN      500.0        /* C:N for all lignin components */
#define CEL_CN      250.0        /* C:N for all cellulose components */
#define SOIL1_CN    12.0         /* C:N for fast microbial recycling pool */
#define SOIL2_CN    12.0         /* C:N for slow microbial recycling pool */
#define SOIL3_CN    10.0         /* C:N for recalcitrant SOM pool (humus) */
#define SOIL4_CN    8.0         /* C:N for recalcitrant SOM pool (lignin) */

#define NONWOOD_GRPERC 1.2  /* (DIM) growth resp per unit of nonwood C grown */
#define WOOD_GRPERC    2.0  /* (DIM) growth resp per unit of woody C grown */


#define  PARTICLE_DENSITY	2.65	/* soil particle density g/cm3 (Dingman) */

#endif
