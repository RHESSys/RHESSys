Last revised: Sept 14, 2012

A description of the lairead program can be found at 

    http://wiki.icess.ucsb.edu/rhessys/Defining_stratum_state_variables_from_LAI_%28lairead%29

The current version of lairead (July 30, 2012) now reads GRASS the following raster maps directly:

   vegid
   lai
   zone
   patch
   hill

The syntax of lairead is now 

    lairead old=<old file> redef=<redef file> allom=<allometric file> lai=<lai> vegid=<vegid> zone=<patch120> hill=<hillslope> patch=<patch120>
 
where:
    "<old file>" is the existing world file that lairead will read in
    "<redef file>" is the new "redefined" world file that lairead will write out
    "<allometric file> is the name of the allometric file to read in
    "<lai>" is the name of the GRASS raster that contains the lai data
    "<vegid>" is the name of the GRASS raster that contains the vegid data
    "<zone>" is the name of the GRASS raster that contains the zone data
    "<hill>" is the name of the GRASS raster that contains the hill data
    "<patch>" is the name of the GRASS raster that contains the patch data

The following example command reads the world file "world.testing" and writes out the file "world.testing.Y2005M10DH1"
   
    lairead old=world.testing redef=world.testing.Y2005M10D2H1 allom=allometric.txt lai=lai vegid=vegid zone=patch120 hill=hillslope patch=patch120

The lairead program must be run from your GRASS environment, with a MASK defined for your area of interest.
