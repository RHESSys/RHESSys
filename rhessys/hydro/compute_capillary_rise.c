/*--------------------------------------------------------------*/
/*                                                              */
/*		compute_capillary_rise				*/
/*                                                              */
/*  NAME                                                        */
/*		compute_capillary_rise				*/
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/*  void compute_capillary_rise(				*/
/*					int	,		*/	
/*					double	,		*/
/*					double	,		*/
/*					double	,		*/
/*					double	,		*/
/*					double	);		*/
/*                                                              */
/*  OPTIONS                                                     */
/*	int	verbose_flag					*/
/*	double	Z - (m) water table depth 			*/
/*	double	psi_1 - (Pa)  air_entry_pressure		*/
/*	double	pore_size_index,				*/
/*	double	m - TOPMODEL conductivitity decay parameter	*/
/*	double	Ksat_0	- (m/day) sat. hydraulic conductivity	*/
/*				at the surface			*/
/*                                                              */
/*                                                              */
/*  DESCRIPTION                                                 */
/*                                                              */
/*	This routine estimates the rate of capillary rise in	*/
/*	a soil profile following the methods and assumptions	*/
/*	of Gardiner (1958) as found in Eagleson (1978).		*/
/*								*/
/*	A basic outline of the development is as follows:	*/
/*								*/
/*	i) Use a 1-D Richard's Equation:			*/
/*								*/
/*		dT = d [ K(T) [ d psi(T) + 1 ]			*/
/*		__   __         _______				*/
/*		dt   dz         dz                              */
/*								*/
/*	where T - theta (effective soil moisture content ) (0-1)*/
/*		= (actual water vol. - min. water vol. )/	*/
/*		  (porosity - min. water. vol. )		*/
/*	      t - time (s) 					*/
/*	      K(T) - vertical hydraulic conductivity as a 	*/
/*			function of T (m/s)			*/
/*	      psi(T) - soil matric potential as a function of 	*/
/*			T (Pa)					*/
/*	and,							*/
/*		we assume that down is positive,		*/
/*		we assume that K an psi functions are in terms  */
/*		of effective moisture content 			*/
/*								*/
/*	ii) We assume that steady state flow has been reached:	*/
/*								*/
/*		dT/dt = 0 OR					*/
/*                                                              */
/*		-w =  K(T) [ d psi(T) + 1 ]			*/
/*		            _______				*/
/*		             dz                            	*/
/*								*/
/*	iii) isolating for dz and integrating from Z to 0 gives */
/*								*/
/*		Z = int(B1..B2) [ d(psi(T)) ]			*/
/*				____________			*/
/*				1 + -w/K(T)			*/
/*								*/
/*	where B1 and B2 are boundary conditions for K,psi,z	*/
/*	for the soil at Z and 0 respectively.			*/
/*								*/
/*	This integral is non-trivial.				*/
/*								*/
/*	iv) Assuming:						*/
/*								*/
/*		K(T) = a * psi(T)^b				*/
/*								*/
/*	where a and b are NOT functions of z.			*/
/* 	The integral can be solved for w.			*/
/*	For example, with b = 2, A=sqrt(a):			*/
/*								*/
/*	w = (A/Z) [ arctan({psi(B2)/A}) - arctan{psi(b2)/A)}	*/	
/*								*/
/*	v) Assuming psi(b2) = infinity and psi(b1) = 0		*/
/*								*/
/*	Gardiner (1958) provided an analytical approximation	*/
/*								*/
/*	w = w_max = Ba/(Z^b)					*/
/*								*/
/*	where B = 1 + 1.5/(b-1)					*/
/*								*/
/*	vi) The specification of a follows by assuming that	*/
/*								*/
/*	K(t)/K(1) = [psi(1)/psi(t)]^b				*/
/*								*/
/*	this assumption itself comes by assuminga Brooks and	*/
/*	Corey (1958) wetting curve for K(t) and psi(t):		*/
/*								*/
/*	k(t) = k(1)s^[(2+3m)/m]					*/
/*	psi(t) = psi(1)s^(-1/m)					*/
/*								*/
/*	and m is the pore size distribution index.		*/
/*								*/
/*	when b = [(2+3m)/m]*(-1/m)				*/
/*								*/
/*	a = k(1)[psi(1)]^b					*/
/*								*/
/*								*/
/*	Comments:						*/
/*								*/
/*	The first concern in using this formulation is		*/
/*	the steady state flow assumption.  However, if applied	*/
/*	at daily time steps this is likely OK.			*/
/*								*/
/*	The second concern is the use of the Brook's and Corey	*/
/*	curves.  These curves are not useful for soils and peats*/
/*	with a gradual drainage when saturated.  		*/
/*	Ther van Genuchten curves are empirically more suitable */
/*	but would complicate the derivation.			*/
/*								*/
/*	The third concern is that in reality k(1) and psi(1)	*/
/*	and m change with depth.  Typically both porosity  	*/
/*	and fraction of large pores drops with depth.		*/
/*								*/
/*	In light of these comments the user of this algorithm 	*/
/*	may want to consider it to be a non-linear regression	*/
/*	of sorts and may want to also consider the application	*/
/*	of strictly empirical fits to get cap rise as a function*/
/*	of depth of the water table.				*/
/*								*/
/*	If z is small we have an embarrassing situation.	*/
/*	we will assume the potential rate is essentially	*/
/*	infnite.						*/
/*								*/
/*  PROGRAMMER NOTES                                            */
/*                                                              */
/*                                                              */
/*--------------------------------------------------------------*/

#include <stdio.h>
#include <math.h>

double	compute_capillary_rise(
							   int	verbose_flag,
							   double	Z,
							   double	psi_1,
							   double	pore_size_index,
							   double	m_z,
							   double	Ksat_0)
{
	/*--------------------------------------------------------------*/
	/*  Local variable definition.                                  */
	/*--------------------------------------------------------------*/
	double	K_1;
	double	mc;
	double	w;
	/*--------------------------------------------------------------*/
	/*	Local function declaration.				*/
	/*--------------------------------------------------------------*/
	double	Ksat_z_curve(
		int,
		double,
		double,
		double);
	/*--------------------------------------------------------------*/
	/*	If z is small we have an embarrassing situation.	*/
	/*	we will assume the potential rate is essentially	*/
	/*	infnite.						*/
	/*--------------------------------------------------------------*/
	if ( (Z-psi_1) > 0 ){
		/*--------------------------------------------------------------*/
		/*	Estimate K_1  (i.e. K_sat)				*/
		/*--------------------------------------------------------------*/
		K_1 = Ksat_z_curve(
			verbose_flag,
			m_z,
			Z,
			Ksat_0 );
		mc = 2 + 3 * pore_size_index;
		w = K_1 * ( 1 + 1.5 / (mc - 1))
			* pow((psi_1/(Z-psi_1)),mc);
	}
	else{
		w = 0;
	}

	
	return(w);
}/*end compute_capillary_rise.c*/

