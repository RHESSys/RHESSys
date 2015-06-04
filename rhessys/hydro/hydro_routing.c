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
/*          vertical ground-water processes                                 */
/*                                                                          */
/*  CONTAINS                                                                */
/*      void hydro_routing( external time-step, etc... )                    */
/*          main driver-routine                                             */
/*          copies relevant model-state into working variables              */
/*          computes external-timestep stream baseflow                      */
/*          internal adaptive time-step loop:                               */
/*              sub_routing()                                               */
/*              sfc_routing()                                               */
/*              stream_routing()                                            */
/*              sub_vertical()                                              */
/*          copies working variables back to model-state                    */
/*                                                                          */
/*      static void init_hydro_routing()                                    */
/*          allocates working data structures and computes                  */
/*          time independent sfcknl, sfccnti, sfcndxi, sfcgam               */
/*                                                                          */
/*      static void sub_routing()                                           */
/*          horizontal groundwater routing; determine coupling timestep     */
/*                                                                          */
/*      static void sfc_routing()                                           */
/*          copy initial state-data into working head and chem vectors;     */
/*          loop on adaptive internal time steps                            */
/*              parallel loop computing the  d(head)/dt and d(chem)/dt      */
/*              compute Courant-stable time step dt                         */
/*              parallel loop updating the surface head and chem            */
/*              in terms of flow, precip+canopy input rates, while          */
/*              computing deep groudnwater + infiltration effects           */
/*              for time-step dt                                            */
/*                                                                          */
/*      static void stream_routing()                                        */
/*          lateral inflow from surface                                     */
/*          stream-network routing                                          */
/*                                                                          */
/*      static void sub_vertical()                                          */
/*          infiltration, groundwater-balancing, and exfiltration           */
/*                                                                          */
/*  DESCRIPTION:  sub_routing()                                             */
/*      Calculates a Courant-stable "coupling time step" DT used by         */
/*      the time-step loop of hydro_routing(), subject to constraint        */
/*      that DT <= CPLMAX == 1800 sec.                                      */
/*      Uses a parallel implementation based on run-time calculation        */
/*      of groundwater-slope for all neighbors.  Lateral flow is            */
/*      calculated for each neighbor with positive (downward) slope,        */
/*      with velocity calculated from that slope, and with weights          */
/*      proportional to that slope.                                         */
/*      Computes arrays lat*[] of net subsurface lateral transfer to        */
/*      each patch.                                                         */
/*                                                                          */
/*  DESCRIPTION:  sfc_routing()                                             */
/*      Uses a parallel implementation based on "inflow matrices" and an    */
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
/*      sfccnti(R)   = number     of sources that flow into cell R          */
/*      sfcndxi(R,S) = subscripts of sources that flow into cell R          */
/*      sfcgam(R,S)  = gamma(S,R) * area(S) } / area(R)                     */
/*                                                                          */
/*      The "inflow matrices" approach is necessary for a parallel          */
/*      implementation, since we need a single point of update for          */
/*      the state at each patch:  it is unsafe for one patch to update      */
/*      the state of its downhill neighbors:  more than one patch might     */
/*      be updating any one of these neighbors simultaneously, leading      */
/*      to race conditions and incorrect results.                           */
/*                                                                          */
/*      Also computes potential-infiltration arrays inf*[] and              */
/*      direct groundwater seepage arrays gnd*[].                           */
/*                                                                          */
/*  DESCRIPTION:  stream_routing()                                          */
/*     Computes lateral inflow from surface water on stream cells,          */
/*     and adjusts surface-water variables accordingly.                     */
/*     Uses a parallel implementation, currently with kinematic routing     */
/*     and with Courant-stable adaptive internal time steps.                */
/*                                                                          */
/*  DESCRIPTION:  sub_vertical() )                                          */
/*     Updates subsurface water and water table, and related subsurface     */
/*     pollutant arrays, from subsurface lateral flows lat*[] and           */
/*     infiltration inf*[].                                                 */
/*                                                                          */
/*  PROGRAMMER NOTES                                                        */
/*      Initial version Spring 2015 by Carlie J. Coats, Jr., UNC IE         */
/*--------------------------------------------------------------------------*/
#include <stdio.h>
#include <math.h>
#include "rhessys.h"

/*	Local function definitions.				*/

extern void  * alloc( size_t, char *, char *);

double	compute_z_final( int	verbose_flag,
						 double	p_0,
						 double	p,
						 double	soil_depth,
						 double	z_initial,
						 double	delta_water);

/*  MAXNEIGHBOR should be a multiple of 4, for memory-alignment reasons */
/*  EPSILON     used for roundoff-tolerance criterion (sec)  = 10 usec  */

#define     MAXNEIGHBOR     (16)
#define     TWOTHD          (2.0/3.0)
#define     DEG2RAD         (M_PI/180.0)
#define     EPSILON         (1.0e-5)
#define     D3600           (1.0/3600.0)

/*  "array-of-neighbors"  types:  */

typedef     double    NBRdble[ MAXNEIGHBOR ] ;
typedef     unsigned  NBRuint[ MAXNEIGHBOR ] ;

static double   CPLMAX ;        /*  Max coupling timestep (sec; returned by sub_routing() )     */
static double   COUMAX ;        /*  Courant-stability threshold                                 */

static int      verbose ;

static unsigned num_patches  = -9999 ;      /* Number of patches, set in init_hydro_routing()  */
static unsigned num_strm     = -9999 ;      /* Number of streams, set in init_hydro_routing()  */
static unsigned num_hills    = -9999 ;      /* Number of streams, set in init_hydro_routing()  */
static unsigned strm_patch   = -9999 ;      /* Number of patches draining into streams, set in init_hydro_routing()  */

static double   basin_area ;
static double   std_scale  ;

struct date     now_date ;

struct     patch_object * * plist ;     /*  array of pointers plist[num_patches] to the patches     */

struct reservoir_object * * reslist ;

struct hillslope_object * * hillist ;

static double * parea ;         /*  patch->area (M^2)                                       */
static double * psize ;         /*  cell-size:  sqrt( patch->area ) (M)                     */
static double * pscale;         /*  patch->std  * std_scale                                 */
static double * perim ;         /*  patch->perimeter                                        */

static unsigned * nsoil ;       /*  patch->num_soil_intervals                               */
static double   * dzsoil ;      /*  patch->soil_defaults[0][0].interval_size                */

static double * retdep ;        /*  patch->soil_defaults[0][0].detention_store_size         */
static double * rootzs ;        /*  patch->rootzone.S or patch->S                           */
static double * ksatv  ;        /*  patch->Ksat_vertical                                    */
static double * ksat_0 ;        /*  patch->soil_defaults[0][0].Ksat_0_v                     */
static double * mz_v   ;        /*  patch->soil_defaults[0][0].mz_v                         */
static double * por_0  ;        /*  patch->Ksoil_defaults[0][0].porosity_0                  */
static double * por_d  ;        /*  patch->soil_defaults[0][0].porosity_decay               */
static double * psiair ;        /*  patch->soil_defaults[0][0].psi_air_entry                */
static double * zsoil  ;        /*  patch->soil_defaults[0][0].soil_depth                   */
static double * Ndecay ;        /*  patch->soil_defaults[0][0].N_decay_rate                 */
static double * Ddecay ;        /*  patch->soil_defaults[0][0].DOM_decay_rate               */
static double * cancap ;        /*  patch->soil_defaults[0][0].DOM_decay_rate               */

static double * waterz ;        /*  water table elevation (vertical M)                      */

static double * capH2O ;        /*  patch->field_capacity                                   */
static double * totH2O ;        /*  total column water (vertical M)                         */
static double * totNO3 ;        /*  total column NO3                                        */
static double * totNH4 ;        /*  total column NH4                                        */
static double * totDON ;        /*  total column DON                                        */
static double * totDOC ;        /*  total column DOC                                        */

static double * sfcH2O ;        /*  patch->detention_store water                            */
static double * sfcNO3 ;        /*  patch->surface_NO3                                      */
static double * sfcNH4 ;        /*  patch->surface_NH4                                      */
static double * sfcDOC ;        /*  patch->surface_DOC                                      */
static double * sfcDON ;        /*  patch->surface_DON                                      */
static double * sfcknl ;        /*  time-independent factor in surface velocity calc.       */

static double * infH2O ;        /*  H2O infiltration to sub-surface                         */
static double * infNO3 ;        /*  NO3 infiltration                                        */
static double * infNH4 ;        /*  NH4 infiltration                                        */
static double * infDOC ;        /*  DOC infiltration                                        */
static double * infDON ;        /*  DON infiltration                                        */

static double * litH2O ;        /*  H2O canopy/litter storage                               */
static double * litNO3 ;        /*  NO3 "                                                   */
static double * litNH4 ;        /*  NH4 "                                                   */
static double * litDON ;        /*  DON "                                                   */
static double * litDOC ;        /*  DOC "                                                   */

static double * gndH2O ;        /*  H2O to groundwater from surface                         */
static double * gndNO3 ;        /*  NO3 to groundwater from surface                         */
static double * gndNH4 ;        /*  NH4 to groundwater from surface                         */
static double * gndDOC ;        /*  DOC to groundwater from surface                         */
static double * gndDON ;        /*  DON to groundwater from surface                         */

static double * latH2O ;        /*  H2O lateral flow from sub_route()                       */
static double * latNO3 ;        /*  NO3 lateral flow                                        */
static double * latNH4 ;        /*  NH4 lateral flow                                        */
static double * latDOC ;        /*  DOC lateral flow                                        */
static double * latDON ;        /*  DON lateral flow                                        */

