/*--------------------------------------------------------------*/
/*                                                              */
/*		 create_stream_table				*/
/*                                                              */
/*  NAME                                                        */
/*		 create_stream_table				*/
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/* 		 create_stream_table			        */
/*                                                              */
/*  OPTIONS                                                     */
/*                                                              */
/*		-v Verbose Option				*/
/*		-i input filename 				*/
/*		-o output file name 				*/
/*                                                              */
/*  DESCRIPTION                                                 */
/*                                                              */
/*  revision:  1.0  27 March, 2012                              */
/*  PROGRAMMER NOTES                                            */
/*                                                              */
/*--------------------------------------------------------------*/

#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <grass/gis.h>
#include "grassio.h"
#include "blender.h"
#include "glb.h"
#include "sub.h"
#include "stream.h"

/* Function prototypes */
streamEntry* findStreamEntry(int, streamEntry**, int*);
int  checkStreamIntxns(streamEntry*, int, int, int, int, streamEntry*, int, int*, unresolvedStreamIntxn**, int*);
int  addStreamIntxn(streamEntry*, streamEntry*, int, int, int);
void addBasinDivision(streamEntry*, int, int*, int*, int*);
basinDivision* findBasinDivision(streamEntry*, int, int*, int*, int*);
void addUpstreamIntxn(streamEntry*, streamEntry*, int, int);

