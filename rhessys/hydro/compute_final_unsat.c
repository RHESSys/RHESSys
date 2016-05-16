/*--------------------------------------------------------------------------*/
/*  void compute_final_unsat()                                              */
/*                                                                          */
/*  SYNOPSIS                                                                */
/*      updates water-table depth  *satdefz  and unsat storage  *unsh2o     */
/*      for a change delh2o in unsaturated storage                          */
/*                                                                          */
/*  USES/PROVIDES                                                           */
/*      "integrate_*() routines that perform adaptive-Simpson               */
/*      integration of field capacity, residual, or both.                   */
/*                                                                          */
/*  PRECONDITION                                                            */
/*      valid only for final column with water table below surface          */
/*      [update of is trivial in that case :-) ]                            */
/*                                                                          */
/*  ASSUMPTIONS:                                                            */
/*      CASE 1  unsat-storage >= field capacity                             */
/*      the ratio of air-capacity to _excess_ unsaturated-zone water        */
/*      (i.e., above field capacity) is independent of depth.               */
/*                                                                          */
/*      CASE 2  unsat-storage <  field capacity                             */
/*      the ratio of field-capacity to _deficit_ unsaturated-zone water     */
/*      (i.e., below field capacity) is independent of depth.               */
/*                                                                          */
/*  NOTES                                                                   */
/*      code (very) loosely adapted from "compute_field_capacity.c"         */
/*                                                                          */
/*      delh2o > 0 means net _inflow_ to this vertical column               */
/*                                                                          */
/*      assumes a unified treatment of root-zone excess water and           */
/*      unsaturated-zone water, as for "hyrdo_routing.c"                    */
/*                                                                          */
/*      Uses "Regula Falsi" Secant Solver, for which (proportionally)       */
/*      the        number of significant bits ~ (number of iterations)^1.7  */
/*      (Bisection solver:   significant bits ~ (number of iterations)      */
/*      True Newton solver:  significant bits ~ (number of iterations)^2 )  */
/*      Note that the proportionality-constants depend upon the accuracy    */
/*      of the initial "trapping" estimates  za <= z <= zb                  */
/*                                                                          */
/*      Starting Z-estimates za,zb use displacement coming from constant    */
/*      porosity with unsat==0 (which must underestimate the zsat-change)   */
/*--------------------------------------------------------------------------*/

#include <stdio.h>
#include <math.h>

/*  tolerance:  EPSILON = 1.0e-3 mm.  DZ = 1 cm   */

#define     MAXLVL          (15)
#define     EPSILON         (1.0e-6)
#define     DZ              (1.0e-2)
#define     D3              (1.0 / 3.0)
#define     D6              (1.0 / 6.0)
#define     D12             (1.0 /12.0)
                
#define     MAX( A, B )     ((A) > (B) ? (A) : (B))
#define     MIN( A, B )     ((A) < (B) ? (A) : (B))

/*  macro for the theta-calculation from "compute_field_capacity.c"     */
/*  Four cases:                                                         */
/*      psi > psi_air_entry  ~~>  theta = 1                             */
/*      Use   theta_psi_curve  1  for theta                             */
/*      Use   theta_psi_curve  2  for theta                             */
/*      Use   theta_psi_curve  3  for theta                             */

#define     THETA( CURVE, PSI, PSIAIR, PORDEX, P3, P4 )                 \
    ( PSI <= PSIAIR ? 1.0                                               \
      : ( CURVE == 1 ? pow( PSIAIR / PSI, PORDEX )                      \
          : ( CURVE == 2 ? pow( 1.0 + pow( PSI/PSIAIR, P3 ), -PORDEX )  \
                         : exp( ( log( PSI ) - P3 ) / P4 ) ) ) )