static double * thruH2O;        /*  H2O from rain/canopy to surface (m/s)                   */
static double * thruNO3;        /*  NO3 from rain/canopy to surface (m/s)                   */

static double * gwcoef ;        /*  patch[0].soil_defaults[0][0].sat_to_gw_coeff/3600.0     */

/*  Surface-routing Drainage Matrix  */

static unsigned * sfccnti ;     /*   used as sfccnti[num_patches]                */
static NBRuint  * sfcndxi ;     /*   used as sfcndxi[num_patches][MAXNEIGHBOR]:  outflow-subscripts   */
static NBRdble  * sfcgam ;      /*   used as  sfcgam[num_patches][MAXNEIGHBOR]   */

/*  Sub-Surface-routing Drainage Matrix  */

static unsigned * subcnto ;     /*   used as  subcnto[num_patches]:                outflow count        */
static unsigned * subcnti ;     /*   used as  subcnti[num_patches]:                 inflow count        */
static NBRuint  * subdexo ;     /*   used as  subdexo[num_patches][MAXNEIGHBOR]:   outflow subscripts   */
static NBRuint  * subdexi ;     /*   used as  subdexi[num_patches][MAXNEIGHBOR]:    inflow subscripts   */
static NBRuint  * subnbri ;     /*   used as  subnbri[num_patches][MAXNEIGHBOR]:    inflow subscripts   */
static NBRdble  * perimf  ;     /*   used as   perimf[num_patches][MAXNEIGHBOR]   */
static NBRdble  * subdist ;     /*   used as  subdist[num_patches][MAXNEIGHBOR]   */

/*  Groundwater per hillslope drainage matrix */

static unsigned * hillslo  ;     /*  [num_hills]    */
static unsigned * hillshi  ;     /*  [num_hills]    */
static unsigned * hillsdx  ;     /*  [num_patches]  */
static double   * invhill  ;     /*  [num_hills] reciprocal of hillslope area */

/*  Stream-routing Lateral-flow drainage matrix; stream properties; tributary incidence-matrix; stream state  */

static unsigned * strmlo  ;     /*  [num_strm]    */
static unsigned * strmhi  ;     /*  [num_strm]    */
static unsigned * strmdex ;     /*  [strm_patch]  */
static double   * strmfac ;     /*  [strm_patch]  */

static unsigned * strm_ID  ;    /*  [num_strm]    */
static unsigned * resv_ID  ;    /*  [num_strm]    */
static double   * strmlen  ;    /*  [num_strm]   */
static double   * manning  ;    /*  [num_strm]   */
static double   * botwdth  ;    /*  [num_strm]   */
static double   * bf_wdth  ;    /*  [num_strm]   */
static double   * bf_hght  ;    /*  [num_strm]   */
static double   * bf_area  ;    /*  [num_strm]   */
static double   * sideslp  ;    /*  [num_strm]   */
static double   * sqrtslp  ;    /*  [num_strm]   */

static double   * baseflo  ;    /*  [num_strm] baseflow (M^3)/s  */

static unsigned * triblo   ;     /*  [num_strm]  lo-#( tributary-list ) for incidence matrix */
static unsigned * tribhi   ;     /*  [num_strm]  hi-#( tributary-list ) for incidence matrix */
static unsigned * tribdex  ;     /*  [num_strm]  tributary-list      for incidence matrix */
static unsigned * distfac  ;     /*  [num_strm]  distributary-factor  or incidence matrix */

static unsigned * downslo  ;     /*  [num_strm]  lo-#( downstream-reach-list ) for incidence matrix */
static unsigned * downshi  ;     /*  [num_strm]  hi-#( downsutary-list ) for incidence matrix */
static unsigned * downsdex ;     /*  [num_strm]  downstream-list      for incidence matrix */

static double   * strmH2O  ;    /*  [num_strm] stream water volume (M^3)  */
static double   * strmNO3  ;    /*  [num_strm]   */
static double   * strmNH4  ;    /*  [num_strm]   */
static double   * strmDON  ;    /*  [num_strm]   */
static double   * strmDOC  ;    /*  [num_strm]   */
static double   * strmflo  ;    /*  [num_strm] stream water outflow rate (M^3/S)  */

/*  Coefficients for computing distribution of transmissivity  */

