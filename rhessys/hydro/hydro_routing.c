/*--------------------------------------------------------------------------*/
/*  NAME                                                                    */
/*                                                                          */
/*                                                                          */
/*  SYNOPSIS                                                                */
/*      Integrated routing package:                                         */
/*          subsurface routing                                              */
/*          canopy-routing                                                  */
/*          surface routing (kinematic)                                     */
/*          stream-network routing (tbd)                                    */
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
/*      sfcknl = sqrt( slope ) / ( dx * roughness )                         */
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
/*      sfccnt(R)   = number     of sources that flow into cell R           */
/*      sfcndx(R,S) = subscripts of sources that flow into cell R           */
/*      sfcgam(R,S) = gamma(S,R) * area(S) } / area(R)                      */
/*                                                                          */
/*      The "inflow matrices" approach is necessary for a parallel          */
/*      implementation, since we need a single point of update for          */
/*      the state at each patch:  it is unsafe for one patch to update      */
/*      the state of its downhill neighbors:  more than one patch might     */
/*      be updating any one of these neighbors simultaneously, leading      */
/*      to race conditions and incorrect results.                           */
/*                                                                          */
/*  CONTAINS                                                                */
/*      void hydro_routing()                                                */
/*                                                                          */
/*      static void init_hydro_routing()                                    */
/*          allocates working data structures and computes                  */
/*          time independent sfcknl, sfccnt, sfcndx, sfcgam                 */
/*                                                                          */
/*      static void sub_routing()                                           */
/*                                                                          */
/*      static void sub_infilt()                                            */
/*                                                                          */
/*      static void can_routing()                                           */
/*                                                                          */
/*      static void sfc_routing()                                           */
/*          copy initial state-data into working head and chem vectors;     */
/*          loop on adaptive internal time steps                            */
/*              parallel loop computing the  d(head)/dt and d(chem)/dt      */
/*              compute Courant-stable time step dt                         */
/*              parallel loop updating the head and chem for time dt        */
/*          update state-data from working vectors                          */
/*                                                                          */
/*      static void stm_routing()                                           */
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

struct patch_object * * plist ;     /*  array of pointers plist[num_patches] to the patches     */

static double * retdep ;            /*  patch->soil_defaults[0][0].detention_store_size         */
static double * rootzs ;            /*  patch->rootzone.S or patch->S                           */
static double * ksatv  ;            /*  patch->Ksat_vertical                                    */
static double * ksat_0 ;            /*  patch->soil_defaults[0][0].Ksat_0_v                     */
static double * mz_v   ;            /*  patch->soil_defaults[0][0].mz_v                         */
static double * por_0  ;            /*  patch->Ksoil_defaults[0][0].porosity_0                  */
static double * por_d  ;            /*  patch->soil_defaults[0][0].porosity_decay               */
static double * psiair ;            /*  patch->soil_defaults[0][0].psi_air_entry                */

static double * sfcH2O ;            /*  patch->detention_store                                  */
static double * sfcNO3 ;
static double * sfcNH4 ;
static double * sfcDOC ;
static double * sfcDON ;
static double * sfcknl ;


static double * infH2O ;            /*  H2O infiltration                                        */
static double * infNO3 ;            /*  NO3 infiltration                                        */
static double * infNH4 ;            /*  NH4 infiltration                                        */
static double * infDOC ;            /*  DOC infiltration                                        */
static double * infDON ;            /*  DON infiltration                                        */

/*  Drainage Matrix  */

static unsigned * sfccnt ;      /*   used as sfccnt[num_patches]                */
static unsigned * sfcndx ;      /*   used as sfcndx[num_patches][MAXNEIGHBOR]   */
static double   * sfcgam ;      /*   used as sfcgam[num_patches][MAXNEIGHBOR]   */

/*--------------------------------------------------------------------------*/