/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--==-=-=-*/
/*  integrate_*()                                                                   */
/*                                                                                  */
/*  General Numerical Integration for field capacity and residual air capacity,     */
/*  after accounting for porosity and field capacity.                               */
/*  Uses Adaptive Simpson's First Rule with  tolerance  EPSILON                     */
/*                                                                                  */
/*  The strategy used for computing field capacity is:                              */
/*                                                                                  */
/*      i) adaptively discretize the soil column into layers from the               */
/*         water table to the soil surface.                                         */
/*      ii) for each layer,                                                         */
/*          a) compute psi as distance above water table                            */
/*             scaled by 1/weight density of water                                  */
/*          b) use the theta-psi curve to estimate theta                            */
/*          c) use the porosity decay curve with depth to estimate the porosity.    */
/*          d) solve the equation given above for theta_actual given theta_min      */
/*                                                                                  */
/*  If pord >= 999.0, then porosity is independent of depth:                        */
/*  return analytical solution for integrate_fldcap()                               */
/*                                                                                  */
/*  PRECONDITION                                                                    */
/*      zsat >= z0 >= z1 >= 0                                                       */
/*      dzmax > 0                                                                   */
/*                                                                                  */
/*  PROGRAMMERS NOTES                                                               */
//*  Set up, then call recursive worker-routines adapt_*()                          */
/*                                                                                  */
/*  General Numerical Integration for field capacity and residual air capacity,     */
/*  after accounting for porosity and field capacity.                               */
/*                                                                                  */
/*  ALGORITHM:  Adaptive Simpson's Rule                                             */
/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--==-=-=-*/

double adapt_fldcap( double  tol,         /*  accuracy-tolerance                              */
                     double  za,          /*  layer-top    soil-depth (M)                     */
                     double  zb,          /*  layer-bottom soil-depth (M)                     */
                     double  zc,          /*  layer-middle soil-depth (M)                     */
                     double  fa,
                     double  fb,
                     double  fc,
                     double  zsat,        /*  water-table  soil-depth (M)                     */
                     int     tpcurv,      /*  IN:  patch->soil_defaults[0][0].theta_psi_curve */
                     double  psiair,      /*  IN:  patch->soil_defaults[0][0].psi_air_entry   */
                     double  pordex,      /*  IN:  patch->soil_defaults[0][0].pore_size_index */
                     double  p3,          /*  IN:  patch->soil_defaults[0][0].p3              */
                     double  p4,          /*  IN:  patch->soil_defaults[0][0].p4              */
                     double  por0,        /*  IN:  patch->Ksoil_defaults[0][0].porosity_0     */
                     double  pord,        /*  IN:  patch->soil_defaults[0][0].porosity_decay  */
                     int     lvl
                     )
    {
    double  dz, depth, psi, poros, theta ;
    double  zd, ze, fd, fe, s1, s2 ;
    int     maxr ;

    zd    = 0.5 * ( za + zc ) ;
    depth = zd ;
    psi   = zsat - depth ;
    theta = THETA( tpcurv, psi, psiair, pordex, p3, p4 ) ;
    poros = por0 * exp( -depth / pord ) ;
    fd    = poros * theta ;

    ze    = 0.5 * ( zc + zb ) ;
    depth = ze ;
    psi   = zsat - depth ;
    theta = THETA( tpcurv, psi, psiair, pordex, p3, p4 ) ;
    poros = por0 * exp( -depth / pord ) ;
    fe    = poros * theta ;

    dz    = zb - za ;
    s1    = D6 * dz * ( fa + 4.0*fc + fb ) ;
    s2    = D12 * dz * ( fa + fb + 2.0 * fc + 4.0 * ( fd + fe ) ) ;

    if ( lvl >= MAXLVL )
        {
        return s2 ;
        }
    else if ( fabs( s2 - s1 ) < 15.0 * tol )
        {
        return s2 ;
        }
    else
        {
        lvl = lvl + 1 ;
        tol = 0.5 * tol ;
        return ( adapt_fldcap( tol, za, zc, zd, fa, fc, fd, zsat, tpcurv, psiair, pordex, p3, p4, por0, pord, lvl )
               + adapt_fldcap( tol, zc, zb, ze, fc, fb, fe, zsat, tpcurv, psiair, pordex, p3, p4, por0, pord, lvl ) ) ;
        }

    }           /*  end adapt_fldcap()  */


/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--==-=-=-*/