static double   normal[9] = { 0.0, 0.253, 0.524, 0.842, 1.283, -0.253, -0.524, -0.842, -1.283 };
static double     perc[9] = { 0.2, 0.1,   0.1,   0.1,   0.1,    0.1,    0.1,    0.1,    0.1   };


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static unsigned patchdex( struct patch_object * patch )   /*  patch-subscript in plist[]  */
    {
    unsigned    i ;
    for ( i = 0 ; i < num_patches; i++ )
        {
        if ( patch == plist[i] )  return( i ) ;
        }
    return( num_patches+1 ) ;
    }


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static unsigned  streamdex( unsigned ID )   /*  stream-subscript in strm_ID[]  */
    {
    unsigned    i ;
    for ( i = 0 ; i < num_strm; i++ )
        {
        if ( ID == strm_ID[i] )  return( i ) ;
        }
    return( num_strm+1 ) ;
    }


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static void init_hydro_routing( struct command_line_object * command_line,
                                struct basin_object        * basin )
    {
    unsigned    h, i, j, k, m, n, p ;
    double      gfac, dx, dy, diagf, rat, head, area, afac ;
    unsigned    dcount[basin->route_list->num_patches] ;    /*  array of surface-downhill-neighbor counts  */
    double      dfrac [basin->route_list->num_patches][MAXNEIGHBOR] ;

	struct patch_object          * patch ;
	struct patch_object          * neigh ;
    struct stream_network_object * stream;
    struct hillslope_object      * hillslope;

    verbose   = command_line->verbose_flag ;
    std_scale = command_line->std_scale ;

    CPLMAX    = 1800.0 ;                    /*  max hydro coupling time step (sec)  */
    COUMAX    =    0.2 ;                    /*  max Courant condition (number)      */

    num_patches = basin->route_list->num_patches ;
    num_strm    = basin->stream_list.num_reaches ;
    num_hills   = basin->num_hillslopes ;
    strm_patch  = 0 ;
    for ( i = 0 ; i < num_strm ; i++ )
        {
        strm_patch += basin->stream_list.stream_network[i].num_lateral_inputs ;
        }

    plist   = (struct patch_object * *) alloc( num_patches * sizeof(struct patch_object *), "plist", "hydro_routing/init_hydro_routing()" ) ;

    psize   = (double   *) alloc( num_patches * sizeof(   double ), "psize",  "hydro_routing/init_hydro_routing()" ) ;
    parea   = (double   *) alloc( num_patches * sizeof(   double ), "parea",  "hydro_routing/init_hydro_routing()" ) ;
    nsoil   = (unsigned *) alloc( num_patches * sizeof(   double ), "nsoil",  "hydro_routing/init_hydro_routing()" ) ;
    dzsoil  = (double   *) alloc( num_patches * sizeof(   double ), "dzsoil", "hydro_routing/init_hydro_routing()" ) ;
    pscale  = (double   *) alloc( num_patches * sizeof(   double ), "pscale", "hydro_routing/init_hydro_routing()" ) ;

    retdep  = (double   *) alloc( num_patches * sizeof(   double ), "retdep", "hydro_routing/init_hydro_routing()" ) ;
    rootzs  = (double   *) alloc( num_patches * sizeof(   double ), "rootzs", "hydro_routing/init_hydro_routing()" ) ;
    ksatv   = (double   *) alloc( num_patches * sizeof(   double ), "ksatv",  "hydro_routing/init_hydro_routing()" ) ;
    ksat_0  = (double   *) alloc( num_patches * sizeof(   double ), "ksat_0", "hydro_routing/init_hydro_routing()" ) ;
    mz_v    = (double   *) alloc( num_patches * sizeof(   double ), "mz_v",   "hydro_routing/init_hydro_routing()" ) ;
    por_0   = (double   *) alloc( num_patches * sizeof(   double ), "por_0",  "hydro_routing/init_hydro_routing()" ) ;
    por_d   = (double   *) alloc( num_patches * sizeof(   double ), "por_d",  "hydro_routing/init_hydro_routing()" ) ;
    psiair  = (double   *) alloc( num_patches * sizeof(   double ), "psiair", "hydro_routing/init_hydro_routing()" ) ;
    zsoil   = (double   *) alloc( num_patches * sizeof(   double ), "zsoil",  "hydro_routing/init_hydro_routing()" ) ;
    zsoil   = (double   *) alloc( num_patches * sizeof(   double ), "zsoil",  "hydro_routing/init_hydro_routing()" ) ;
    Ndecay  = (double   *) alloc( num_patches * sizeof(   double ), "Ndecay", "hydro_routing/init_hydro_routing()" ) ;
    Ddecay  = (double   *) alloc( num_patches * sizeof(   double ), "Ddecay", "hydro_routing/init_hydro_routing()" ) ;
    gwcoef  = (double   *) alloc( num_patches * sizeof(   double ), "gwcoef", "hydro_routing/init_hydro_routing()" ) ;
    cancap  = (double   *) alloc( num_patches * sizeof(   double ), "cancap", "hydro_routing/init_hydro_routing()" ) ;

    waterz  = (double   *) alloc( num_patches * sizeof(   double ), "waterz", "hydro_routing/init_hydro_routing()" ) ;

    capH2O  = (double   *) alloc( num_patches * sizeof(   double ), "capH2O", "hydro_routing/init_hydro_routing()" ) ;

    totH2O  = (double   *) alloc( num_patches * sizeof(   double ), "totH2O", "hydro_routing/init_hydro_routing()" ) ;
    totNO3  = (double   *) alloc( num_patches * sizeof(   double ), "totNO3", "hydro_routing/init_hydro_routing()" ) ;
    totNH4  = (double   *) alloc( num_patches * sizeof(   double ), "totNH4", "hydro_routing/init_hydro_routing()" ) ;
    totDOC  = (double   *) alloc( num_patches * sizeof(   double ), "totDOC", "hydro_routing/init_hydro_routing()" ) ;
    totDON  = (double   *) alloc( num_patches * sizeof(   double ), "totDON", "hydro_routing/init_hydro_routing()" ) ;

    infH2O  = (double   *) alloc( num_patches * sizeof(   double ), "infH2O", "hydro_routing/init_hydro_routing()" ) ;
    infNO3  = (double   *) alloc( num_patches * sizeof(   double ), "infNO3", "hydro_routing/init_hydro_routing()" ) ;
    infNH4  = (double   *) alloc( num_patches * sizeof(   double ), "infNH4", "hydro_routing/init_hydro_routing()" ) ;
    infDOC  = (double   *) alloc( num_patches * sizeof(   double ), "infDOC", "hydro_routing/init_hydro_routing()" ) ;
    infDON  = (double   *) alloc( num_patches * sizeof(   double ), "infDON", "hydro_routing/init_hydro_routing()" ) ;

    litH2O  = (double   *) alloc( num_patches * sizeof(   double ), "litH2O", "hydro_routing/init_hydro_routing()" ) ;
    litNO3  = (double   *) alloc( num_patches * sizeof(   double ), "litNO3", "hydro_routing/init_hydro_routing()" ) ;
    litNH4  = (double   *) alloc( num_patches * sizeof(   double ), "litNH4", "hydro_routing/init_hydro_routing()" ) ;
    litDOC  = (double   *) alloc( num_patches * sizeof(   double ), "litDOC", "hydro_routing/init_hydro_routing()" ) ;
    litDON  = (double   *) alloc( num_patches * sizeof(   double ), "litDON", "hydro_routing/init_hydro_routing()" ) ;

    latH2O  = (double   *) alloc( num_patches * sizeof(   double ), "latH2O", "hydro_routing/init_hydro_routing()" ) ;
    latNO3  = (double   *) alloc( num_patches * sizeof(   double ), "latNO3", "hydro_routing/init_hydro_routing()" ) ;
    latNH4  = (double   *) alloc( num_patches * sizeof(   double ), "latNH4", "hydro_routing/init_hydro_routing()" ) ;
    latDOC  = (double   *) alloc( num_patches * sizeof(   double ), "latDOC", "hydro_routing/init_hydro_routing()" ) ;
    latDON  = (double   *) alloc( num_patches * sizeof(   double ), "latDON", "hydro_routing/init_hydro_routing()" ) ;

    sfcH2O  = (double   *) alloc( num_patches * sizeof(   double ), "sfcH2O", "hydro_routing/init_hydro_routing()" ) ;
    sfcNO3  = (double   *) alloc( num_patches * sizeof(   double ), "sfcNO3", "hydro_routing/init_hydro_routing()" ) ;
    sfcNH4  = (double   *) alloc( num_patches * sizeof(   double ), "sfcNH4", "hydro_routing/init_hydro_routing()" ) ;
    sfcDOC  = (double   *) alloc( num_patches * sizeof(   double ), "sfcDOC", "hydro_routing/init_hydro_routing()" ) ;
    sfcDON  = (double   *) alloc( num_patches * sizeof(   double ), "sfcDON", "hydro_routing/init_hydro_routing()" ) ;

    thruH2O = (double   *) alloc( num_patches * sizeof(   double ), "thruH2O","hydro_routing/init_hydro_routing()" ) ;
    thruNO3 = (double   *) alloc( num_patches * sizeof(   double ), "thruNO3","hydro_routing/init_hydro_routing()" ) ;

    gndH2O  = (double   *) alloc( num_patches * sizeof(   double ), "gndH2O", "hydro_routing/init_hydro_routing()" ) ;
    gndNO3  = (double   *) alloc( num_patches * sizeof(   double ), "gndNO3", "hydro_routing/init_hydro_routing()" ) ;
    gndNH4  = (double   *) alloc( num_patches * sizeof(   double ), "gndNH4", "hydro_routing/init_hydro_routing()" ) ;
    gndDOC  = (double   *) alloc( num_patches * sizeof(   double ), "gndDOC", "hydro_routing/init_hydro_routing()" ) ;
    gndDON  = (double   *) alloc( num_patches * sizeof(   double ), "gndDON", "hydro_routing/init_hydro_routing()" ) ;

    hillslo = (unsigned *) alloc(   num_hills * sizeof( unsigned ), "hillslo", "hydro_routing/init_hydro_routing()" ) ;
    hillshi = (unsigned *) alloc(   num_hills * sizeof( unsigned ), "hillshi", "hydro_routing/init_hydro_routing()" ) ;
    hillsdx = (unsigned *) alloc( num_patches * sizeof( unsigned ), "hillsdx", "hydro_routing/init_hydro_routing()" ) ;
    invhill = (double   *) alloc( num_patches * sizeof(   double ), "invhill", "hydro_routing/init_hydro_routing()" ) ;

    sfcknl  = (double   *) alloc( num_patches * sizeof(   double ), "sfcknl", "hydro_routing/init_hydro_routing()" ) ;
    sfccnti = (unsigned *) alloc( num_patches * sizeof( unsigned ), "sfccnti", "hydro_routing/init_hydro_routing()" ) ;
    sfcndxi = (NBRuint  *) alloc( num_patches * sizeof(  NBRuint ), "sfcndxi", "hydro_routing/init_hydro_routing()" ) ;
    sfcgam  = (NBRdble  *) alloc( num_patches * sizeof(  NBRdble ), "sfcgam",  "hydro_routing/init_hydro_routing()" ) ;

    subcnto = (unsigned *) alloc( num_patches * sizeof( unsigned ), "subcnto", "hydro_routing/init_hydro_routing()" ) ;
    subcnti = (unsigned *) alloc( num_patches * sizeof( unsigned ), "subcnti", "hydro_routing/init_hydro_routing()" ) ;
    subdexo = (NBRuint  *) alloc( num_patches * sizeof(  NBRuint ), "subdexo", "hydro_routing/init_hydro_routing()" ) ;
    subdexi = (NBRuint  *) alloc( num_patches * sizeof(  NBRuint ), "subdexi", "hydro_routing/init_hydro_routing()" ) ;
    subnbri = (NBRuint  *) alloc( num_patches * sizeof(  NBRuint ), "subnbri", "hydro_routing/init_hydro_routing()" ) ;
    perimf  = (NBRdble  *) alloc( num_patches * sizeof(  NBRdble ), "perimf",  "hydro_routing/init_hydro_routing()" ) ;
    subdist = (NBRdble  *) alloc( num_patches * sizeof(  NBRdble ), "subdist", "hydro_routing/init_hydro_routing()" ) ;

    strmlo  = (unsigned *) alloc(    num_strm * sizeof( unsigned ), "strmlo",  "hydro_routing/init_hydro_routing()" ) ;
    strmhi  = (unsigned *) alloc(    num_strm * sizeof( unsigned ), "strmhi",  "hydro_routing/init_hydro_routing()" ) ;
    strmdex = (unsigned *) alloc(  strm_patch * sizeof( unsigned ), "strmdex", "hydro_routing/init_hydro_routing()" ) ;
    strmfac = (double   *) alloc(  strm_patch * sizeof(   double ), "strmfac", "hydro_routing/init_hydro_routing()" ) ;

    strm_ID = (unsigned *) alloc(    num_strm * sizeof( unsigned ), "strm_ID", "hydro_routing/init_hydro_routing()" ) ;
    resv_ID = (unsigned *) alloc(    num_strm * sizeof( unsigned ), "resv_ID", "hydro_routing/init_hydro_routing()" ) ;
    triblo  = (unsigned *) alloc(    num_strm * sizeof( unsigned ), "triblo",  "hydro_routing/init_hydro_routing()" ) ;
    tribhi  = (unsigned *) alloc(    num_strm * sizeof( unsigned ), "tribhi",  "hydro_routing/init_hydro_routing()" ) ;
    tribdex = (unsigned *) alloc(    num_strm * sizeof( unsigned ), "tribdex", "hydro_routing/init_hydro_routing()" ) ;
    distfac = (unsigned *) alloc(    num_strm * sizeof( unsigned ), "tribdex", "hydro_routing/init_hydro_routing()" ) ;
    downslo = (unsigned *) alloc(    num_strm * sizeof( unsigned ), "downslo", "hydro_routing/init_hydro_routing()" ) ;
    downshi = (unsigned *) alloc(    num_strm * sizeof( unsigned ), "downshi", "hydro_routing/init_hydro_routing()" ) ;
    downsdex= (unsigned *) alloc(    num_strm * sizeof( unsigned ), "downsdex","hydro_routing/init_hydro_routing()" ) ;
    strmlen = (double   *) alloc(    num_strm * sizeof(   double ), "strmlen", "hydro_routing/init_hydro_routing()" ) ;
    manning = (double   *) alloc(    num_strm * sizeof(   double ), "manning", "hydro_routing/init_hydro_routing()" ) ;
    botwdth = (double   *) alloc(    num_strm * sizeof(   double ), "botwdth", "hydro_routing/init_hydro_routing()" ) ;
    bf_wdth = (double   *) alloc(    num_strm * sizeof(   double ), "bf_wdth", "hydro_routing/init_hydro_routing()" ) ;
    bf_hght = (double   *) alloc(    num_strm * sizeof(   double ), "bf_hght", "hydro_routing/init_hydro_routing()" ) ;
    bf_area = (double   *) alloc(    num_strm * sizeof(   double ), "bf_area", "hydro_routing/init_hydro_routing()" ) ;
    sideslp = (double   *) alloc(    num_strm * sizeof(   double ), "sideslp", "hydro_routing/init_hydro_routing()" ) ;
    sqrtslp = (double   *) alloc(    num_strm * sizeof(   double ), "sqrtslp", "hydro_routing/init_hydro_routing()" ) ;

    baseflo = (double   *) alloc(    num_strm * sizeof(   double ), "baseflo", "hydro_routing/init_hydro_routing()" ) ;
    strmH2O = (double   *) alloc(    num_strm * sizeof(   double ), "strmH2O", "hydro_routing/init_hydro_routing()" ) ;
    strmNO3 = (double   *) alloc(    num_strm * sizeof(   double ), "strmNO3", "hydro_routing/init_hydro_routing()" ) ;
    strmNH4 = (double   *) alloc(    num_strm * sizeof(   double ), "strmNH4", "hydro_routing/init_hydro_routing()" ) ;
    strmDON = (double   *) alloc(    num_strm * sizeof(   double ), "strmDON", "hydro_routing/init_hydro_routing()" ) ;
    strmDOC = (double   *) alloc(    num_strm * sizeof(   double ), "strmDOC", "hydro_routing/init_hydro_routing()" ) ;
    strmflo = (double   *) alloc(    num_strm * sizeof(   double ), "strmflo", "hydro_routing/init_hydro_routing()" ) ;

    reslist = (struct reservoir_object * *) alloc(num_strm * sizeof(struct reservoir_object *), "reslist", "hydro_routing/init_hydro_routing()" ) ;

    hillist = (struct hillslope_object * *) alloc(num_strm * sizeof(struct hillslope_object *), "hillist", "hydro_routing/init_hydro_routing()" ) ;

    diagf = 0.5 * sqrt( 0.5 ) ;     /*  "perimeter" factor for diagonals */
    basin_area = 0.0 ;

    // Initialize plist so that we can invert the flow tables (which needs
    // to refer to patches in an arbitrary order compared to that of the
    // route list.
#pragma omp parallel for                                                \
        default( none )                                                 \
        private( i )												    \
		shared( plist )													\
		schedule( guided )
    for ( i = 0; i < num_patches; i++ )
    	{
    		plist [i] = basin->route_list->list[i] ;
    	}

#pragma omp parallel for                                                \
        default( none )                                                 \
        private( i, j, k, patch, neigh, gfac, dx, dy )                  \
         shared( num_patches, basin, plist, parea, psize, sfccnti,      \
                 retdep, rootzs, ksatv, ksat_0, mz_v, psiair, zsoil,    \
                 nsoil, dzsoil, std_scale, pscale, Ndecay, Ddecay,      \
                 sfcknl, dcount, dfrac, capH2O, por_0, por_d, gwcoef,   \
                 subdist, subdexo, perimf, diagf, subcnti, subcnto,     \
                 litH2O, litNO3, litNH4, litDOC, litDON, cancap )       \
      reduction( +:  basin_area )                                       \
       schedule( guided )

    for ( i = 0; i < num_patches; i++ )
        {
        patch     = basin->route_list->list[i] ;
        plist [i] = patch ;
        capH2O[i] = patch->field_capacity ;
        parea [i] = patch->area ;
        psize [i] = sqrt( patch->area ) ;
        nsoil [i] = patch->num_soil_intervals ;
        dzsoil[i] = patch->soil_defaults[0][0].interval_size ;
        pscale[i] = std_scale * patch->std ;
        retdep[i] = patch->soil_defaults[0][0].detention_store_size ;
        rootzs[i] = ( patch->rootzone.depth > ZERO ? patch->rootzone.S : patch->S ) ;
        ksatv [i] = patch->Ksat_vertical ;
        ksat_0[i] = patch->soil_defaults[0][0].Ksat_0_v ;
        mz_v  [i] = patch->soil_defaults[0][0].mz_v ;
        por_0 [i] = patch->soil_defaults[0][0].porosity_0 ;
        por_d [i] = patch->soil_defaults[0][0].porosity_decay ;
        psiair[i] = patch->soil_defaults[0][0].psi_air_entry ;
        zsoil [i] = patch->soil_defaults[0][0].soil_depth ;
        Ndecay[i] = patch->soil_defaults[0][0].N_decay_rate ;
        Ddecay[i] = patch->soil_defaults[0][0].DOM_decay_rate ;
        gwcoef[i] = patch->soil_defaults[0][0].sat_to_gw_coeff ;        /*  per sec, instead of per hour  */
        sfcknl[i] = sqrt( tan( patch->slope_max ) ) / ( patch->mannN * psize[i] ) ;
        dcount[i] = patch->surface_innundation_list->num_neighbours ;
        cancap[i] = patch->litter.rain_capacity ;

        sfccnti[i] = 0 ;
        subcnto[i] = patch->innundation_list->num_neighbours ;
        subcnti[i] = 0 ;
        
        litH2O[i] = patch->litter.rain_stored ;
        litNO3[i] = 0.0 ;
        litNH4[i] = 0.0 ;
        litDON[i] = 0.0 ;
        litDOC[i] = 0.0 ;

        gfac = 0.0 ;
        for ( j = 0; j < dcount[i]; j++ )       /*  compute normalized outflow-fractions  */
            {
            gfac += plist[i]->surface_innundation_list->neighbours[j].gamma ;
            }
        gfac = 1.0 / gfac ;
        for ( j = 0; j < dcount[i]; j++ )       /*  compute normalized outflow-fractions from         */
            {                                   /*  flow-rates gamma and uphill/downhill area ratios  */
            neigh = plist[i]->surface_innundation_list->neighbours[j].patch;
            dfrac[i][j] = gfac * plist[i]->surface_innundation_list->neighbours[j].gamma  * plist[i]->area / neigh->area ;
            }

        for ( j = 0; j < subcnto[i]; j++ )
            {
            neigh = plist[i]->innundation_list->neighbours[j].patch;
            dx    = neigh->x - plist[j]->x ;
            dy    = neigh->y - plist[j]->y ;
            subdist[i][j] = sqrt( dx*dx + dy*dy )  ;
            subdexo[i][j] = patchdex( neigh ) ;
            if ( dx+dy < 1.1*subdist[i][j] )
                {
                perimf[i][j] = diagf * plist[i]->area / neigh->area ;    /* diagonal-direction factor */
                }
            else{
                perimf[i][j] = 0.5 * plist[i]->area / neigh->area ;     /* along-axis factor  */
                }
            }
        }                   /*  end first (parallel) initialization loop  */

    /*  !! Serial loop !!  -- computing inflow-neighbor tables and matrices  */

    for ( i = 0; i < num_patches; i++ )
        {

        /*  invert the surface-routing table  */

        for ( j = 0; j < dcount[i]; j++ )
            {
            neigh = plist[i]->surface_innundation_list->neighbours[j].patch ;
            k = patchdex( neigh ) ;
            if ( sfccnti[k] < MAXNEIGHBOR-1 )
                {
                m = sfccnti[k] ;
                sfccnti[k]++ ;
                sfcndxi[k][m] = j ;
                sfcgam [k][m] = dfrac[k][j] ;
                break ;
                }
            else{
                fprintf( stderr, "ERROR:  matrix-overflow in hydro_routing.c:  increase MAXNEIGHBOR and re-=compile" );
                exit(EXIT_FAILURE);
                }
            }

        /*  use existing subsurface-routing table:  need distances, area-ratios for neighbors  */

        for ( j = 0; j < subcnto[i]; j++ )
            {
            neigh = plist[ subdexo[i][j] ] ;
            k     = patchdex( neigh ) ;
            m     = MAXNEIGHBOR * k + subcnti[k]  ;
            subdexi[k][j] = i ;
            subnbri[k][j] = k ;
            subcnti[k]++  ;
            }

        }       /*  end serial loop constructing drainage matrices  */

        /*  Initialize data structures for streams  */
        /*  NOTE:  NO3, NH4, DON, DOC initializes to zero, for now...  */

#pragma omp parallel for                                                \
        default( none )                                                 \
        private( i, stream, head, area )                                \
         shared( num_strm, basin, strm_ID, strmlen, manning, strmflo,   \
                 botwdth, bf_wdth, bf_hght, bf_area, sideslp, sqrtslp,  \
                 resv_ID, strmH2O, strmNO3, strmNH4, strmDOC, strmDON,  \
                 reslist, distfac )

    for ( i = 0 ; i < num_strm ; i++ )
        {
        stream     = & ( basin->stream_list.stream_network[i] ) ;
        strm_ID[i] = stream->reach_ID ;
        resv_ID[i] = stream->reservoir_ID ;
        strmlen[i] = stream->length ;
        manning[i] = stream->manning ;
        botwdth[i] = stream->bottom_width ;
        bf_wdth[i] = stream->top_width ;
        bf_hght[i] = stream->max_height ;
        bf_area[i] = 0.5 * bf_hght[i] * ( botwdth[i] + bf_wdth[i] ) ;
        sqrtslp[i] = sqrt( stream->stream_slope ) ;
        sideslp[i] = 0.5 * ( bf_wdth[i] - botwdth[i] ) / bf_hght [i] ;
        strmflo[i] = stream->initial_flow ;
        
        if ( resv_ID[i] )
            {
            reslist[i] = & ( stream->reservoir ) ;
            }
        else{
            reslist[i] = ( struct reservoir_object *) NULL ;
            }

        head = stream->water_depth ;
        area = head * ( botwdth[i] + head / sideslp[i] ) ;
        strmH2O[i] = area * strmlen[i] ;
        strmNO3[i] = 0.0 ;
        strmNH4[i] = 0.0 ;
        strmDON[i] = 0.0 ;
        strmDOC[i] = 0.0 ;
        distfac[i] = 1.0 ;
        }

    /*  !! Serial loop !!  -- computing surface-to-stream lateral-inflow matrix  */

    k = 0 ;
    for ( i = 0 ; i < num_strm ; i++ )
        {
        n         = basin->stream_list.stream_network[i].num_lateral_inputs ;
        strmlo[i] = k ;
        strmhi[i] = k + n - 1 ;
        for ( j = 0 ; j < n ; j++, k++ )
            {
            strmdex[k] = patchdex( basin->stream_list.stream_network[i].lateral_inputs[j] ) ;
            rat = bf_wdth[i] / psize[ strmdex[k] ] ;
            strmfac[k] = ( rat > 1.0 ? 1.0 : rat ) ;
            }
        }

    /*  !! Serial loops !!  -- computing incidence matrix for tributary-, downstream-relations  */
    /*  NOTE:  #{ tribs } <= #{ streams }   */
    /*  distfac[:] is distributary-factor:  proportion of upstream-inflow going to this distributary  */

    m = 0 ;
    k = 0 ;
    for ( i = 0 ; i < num_strm ; i++ )
        {
        afac      = 0.0 ;
        n         = basin->stream_list.stream_network[i].num_downstream_neighbours ;
        downslo[i] = m ;
        downshi[i] = m + n - 1 ;
        for ( j = 0 ; j < n ; j++, m++ )
            {
            downsdex[m] = streamdex( basin->stream_list.stream_network[i].downstream_neighbours[j] ) ;
            afac        = afac + bf_area[ downsdex[m] ] ;
            }
        if ( n > 1 )    /*  normalize distfac[]  */
            {
            afac = 1.0 / afac ;
            for ( j = downslo[i] ; j < downshi[i] ; j++ )
                {
                distfac[j] = afac * bf_area[j] ;
                }
            }
        n         = basin->stream_list.stream_network[i].num_upstream_neighbours ;
        triblo[i] = k ;
        tribhi[i] = k + n - 1 ;
        for ( j = 0 ; j < n ; j++, k++ )
            {
            p          = streamdex( basin->stream_list.stream_network[i].upstream_neighbours[j] ) ;
            tribdex[k] = p ;
            }
        }

    /*  !! Serial loop !!  -- Hillslope groundwater-accumulation matrix   */
    /*  NOTE that we don't need per-zone decomposition on the hillslopes  */

    k = 0 ;
    for ( h = 0 ; i < num_hills ; h++ )
        {
        hillslo[h] = k ;
        hillslope = basin->hillslopes[h] ;
        for ( i = 0 ; i < hillslope->num_zones ; i++ )
            {
            for ( j = 0 ; j < hillslope->zones[i]->num_patches ; j++ )
                {
                hillsdx[k] = patchdex( hillslope->zones[i]->patches[j] );
                k++ ;
                }
            }
        hillshi[h] = k - 1 ;
        invhill[h] = 1.0 / hillslope->area ;
        }

    return ;

    }           /*  end init_hydro_routing()  */


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static void sub_routing( double   tstep,        /*  external time step      */
                         double * substep )     /*  hydro-coupling time step: <= min( CPLMAX, tstep )  */
    {
    double      z1, z2, zz, vel, slope, cmax, dt ;
    double      ss, std, ssum, tsum, wsum, fac ;
    double      trans, dH2Odt ;
    double      dH2O, dNO3, dNH4, dDOC, dDON ;
    unsigned    i, j, k, kk, m, mm ;
    int         n ;
    double      outH2O[num_patches] ;
    double      gammaf[num_patches][MAXNEIGHBOR] ;
	struct patch_object *   patch ;
	struct patch_object *   neigh ;

        cmax = COUMAX / min( tstep, CPLMAX ) ;             /*  "Courant-stable for one time-step"  */

#pragma omp parallel for                                            \
        default( none )                                             \
        private( i, j, kk, m, n, patch, z1, z2, zz, trans, slope,   \
                 ssum, tsum, wsum, fac, vel, dH2Odt )               \
         shared( num_patches, plist, pscale, nsoil, dzsoil, perimf, \
                 normal, perc, waterz, outH2O, totH2O, gammaf,      \
                 subcnto, subdexo, subdist, psize )                 \
      reduction( max: cmax )                                        \
       schedule( guided )

    for ( i = 0; i < num_patches; i++ )           /*  calculate  dH2Odt[]  */
        {
        patch = plist[i] ;

        if ( pscale[i] > 0 )        /*  compute transmissivity and re-scale by cell-size  */
            {
            tsum = 0.0 ;
            for ( m = 0 ; m < 9 ; m++ )
                {
                n = min( (int)nsoil[i], (int) lround( patch->sat_deficit + normal[m]*pscale[i])/dzsoil[i] ) ;
                tsum += patch->transmissivity_profile[n] * perc[m] ;
                }
            trans = tsum / psize [i] ;
            }
        else{
            n     = min( (int)nsoil[i], (int)lround( patch->sat_deficit/dzsoil[i] ) ) ;
            trans = patch->transmissivity_profile[n] / psize [i] ;
            }

        z1    = waterz[i]  ;
        ssum  = 0.0 ;                           /*  sum of slopes (for normalizing gamma  */
        wsum  = 0.0 ;                           /*  sum of exiting water  */

        for ( j = 0; j < subcnto[i]; j++ )      /*  find max (positive-) slope for outflow  */
            {
            kk    = subdexo[i][j] ;
            z2    = waterz[kk] ;
            slope = ( z1 - z2 ) / subdist[i][j] ;
            if ( slope > ZERO )
                {
                zz           = 0.5 * ( z1 + z2 ) ;
                vel          = slope * trans ;                  /*  cells/sec  */
                dH2Odt       = perimf[i][j] * zz * vel ;        /*  outflow  */
                ssum         =  ssum + slope ;
                wsum         =  wsum + dH2Odt ;
                gammaf[i][j] = slope * dH2Odt ;
                if ( vel > cmax )  cmax = vel ;
                }
            else{
                gammaf[i][j] = 0.0 ;
                }
            }
        outH2O[i] = wsum ;
        if ( ssum > ZERO )     /*  Normalize gammaf[i][:] and divide by totH2O[i][:] */
            {
            ssum = 1.0 / ( ssum * totH2O[i] ) ;
            for ( j = 0; j < subcnto[i]; j++ )
                {
                gammaf[i][j] = ssum * gammaf[i][j] ;
                };
            }
        }           /*  end loop calculating  dH2Odt[]  */

    *substep = dt = min( COUMAX / cmax , tstep ) ;

#pragma omp parallel for                                            \
        default( none )                                             \
        private( i, j, k, n, fac, dH2O, dNO3, dNH4, dDOC, dDON )    \
         shared( num_patches, plist, subcnti, subdexi, subnbri,     \
                 dt, gammaf, outH2O,                                \
                 totH2O, totNO3, totNH4, totDON, totDOC,            \
                 latH2O, latNO3, latNH4, latDON, latDOC )           \
       schedule( guided )

    for ( i = 0; i < num_patches; i++ )     /*  update H2O, NO3, NH4, DON, DOC  */
        {
        fac  = outH2O[i] / totH2O[i] ;
        dH2O = -fac * totH2O[i] ;
        dNO3 = -fac * totNO3[i] ;
        dNH4 = -fac * totNH4[i] ;
        dDON = -fac * totDON[i] ;
        dDOC = -fac * totDOC[i] ;
        for ( j = 0; j < subcnti[i]; j++ )
            {
            k     = subdexi[i][j] ;
            n     = subnbri[i][j] ;
            dH2O += gammaf[k][n] * totH2O[k] ;
            dNO3 += gammaf[k][n] * totNO3[k] ;
            dNH4 += gammaf[k][n] * totNH4[k] ;
            dDON += gammaf[k][n] * totDON[k] ;
            dDOC += gammaf[k][n] * totDOC[k] ;
            }
        latH2O[i] = dt * dH2O ;
        latNO3[i] = dt * dNO3 ;
        latNH4[i] = dt * dNH4 ;
        latDON[i] = dt * dDON ;
        latDOC[i] = dt * dDOC ;
        }       /*  end loop updating state-variables  */

    return ;

    }           /*  end sub_routing()  */


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static void sfc_routing( double  tstep )        /*  process time-step  */
    {
    unsigned                i, j, k, m ;
    double                  z, poro, ksat, Sp, psi_f, theta, intensity, tp, delta ;
    double                  t, tfinal, dt1, dt2, dt ;
    double                  hh, vel, div, cmax ;
    double                  afac, gfac, dH2O, dNO3, dNH4, dDOC, dDON ;
    double                  sumH2O, sumNO3, sumNH4, sumDOC, sumDON ;
    double                  capacity, water ;
    double                  outH2O[num_patches] ;       /*  outflow rates  */
    double                  outNO3[num_patches] ;
    double                  outNH4[num_patches] ;
    double                  outDOC[num_patches] ;
    double                  outDON[num_patches] ;
	struct patch_object *   patch ;

    /*  Initialize infiltration: */

#pragma omp parallel for        \
        default( none )         \
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
        default( none )                                                 \
        private( i, z, ksat, poro, Sp, psi_f,theta, hh, vel, div )      \
         shared( num_patches, retdep, sfcknl,                           \
                 sfcH2O, sfcNO3, sfcNH4, sfcDOC, sfcDON,                \
                 outH2O, outNO3, outNH4, outDOC, outDON )               \
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

        dt = min( COUMAX / cmax , tstep - t ) ; /*  Courant-stable time step  */

#pragma omp parallel for                                                \
        default( none )                                                 \
        private( i, j, k, sumH2O, sumNO3, sumNH4, sumDOC, sumDON, z,    \
                 ksat, poro, theta, psi_f, Sp, intensity, tp, delta,    \
                 afac, gfac, dH2O, dNO3, dNH4, dDOC, dDON, capacity,    \
                 water )                                                \
         shared( num_patches, plist, mz_v, ksat_0, ksatv, por_d, por_0, \
                 dt, sfccnti, sfcndxi, sfcgam, rootzs, psiair,          \
                 sfcH2O, sfcNO3, sfcNH4, sfcDOC, sfcDON,                \
                 thruH2O,thruNO3,                                       \
                 litH2O, litNO3, litNH4, litDON, litDOC, cancap,        \
                 gndH2O, gndNO3, gndNH4, gndDOC, gndDON, gwcoef,        \
                 infH2O, infNO3, infNH4, infDOC, infDON,                \
                 outH2O, outNO3, outNH4, outDOC, outDON )               \
       schedule( guided )

        for ( i = 0; i < num_patches; i++ )     /*  update&infiltration loop  */
            {

            /*  accumulate and apply net in-flows  */

            sumH2O = -outH2O[i] ;   /*  start with outflow rate  */
            sumNO3 = -outNO3[i] ;
            sumNH4 = -outNH4[i] ;
            sumDOC = -outDOC[i] ;
            sumDON = -outDON[i] ;
            for ( j = 0 ; j < sfccnti[i] ; j++ )     /*  add the inflow rates  */
                {
                k = sfcndxi[i][j] ;
                sumH2O += sfcgam[i][j] * outH2O[k] ;
                sumNO3 += sfcgam[i][j] * outNO3[k] ;
                sumNH4 += sfcgam[i][j] * outNH4[k] ;
                sumDOC += sfcgam[i][j] * outDOC[k] ;
                sumDON += sfcgam[i][j] * outDON[k] ;
                }
            sfcH2O[i] += sumH2O *dt ;   /*  update surface state  */
            sfcNO3[i] += sumNO3 *dt ;
            sfcNH4[i] += sumNH4 *dt ;
            sfcDOC[i] += sumDOC *dt ;
            sfcDON[i] += sumDON *dt ;
            
            if ( cancap[i] > EPSILON )              /*  Calculate rain + canopy/litter effects  */
                {
                litH2O[i] = litH2O[i] + thruH2O[i] * dt ;   /*  rain falls preferentially on canopy   */
                litNO3[i] = litNO3[i] + thruNO3[i] * dt ;   /*  rain falls preferentially on canopy   */
                capacity  = cancap[i] - litH2O[i] ;         /*  available canopy/litter storage       */
                water     = litH2O[i] + sfcH2O[i] ;         /*  available total surface+canopy water  */

                if ( capacity < 0.0 )        /*  excess canopy water, etc., to surface  */
                    {
                    afac = ( litH2O[i] - cancap[i] ) / cancap[i] ;
                    dH2O = afac * litH2O[i] ;
                    dNO3 = afac * litNO3[i] ;
                    dNH4 = afac * litNH4[i] ;
                    dDON = afac * litDON[i] ;
                    dDOC = afac * litDOC[i] ;
                    sfcH2O[i] += dH2O ;
                    sfcNO3[i] += dNO3 ;
                    sfcNH4[i] += dNH4 ;
                    sfcDON[i] += dDON ;
                    sfcDOC[i] += dDOC ;
                    litH2O[i] -= dH2O ;
                    litNO3[i] -= dNO3 ;
                    litNH4[i] -= dNH4 ;
                    litDON[i] -= dDON ;
                    litDOC[i] -= dDOC ;
                    }                     
                else if ( sfcH2O[i] > EPSILON )      /*  canopy scavenges surface water, etc.  */
                    {                  
                    afac = min( capacity / sfcH2O[i] , 1.0 ) ;
                    dH2O = afac * sfcH2O[i] ;
                    dNO3 = afac * sfcNO3[i] ;
                    dNH4 = afac * sfcNH4[i] ;
                    dDON = afac * sfcDON[i] ;
                    dDOC = afac * sfcDOC[i] ;
                    sfcH2O[i] -= dH2O ;
                    sfcNO3[i] -= dNO3 ;
                    sfcNH4[i] -= dNH4 ;
                    sfcDON[i] -= dDON ;
                    sfcDOC[i] -= dDOC ;
                    litH2O[i] += dH2O ;
                    litNO3[i] += dNO3 ;
                    litNH4[i] += dNH4 ;
                    litDON[i] += dDON ;
                    litDOC[i] += dDOC ;
                    }
                }                       /*  end calculating canopy/litter effects  */
            else{
                sfcH2O[i] = sfcH2O[i] + thruH2O[i] * dt ;    /*  no litter:  rain falls on surface  */
                sfcNO3[i] = sfcNO3[i] + thruNO3[i] * dt ;    /*  no litter:  rain falls on surface  */
                }

            if ( sfcH2O[i] < EPSILON )  continue ;

            /*  Calculate groundwater contribution  */

            gfac = gwcoef[i] * dt ;     /* == this-timestep fraction going into groundwater reservoir  */
            dH2O = gfac * sfcH2O[i] ;
            dNO3 = gfac * sfcNO3[i] ;
            dNH4 = gfac * sfcNH4[i] ;
            dDON = gfac * sfcDON[i] ;
            dDOC = gfac * sfcDOC[i] ;

            sfcH2O[i] -= dH2O ;   /*  update surface state  */
            sfcNO3[i] -= dNO3 ;
            sfcNH4[i] -= dNH4 ;
            sfcDOC[i] -= dDOC ;
            sfcDON[i] -= dDON ;

            gndH2O[i] += dH2O ;   /*  update groundwater state  */
            gndNO3[i] += dNO3 ;
            gndNH4[i] += dNH4 ;
            gndDOC[i] += dDOC ;
            gndDON[i] += dDON ;

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
                    dH2O = ksatv[i]*sfcH2O[i] ;
                    }
                else{
                    afac = sqrt( ksat ) ;
                    afac = afac*afac*afac / 3.0 ;
                    dH2O = Sp * sqrt( dt - tp ) + afac + tp * sfcH2O[i] ;
                    dH2O = ksatv[i]*( dH2O < sfcH2O[i] ? dH2O :  sfcH2O[i] ) ;
                    }

	            /* Update surface and infiltration variables: */

                afac = dH2O / sfcH2O[i] ;      /*  new-infiltration fraction  */
                dNO3 = afac * sfcNO3[i] ;
                dNH4 = afac * sfcNH4[i] ;
                dDON = afac * sfcDON[i] ;
                dDOC = afac * sfcDOC[i] ;

                sfcH2O[i] -= dH2O ;   /*  update surface state  */
                sfcNO3[i] -= dNO3 ;
                sfcNH4[i] -= dNH4 ;
                sfcDOC[i] -= dDOC ;
                sfcDON[i] -= dDON ;

                infH2O[i] += dH2O ;   /*  update groundwater state  */
                infNO3[i] += dNO3 ;
                infNH4[i] += dNH4 ;
                infDOC[i] += dDOC ;
                infDON[i] += dDON ;

                }           /*  Calculate Infiltration : if ( rootzs[i] < 1.0 ) && ( ksat_0[i] > ZERO )  */

            }               /*  end update&infiltration loop  */

        }                   /*  end internal timestep loop  */

    return ;

    }       /*  end sfc_routing()  */


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static double headfn( double area,      /*  returns head for a specified water-area and stream geometry  */
                      double botw,
                      double sslp )
    {
    double hh ;
    hh = sqrt( botw*sslp + 4.0*area*sslp ) - botw*sslp ;
    return( hh > ZERO ? 0.5*hh : ZERO ) ;
    }


