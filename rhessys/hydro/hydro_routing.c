/*--------------------------------------------------------------------------*/
/*  NAME                                                                    */
/*                                                                          */
/*                                                                          */
/*  SYNOPSIS                                                                */
/*      Kinematic surface routing.                                          */
/*                                                                          */
/*  DESCRIPTION                                                             */
/*      Uses an implementation based on "inflow matrices" and an            */
/*      adaptive time step.  For each cell and for surface-head and for     */
/*      each species chem in {H)3, NH4, DOC, DON}:                          */
/*                                                                          */
/*      We represent head and chem as column-vectors.                       */
/*                                                                          */
/*      hh  = head - retdep [M]                                             */
/*      vel = hh^(2/3) * sqrt( slope ) / ( dx * roughness ) [cells/S]       */
/*      d(head)/dt = vel * hh                                               */
/*      d(chem)/dt = vel * chem * ( hh / head )                             */
/*                                                                          */
/*      Note time-independent factor                                        */
/*      kernel = sqrt( slope ) / ( dx * roughness )                         */
/*                                                                          */
/*      Then for each source-cell S and receptor-cell R there is an         */
/*      exchange fraction gamma(S,R), so that the state-update equations    */
/*      are:                                                                */
/*                                                                          */
/*      head(R) = head(R) - dt * d(head)/dt(R)                              */
/*        + dt * sum{ gamma(S,R) * d(head)/dt(S) * area(S) } / area(R)      */
/*                                                                          */
/*      and similarly for each species chem.                                */
/*                                                                          */
/*      Again, note the time independent indexing arrays and factors        */
/*      mcount(R)   = number     of sources that flow into cell R           */
/*      mindex(R,S) = subscripts of sources that flow into cell R           */
/*      mgamma(R,S) = gamma(S,R) * area(S) } / area(R)                      */
/*                                                                          */
/*      The "inflow matrices" approach is necessary for a parallel          */
/*      implementation, since we need a single point of update for          */
/*      the state at each patch:  it is unsafe for one patch to update      */
/*      the state of its downhill neighbors:  more than one patch might     */
/*      be updating any one of these neighbors simultaneously, leading      */
/*      to race conditions and incorrect results.                           */
/*                                                                          */
/*  CONTAINS                                                                */
/*      void init_hydro_routing()                                           */
/*          allocates working data structures and computes                  */
/*          time independent kernel, mcount, mindex, mgamma                 */
/*                                                                          */
/*      void hydro_sfc_routing()                                            */
/*          copy initial state-data into working head and chem vectors;     */
/*          loop on adaptive internal time steps                            */
/*              parallel loop computing the  d(head)/dt and d(chem)/dt      */
/*              compute Courant-stable time step dt                         */
/*              parallel loop updating the head and chem for time dt        */
/*          update state-data from working vectors                          */
/*                                                                          */
/*  PROGRAMMER NOTES                                                        */
/*      Initial version 3/12/2015 by Carlie J. Coats, Jr., UNC IE           */
/*--------------------------------------------------------------------------*/
#include <stdio.h>
#include <math.h>
#include "rhessys.h"

void    *alloc( size_t, char *, char *);

/*  MAXNEIGHBOR should be a multiple of 4, for memory-alignment reasons */
/*  EPSILON     used for roundoff-tolerance criterion (sec)             */
/*  COUMAX      Courant-stability threshold                             */

#define     MAXNEIGHBOR     (16)
#define     TWOTHD          (2.0/3.0)
#define     DEG2RAD         (M_PI/180.0)
#define     EPSILON         (1.0e-5)
#define     COUMAX          (0.5)

static unsigned num_patches = -9999 ;

struct patch_object * * plist ;

static double * retdep ;
static double * sfcH2O ;
static double * sfcNO3 ;
static double * sfcNH4 ;
static double * sfcDOC ;
static double * sfcDON ;
static double * kernel ;
static double * dH2Odt ;
static double * dNO3dt ;
static double * dNH4dt ;
static double * dDOCdt ;
static double * dDONdt ;
static double * dNO3dt ;

/*  Drainage Matrix  */

static unsigned * mcount ;      /*   used as mcount[num_patches]                */
static unsigned * mindex ;      /*   used as mindex[num_patches][MAXNEIGHBOR]   */
static double   * mgamma ;      /*   used as mgamma[num_patches][MAXNEIGHBOR]   */

/*--------------------------------------------------------------------------*/

