/*----------------------------------------------------------------------*/
/*  void compute_final_unsat()                                          */
/*                                                                      */
/*  SYNOPSIS                                                        *   */
/*      updates water-table depth  *satdefz  and unsat storage  *unsh2o */
/*      for a change delh2o in saturated storage                        */
/*                                                                      */
/*  PRECONDITION                                                        */
/*      valid only for final column with water table below surface      */
/*      [update of is trivial in that case :-) ]                        */
/*                                                                      */
/*  NOTES                                                               */
/*      code adapted from "compute_field_capacity.c"                    */
/*                                                                      */
/*      delh2o > 0 means net _inflow_ to this vertical column           */
/*                                                                      */
/*      assumes a unified treatment of root-zone excess water and       */
/*      unsaturated-zone water, as for "hyrdo_routing.c"                */
/*----------------------------------------------------------------------*/

#include <stdio.h>
#include <math.h>
#include "rhessys.h"    /*  defines INTERVAL_SIZE   */

/*  tolerance:  1.0e-3 mm   */

#define     EPSILON         (1.0e-6)
#define     D34             (1.0 / 3.4)

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

#define     DINGMAN( POR_0, PSIAIR, PORDEX, PSI )                       \
    ( POR_0 * PSI * pow( D34 * PSIAIR , PORDEX ) 

void    compute_final_unsat(
                           int     tpcurv,      /*  IN:  patch->soil_defaults[0][0].theta_psi_curve */
                           double  psiair,      /*  IN:  patch->soil_defaults[0][0].psi_air_entry   */
                           double  pordex,      /*  IN:  patch->soil_defaults[0][0].pore_size_index */
                           double  p3parm,      /*  IN:  patch->soil_defaults[0][0].p3              */
                           double  p4parm,      /*  IN:  patch->soil_defaults[0][0].p4              */
                           double  por_0,       /*  IN:  patch->Ksoil_defaults[0][0].porosity_0     */
                           double  por_d,       /*  IN:  patch->soil_defaults[0][0].porosity_decay  */
                           double  zsoil,       /*  IN:  soil depth        (M b.g.l.)               */
                           double  fldcap,      /*  IN:  unsat-zone field capacity (vertical M)     */
                           double  delh2o,      /*  IN:  water change      (vertical M)             */
					       double *satdefz,     /*  INOUT:  water table depth (M b.g.l.)            */
					       double *unsh2o       /*  INOUT:  unsat-zone water storage (vertical M)   */
                           )
    {
    double  zsat, unsat, unslyr, airlyr, unsfac, depth, psi, theta, poros, fraclyr ;

    zsat   = *satdefz ;
    unsat  = *unsh2o ;
    unsfac =  unsat / fldcap ;          /*  ratio of unsat zone water to field-capacity water   */

    if ( delh2o > EPSILON )             /*  case that water table moves upwards */
        {

        /* For positive  delh2o, integrate upwards until delh20 is exhausted.   */
        /* Depth  zsat  decrements by INTERVAL_SIZE  while unsaturated-zone     */
        /* unsat-zone water  unsat decrements by unslyr == unsfac times theta   */
        /* "new" water  delh2o     decrements by airlyr == remaining porosity   */

        for ( psi = 0.0, depth = zsat ; depth > 0.0 ; depth -= INTERVAL_SIZE, psi += INTERVAL_SIZE )
            {
            theta  = THETA( tpcurv, psi, psiair, pordex, p3parm, p4parm ) ;
            poros  = por_0 * exp( -depth / por_d ) ;
            unslyr = INTERVAL_SIZE * poros * unsfac * theta ;           /*  unsat water for this layer   */
            airlyr = INTERVAL_SIZE * poros - unslyr ;                   /*  this-layer air-filled space  */

            if ( airlyr < delh2o )     /*  this layer does not exhaust  delh20     */
                {
                delh2o -= airlyr ;
                zsat   -= INTERVAL_SIZE ;
                unsat   = unsat - unslyr ;
                }

            else            /*  fraction  fraclyr  of this layer exhausts delh20:  finish up and quit loop    */
                {
                fraclyr  = delh2o / airlyr ;
                zsat    -= fraclyr * INTERVAL_SIZE ;
                unsat    = unsat - fraclyr * unslyr ;
                break ;
                }
            }
        *satdefz = zsat ;
        *unsh2o  = unsat ;
        }

    else if ( delh2o < -EPSILON )             /*  *case that water table  moves downwards   */
        {

        /* TODO:  Is this formulation for "theta" correct for this case ??                  */

        /* For negative  delh2o, integrate downwards until delh20 is exhausted.             */
        /* Depth  zsat  increments by INTERVAL_SIZE                                         */
        /* unsaturated-zone water  unsat  decrements by unslyr == unsfac times theta        */
        /* (negative) "new" water  delh2o increments by airlyr == remaining porosity        */

        for ( psi = 0.0, depth = zsat ; depth < zsoil; depth += INTERVAL_SIZE, psi += INTERVAL_SIZE )
            {
            theta  = THETA( tpcurv, psi, psiair, pordex, p3parm, p4parm ) ;
            poros  = por_0 * exp( -depth / por_d ) ;
            unslyr = INTERVAL_SIZE * poros * unsfac * theta ;           /*  unsat water for this layer   */
            airlyr = INTERVAL_SIZE * poros - unslyr ;                   /*  available air-filled space   */

            if ( airlyr < -delh2o )     /*  this layer does not exhaust  delh20  (which is negative)   */
                {
                delh2o += airlyr ;                          /*  add this available space to (negative) delh20  */
                zsat   += INTERVAL_SIZE ;
                unsat   = unsat + unslyr ;
                }

            else        /*  fraction  fraclyr  of this layer exhausts delh20:  finish up and quit loop     */
                {
                fraclyr = -delh2o / unslyr ;
                zsat   +=  fraclyr * INTERVAL_SIZE ;
                unsat   =  unsat + fraclyr * unslyr ;
                break ;
                }
            }
        *satdefz = zsat ;
        *unsh2o  = unsat ;
        }

    /*  else{} -- negligible delh2o:  return unchanged values  */

    }