static double kinvel( double area,      /*  returns velocity for kinematic flow     */
                      double head,      /*  water depth (M)                         */
                      double botw,      /*  bottom width (M)                        */
                      double dmann,     /*  reciprocal of Mannings coefficient      */
                      double dside,     /*  reciprocal of side-slope                */
                      double rtslp )    /*  square root of stream-bed slope         */
    {
    double  rhyd ;
    if ( head < EPSILON )
        {
        return( 0.0 ) ;
        }
    else{
        rhyd = area / ( botw + 2.0*head*sqrt( 1.0 + dside*dside ) ) ;
        return( dmann * rtslp * pow( rhyd, TWOTHD ) ) ;
        }
    }


static double resflow( struct reservoir_object * reservoir,
                       double                    inflow,
                       double                    dt )
    {
    double  storage, outflow ;

    outflow = reservoir->min_outflow ;
    storage = reservoir->initial_storage + dt * ( inflow - outflow ) ;
    
    if ( storage > reservoir->month_max_storage[now_date.month-1] )
        {
        outflow = outflow + (storage - reservoir->month_max_storage[now_date.month-1]) / dt ;
        storage = reservoir->month_max_storage[now_date.month-1] ;
        }
    else if ( storage < reservoir->min_storage )
        {
        if ( reservoir->flag_min_flow_storage != 0 )
            {
            storage = reservoir->min_storage ;
            }
        else if ( storage < 0 )   /*min_flow has higher priority*/
            {
            storage = 0.0 ;
            outflow = inflow + reservoir->initial_storage / dt ;
            }
        }
    reservoir->initial_storage = storage ;
    return( outflow ) ;
    }