void init_hydro_routing( struct command_line_object * command_line,
                         struct basin_object        * basin )
    {
    unsigned                i, j, k, m, n ;
    unsigned                num_matrix ;
    double                  ddx ;
	struct patch_object *   patch ;
	struct patch_object *   neigh ;

    num_patches = basin->route_list->num_patches ;
    num_matrix  = MAXNEIGHBOR * num_patches ;

    plist  = (struct patch_object * *) alloc( num_patches * sizeof(struct patch_object *), "plist", "hydro_routing/init_hydro_routing()" ) ;

    retdep = (double   *) alloc( num_patches * sizeof(   double ), "retdep", "hydro_routing/init_hydro_routing()" ) ;
    sfcH2O = (double   *) alloc( num_patches * sizeof(   double ), "sfcH2O", "hydro_routing/init_hydro_routing()" ) ;
    sfcH2O = (double   *) alloc( num_patches * sizeof(   double ), "sfcNO3", "hydro_routing/init_hydro_routing()" ) ;
    sfcH2O = (double   *) alloc( num_patches * sizeof(   double ), "sfcNH4", "hydro_routing/init_hydro_routing()" ) ;
    sfcH2O = (double   *) alloc( num_patches * sizeof(   double ), "sfcDOC", "hydro_routing/init_hydro_routing()" ) ;
    sfcH2O = (double   *) alloc( num_patches * sizeof(   double ), "sfcDON", "hydro_routing/init_hydro_routing()" ) ;
    kernel = (double   *) alloc( num_patches * sizeof(   double ), "kernel", "hydro_routing/init_hydro_routing()" ) ;
    dH2Odt = (double   *) alloc( num_patches * sizeof(   double ), "dH2Odt", "hydro_routing/init_hydro_routing()" ) ;
    dNO3dt = (double   *) alloc( num_patches * sizeof(   double ), "dNO3dt", "hydro_routing/init_hydro_routing()" ) ;
    dNH4dt = (double   *) alloc( num_patches * sizeof(   double ), "dNH4dt", "hydro_routing/init_hydro_routing()" ) ;
    dDOCdt = (double   *) alloc( num_patches * sizeof(   double ), "dDOCdt", "hydro_routing/init_hydro_routing()" ) ;
    dDONdt = (double   *) alloc( num_patches * sizeof(   double ), "dDONdt", "hydro_routing/init_hydro_routing()" ) ;
    mcount = (unsigned *) alloc( num_patches * sizeof( unsigned ), "mcount", "hydro_routing/init_hydro_routing()" ) ;
    mindex = (unsigned *) alloc( num_matrix  * sizeof( unsigned ), "mindex", "hydro_routing/init_hydro_routing()" ) ;
    mgamma = (double   *) alloc( num_matrix  * sizeof(   double ), "mgamma", "hydro_routing/init_hydro_routing()" ) ;

#pragma omp parallel for    \
        private( i, patch ) \
        shared( num_patches, basin, plist, mcount, retdep, kernel )
    for ( i = 0; i < num_patches; i++ )
        {
        patch     = basin->route_list->list[i] ;
        plist[i]  = patch ;
        mcount[i] = 0 ;
        retdep[i] = patch->soil_defaults[0][0].detention_store_size ;
        kernel[i] = sqrt( tan( patch->slope_max ) ) / ( patch->mannN * sqrt(patch->area ) ) ;
        }

    for ( i = 0; i < num_patches; i++ )         /*  !! Serial loop !!  */
        {
        for ( j = 0; j < plist[i]->surface_innundation_list->num_neighbours; j++ )
            {
            neigh = plist[i]->surface_innundation_list->neighbours[j].patch;
            for ( k = 0, m = -1 ; k < num_patches; k++ )
                {
                if ( neigh == plist[k] )
                    {
                    if ( mcount[k] < MAXNEIGHBOR-1 )
                        {
                        m = MAXNEIGHBOR * k + mcount[k] ;
                        mcount[k]++ ;
                        mindex[m] = k ;
                        mgamma[m] = plist[k]->innundation_list[j].gamma * plist[k]->area / neigh->area ;
                        break ;
                        }
                    else{
                        fprintf( stderr, "ERROR:  matrix-overflow in hydro_routing.c:  increase MAXNEIGHBOR and re-=compile" );
                        exit(EXIT_FAILURE);
                        }
                    }
                }
            }
        }       /*  end loop constructing drainage matrix  */

    free( plist ) ;
    return ;
    }           /*  end init_hydro_routing()  */


/*--------------------------------------------------------------------------*/