main(int argc, char *argv[])
{

    /* Constants.
       If no command line argument is provided that specifies a raster map for
       these values, then use the constant value for the entire mapset.
     */
    float ManningsN         = 0.05;
    float streamTopWidth    = 1.0;
    float streamBottomWidth = 1.0;
    float streamDepth       = 1.0;

    /* local variable declarations */
    int	    i;
    int	    maxr, maxc;
    char    input_prefix[MAXS];
    char    output_suffix[MAXS];
    int streamCnt = 0;

    streamEntry *nStreamPtr = NULL;
    streamEntry *streamEntryPtr = NULL;

    unresolvedStreamIntxn *unresolvedStreamIntxnPtr = NULL;
    int unresolvedCnt = 0;

    /* filenames for each image and file */
    char   name[MAXS], name2[MAXS];
    char*  rnhill;
    char*  rnzone;
    char*  rnpatch;
    char*  rndem;
    char*  rnstream;
    char*  rnbasin;
    char*  rnManningsN;
    char*  rnStreamWidth;
    char*  rnStreamDepth;

    /* set pointers for images */

    double *dem;
    int    *patch;
    float  *slope;
    int    *hill;
    int    *zone;
    int	   *stream;
    int	   *basin;
    float  *ManningsN_map;
    float  *streamDepth_map;
    float  *streamWidth_map;

    // GRASS init
    G_gisinit(argv[0]);

    // GRASS module header
    struct GModule* module;
    module = G_define_module();
    module->keywords = "RHESSys";
    module->description = "Creates a stream table file for input into RHESSys";

    // GRASS arguments

    struct Option* output_name_opt = G_define_option();
    output_name_opt->key = "output";
    output_name_opt->type = TYPE_STRING;
    output_name_opt->required = YES;
    output_name_opt->description = "Output name";

    // Arguments that specify the names of required raster maps
    struct Option* stream_raster_opt = G_define_option();
    stream_raster_opt->key = "stream";
    stream_raster_opt->type = TYPE_STRING;
    stream_raster_opt->required = YES;
    stream_raster_opt->description = "stream";

    struct Option* dem_raster_opt = G_define_option();
    dem_raster_opt->key = "dem";
    dem_raster_opt->type = TYPE_STRING;
    dem_raster_opt->required = YES;
    dem_raster_opt->description = "dem";

    struct Option* patch_raster_opt = G_define_option();
    patch_raster_opt->key = "patch";
    patch_raster_opt->type = TYPE_STRING;
    patch_raster_opt->required = YES;
    patch_raster_opt->description = "patch";

    struct Option* zone_raster_opt = G_define_option();
    zone_raster_opt->key = "zone";
    zone_raster_opt->type = TYPE_STRING;
    zone_raster_opt->required = YES;
    zone_raster_opt->description = "zone";

    struct Option* hill_raster_opt = G_define_option();
    hill_raster_opt->key = "hill";
    hill_raster_opt->type = TYPE_STRING;
    hill_raster_opt->required = YES;
    hill_raster_opt->description = "hill";

    struct Option* basin_raster_opt = G_define_option();
    basin_raster_opt->key = "basin";
    basin_raster_opt->type = TYPE_STRING;
    basin_raster_opt->required = NO;
    basin_raster_opt->description = "basin";

    struct Option* ManningsN_raster_opt = G_define_option();
    ManningsN_raster_opt->key = "ManningsN";
    ManningsN_raster_opt->type = TYPE_STRING;
    ManningsN_raster_opt->required = NO;
    ManningsN_raster_opt->description = "ManningsN";

    struct Option* streamWidth_raster_opt = G_define_option();
    streamWidth_raster_opt->key = "streamWidth";
    streamWidth_raster_opt->type = TYPE_STRING;
    streamWidth_raster_opt->required = NO;
    streamWidth_raster_opt->description = "streamWidth";

    struct Option* streamDepth_raster_opt = G_define_option();
    streamDepth_raster_opt->key = "streamDepth";
    streamDepth_raster_opt->type = TYPE_STRING;
    streamDepth_raster_opt->required = NO;
    streamDepth_raster_opt->description = "streamDepth";

    // Parse GRASS arguments
    if (G_parser(argc, argv))
        exit(1);

    // Get values from GRASS arguments
    // Name for output files, default to template file name
    strcpy(input_prefix, output_name_opt->answer);
    strcpy(output_suffix, "_stream_table.dat");

    rndem    = dem_raster_opt->answer;
    rnpatch  = patch_raster_opt->answer;
    rnzone   = zone_raster_opt->answer;
    rnhill   = hill_raster_opt->answer;
    rnstream = stream_raster_opt->answer;
    rnbasin  = basin_raster_opt->answer;
    rnManningsN = ManningsN_raster_opt->answer;
    rnStreamWidth = streamWidth_raster_opt->answer;
    rnStreamDepth = streamDepth_raster_opt->answer;

    //printf("Reading input data for %s...\n\n", rndem);
    
    struct Cell_head patch_header;
    patch = (int*)raster2array(rnpatch, &patch_header, NULL, NULL, CELL_TYPE);

    struct Cell_head zone_header;
    zone = (int*)raster2array(rnzone, &zone_header, NULL, NULL, CELL_TYPE);

    struct Cell_head hill_header;
    hill = (int*)raster2array(rnhill, &hill_header, NULL, NULL, CELL_TYPE);

    struct Cell_head stream_header;
    stream = (int*)raster2array(rnstream, &stream_header, &maxr, &maxc, CELL_TYPE);

    struct Cell_head dem_header;
    dem = (double*)raster2array(rndem, &dem_header, NULL, NULL, DCELL_TYPE);

    if (rnbasin != NULL) {
        struct Cell_head basin_header;
        basin = (int*)raster2array(rnbasin, &basin_header, NULL, NULL, CELL_TYPE);
    }

    if (rnManningsN != NULL) {
        struct Cell_head ManningsN_header;
        ManningsN_map = (float*)raster2array(rnManningsN, &ManningsN_header, NULL, NULL, CELL_TYPE);
    }

    if (rnStreamWidth != NULL) {
        struct Cell_head streamWidth_header;
        streamWidth_map = (float*)raster2array(rnStreamWidth, &streamWidth_header, NULL, NULL, CELL_TYPE);
    }

    if (rnStreamDepth != NULL) {
        struct Cell_head streamDepth_header;
        streamDepth_map = (float*)raster2array(rnStreamDepth, &streamDepth_header, NULL, NULL, CELL_TYPE);
    }

    printf("maxr: %d\n", maxr);
    printf("maxc: %d\n", maxc);
    printf("Cataloging streams...\n\n");

    int row, col;
    int streamId;
    int basinMaskValue;
    int index;
    double demValue;
    double xPos, yPos;
    streamEntry *newStreamPtr = NULL;

    /* Loop though the stream raster map and collect info on each stream reach that is found.
       Only include stream reaches that are within the basin boundaries.
     */
    
    for (row = 0; row < maxr; ++row) {
        for (col = 0; col < maxc; ++col) {
            index = col + row*maxc;

            /* If a basin name was provided on the command line, only process cells within the basin. */
            if (rnbasin != NULL) {
                basinMaskValue = ((int*)basin)[index];
                if ((basinMaskValue == -2147483648) || (basinMaskValue == 0))
                    continue;
            }

            streamId = ((int*)stream)[index];
            if (streamId != -2147483648) {
                newStreamPtr = findStreamEntry(streamId, &streamEntryPtr, &streamCnt);
                (newStreamPtr->pixelCount)++;

                addBasinDivision(newStreamPtr, index, zone, hill, patch);

                // Adding .5 to col, row causes position of center of cell to be returned.
                xPos = G_col_to_easting((double) col + .5, &stream_header);
                yPos = G_row_to_northing((double) row + .5, &stream_header); 
                //printf("streamId: %d, xPos: %7.2f, yPos: %7.2f\n", streamId, xPos, yPos);

                demValue = dem[index];
                //printf("height: %6.2f\n", demValue);

                // Set max height if this DEM value is higher than current max.
                if (demValue > newStreamPtr->maxElevation) {
                    newStreamPtr->maxElevation = demValue;
                    newStreamPtr->headXpos = xPos;
                    newStreamPtr->headYpos = yPos;
                }

                // Set min height if this DEM value is lower than current min.
                if (demValue < newStreamPtr->minElevation) {
                    newStreamPtr->minElevation = demValue;
                    newStreamPtr->outletXpos = xPos;
                    newStreamPtr->outletYpos = yPos;
                }
            }
        }
    }

    printf("Searching for stream intersections...\n\n");
    streamEntry *currentStreamPtr;
    streamEntry *adjacentStreamPtr;
    int nRow, nCol;

    /* Loop though the raster map, and look for stream intersections, determining only
       the downstream intersections from the current raster cell. 
     */
    for (row = 0; row < maxr; ++row) {
    //for (row = 0; row < 5; ++row) {
        //printf("row %d\n", row);
        for (col = 0; col < maxc; ++col) {
            index = col + row*maxc;

            /* If a basin name was provided on the command line, only process cells within the basin. */
            if (rnbasin != NULL) {
                basinMaskValue = ((int*)basin)[index];
                if ((basinMaskValue == -2147483648) || (basinMaskValue == 0))
                    continue;
            }

            streamId = ((int*)stream)[index];
            if (streamId != -2147483648) {
                //printf("Checking intersections for stream %d\n", streamId);
                currentStreamPtr = findStreamEntry(streamId, &streamEntryPtr, &streamCnt);

                /* Check adjacent cells (8) of current raster cell and check for stream intersections, i.e.
                
                        A  A  A
                        A  C  A
                        A  A  A

                   where 'C' is the current raster cell and 'A' is an adjacent cell.
                */

                /* We are searching for an intersection of the current raster cell with a downstream reach, and may find one before
                   all eight adjacent pixels are checked. We are going to check all eight however, because there may be multiple
                   intersecting downstream reaches, and we will select the lowest one.
                 */

                /* Check upper left adjacent cell */
                checkStreamIntxns(currentStreamPtr, streamId, row - 1, col -1, maxc, streamEntryPtr, streamCnt, stream, &unresolvedStreamIntxnPtr, &unresolvedCnt);

                /* Check upper center adjacent cell */
                checkStreamIntxns(currentStreamPtr, streamId, row - 1, col, maxc, streamEntryPtr, streamCnt, stream, &unresolvedStreamIntxnPtr, &unresolvedCnt);

                /* Check upper right adjacent cell */
                checkStreamIntxns(currentStreamPtr, streamId, row - 1, col + 1, maxc, streamEntryPtr, streamCnt, stream, &unresolvedStreamIntxnPtr, &unresolvedCnt);

                /* Check center right adjacent cell */
                checkStreamIntxns(currentStreamPtr, streamId, row, col + 1, maxc, streamEntryPtr, streamCnt, stream, &unresolvedStreamIntxnPtr, &unresolvedCnt);

                /* Check lower right adjacent cell */
                checkStreamIntxns(currentStreamPtr, streamId, row + 1, col + 1, maxc, streamEntryPtr, streamCnt, stream, &unresolvedStreamIntxnPtr, &unresolvedCnt);

                /* Check lower center adjacent cell */
                checkStreamIntxns(currentStreamPtr, streamId, row + 1, col, maxc, streamEntryPtr, streamCnt, stream, &unresolvedStreamIntxnPtr, &unresolvedCnt);

                /* Check lower left adjacent cell */
                checkStreamIntxns(currentStreamPtr, streamId, row + 1, col - 1, maxc, streamEntryPtr, streamCnt, stream, &unresolvedStreamIntxnPtr, &unresolvedCnt);

                /* Check center left adjacent cell */
                checkStreamIntxns(currentStreamPtr, streamId, row, col - 1, maxc, streamEntryPtr, streamCnt, stream, &unresolvedStreamIntxnPtr, &unresolvedCnt);
            }
        }
    }


    /* Resolve any stream intersections which could not be resolved on the first pass */
    unresolvedStreamIntxn *currentUnresolvedStreamIntxnPtr;

    //printf("fixing %d unresolved stream cells\n", unresolvedCnt);

    //for (i = 0; i = 0; i++) {
    for (i = 0; i < unresolvedCnt; i++) {
        //printf("i %d, unresolved count %d\n", i, unresolvedCnt);
        currentUnresolvedStreamIntxnPtr = unresolvedStreamIntxnPtr + i;
        streamId = currentUnresolvedStreamIntxnPtr->streamId;
        currentStreamPtr = findStreamEntry(streamId, &streamEntryPtr, &streamCnt);

        if (currentStreamPtr->downstreamReach.streamId != 0)
            continue;

        //printf("streamId: %d, undetermined downstream reach.\n", streamId);

        row = currentUnresolvedStreamIntxnPtr->row;
        col = currentUnresolvedStreamIntxnPtr->col;

        /* Check upper left adjacent cell */
        checkStreamIntxns(currentStreamPtr, streamId, row - 1, col -1, maxc, streamEntryPtr, streamCnt, stream, &unresolvedStreamIntxnPtr, NULL);

        /* Check upper center adjacent cell */
        checkStreamIntxns(currentStreamPtr, streamId, row - 1, col, maxc, streamEntryPtr, streamCnt, stream, &unresolvedStreamIntxnPtr, NULL);

        /* Check upper right adjacent cell */
        checkStreamIntxns(currentStreamPtr, streamId, row - 1, col + 1, maxc, streamEntryPtr, streamCnt, stream, &unresolvedStreamIntxnPtr, NULL);

        /* Check center right adjacent cell */
        checkStreamIntxns(currentStreamPtr, streamId, row, col + 1, maxc, streamEntryPtr, streamCnt, stream, &unresolvedStreamIntxnPtr, NULL);

        /* Check lower right adjacent cell */
        checkStreamIntxns(currentStreamPtr, streamId, row + 1, col + 1, maxc, streamEntryPtr, streamCnt, stream, &unresolvedStreamIntxnPtr, NULL);

        /* Check lower center adjacent cell */
        checkStreamIntxns(currentStreamPtr, streamId, row + 1, col, maxc, streamEntryPtr, streamCnt, stream, &unresolvedStreamIntxnPtr, NULL);

        /* Check lower left adjacent cell */
        checkStreamIntxns(currentStreamPtr, streamId, row + 1, col - 1, maxc, streamEntryPtr, streamCnt, stream, &unresolvedStreamIntxnPtr, NULL);

        /* Check center left adjacent cell */
        checkStreamIntxns(currentStreamPtr, streamId, row, col - 1, maxc, streamEntryPtr, streamCnt, stream, &unresolvedStreamIntxnPtr, NULL);
        
        //if (currentUnresolvedStreamIntxnPtr->.downstreamReach)
    }


    /* Now that all the downstream reaches have been defined, we must make one final pass through the stream array in
       order to determine all the upstream reaches, simply by check where the downstream reaches have been assigned
       and reverse mapping upstream reaches accordingly. Remember that we have only assigned one downstream reach to each
       reach, bu may assign multiple upstream reaches to each reach.
    */

    streamEntry *downstreamPtr = NULL;
    int downstreamId;

    for (i = 0; i < streamCnt ; i++) {
        currentStreamPtr = streamEntryPtr + i;
        streamId = currentStreamPtr->streamId;
        row = currentStreamPtr->downstreamReach.row;
        col = currentStreamPtr->downstreamReach.col;

        downstreamId = currentStreamPtr->downstreamReach.streamId;
        /* There still may be reaches that are not connected to any other stream. */
        if (downstreamId == 0)
            continue;

        /* Find the the stream reach entry that is downstream from the current stream. */
        downstreamPtr = findStreamEntry(downstreamId, &streamEntryPtr, &streamCnt);

        /* Now add the current stream as an upstream entry to the downstream reaches' entry */
        //printf("Checking stream %d for downstreamId %d\n", streamId, downstreamId);
        addUpstreamIntxn(downstreamPtr, currentStreamPtr, row, col);
    }

    /* Loop through all streams and print info out to a file */
    FILE *streamOutFile = fopen(output_name_opt->answer, "w");
    printf("Writing out file %s\n", output_name_opt->answer);
    double distance;
    double slopeValue;
    basinDivision *currentBasinDivisionPtr;
    streamIntxn *currentStreamIntxnPtr;

    G_begin_distance_calculations();
    int j;
    int k;
    for (i = 0; i < streamCnt ; i++) {
        currentStreamPtr = streamEntryPtr + i;
        
        distance = G_distance(currentStreamPtr->headXpos, currentStreamPtr->headYpos, currentStreamPtr->outletXpos, currentStreamPtr->outletYpos);
        // If the stream only contains one or two pixels, distance will be calculated to be 0.0. 
        // If this is the case, then artificially set distance to 30, the width of one pixel.
        if (distance == 0.0 && currentStreamPtr->pixelCount == 2)
            distance = 30;

        if (currentStreamPtr->maxElevation == currentStreamPtr->minElevation)
            slopeValue = 0;
        else
            // Calculate slope: m = (y2 - y1)/(x2 - x1)
            slopeValue = distance / (currentStreamPtr->maxElevation - currentStreamPtr->minElevation);
  
        //printf("streamId: %d, downstreamId: %d, max height: %6.2f, min height: %6.2f, distance: %7.2f, slope: %7.2f, pixel count: %d, division count: %d\n", 
        //     currentStreamPtr->streamId, currentStreamPtr->downstreamReach.streamId, currentStreamPtr->maxElevation, currentStreamPtr->minElevation, 
        //     distance, slopeValue, currentStreamPtr->pixelCount, currentStreamPtr->basinDivisionCnt);

        fprintf(streamOutFile, "%d %7.2f %7.2f %7.2f %7.2f %7.2f %d\n", currentStreamPtr->streamId, streamBottomWidth, 
           streamTopWidth, streamDepth, slopeValue, ManningsN, currentStreamPtr->pixelCount);
        fprintf(streamOutFile, "%d\n", currentStreamPtr->basinDivisionCnt);

        for (j = 0; j < currentStreamPtr->basinDivisionCnt; j++) {
            currentBasinDivisionPtr = currentStreamPtr->basinDivisions + j;
            //printf("patch: %d, zone%d, hill%d\n", currentBasinDivisionPtr->patchId, currentBasinDivisionPtr->zoneId, currentBasinDivisionPtr->hillId);
            fprintf(streamOutFile, "    %d, %d, %d\n", currentBasinDivisionPtr->patchId, currentBasinDivisionPtr->zoneId, currentBasinDivisionPtr->hillId);
        }

        /* Now print out upstream reaches */
        fprintf(streamOutFile, "%d\n", currentStreamPtr->upstreamCnt);
        for (k = 0; k < currentStreamPtr->upstreamCnt; k++) {
            currentStreamIntxnPtr = currentStreamPtr->upstreamReaches + k;
            fprintf(streamOutFile, "    %d\n", currentStreamIntxnPtr->streamId);
        }

        /* Now print out downstream reaches */
        if (currentStreamPtr->downstreamReach.streamId == 0) {
            fprintf(streamOutFile, "0\n");
        } else {
            fprintf(streamOutFile, "1\n");
            fprintf(streamOutFile, "    %d\n", currentStreamPtr->downstreamReach.streamId);
        }

        fprintf(streamOutFile, "\n");
    }

    //printf("stream count %d\n", streamCnt);

    fclose(streamOutFile);

    exit(0);

 /* end cst.c */
}