static void stream_routing( double  tstep )        /*  process time-step  */
    {
    unsigned    i, j, k ;
    double      t , dt, ddt, area, head, vel, flow;
    double      cmax, cscr, dmann, dside, p, q, frac ;
    double      dH2O, dNO3, dNH4, dDOC, dDON ;
    double      latsH2O[num_strm] ;     /*  lateral inflow from surface (M^3/S)  */
    double      latsNO3[num_strm] ;     /*  lateral inflow from surface (?^3/S)  */
    double      latsNH4[num_strm] ;     /*  lateral inflow from surface (?^3/S)  */
    double      latsDOC[num_strm] ;     /*  lateral inflow from surface (?^3/S)  */
    double      latsDON[num_strm] ;     /*  lateral inflow from surface (?^3/S)  */
    double      tribH2O[num_strm] ;     /*  tributary inflow (M^3/S)  */
    double      tribNO3[num_strm] ;     /*  tributary inflow (?^3/S)  */
    double      tribNH4[num_strm] ;     /*  tributary inflow (?^3/S)  */
    double      tribDOC[num_strm] ;     /*  tributary inflow (?^3/S)  */
    double      tribDON[num_strm] ;     /*  tributary inflow (?^3/S)  */
    double      downfac[num_strm] ;     /*  strmflo[i] / (strmH2O[i] + sum_downstream{ strmH2O[j] }  */

    /*  scavenge lateral inflow  */

    ddt = 1.0 / tstep ;

#pragma omp parallel for    \
        default( none )     \
        private( i, j, k, dH2O, dNO3, dNH4, dDOC, dDON, p, q )  \
         shared( num_strm, strmlo, strmhi, strmdex, strmfac,    \
                 latsH2O, latsNO3, latsNH4, latsDOC, latsDON,   \
                 sfcH2O,  sfcNO3,  sfcNH4,  sfcDOC,  sfcDON,    \
                 parea, tstep, ddt ) \
       schedule( guided )

    for ( i = 0 ; i < num_strm ; i++ )
        {
        dH2O = 0.0 ;
        dNO3 = 0.0 ;
        dNH4 = 0.0 ;
        dDON = 0.0 ;
        dDOC = 0.0 ;
        for ( j = strmlo[i]; j < strmhi[i] ; j++ )
            {
            k = strmdex[j] ;
            q = 1.0 - strmfac[j] ;      /*  remaining fraction after lateral-flow scavenging  */
            p = strmfac[j] * parea[k] ; /*  fraction of head scavenged from patch * area of patch  */
            dH2O      = p * sfcH2O[i] ;
            sfcH2O[i] = q * sfcH2O[i] ;
            dNO3      = p * sfcNO3[i] ;
            sfcNO3[i] = q * sfcNO3[i] ;
            dNH4      = p * sfcNH4[i] ;
            sfcNH4[i] = q * sfcNH4[i] ;
            dDON      = p * sfcDON[i] ;
            sfcDON[i] = q * sfcDON[i] ;
            dDOC      = p * sfcDOC[i] ;
            sfcDOC[i] = q * sfcDOC[i] ;
            }
        latsH2O[i] = ddt * dH2O ;
        latsNO3[i] = ddt * dNO3 ;
        latsNH4[i] = ddt * dNH4 ;
        latsDON[i] = ddt * dDON ;
        latsDOC[i] = ddt * dDOC ;
        }

    /*  Main processing loop  */

    for ( t = tstep ; t > EPSILON ; t-=dt )
        {

        /*  Compute tributary inflow rate (M^3/sec, Kg/Sec)  */
        /*  Interpolate concentrations to the unpstream confluence  */
        /*  Use distfac[] for distributary-confluences  */

#pragma omp parallel for                                                \
        default( none )                                                 \
        private( i, j, k, frac, dH2O, dNO3, dNH4, dDOC, dDON )          \
         shared( num_strm, triblo, tribhi, tribdex, distfac, strmflo,   \
                 strmH2O, strmNO3, strmNH4, strmDOC, strmDON,           \
                 tribH2O, tribNO3, tribNH4, tribDOC, tribDON )          \
       schedule( guided )

        for ( i = 0 ; i < num_strm ; i++ )
        {
        dH2O = 0.0 ;
        dNO3 = 0.0 ;
        dNH4 = 0.0 ;
        dDON = 0.0 ;
        dDOC = 0.0 ;
        for ( j = triblo[i]; j < tribhi[i] ; j++ )
            {
            k     = tribdex[j] ;
            frac  = distfac[i] * strmflo[k] / ( strmH2O[i] + strmH2O[k] ) ;
            dH2O += frac * ( strmH2O[i] + strmH2O[k] ) ;             /* == strmflo[k]  */
            dNO3 += frac * ( strmNO3[i] + strmNO3[k] ) ;
            dNH4 += frac * ( strmNH4[i] + strmNH4[k] ) ;
            dDON += frac * ( strmDON[i] + strmDON[k] ) ;
            dDOC += frac * ( strmDOC[i] + strmDOC[k] ) ;
            }
        tribH2O[i] = dH2O ;
        tribNO3[i] = dNO3 ;
        tribNH4[i] = dNH4 ;
        tribDON[i] = dDON ;
        tribDOC[i] = dDOC ;
        }

        cmax = 0.0 ;

        /*  Compute velocities, water outflow rate (M^3/sec)  */

#pragma omp parallel for                                                \
        default( none )                                                 \
        private( i, j, area, head, vel, dmann, dside, cscr, dH2O )      \
         shared( num_strm, resv_ID, strmH2O, strmlen, botwdth,          \
                 manning, sideslp, sqrtslp, strmflo, tribH2O, dt,       \
                 reslist, strmfac )                                     \
      reduction( max: cmax )                                            \
       schedule( guided )

        for ( i = 0 ; i < num_strm ; i++ )
            {
            if ( resv_ID[i] == 0 )
                {
                area  = strmH2O[i] / strmlen[i] ;
                head  = headfn( area, botwdth[i], sideslp[i] );
                dmann = 1.0 / manning[i] ;
                dside = 1.0 / sideslp[i] ;
                vel   = kinvel( area, head, botwdth[i], dmann, dside, sqrtslp[i] ) ;
                cscr  = vel / strmlen[i] ;
                if ( cscr > cmax )
                    {
                    cmax = cscr ;
                    }
                strmflo[i] = area * vel ;
                }
            else{
                strmflo[i] = resflow( reslist[i], tribH2O[i], dt ) ;
                }
            if ( strmH2O[i] > EPSILON )
                {
                strmfac[i] = strmflo[i] / strmH2O[i] ;
                }
            else{
                strmfac[i] = 0.0 ;
                }
            }

        dt = min( COUMAX / cmax , t ) ;     /*  Courant-stable internal time step  */

        /*  Update stream-state (M^3, Kg)  */

#pragma omp parallel for                                        \
        default( none )                                         \
        private( i, frac )                                      \
         shared( num_strm, strmfac, strmflo, baseflo, dt,       \
                 strmH2O, strmNO3, strmNH4, strmDOC, strmDON,   \
                 tribH2O, tribNO3, tribNH4, tribDOC, tribDON,   \
                 latsH2O, latsNO3, latsNH4, latsDOC, latsDON )

        for ( i = 0 ; i < num_strm ; i++ )
            {
            frac       = 1.0 - dt * strmfac[i] ;    /* residual-fraction == 1 - outflow-fraction  */
            strmH2O[i] = frac * strmH2O[i] + dt * ( tribH2O[i] + latsH2O[i] + baseflo[i] ) ;
            strmNO3[i] = frac * strmNO3[i] + dt * ( tribNO3[i] + latsNO3[i] ) ;
            strmNH4[i] = frac * strmNH4[i] + dt * ( tribNH4[i] + latsNH4[i] ) ;
            strmDON[i] = frac * strmDON[i] + dt * ( tribDON[i] + latsDON[i] ) ;
            strmDOC[i] = frac * strmDOC[i] + dt * ( tribDOC[i] + latsDOC[i] ) ;
            }

        }       /*  end internal time-step loop  */

    /*  (?) copy stream-overflow to surface  */

    return ;

    }           /*  end stream_routing()  */


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static void sub_vertical( double  tstep )        /*  process time-step  */
    {
    unsigned                i, j ;
    double                  fac, dH2O, dNO3, dNH4, dDOC, dDON ;
	struct patch_object *   patch ;

#pragma omp parallel for                                    \
        default( none )                                     \
        private( i, fac, dH2O, dNO3, dNH4, dDOC, dDON )     \
         shared( num_patches, capH2O, plist,                \
                 verbose, por_0, por_d, dzsoil, waterz,     \
                 totH2O, totNO3, totNH4, totDOC, totDON,    \
                 infH2O, infNO3, infNH4, infDOC, infDON,    \
                 latH2O, latNO3, latNH4, latDOC, latDON,    \
                 sfcH2O, sfcNO3, sfcNH4, sfcDOC, sfcDON )   \
       schedule( guided )

    for ( i = 0; i < num_patches; i++ )     /*  loop on patches  */
        {

        /*  Add infiltration, lateral inflow  */

        totH2O[i] = totH2O[i] + infH2O[i] + latH2O[i] ;
        totNO3[i] = totNO3[i] + infNO3[i] + latNO3[i] ;
        totNH4[i] = totNH4[i] + infNH4[i] + latNH4[i] ;
        totDON[i] = totDON[i] + infDON[i] + latDON[i] ;
        totDOC[i] = totDOC[i] + infDOC[i] + latDOC[i] ;

        /*  re-compute surface water  */

        if ( totH2O[i] > capH2O[i] )
            {
            fac  = ( totH2O[i] - capH2O[i] ) / totH2O[i] ;       /*  excess-water fraction  */
            dH2O = fac * totH2O[i] ;
            dNO3 = fac * totNO3[i] ;
            dNH4 = fac * totNH4[i] ;
            dDON = fac * totDON[i] ;
            dDOC = fac * totDOC[i] ;
            sfcH2O[i] = sfcH2O[i] + dH2O ;
            sfcNO3[i] = sfcNO3[i] + dNO3 ;
            sfcNH4[i] = sfcNH4[i] + dNH4 ;
            sfcDON[i] = sfcDON[i] + dDON ;
            sfcDOC[i] = sfcDOC[i] + dDOC ;
            totH2O[i] = totH2O[i] - dH2O ;
            totNO3[i] = totNO3[i] - dNO3 ;
            totNH4[i] = totNH4[i] - dNH4 ;
            totDON[i] = totDON[i] - dDON ;
            totDOC[i] = totDOC[i] - dDOC ;
            waterz[i] = plist[i]->z ;
            }
        else{
            dH2O      = totH2O[i] - capH2O[i] ;
            waterz[i] = plist[i]->z - compute_z_final( verbose, por_0[i], por_d[i], dzsoil[i], 0.0, dH2O ) ;
            }

        }           /*  end loop on patches  */

    return ;

    }               /*  end sub_vertical()  */


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