void hydro_sfc_routing( double                 tstep,
                         struct basin_object * basin )
    {
    unsigned                i, j, k, m, flag ;
    double                  t, tfinal, dt ;
    double                  hh, vel, div, cmax ;
    double                  sumH2O, sumNO3, sumNH4, sumDOC, sumDON ;
	struct patch_object *   patch ;

    if ( num_patches == -9999 )
        {
        fprintf( stderr, "ERROR:  must call init_hydro_routing() before hydro_sfc_routing()" );
        exit(EXIT_FAILURE);
        }

    /*  copy into working variables  */

    flag = 0 ;

#pragma omp parallel for    \
        private( i, patch ) \
        shared( num_patches, plist, sfcH2O, sfcNO3, sfcNH4, sfcDOC, sfcDON )  \
        reduction( |: flag )  

    for ( i = 0; i < num_patches; i++ )
        {
        patch     = plist[i] ;
        sfcH2O[i] = patch->detention_store ;
        sfcNO3[i] = patch->surface_NO3 ;
        sfcNH4[i] = patch->surface_NH4 ;
        sfcDOC[i] = patch->surface_DOC ;
        sfcDON[i] = patch->surface_DON ;

        if ( sfcH2O[i] > retdep[i] )  flag = 1 ;
        }

    if ( flag == 0 )  return;

    /*  internal timestep loop: */

    tfinal = tstep - EPSILON ;              /*  avoid round-off problems with loop-termination */

    for ( t = 0.0 ; t < tfinal ; t+=dt )
        {
        cmax = COUMAX / tstep ;             /*  "Courant-stable for one external time-step"  */

#pragma omp parallel for                                        \
        private( i, hh, vel, div )                              \
        shared( num_patches, sfcH2O, sfcNO3, sfcNH4, sfcDOC,    \
                sfcDON, dH2Odt, dNO3dt, dNH4dt, dDOCdt, dDONdt )\
        reduction( max: cmax )                                  \
        schedule( guided )

        for ( i = 0; i < num_patches; i++ ) /*  compute drainage rates  */
            {
            hh = sfcH2O[i] - retdep[i] ;
            if ( hh > 0.0 )
                {
                vel = kernel[i] * pow( hh, TWOTHD ) ;  /*  vel units: cells per unit time  */
                div = hh / sfcH2O[i] ;
                dH2Odt[i] = vel * hh ;
                dNO3dt[i] = vel * div * sfcNO3[i] ;
                dNH4dt[i] = vel * div * sfcNH4[i] ;
                dDOCdt[i] = vel * div * sfcDOC[i] ;
                dDONdt[i] = vel * div * sfcDON[i] ;
                if ( vel > cmax )  cmax = vel ;
                }
            else{
                dH2Odt[i] = 0.0 ;
                dNO3dt[i] = 0.0 ;
                dNH4dt[i] = 0.0 ;
                dDOCdt[i] = 0.0 ;
                dDONdt[i] = 0.0 ;
                }
            }                           /*  end drainage rate loop  */

        dt = COUMAX / cmax ;            /*  Courant-stable time step  */

#pragma omp parallel for                                                \
        private( i, sumH2O, sumNO3, sumNH4, sumDOC, sumDON )            \
        shared( num_patches, sfcH2O, sfcNO3, sfcNH4, sfcDOC, sfcDON,    \
                dH2Odt, dNO3dt, dNH4dt, dDOCdt, dDONdt, dt )            \
        schedule( guided )

        for ( i = 0; i < num_patches; i++ )
            {
            sumH2O = -dH2Odt[i] ;   /*  start with outflow rate  */
            sumNO3 = -dNO3dt[i] ;
            sumNH4 = -dNH4dt[i] ;
            sumDOC = -dDOCdt[i] ;
            sumDON = -dDONdt[i] ;
            for ( j = 0 ; j < mcount[i] ; j++ )     /*  add the inflow rates  */
                {
                m = MAXNEIGHBOR * i + j ;
                k = mindex[m] ;                
                sumH2O += mgamma[m] * dH2Odt[k] ;
                sumNO3 += mgamma[m] * dNO3dt[k] ;
                sumNH4 += mgamma[m] * dNH4dt[k] ;
                sumDOC += mgamma[m] * dDOCdt[k] ;
                sumDON += mgamma[m] * dDONdt[k] ;
                }
            sfcH2O[i] += sumH2O *dt ;
            sfcNO3[i] += sumNO3 *dt ;
            sfcNH4[i] += sumNH4 *dt ;
            sfcDOC[i] += sumDOC *dt ;
            sfcDON[i] += sumDON *dt ;
            }
        }           /*  end internal timestep loop  */

    /*  copy back into model-state  */

#pragma omp parallel for    \
        private( i, patch ) \
        shared( num_patches, plist, sfcH2O, sfcNO3, sfcNH4, sfcDOC, sfcDON )

    for ( i = 0; i < num_patches; i++ )
        {
        patch                  =  plist[i] ;
        patch->detention_store = sfcH2O[i] ;
        patch->surface_NO3     = sfcNO3[i] ;
        patch->surface_NH4     = sfcNH4[i] ;
        patch->surface_DOC     = sfcDOC[i] ;
        patch->surface_DON     = sfcDON[i] ;
        }

    return ;

    }       /*  end hydro_sfc_routing()  */