streamEntry* findStreamEntry(int streamId, streamEntry **streamEntryPtr, int *streamCnt) {

    streamEntry *currentStreamPtr = NULL;
    streamEntry *newStreamPtr = NULL;

    int i;
    int found = 0;

    currentStreamPtr = *streamEntryPtr;

    if (*streamEntryPtr == NULL) {
        //printf("Allocating first stream, streamId: %d\n", streamId);
        (*streamCnt)++;
        newStreamPtr = (streamEntry *) malloc(sizeof(streamEntry));
        *streamEntryPtr = newStreamPtr;
        newStreamPtr->streamId = streamId;
        newStreamPtr->minElevation = DBL_MAX;
        newStreamPtr->maxElevation = -DBL_MAX;
        newStreamPtr->adjacentStreamMinElevation = DBL_MAX;
        found = 1;
        newStreamPtr->pixelCount = 1;
        newStreamPtr->upstreamCnt = 0;
        newStreamPtr->downstreamReach.streamId = 0;
        newStreamPtr->basinDivisions = NULL;
        return newStreamPtr;
    } else {
        //printf("searching for streamId: %d\n", streamId);
        for(i=0; i < *streamCnt; i++) {
            currentStreamPtr = *streamEntryPtr + i;
            
            //printf("  current stream %d\n", currentStreamPtr->streamId);
            if (currentStreamPtr->streamId == streamId) {
                //printf("Found stream id: %d\n", streamId);
                found = 1;
                //printf("Found min height: %6.2f\n", currentStreamPtr->minElevation);
                return currentStreamPtr;
            }
        }
    }

    if (found == 0) {
        (*streamCnt)++;
        //printf("Allocating new stream entry for streamId: %d, count %d\n", streamId, *streamCnt);
        // Now allocate space for the new stream entry
        newStreamPtr = (streamEntry *) realloc(*streamEntryPtr, sizeof(streamEntry) * (*streamCnt));
        *streamEntryPtr = newStreamPtr;
        // Now find the offset to the new entry and initialize it.
        currentStreamPtr = newStreamPtr + ((*streamCnt) - 1);
        currentStreamPtr->minElevation = DBL_MAX;
        currentStreamPtr->maxElevation = -DBL_MAX;
        currentStreamPtr->adjacentStreamMinElevation = DBL_MAX;
        currentStreamPtr->streamId = streamId;
        currentStreamPtr->pixelCount = 1;
        currentStreamPtr->upstreamCnt = 0;
        currentStreamPtr->basinDivisions = NULL;
    } else {
        printf("Error, streamId %d neither found nor newly allocated\n", streamId);
    }

    return currentStreamPtr;
}