double integrate_fldcap( double  za,        /*  layer-top    soil-depth (M)                   */
                         double  zb,        /*  layer-bottom soil-depth (M)                   */
                         double  zsat,      /*  water-table  soil-depth (M)                   */
                         int     curve,     /*  patch->soil_defaults[0][0].theta_psi_curve    */
                         double  psiair,    /*  patch->soil_defaults[0][0].psi_air_entry (Pa) */
                         double  pordex,    /*  patch->soil_defaults[0][0].pore_size_index    */
                         double  p3,        /*  patch->soil_defaults[0][0].p3                 */
                         double  p4,        /*  patch->soil_defaults[0][0].p4                 */
                         double  por0,      /*  patch->soil_defaults[0][0].porosity_0         */
                         double  pord )     /*  patch->soil_defaults[0][0].porosity_decay     */
    {
    double  zc, fa, fb, fc, depth, psi, poros, theta, dz, wc ;

	pord = MAX( pord, 0.00000001 ) ;
	por0 = MAX( por0, 0.00000001 ) ;

    za = MAX( za, 0.0 ) ;
    zb = MAX( zb, za ) ;
    zb = MIN( zb, zsat ) ;
    za = MIN( za, zb ) ;

    if ( pord >= 999.0 )
        {
        /*  Return analytic solution:                                           */
        /*  Equation using fixed pressure head == -340 cm, from Dingman p. 235  */
        wc = por0 * pord * exp( -zb/pord ) - exp( -za/pord ) ;
        return  MIN( wc, por0 * pow( psiair/3.4, pordex ) * (zb - za ) ) ;
        }

    depth = za ;
    psi   = zsat - depth ;
    theta = THETA( curve, psi, psiair, pordex, p3, p4 ) ;
    poros = por0 * exp( -depth / pord ) ;
    fa    = poros * theta ;

    depth = zb ;
    psi   = zsat - depth ;
    theta = THETA( curve, psi, psiair, pordex, p3, p4 ) ;
    poros = por0 * exp( -depth / pord ) ;
    fb    = poros * theta ;

    dz    = zb - za ;
    zc    = 0.5 * ( za + zb ) ;
    depth = zc ;
    psi   = zsat - depth ;
    theta = THETA( curve, psi, psiair, pordex, p3, p4 ) ;
    poros = por0 * exp( -depth / pord ) ;
    fc    = poros * theta ;

    return adapt_fldcap( EPSILON, za, zb, zc, fa, fb, fc, zsat, curve, psiair, pordex, p3, p4, por0, pord, 0 ) ;

    }           /*  end integrate_fldcap()  */

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--==-=-=-*/

 void integrate_capacity( double  za,         /*  layer-top    soil-depth (M)                     */
                          double  zb,         /*  layer-bottom soil-depth (M)                     */
                          double  zsat,       /*  water-table  soil-depth (M)                     */
                          int     curve,      /*  patch->soil_defaults[0][0].theta_psi_curve      */
                          double  psiair,     /*  patch->soil_defaults[0][0].psi_air_entry (Pa)   */
                          double  pordex,     /*  patch->soil_defaults[0][0].pore_size_index      */
                          double  p3,         /*  patch->soil_defaults[0][0].p3                   */
                          double  p4,         /*  patch->soil_defaults[0][0].p4                   */
                          double  por0,       /*  patch->soil_defaults[0][0].porosity_0           */
                          double  pord,       /*  patch->soil_defaults[0][0].porosity_decay       */
                          double *fldcap,     /*  field capacity of this layer (M)                */
                          double *rescap )    /*  residual (non-field-capacity porosity) (M)      */
    {
    double fc, wc ;

	pord = MAX( pord, 0.00000001 ) ;
	por0 = MAX( por0, 0.00000001 ) ;

    za = MAX( za, 0.0 ) ;
    zb = MAX( zb, za ) ;
    zb = MIN( zb, zsat ) ;
    za = MIN( za, zb ) ;
    wc = por0 * pord * ( exp( -za/pord ) - exp( -zb/pord ) ) ;
    fc = integrate_fldcap( za, zb, zsat, curve, psiair, pordex, p3, p4, por0, pord ) ;

    *fldcap = fc ;
    *rescap = wc - fc ;

    return ;
    }           /*  end integrate_field_capacity()  */


/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--==-=-=-*/

