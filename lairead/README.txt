Last revised: July 31, 2012

A description of the lairead program can be found at 

    http://wiki.icess.ucsb.edu/rhessys/Defining_stratum_state_variables_from_LAI_%28lairead%29

The current version of lairead (July 30, 2012) now reads GRASS the following raster maps directly:

   vegid
   lai
   zone
   patch
   hill

The previous version of lairead
of reading text versions of 
Map (DEM) and creates a summary report of information about the streams.

This program determines for each stream reach which streams are upstream and which are
downstream from it.

Also the slope, number of pixels and intersections with each zone, hill and patch are determined.

The format of the file create by cst.c is:

Number of stream reaches
Number of patches

ReachID  bottom_width top_width max_height slope ManningsN stream_length
Numbers of intersecting units
    PatchID  ZoneID  HillID
    PatchID  ZoneID  HillID
    .
    .
    .
Number of upstream reaches
    upstream streamId
    upstream streamId
    .
    .
Number of downstream reaches
    downstream streamId
    .
    .
    .
EOF

Here is a sample output file showing just the first two stream reaches:

184
282030

1610    2.10    1.30    5.50    0.03    0.55  660.00
14
    10408, 1609, 1609
    10569, 1610, 1610
    10568, 1610, 1610
    10569, 1609, 1609
    10405, 1610, 1610
    10568, 1609, 1609
    10727, 1609, 1609
    10727, 1610, 1610
    10726, 1610, 1610
    10897, 1610, 1610
    10896, 1610, 1610
    11066, 1610, 1610
    10894, 1610, 1610
    11393, 1610, 1610
1
    1612
1
    1608

1604    2.10    1.30    5.50    0.02    0.55  600.00
12
    8609, 1604, 1604
    8609, 1603, 1603
    8445, 1604, 1604
    8445, 1603, 1603
    8928, 1604, 1604
    8927, 1604, 1604
    8927, 1603, 1603
    9091, 1604, 1604
    9091, 1603, 1603
    9420, 1604, 1604
    9420, 1603, 1603
    9753, 1603, 1603
0
1
    1602
.
.
.
EOF

Command Usage
-------------

The cst program must be run within a GRASS session:

  GRASS 6.4.2 (GRASSnan):~/trunk/cst > cst --help

  Description:
   Creates a stream table file for input into RHESSys

  Keywords:
   RHESSys

  Usage:
   cst [-vd] output=string stream=string dem=string patch=string
     zone=string hill=string [basin=string] [ManningsN=string]
     [streamTopWidth=string] [streamBottomWidth=string] [streamDepth=string]
     [maxPasses=string]
  
  Flags:
    -v   print verbose information
    -d   print debug info
  
  Parameters:
               output   Output name
               stream   stream
                  dem   dem
                patch   patch
                 zone   zone
                 hill   hill
                basin   basin
            ManningsN   ManningsN
       streamTopWidth   streamTopWidth
    streamBottomWidth   streamBottomWidth
          streamDepth   streamDepth
            maxPasses   maxPasses


--- End of cst command usage listing

If a basin raster map is specified using the "basin=string" option, then only raster cells within the basin
will be processed.

The options "ManningsN", "streamTopWidth", "streamBottomWidth", "streamDepth" can either be specified as a
decimal value or as raster map name, for example:

     ManningsN=0.5

or

     ManningsN=manningsn

If a decimal value is specified, then this value will be used for every cell. If a raster map name is
specified, then each value from the raster map is considered and the average value for each stream is 
output to the stream table file.

Algorithm
---------

The cst program first looks at each cell the stream raster map and collects information for each stream
using information from the DEM, patch, zone, hill maps and ManningsN, streamTopWidth, streamBottomWidth,
streamDepth maps if specified.

The program then inspects each cell in the stream raster again and for each cell of each stream determines
what other streams are adjacent to it. For example, in the following diagram, the cell "C" is the current
stream cell that we are trying to find connections to, and the cells marked "A" are the ones that will
be inspected as possible adjacent stream cells to connect to:

                        A  A  A
                        A  C  A
                        A  A  A

The program then inspects each adjacent stream and determines if the adjacent stream in that cell is 
downstream from the current stream. The cells around each stream cell are inspected in the following order:

The cells are inspected in the following order

                        1  2  3
                        8  C  4
                        7  6  5

Because the program simply scans top to bottom, left to right when determining stream connections, not all
streams may be connected on the first pass. The 'maxPasses' option specifies how many times the program
will scan through the raster to determine stream connections.