int checkStreamIntxns(streamEntry *currentStreamPtr, int streamId, int nRow, int nCol, int maxc, streamEntry *streamEntryPtr, int streamCnt, 
    int *stream, unresolvedStreamIntxn **unresolvedStreamIntxnPtr, int *unresolvedCnt) {
    /*
      If an intersection is found, then modify the stream intersection structure for the current stream raster cell.

      The stream reaches in the 'stream' mapset are created such that a stream can have only one upstream reach and
      only one downstream reach. A stream reach is "upstream" from another reach if it's max. elevation is higher than
      it's adjacent cell.
    */

    int index;
    int adjacentStreamId;
    int retStat = 0;
    streamEntry *adjacentStreamPtr = NULL;
    streamEntry *currentDownStreamPtr = NULL;
    unresolvedStreamIntxn *newUnresolvedStreamIntxnPtr = NULL;
    unresolvedStreamIntxn *currentUnresolvedStreamIntxnPtr = NULL;

    // Raster cell that we are checking is out of bounds (off the map)
    if (nRow < 0 || nCol < 0)
        return 0;

    index = nCol + nRow*maxc;
    adjacentStreamId = ((int*)stream)[index];

    // The cell we are checking is not a stream
    if (adjacentStreamId == -2147483648)
        return 0;

    // The cell we are checing contains the current stream
    if (adjacentStreamId == streamId)
        return 0;

    // This cell must be another stream, so find it's stream entry
    adjacentStreamPtr = findStreamEntry(adjacentStreamId, &streamEntryPtr, &streamCnt);

    // Determine if the adjacent stream is downstream in relation to the current stream.
    if (adjacentStreamPtr->minElevation < currentStreamPtr->minElevation) {
        /* Check if we have added a downstream reach before .
           If no downstream reach has been added before, then add the current one now. 
         */
        if (currentStreamPtr->downstreamReach.streamId == 0)
            addStreamIntxn(currentStreamPtr, adjacentStreamPtr, nRow, nCol, index);
        /* else if this stream has been added before, then replace the previous entry if the new one has a lower elevation. */
        else {
            currentDownStreamPtr = findStreamEntry(currentStreamPtr->downstreamReach.streamId, &streamEntryPtr, &streamCnt);
            if (adjacentStreamPtr->minElevation < currentDownStreamPtr->minElevation ) {
                addStreamIntxn(currentStreamPtr, adjacentStreamPtr, nRow, nCol, index);
            }
        } 
    } else if (adjacentStreamPtr->minElevation == currentStreamPtr->minElevation) {
         //printf("child min elevation: %7.2f\n", adjacentStreamPtr->adjacentStreamMinElevation);
         /* The adjacent stream has same elevation as current stream, so check the adjacent
            streams lowest outlet stream and see if it is lower. 
          */
         if (adjacentStreamPtr->adjacentStreamMinElevation < currentStreamPtr->minElevation) {
             addStreamIntxn(currentStreamPtr, adjacentStreamPtr, nRow, nCol, index);
             //printf("found lower grandchild\n");
        } else if (unresolvedCnt != NULL) {
            if (*unresolvedStreamIntxnPtr == NULL) {
                *unresolvedCnt = 1;
                newUnresolvedStreamIntxnPtr = (unresolvedStreamIntxn *) malloc(sizeof(unresolvedStreamIntxn));
                newUnresolvedStreamIntxnPtr->streamId = streamId;
                newUnresolvedStreamIntxnPtr->row = nRow;
                newUnresolvedStreamIntxnPtr->col = nCol;
                *unresolvedStreamIntxnPtr = newUnresolvedStreamIntxnPtr;
                //printf("found first unresolved streamId: %d\n", streamId);
            } else {
                (*unresolvedCnt)++;
                newUnresolvedStreamIntxnPtr = (unresolvedStreamIntxn *) realloc(*unresolvedStreamIntxnPtr, sizeof(unresolvedStreamIntxn) * (*unresolvedCnt));
                *unresolvedStreamIntxnPtr = newUnresolvedStreamIntxnPtr;
                currentUnresolvedStreamIntxnPtr = newUnresolvedStreamIntxnPtr + ((*unresolvedCnt) - 1);
                currentUnresolvedStreamIntxnPtr->streamId = streamId;
                currentUnresolvedStreamIntxnPtr->row = nRow;
                currentUnresolvedStreamIntxnPtr->col = nCol;
                //printf("found another unresolved streamId: %d\n", streamId);
            }
        }
    }
    
    //printf("Stream %d, elevation: %7.2f, Found intersecting stream %d, elevation %7.2f\n", streamId, currentStreamPtr->maxElevation, adjacentStreamId, adjacentStreamPtr->maxElevation);
    // Check if we have intersected this stream before
    

    // First, find the stream entry for the adjacent stream

    // 
    return 0;
}