double integrate_resid( double  za,        /*  layer-top    soil-depth (M)                   */
                        double  zb,        /*  layer-bottom soil-depth (M)                   */
                        double  zsat,      /*  water-table  soil-depth (M)                   */
                        int     curve,     /*  patch->soil_defaults[0][0].theta_psi_curve    */
                        double  psiair,    /*  patch->soil_defaults[0][0].psi_air_entry (Pa) */
                        double  pordex,    /*  patch->soil_defaults[0][0].pore_size_index    */
                        double  p3,        /*  patch->soil_defaults[0][0].p3                 */
                        double  p4,        /*  patch->soil_defaults[0][0].p4                 */
                        double  por0,      /*  patch->soil_defaults[0][0].porosity_0         */
                        double  pord )     /*  patch->soil_defaults[0][0].porosity_decay     */
    {
    double  wc ;

    za = MAX( za, 0.0 ) ;
    zb = MAX( zb, za ) ;
    zb = MIN( zb, zsat ) ;
    za = MIN( za, zb ) ;
    wc = por0 * pord * ( exp( -za/pord ) - exp( -zb/pord ) ) ;

    return  wc - integrate_fldcap( za, zb, zsat, curve, psiair, pordex, p3, p4, por0, pord ) ;

    }           /*  end integrate_resid()  */





/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--==-=-=-*/