static void init_hydro_routing( struct command_line_object * command_line,
                                struct basin_object        * basin )
    {
    unsigned                i, j, k, m, n ;
    unsigned                num_matrix, num_neigh ;
    double                  gfac ;
    unsigned                dcount[basin->route_list->num_patches] ;    /*  array of downhill-neighbor counts  */
    double                  dfrac [basin->route_list->num_patches][MAXNEIGHBOR] ;
    size_t                  psize ;
	struct patch_object *   patch ;
	struct patch_object *   neigh ;

    num_patches = basin->route_list->num_patches ;
    num_matrix  = MAXNEIGHBOR * num_patches ;

    plist  = (struct patch_object * *) alloc( num_patches * sizeof(struct patch_object *), "plist", "hydro_routing/init_hydro_routing()" ) ;

    retdep = (double   *) alloc( num_patches * sizeof(   double ), "retdep", "hydro_routing/init_hydro_routing()" ) ;
    rootzs = (double   *) alloc( num_patches * sizeof(   double ), "rootzs", "hydro_routing/init_hydro_routing()" ) ;
    ksatv  = (double   *) alloc( num_patches * sizeof(   double ), "ksatv",  "hydro_routing/init_hydro_routing()" ) ;
    ksat_0 = (double   *) alloc( num_patches * sizeof(   double ), "ksat_0", "hydro_routing/init_hydro_routing()" ) ;
    mz_v   = (double   *) alloc( num_patches * sizeof(   double ), "mz_v",   "hydro_routing/init_hydro_routing()" ) ;
    por_0  = (double   *) alloc( num_patches * sizeof(   double ), "por_0",  "hydro_routing/init_hydro_routing()" ) ;
    por_d  = (double   *) alloc( num_patches * sizeof(   double ), "por_d",  "hydro_routing/init_hydro_routing()" ) ;
    psiair = (double   *) alloc( num_patches * sizeof(   double ), "psiair", "hydro_routing/init_hydro_routing()" ) ;
    infH2O = (double   *) alloc( num_patches * sizeof(   double ), "infH2O", "hydro_routing/init_hydro_routing()" ) ;
    infNO3 = (double   *) alloc( num_patches * sizeof(   double ), "infNO3", "hydro_routing/init_hydro_routing()" ) ;
    infNH4 = (double   *) alloc( num_patches * sizeof(   double ), "infNH4", "hydro_routing/init_hydro_routing()" ) ;
    infDOC = (double   *) alloc( num_patches * sizeof(   double ), "infDOC", "hydro_routing/init_hydro_routing()" ) ;
    infDON = (double   *) alloc( num_patches * sizeof(   double ), "infDON", "hydro_routing/init_hydro_routing()" ) ;
    sfcH2O = (double   *) alloc( num_patches * sizeof(   double ), "sfcH2O", "hydro_routing/init_hydro_routing()" ) ;
    sfcNO3 = (double   *) alloc( num_patches * sizeof(   double ), "sfcNO3", "hydro_routing/init_hydro_routing()" ) ;
    sfcNH4 = (double   *) alloc( num_patches * sizeof(   double ), "sfcNH4", "hydro_routing/init_hydro_routing()" ) ;
    sfcDOC = (double   *) alloc( num_patches * sizeof(   double ), "sfcDOC", "hydro_routing/init_hydro_routing()" ) ;
    sfcDON = (double   *) alloc( num_patches * sizeof(   double ), "sfcDON", "hydro_routing/init_hydro_routing()" ) ;
    sfcknl = (double   *) alloc( num_patches * sizeof(   double ), "sfcknl", "hydro_routing/init_hydro_routing()" ) ;
    sfccnt = (unsigned *) alloc( num_patches * sizeof( unsigned ), "sfccnt", "hydro_routing/init_hydro_routing()" ) ;
    sfcndx = (unsigned *) alloc( num_matrix  * sizeof( unsigned ), "sfcndx", "hydro_routing/init_hydro_routing()" ) ;
    sfcgam = (double   *) alloc( num_matrix  * sizeof(   double ), "sfcgam", "hydro_routing/init_hydro_routing()" ) ;

#pragma omp parallel for    \
        private( i, j, patch, neigh, gfac ) \
        shared( num_patches, basin, plist, sfccnt, retdep, rootzs,  \
                ksatv, ksat_0, mz_v, psiair, sfcknl, dcount, dfrac )
    for ( i = 0; i < num_patches; i++ )
        {
        patch     = basin->route_list->list[i] ;
        plist[i]  = patch ;
        sfccnt[i] = 0 ;
        retdep[i] = patch->soil_defaults[0][0].detention_store_size ;
        rootzs[i] = ( patch->rootzone.depth > ZERO ? patch->rootzone.S : patch->S ) ;
        ksatv [i] = patch->Ksat_vertical ;
        ksat_0[i] = patch->soil_defaults[0][0].Ksat_0_v ;
        mz_v  [i] = patch->soil_defaults[0][0].mz_v ;
        por_0 [i] = patch->soil_defaults[0][0].porosity_0 ;
        por_d [i] = patch->soil_defaults[0][0].porosity_decay ;
        psiair[i] = patch->soil_defaults[0][0].psi_air_entry ;
        sfcknl[i] = sqrt( tan( patch->slope_max ) ) / ( patch->mannN * sqrt(patch->area ) ) ;
        dcount[i] = plist[i]->surface_innundation_list->num_neighbours ;

        gfac = 0.0 ;
        for ( j = 0; j < dcount[i]; j++ )       /*  compute normalized outflow-fractions  */
            {
            neigh = plist[i]->surface_innundation_list->neighbours[j].patch;
            gfac += plist[k]->innundation_list[j].gamma ;
            }
        gfac = 1.0 / gfac ;
        for ( j = 0; j < dcount[i]; j++ )       /*  compute normalized outflow-fractions from         */
            {                                   /*  flow-rates gamma and uphill/downhill area ratios  */
            neigh = plist[i]->surface_innundation_list->neighbours[j].patch;
            dfrac[i][j] = gfac * plist[i]->innundation_list[j].gamma  * plist[i]->area / neigh->area ;
            }
        }

    for ( i = 0; i < num_patches; i++ )                     /*  !! Serial loop !!  */
        {
        for ( j = 0; j < dcount[i]; j++ )
            {
            neigh = plist[i]->surface_innundation_list->neighbours[j].patch;
            for ( k = 0, m = -1 ; k < num_patches; k++ )
                {
                if ( neigh == plist[k] )
                    {
                    if ( sfccnt[k] < MAXNEIGHBOR-1 )
                        {
                        m = MAXNEIGHBOR * k + sfccnt[k] ;
                        sfccnt[k]++ ;
                        sfcndx[m] = k ;
                        sfcgam[m] = dfrac[k][j] ;
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

    return ;
    }           /*  end init_hydro_routing()  */


/*--------------------------------------------------------------------------*/

static void sub_routing( double   tstep,        /*  external time step          */
                         double * substep,      /*  hydro-coupling time step (set in sub_routing()  */
                         int    * flag )        /*  is sfc_routing() active     */
    {
    }           /*  end sub_routing()  */


/*--------------------------------------------------------------------------*/

static void sub_infilt( 
                         double  tstep )        /*  process time-step  */
    {
    }           /*  end infilt_sub()  */


/*--------------------------------------------------------------------------*/

static void can_routing( 
                         double  tstep )        /*  process time-step  */
    {
    }           /*  end can_routing()  */


/*--------------------------------------------------------------------------*/

static void sfc_routing( 
                         double  tstep,         /*  process time-step  */
                         int     flag )         /*  is sfc_routing(0 active?  */
    {
    unsigned                i, j, k, m ;
    double                  z, poro, ksat, Sp, psi_f, theta, intensity, tp, delta, afac  ;
    double                  t, tfinal, dt ;
    double                  hh, vel, div, cmax ;
    double                  sumH2O, sumNO3, sumNH4, sumDOC, sumDON ;
    double                  outH2O[num_patches] ;       /*  outflow rates  */
    double                  outNO3[num_patches] ;
    double                  outNH4[num_patches] ;
    double                  outDOC[num_patches] ;
    double                  outDON[num_patches] ;
	struct patch_object *   patch ;

    if ( ! flag )  return ;

    /*  Initialize infiltration: */

#pragma omp parallel for        \
        private( i )            \
        shared( num_patches, infH2O, infNO3, infNH4, infDOC, infDON )
    for ( i = 0; i < num_patches; i++ )
        {
        infH2O[i] = 0.0 ;
        infNO3[i] = 0.0 ;
        infNH4[i] = 0.0 ;
        infDOC[i] = 0.0 ;
        infDON[i] = 0.0 ;
        }

    /*  internal timestep loop: */

    tfinal = tstep - EPSILON ;                   /*  tolerance for round-off (10 usec)  */

    for ( t = 0.0 ; t < tfinal ; t+=dt )
        {
        cmax = COUMAX / tstep ;             /*  "Courant-stable for one external time-step"  */

#pragma omp parallel for                                                \
        private( i, z, ksat, poro, Sp, psi_f,theta, hh, vel, div )      \
        shared( num_patches, \
                sfcH2O, sfcNO3, sfcNH4, sfcDOC, sfcDON,                 \
                outH2O, outNO3, outNH4, outDOC, outDON )                \
        reduction( max: cmax )                                          \
        schedule( guided )

        for ( i = 0; i < num_patches; i++ ) /*  compute drainage rates  */
            {           

            /*  Transport  */

            hh = sfcH2O[i] - retdep[i] ;
            if ( hh > 0.0 )
                {
                vel = sfcknl[i] * pow( hh, TWOTHD ) ;  /*  vel units: cells per unit time  */
                div = hh / sfcH2O[i] ;
                outH2O[i] = vel * hh ;
                outNO3[i] = vel * div * sfcNO3[i] ;
                outNH4[i] = vel * div * sfcNH4[i] ;
                outDOC[i] = vel * div * sfcDOC[i] ;
                outDON[i] = vel * div * sfcDON[i] ;
                if ( vel > cmax )  cmax = vel ;
                }
            else{
                outH2O[i] = 0.0 ;
                outNO3[i] = 0.0 ;
                outNH4[i] = 0.0 ;
                outDOC[i] = 0.0 ;
                outDON[i] = 0.0 ;
                }
            }                           /*  end drainage rate loop  */

        dt = COUMAX / cmax ;            /*  Courant-stable time step  */

#pragma omp parallel for                                                \
        private( i, sumH2O, sumNO3, sumNH4, sumDOC, sumDON )            \
        shared( num_patches, plist, mz_v, ksat_0, ksatv, por_d, por_0,  \
                intensity, delta, afac,                                 \
                sfcH2O, sfcNO3, sfcNH4, sfcDOC, sfcDON,                 \
                infH2O, infNO3, infNH4, infDOC, infDON,                 \
                outH2O, outNO3, outNH4, outDOC, outDON, dt )            \
        schedule( guided )

        for ( i = 0; i < num_patches; i++ )     /*  update-loop  */
            {
            
            /*  accumulate and apply net in-flows  */

            sumH2O = -outH2O[i] ;   /*  start with outflow rate  */
            sumNO3 = -outNO3[i] ;
            sumNH4 = -outNH4[i] ;
            sumDOC = -outDOC[i] ;
            sumDON = -outDON[i] ;
            for ( j = 0 ; j < sfccnt[i] ; j++ )     /*  add the inflow rates  */
                {
                m = MAXNEIGHBOR * i + j ;
                k = sfcndx[m] ;                
                sumH2O += sfcgam[m] * outH2O[k] ;
                sumNO3 += sfcgam[m] * outNO3[k] ;
                sumNH4 += sfcgam[m] * outNH4[k] ;
                sumDOC += sfcgam[m] * outDOC[k] ;
                sumDON += sfcgam[m] * outDON[k] ;
                }
            sfcH2O[i] += sumH2O *dt ;
            sfcNO3[i] += sumNO3 *dt ;
            sfcNH4[i] += sumNH4 *dt ;
            sfcDOC[i] += sumDOC *dt ;
            sfcDON[i] += sumDON *dt ;

            /*  Calculate Infiltration  */
            
            if ( ( rootzs[i] < 1.0 ) && ( ksat_0[i] > ZERO ) )
                {
                z     = plist[i]->sat_deficit_z ;

	            /*	use mean K and p (porosity) given current saturation depth   */
                
                ksat  = ( mz_v[i] > ZERO   ? mz_v[i]*ksat_0[i]*(1.0 - exp(-z/mz_v[i]) )/z : ksat_0[i] ) ;
                poro  = ( por_d[i] < 999.9 ? por_d[i]*por_0[i]*(1.0 - exp(-z/por_d[i]))/z : por_0[i]  ) ;

                /*	soil moisture deficit - S must be converted to theta	*/
                
                theta = rootzs[i] * poro ;

                /*	estimate sorptivity					*/
                
                psi_f = 0.76 * psiair[i] ;
                Sp    = sqrt( 2.0 * ksat * psi_f ) ;
                intensity = sfcH2O[i] / dt ;
	            if (intensity > ksat)
                    {
                    tp = ksat *  psi_f * ( poro - theta ) / ( intensity * (intensity-ksat) ) ;
                    }
                else{
                    tp = dt ;
                    }
                    
	            /* use Ksat_vertical to limit infiltration only to pervious area */
                
                if ( dt <= tp )
                    {
                    delta = ksatv[i]*sfcH2O[i] ;
                    }
                else{
                    afac  = sqrt( ksat ) ;
                    afac  = afac*afac*afac / 3.0 ;
                    delta = Sp * sqrt( dt - tp ) + afac + tp * sfcH2O[i] ;
                    delta = ksatv[i]*( delta < sfcH2O[i] ? delta :  sfcH2O[i] ) ;
                    }
                    
	            /* Update surface and infiltration variables: */
                
                afac = delta / sfcH2O[i] ;      /*  new-infiltration fraction  */
                infH2O[i] += delta ;
                sfcH2O[i] -= delta ;
                infNO3[i] += afac * sfcNO3[i] ;
                sfcNO3[i] -= afac * sfcNO3[i] ;
                infNH4[i] += afac * sfcNH4[i] ;
                sfcNH4[i] -= afac * sfcNH4[i] ;
                infDOC[i] += afac * sfcDOC[i] ;
                sfcDOC[i] -= afac * sfcDOC[i] ;
                infDON[i] += afac * sfcDON[i] ;
                sfcDON[i] -= afac * sfcDON[i] ;

                }           /*  Calculate Infiltration : if ( rootzs[i] < 1.0 ) && ( ksat_0[i] > ZERO )  */

            }               /*  end update-loop  */
            
        }                   /*  end internal timestep loop  */

    return ;

    }       /*  end sfc_routing()  */


/*--------------------------------------------------------------------------*/

static void stm_routing( 
                         double  tstep,         /*  process time-step  */
                         int     flag )         /*  is sfc_routing() active?  */
    {

    /*  scavenge lateral inflow  */

    /*  Main processing loop  */

    if ( flag )
        {
        }

    /*  copy overflow to surface  */

    }           /*  end stm_routing()  */


/*--------------------------------------------------------------------------*/

void hydro_routing( struct command_line_object * command_line,
                    double                       tstep,
                    struct basin_object        * basin )
    {
    double      substep ;       /*  subsurface (process-coupling) time step     */
    double      t, tfinal ;     /*  time-variables (sec)                        */
    int         flag ;          /*  is surface-routing active?                  */
    unsigned    i ;
	struct patch_object *   patch ;
    
    if ( num_patches == -9999 )
        {
        init_hydro_routing( command_line, basin ) ;
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


    /*  main processing loop:  */

    tfinal = tstep - EPSILON ;     /*  tolerance for round-off (10 usec)  */
    for( t = 0.0; t < tfinal; t+=substep )
        {
        sub_routing( tstep, &substep, &flag ) ;

        can_routing( substep ) ;

        sfc_routing( substep, flag ) ;

        stm_routing( substep, flag ) ;

        sub_infilt ( substep ) ;
        }


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
    }           /*  end hydro_routing()  */