int addStreamIntxn(streamEntry *currentStreamPtr, streamEntry *adjacentStreamPtr, int nRow, int nCol, int index) {

    //printf("Stream %d: adding intxn to stream %d, row: %d, col: %d\n", currentStreamPtr->streamId, adjacentStreamPtr->streamId, nRow, nCol);
    currentStreamPtr->downstreamReach.streamId = adjacentStreamPtr->streamId;
    currentStreamPtr->downstreamReach.row = nRow;
    currentStreamPtr->downstreamReach.col = nCol;
    /* Store the adjacent stream's min elevation with the current stream for convienence when we resolve 
       stream reaches that don't have an obvious downstream reach. We store the adjacent stream's min elevation
       in the pathological case where a stream has no obvious outlet because all the possible downstream reaches
       that it connects to are flat and of the same elevation as the current stream. This can be the case with
       very short reaches (3-4 pixels). In this case we record the min elevation of downstream reaches that these
       flat reaches connect to, in order to determine which 'flat' reaches would provide an outlet for streams connected
       to them. */

    if (currentStreamPtr->adjacentStreamMinElevation == DBL_MAX) {
        currentStreamPtr->adjacentStreamMinElevation = adjacentStreamPtr->minElevation;
    } else if (adjacentStreamPtr->minElevation < currentStreamPtr->adjacentStreamMinElevation) {
        currentStreamPtr->adjacentStreamMinElevation = adjacentStreamPtr->minElevation;
    }
}