void  compute_final_unsat( int     curve,       /*  IN:  patch->soil_defaults[0][0].theta_psi_curve */
                           double  psiair,      /*  IN:  patch->soil_defaults[0][0].psi_air_entry   */
                           double  por_dex,      /*  IN:  patch->soil_defaults[0][0].pore_size_index */
                           double  p3parm,      /*  IN:  patch->soil_defaults[0][0].p3              */
                           double  p4parm,      /*  IN:  patch->soil_defaults[0][0].p4              */
                           double  por_0,       /*  IN:  patch->Ksoil_defaults[0][0].porosity_0     */
                           double  por_d,       /*  IN:  patch->soil_defaults[0][0].porosity_decay  */
                           double  zsoil,       /*  IN:  soil depth        (M b.g.l.)               */
                           double  delh2o,      /*  IN:  water change      (vertical M)             */
					       double *satdefz,     /*  INOUT:  water table depth (M b.g.l.)            */
					       double *unsh2o,      /*  INOUT:  unsat-zone water storage (vertical M)   */
					       double *satdef )     /*  INOUT:  saturation deficit (M)                  */
    {
    double  zsat, unsat, unslyr, resid, fldcap, airfac, unsfac ;
    double  depth, psi, theta, poros, defsat, excess, deficit ;
    double  za, zb, zc, fa, fb, fc, dslope ;
    int     i, k, n ;


    zsat   = *satdefz ;
    unsat  = *unsh2o ;
    defsat = *satdef ;

    integrate_capacity( 0.0, zsat, zsat, curve, psiair, por_dex, p3parm, p4parm, por_0, por_d, &fldcap, &resid ) ;

    if ( fldcap <= unsat )          /*  excess unsaturated water, beyond field capacity  */
        {

        excess = unsat - fldcap ;
        unsfac = excess / resid ;           /*  fraction of residual     filled with unsat zone water   */
        airfac = 1.0 - unsfac ;             /*  fraction of residual not filled with unsat zone water   */
        defsat = airfac * resid ;           /*  == saturation deficit  > delh2o  by PRECONDITION        */

        /* "Regula Falsi" solvers to find new water table depth at which delh2o     */
        /* is exhausted by removing field-capacity plus excess water                */

        if ( delh2o > EPSILON )             /*  case that water table moves upwards */
            {
            /* Objective function                                   */
            /*      F(z) = defsat - airfac * resid( 0:z ) - delh2o  */
            /* where  resid  is computed by integrate_resid()       */
            /* and    air0 = airfac * resid( 0:zsat )               */
            /* F(zsat) = delh2o < 0  and hopefully  F(zsoil) > 0    */
            /* (else the entire column is unsaturated)              */
            /* Starting Z-value estimate   za is z for delh2o       */
            /* assuming constant porosity and no unsat water        */

            za    = ( defsat - delh2o ) / por_0 ;       /*  excess air space beyond delh2o, divided by porosity  */
            resid = integrate_resid( 0.0, za, za, curve, psiair, por_dex, p3parm, p4parm, por_0, por_d ) ;
            fa    = defsat - airfac * resid - delh2o ;
            zb    = zsat - delh2o / por_0 ;             /*  new zsat assuming only constant porosity  */
            resid = integrate_resid( 0.0, zb, zb, curve, psiair, por_dex, p3parm, p4parm, por_0, por_d ) ;
            fb    = defsat - airfac * resid - delh2o ;

            for ( ; ; )
                {
                dslope = ( zb - za ) / ( fb - fa ) ;
                zc     = za - fa * dslope ;
                resid  = integrate_resid( 0.0, zc, zc, curve, psiair, por_dex, p3parm, p4parm, por_0, por_d ) ;
                fc     = defsat - airfac * resid - delh2o ;
                if ( fc > EPSILON )
                    {
                    zb = zc ;
                    fb = fc ;
                    }
                else if ( fc < -EPSILON )
                    {
                    za = zc ;
                    fa = fc ;
                    }
                else if ( zb - za < DZ )
                    {
                    zc = zc - dslope * fc ;
                    break ;
                    }
                else
                    {
                    zc = zc - dslope * fc ;
                    break ;
                    }
                }

            /*  Use Simpson's Rule calculations of fldcap and resid to refine results:  */

            integrate_capacity( 0.0, zc, zc, curve, psiair, por_dex, p3parm, p4parm, por_0, por_d,  &fldcap, &resid ) ;
            *satdefz = zc ;
            *satdef  = airfac * resid ;
            *unsh2o  = fldcap + unsfac * resid ;
            }

        else if ( delh2o < -EPSILON )       /*  *case that water table  moves downwards   */
            {
            /* Objective function                                   */
            /*      F(z) = airfac * resid( 0:z ) + delh2o - satdef  */
            /* where  resid  is computed by integrate_resid()       */
            /* and    satdef = airfac * resid( 0:zsat )               */
            /* F(zsat) = delh2o < 0  and hopefully  F(zsoil) > 0    */
            /* (else the entire column is unsaturated)              */

            poros = por_0 * exp( -zsat / por_d ) ;
            za    = zsat - delh2o / poros ;         /*  new zsat assuming only porosity  */
            fa    = delh2o - defsat ;
            zb    = zsoil ;
            resid = integrate_resid( zsat, zb, zb, curve, psiair, por_dex, p3parm, p4parm, por_0, por_d ) ;
            fb    = airfac * resid + delh2o - defsat ;

            if ( fb <= 0.0 )         /*  entire column is unsaturated  */
                {
                integrate_capacity( 0.0, zsoil, zsoil, curve, psiair, por_dex, p3parm, p4parm, por_0, por_d, &fldcap, &resid ) ;
                *satdefz = zsoil ;
                *unsh2o  = fldcap + unsfac * resid ;
                return ;
                }

            for ( ; ; )
                {
                dslope = ( zb - za ) / ( fb - fa ) ;
                zc     = za - fa * dslope ;
                resid  = integrate_resid( 0.0, zc, zc, curve, psiair, por_dex, p3parm, p4parm, por_0, por_d ) ;
                fc     = airfac * resid + delh2o - defsat ;
                if ( fc > EPSILON )
                    {
                    zb = zc ;
                    fb = fc ;
                    }
                else if ( fc < -EPSILON )
                    {
                    za = zc ;
                    fa = fc ;
                    }
                else if ( zb - za < DZ )
                    {
                    zc = zc - dslope * fc ;
                    break ;
                    }
                else
                    {
                    zc = zc - dslope * fc ;
                    break ;
                    }
                }

            /*  Use Simpson's Rule calculations to refine results:  */

            integrate_capacity( 0.0, zc, zc, curve, psiair, por_dex, p3parm, p4parm, por_0, por_d, &fldcap, &resid ) ;
            *satdefz = zc ;
            *satdef  = airfac * resid ;
            *unsh2o  = fldcap + unsfac * resid ;
            }

        /*  else{} -- negligible delh2o:  return unchanged values  */

        }

    else        /*  unsat < fldcap:  deficit unsaturated water, less than field capacity  */
        {
        deficit = fldcap - unsat ;
        unsfac  = unsat   / fldcap ;
        airfac  = deficit / fldcap ;            /*  air = resid + airfac * fldcap  */
        defsat  = airfac * fldcap + resid ;     /*  == saturation deficit                                   */

        if ( delh2o > EPSILON )                 /*  case that water table moves upwards */
            {
            /* Objective function                                           */
            /*      F(z) = airfac * fldcap( 0:z ) + resid( 0:z ) - delh2o   */
            /* Starting Z-value estimate  za  is z for delh2o assuming      */
            /* constant porosity and no unsat water                         */

            za = delh2o / por_0 ;               /*  delh2o divided by porosity  */
            integrate_capacity( 0.0, za, za, curve, psiair, por_dex, p3parm, p4parm, por_0, por_d, &fldcap, &resid ) ;
            fa = airfac * fldcap + resid - delh2o ;

            poros = por_0 * exp( -zsat / por_d ) ;
            zb    = zsat - delh2o / poros ;
            integrate_capacity( 0.0, zb, zb, curve, psiair, por_dex, p3parm, p4parm, por_0, por_d, &fldcap, &resid ) ;
            fb = airfac * fldcap + resid - delh2o ;

            for ( ; ; )
                {
                dslope = ( zb - za ) / ( fb - fa ) ;
                zc     = za - fa * dslope ;
                integrate_capacity( 0.0, zc, zc, curve, psiair, por_dex, p3parm, p4parm, por_0, por_d, &fldcap, &resid ) ;
                fc = airfac * fldcap + resid - delh2o ;
                if ( fc > EPSILON )
                    {
                    zb = zc ;
                    fb = fc ;
                    }
                else if ( fc < -EPSILON )
                    {
                    za = zc ;
                    fa = fc ;
                    }
                else if ( zb - za < DZ )
                    {
                    zc = zc - dslope * fc ;
                    break ;
                    }
                else
                    {
                    zc = zc - dslope * fc ;
                    break ;
                    }
                }

            /*  Use Simpson's Rule calculations of fldcap and resid to refine results:  */

            integrate_capacity( 0.0, zc, zc, curve, psiair, por_dex, p3parm, p4parm, por_0, por_d, &fldcap, &resid ) ;
            *satdefz = zc ;
            *satdef  = airfac * fldcap + resid ;
            *unsh2o  = unsfac * fldcap + resid ;
            }

        else if ( delh2o < -EPSILON )       /*  *case that water table  moves downwards   */
            {
            /* Objective function                                           */
            /*      F(z) = airfac * fldcap( 0:z ) + resid( 0:z ) + delh2o   */
            /* Starting Z-value estimate  za  is z for delh2o assuming      */
            /* constant porosity and no unsat water                         */

            poros = por_0 * exp( -zsat / por_d ) ;
            za    = zsat - delh2o / poros ;
            integrate_capacity( 0.0, za, za, curve, psiair, por_dex, p3parm, p4parm, por_0, por_d, &fldcap, &resid ) ;
            fa    = airfac * fldcap + resid + delh2o ;
            zb    = zsoil ;
            integrate_capacity( 0.0, zb, zb, curve, psiair, por_dex, p3parm, p4parm, por_0, por_d, &fldcap, &resid ) ;
            fb    = airfac * fldcap + resid + delh2o ;

            if ( fb <= 0.0 )         /*  entire column is unsaturated  */
                {
                *satdefz = zsoil ;
                *unsh2o  = fldcap + unsfac * resid ;
                return ;
                }

            for ( ; ; )
                {
                dslope = ( zb - za ) / ( fb - fa ) ;
                zc     = za - fa * dslope ;
                integrate_capacity( 0.0, zc, zc, curve, psiair, por_dex, p3parm, p4parm, por_0, por_d,  &fldcap, &resid ) ;
                fc     = airfac * fldcap + resid + delh2o ;
                if ( fc > EPSILON )
                    {
                    zb = zc ;
                    fb = fc ;
                    }
                else if ( fc < -EPSILON )
                    {
                    za = zc ;
                    fa = fc ;
                    }
                else if ( zb - za < DZ )
                    {
                    zc = zc - dslope * fc ;
                    break ;
                    }
                else
                    {
                    zc = zc - dslope * fc ;
                    break ;
                    }
                }

            /*  Use Simpson's Rule calculations of fldcap and resid to refine results:  */

            integrate_capacity( 0.0, zc, zc, curve, psiair, por_dex, p3parm, p4parm, por_0, por_d, &fldcap, &resid ) ;
            *satdefz = zc ;
            *satdef  = airfac * fldcap + resid ;
            *unsh2o  = unsfac * fldcap + resid ;
            }

        /*  else{} -- negligible delh2o:  return unchanged values  */

        } ;

    return ;

    }           /*  end void  compute_final_unsat()     */