void hydro_routing( struct command_line_object * command_line,
                    double                       extstep,   /*  external time step  */
                    struct	date	             current_date,
                    struct basin_object        * basin )
    {
    double      substep ;       /*  subsurface (process-coupling) time step     */
    double      t ;             /*  time-variables (sec) [counts down to 0]     */
    double      head, area, bsum ;
    double      dH2O, dNO3, dNH4, dDOC, dDON ;
    unsigned    i, j, k ;
	struct patch_object     * patch ;
    struct hillslope_object * hillslope;

    if ( num_patches == -9999 )
        {
    	printf("Calling init_hydro_routing...\n");
        init_hydro_routing( command_line, basin ) ;
        }


    /*  COPY INTO WORKING VARIABLES  */

    now_date = current_date ;

#pragma omp parallel for  default( none )                       \
        private( i, patch )                                     \
         shared( num_patches, plist, waterz, thruH2O, thruNO3,  \
                 sfcH2O, sfcNO3, sfcNH4, sfcDOC, sfcDON,        \
                 gndH2O, gndNO3, gndNH4, gndDOC, gndDON,        \
                 totH2O, totNO3, totNH4, totDOC, totDON,        \
                 litH2O )

    for ( i = 0; i < num_patches; i++ )
        {
        patch     = plist[i] ;
        sfcH2O[i] = patch->detention_store ;    /*  to working vbles for sfc routing  */
        sfcNO3[i] = patch->surface_NO3 ;
        sfcNH4[i] = patch->surface_NH4 ;
        sfcDOC[i] = patch->surface_DOC ;
        sfcDON[i] = patch->surface_DON ;

        totH2O[i] = patch->field_capacity - patch->sat_deficit  ;    /*  to working vbles for subsfc routing  */
        totNO3[i] = patch->soil_ns.nitrate ;
        totNH4[i] = patch->soil_ns.sminn ;
        totDON[i] = patch->soil_ns.DON ;
        totDOC[i] = patch->soil_cs.DOC ;

        gndH2O[i] = 0.0 ;
        gndNO3[i] = 0.0 ;
        gndNH4[i] = 0.0 ;
        gndDOC[i] = 0.0 ;
        gndDON[i] = 0.0 ;

        litH2O[i] = patch->litter.rain_stored ;
        
        waterz[i]  = patch->z - ( patch->sat_deficit_z > ZERO ? patch->sat_deficit_z : ZERO ) ;

        thruH2O[i] = D3600 * patch->hourly->rain_throughfall ;  /* convert to M/S  */
        thruNO3[i] = D3600 * patch->hourly->NO3_throughfall ;
        }

#pragma omp parallel for                                                \
        default( none )                                                 \
        private( i, j, head, area, bsum )                               \
         shared( num_strm, basin, botwdth, sideslp, strmlen, strmH2O,   \
                 strmflo, hillslope, baseflo )                          \
       schedule( guided )

    for ( i = 0 ; i < num_strm ; i++ )
        {
        head = basin->stream_list.stream_network[i].water_depth ;
        area = head * ( botwdth[i] + head / sideslp[i] ) ;
        strmH2O[i] = area * strmlen[i] ;
        strmflo[i] = basin->stream_list.stream_network[i].Qout ;

        bsum = 0.0 ;
        for ( j = 0 ; j < basin->stream_list.stream_network[i].num_neighbour_hills ; j++ )
            {
            hillslope = basin->stream_list.stream_network[i].neighbour_hill[j] ;
            bsum      = bsum + hillslope->base_flow * hillslope->area ;
            }
        baseflo[i] = bsum ;
        }


    /*  MAIN PROCESSING LOOP:  */

    for( t = extstep; t > EPSILON; t-=substep )     /*  counts down to 0, with 10 usec tolerance for roundoff  */
        {
        sub_routing( t, &substep ) ;                /*  sets substep   */

        sfc_routing( substep ) ;

        stream_routing( substep ) ;

        sub_vertical( substep ) ;
        }


    /*  COPY BACK INTO MODEL-STATE  */

#pragma omp parallel for  default( none )                   \
        private( i, patch )                                 \
         shared( num_patches, plist, waterz,                \
                 sfcH2O, sfcNO3, sfcNH4, sfcDOC, sfcDON,    \
                 totH2O, totNO3, totNH4, totDOC, totDON,    \
                 litH2O )

    for ( i = 0; i < num_patches; i++ )
        {
        patch                  =  plist[i] ;
        patch->detention_store = sfcH2O[i] ;    /*  from working vbles for sfc routing  */
        patch->surface_NO3     = sfcNO3[i] ;
        patch->surface_NH4     = sfcNH4[i] ;
        patch->surface_DOC     = sfcDOC[i] ;
        patch->surface_DON     = sfcDON[i] ;

        patch->sat_deficit_z   = patch->z              - waterz[i]  ;
        patch->sat_deficit     = patch->field_capacity - totH2O[i] ;
        patch->soil_ns.nitrate = totNO3[i] ;
        patch->soil_ns.sminn   = totNH4[i] ;
        patch->soil_ns.DON     = totDON[i] ;
        patch->soil_cs.DOC     = totDOC[i] ;

        patch->litter.rain_stored = litH2O[i] ;
        }

#pragma omp parallel for                                                \
        default( none )                                                 \
        private( i, head, area )                                        \
         shared( num_strm, basin, botwdth, sideslp, strmlen, strmH2O, strmflo )

    for ( i = 0 ; i < num_strm ; i++ )
        {
        area = strmH2O[i] / strmlen[i] ;
        basin->stream_list.stream_network[i].water_depth = headfn( area, botwdth[i], sideslp[i] );
        basin->stream_list.stream_network[i].Qout = strmflo[i] ;
        }

#pragma omp parallel for                                                    \
        default( none )                                                     \
        private( i, j, k, hillslope, dH2O, dNO3, dNH4, dDOC, dDON )         \
         shared( num_hills, hillslo, hillshi, hillsdx, hillist, invhill,    \
                 parea, gndH2O, gndNO3, gndNH4, gndDOC, gndDON )            \
       schedule( guided )

    for ( i = 0 ; i < num_hills ; i++ )
        {
        dH2O = 0.0 ;
        dNO3 = 0.0 ;
        dNH4 = 0.0 ;
        dDON = 0.0 ;
        dDOC = 0.0 ;   
        for ( j = hillslo[i] ; k < hillshi[i] ; j++ )
            {
            k = hillsdx[j] ;
            dH2O += parea[k] * gndH2O[k] ;
            dNO3 += parea[k] * gndNO3[k] ;
            dNH4 += parea[k] * gndNH4[k] ;
            dDON += parea[k] * gndDON[k] ;
            dDOC += parea[k] * gndDOC[k] ;    
            }
        hillslope = hillist[i] ;
        hillslope->gw.storage += dH2O * invhill[i] ;
        hillslope->gw.NO3     += dNO3 * invhill[i] ;
        hillslope->gw.NH4     += dNH4 * invhill[i] ;
        hillslope->gw.DON     += dDON * invhill[i] ;
        hillslope->gw.DOC     += dDOC * invhill[i] ;
        }

    return ;

    }           /*  end hydro_routing()  */