void addBasinDivision(streamEntry *streamPtr, int index, int* zone, int* hill, int* patch) {

    basinDivision* newBasinDivision;

    /* No basin divisions found for this stream entry, so add the division at the beginning of the array */
    if (streamPtr->basinDivisions == NULL) {
        //printf("adding initial division for stream %d\n", streamPtr->streamId);
        streamPtr->basinDivisions = (basinDivision *) malloc(sizeof(basinDivision));
        streamPtr->basinDivisions->zoneId  = ((int*)zone)[index];
        streamPtr->basinDivisions->hillId  = ((int*)hill)[index];
        streamPtr->basinDivisions->patchId = ((int*)patch)[index];
        streamPtr->basinDivisionCnt = 1;
    } else {
        newBasinDivision = findBasinDivision(streamPtr, index, zone, hill, patch);
        /* Basin division wasn't found, so add it */
        if (newBasinDivision == NULL) {
            (streamPtr->basinDivisionCnt)++;
            //printf("adding additional division %d for stream %d\n", streamPtr->basinDivisionCnt, streamPtr->streamId);
            streamPtr->basinDivisions = (basinDivision *) realloc(streamPtr->basinDivisions, sizeof(basinDivision) * (streamPtr->basinDivisionCnt));
            newBasinDivision = streamPtr->basinDivisions + ((streamPtr->basinDivisionCnt) - 1);
            newBasinDivision->zoneId = ((int*)zone)[index];
            newBasinDivision->hillId = ((int*)hill)[index];
            newBasinDivision->patchId = ((int*)patch)[index];
            //printf("done\n");
        }
    }
}

basinDivision* findBasinDivision(streamEntry *streamPtr, int index, int *zone, int *hill, int *patch) {

    int i;
    basinDivision *currentBasinDivision;
    int zoneId;
    int hillId;
    int patchId;

    /* No basin divisions to search through, so return null. */

    if (streamPtr->basinDivisions == NULL) {
        return NULL;
    }

    zoneId  = ((int*)zone)[index];
    hillId  = ((int*)hill)[index];
    patchId = ((int*)patch)[index];

    /* Search all basin divisions for this stream and see if we have already added the current one. */
    for (i = 0; i < streamPtr->basinDivisionCnt; i++) {
        currentBasinDivision = streamPtr->basinDivisions + i;
        //printf("i %d, zone %d, hill %d, patch %d\n", i, zoneId, hillId, patchId);

        if ((currentBasinDivision->zoneId == zoneId ) &&
            (currentBasinDivision->hillId == hillId) &&
            (currentBasinDivision->patchId == patchId)) {
            //printf("Found zoneId %d, hillId %d, patchId %d\n", zoneId, hillId, patchId);
            return currentBasinDivision;
        }
    }

    /* No match found, so return NULL */
    return NULL;
}

void addUpstreamIntxn(streamEntry *downstreamPtr, streamEntry *upstreamPtr, int row, int col) {

    int i;
    int upstreamId; 

    streamIntxn *currentUpstreamIntxnPtr;
    upstreamId = upstreamPtr->streamId;

    for (i = 0; i < downstreamPtr->upstreamCnt; i++) {
        currentUpstreamIntxnPtr = (downstreamPtr->upstreamReaches) + i;
        /* If we find the current stream, then no need to add it again */
        if (currentUpstreamIntxnPtr->streamId == upstreamId) {
            //printf("upstreamId %d already added to stream %d\n", upstreamId, downstreamPtr->streamId);
            return;
        }
    } 
   
    /* We made it to this point, so the upstreamId was not found and we have to add it.*/
    if (downstreamPtr->upstreamCnt == 0) {
        //printf("Adding upstream id %d to stream %d\n", upstreamId, downstreamPtr->streamId);
        downstreamPtr->upstreamReaches = (streamIntxn *) malloc(sizeof(streamIntxn));
        (downstreamPtr->upstreamCnt)++;
        downstreamPtr->upstreamReaches->streamId = upstreamId;
        downstreamPtr->upstreamReaches->row = row;
        downstreamPtr->upstreamReaches->col = col;
    } else {
        //printf("adding upstream id %d to stream %d\n", upstreamId, downstreamPtr->streamId);
        (downstreamPtr->upstreamCnt)++;
        downstreamPtr->upstreamReaches = (streamIntxn *) realloc(downstreamPtr->upstreamReaches, sizeof(streamIntxn) * (downstreamPtr->upstreamCnt));

        currentUpstreamIntxnPtr = (downstreamPtr->upstreamReaches) + ((downstreamPtr->upstreamCnt) - 1);
        currentUpstreamIntxnPtr->streamId = upstreamId;
        currentUpstreamIntxnPtr->row = row;
        currentUpstreamIntxnPtr->col = col;
    }
}
