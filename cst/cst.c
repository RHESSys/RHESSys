/*--------------------------------------------------------------*/
/*                                                              */
/*                                                              */
/*  NAME                                                        */
/*		 create_stream_table				*/
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/* 		 create_stream_table (cst)                      */
/*                                                              */
/*  OPTIONS                                                     */
/*                                                              */
/*		-v Verbose Option				*/
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
#include "limits.h"

/* Function prototypes */
streamEntry* findStreamEntry(int, streamEntry**, int*);
int  checkStreamIntxns(streamEntry*, int, int, int, int, streamEntry*, int, int*, int);
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
    int j;
    int k;
    int	    maxr, maxc;
    int streamCnt = 0;

    streamEntry *nStreamPtr = NULL;
    streamEntry *streamEntryPtr = NULL;

    /* filenames for each image and file */
    char   name[MAXS], name2[MAXS];
    char*  rnhill;
    char*  rnzone;
    char*  rnpatch;
    char*  rndem;
    char*  rnstream;
    char*  rnbasin;
    char*  rnManningsN;
    char*  rnStreamTopWidth;
    char*  rnStreamBottomWidth;
    char*  rnStreamDepth;
    char* tmpStr;

    /* set pointers for images */

    double *dem;
    int    *patch;
    float  *slope;
    int    *hill;
    int    *zone;
    int	   *stream;
    int	   *basin;
    int    iPass;
    int    maxPasses;
    float  *ManningsN_map;
    float  *streamDepth_map;
    float  *streamTopWidth_map;
    float  *streamBottomWidth_map;

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

    struct Option* streamTopWidth_raster_opt = G_define_option();
    streamTopWidth_raster_opt->key = "streamTopWidth";
    streamTopWidth_raster_opt->type = TYPE_STRING;
    streamTopWidth_raster_opt->required = NO;
    streamTopWidth_raster_opt->description = "streamTopWidth";

    struct Option* streamBottomWidth_raster_opt = G_define_option();
    streamBottomWidth_raster_opt->key = "streamBottomWidth";
    streamBottomWidth_raster_opt->type = TYPE_STRING;
    streamBottomWidth_raster_opt->required = NO;
    streamBottomWidth_raster_opt->description = "streamBottomWidth";

    struct Option* streamDepth_raster_opt = G_define_option();
    streamDepth_raster_opt->key = "streamDepth";
    streamDepth_raster_opt->type = TYPE_STRING;
    streamDepth_raster_opt->required = NO;
    streamDepth_raster_opt->description = "streamDepth";

    /* Max passes to make through the list of streams in order to determine downstream reaches. */
    struct Option* maxPasses_opt = G_define_option();
    maxPasses_opt->key = "maxPasses";
    maxPasses_opt->type = TYPE_STRING;
    maxPasses_opt->required = NO;
    maxPasses_opt->description = "maxPasses";

    // Parse GRASS arguments
    if (G_parser(argc, argv))
        exit(1);

    // Get values from GRASS arguments
    // Name for output files, default to template file name

    rndem    = dem_raster_opt->answer;
    rnpatch  = patch_raster_opt->answer;
    rnzone   = zone_raster_opt->answer;
    rnhill   = hill_raster_opt->answer;
    rnstream = stream_raster_opt->answer;
    rnbasin  = basin_raster_opt->answer;

    // For the arguments streamTopWidth, streamBottomWidth, streamDepth, ManningsN,
    // either a constant value or a map name can be specified. If a constant value (a float)
    // is specified, then this value will be used for every cell in the map. If a mapname is
    // specified, then values from the map will be used.

    rnManningsN = ManningsN_raster_opt->answer;
    rnStreamTopWidth = streamTopWidth_raster_opt->answer;
    rnStreamBottomWidth = streamBottomWidth_raster_opt->answer;
    rnStreamDepth = streamDepth_raster_opt->answer;
    tmpStr = maxPasses_opt->answer;

    char *endPtr;
    maxPasses = strtod(tmpStr, &endPtr);
    if (maxPasses == 0)
        maxPasses = 1;

    printf("Max passes: %d\n", maxPasses);

    if (rnbasin != NULL) {
        struct Cell_head basin_header;
        basin = (int*)raster2array(rnbasin, &basin_header, NULL, NULL, CELL_TYPE);
    }

    if (rnManningsN != NULL) {
        struct Cell_head ManningsN_header;
        ManningsN_map = (float*)raster2array(rnManningsN, &ManningsN_header, NULL, NULL, CELL_TYPE);
    }

    if (rnStreamTopWidth != NULL) {
        struct Cell_head streamTopWidth_header;
        streamTopWidth_map = (float*)raster2array(rnStreamTopWidth, &streamTopWidth_header, NULL, NULL, CELL_TYPE);
    }

    if (rnStreamBottomWidth != NULL) {
        struct Cell_head streamBottomWidth_header;
        streamBottomWidth_map = (float*)raster2array(rnStreamBottomWidth, &streamBottomWidth_header, NULL, NULL, CELL_TYPE);
    }

    if (rnStreamDepth != NULL) {
        struct Cell_head streamDepth_header;
        streamDepth_map = (float*)raster2array(rnStreamDepth, &streamDepth_header, NULL, NULL, CELL_TYPE);
    }

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

    //printf("maxr: %d\n", maxr);
    //printf("maxc: %d\n", maxc);
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
                basinMaskValue = basin[index];
                if ((basinMaskValue == -2147483648) || (basinMaskValue == 0))
                    continue;
            }

            //streamId = ((int*)stream)[index];
            streamId = stream[index];
            if (streamId != -2147483648) {
                //printf("streamid: %i\n", streamId);
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

    /* Loop though the raster map, and look for stream intersections, on this pass determining only
       the downstream intersections from the current raster cell.
     */
    for (row = 0; row < maxr; ++row) {
        for (col = 0; col < maxc; ++col) {
            index = col + row*maxc;

            /* If a basin name was provided on the command line, only process cells within the basin. */
            if (rnbasin != NULL) {
                basinMaskValue = basin[index];
                if ((basinMaskValue == -2147483648) || (basinMaskValue == 0))
                    continue;
            }

            streamId = ((int*)stream)[index];
            if (streamId != -2147483648) {
                currentStreamPtr = findStreamEntry(streamId, &streamEntryPtr, &streamCnt);
                //printf("Checking intersections for stream %d, min elev: %f, max elev: %f\n", streamId, currentStreamPtr->minElevation, currentStreamPtr->maxElevation);

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

                int relaxedRules = 0;
                /* Check upper left adjacent cell */
                checkStreamIntxns(currentStreamPtr, streamId, row + 1, col - 1, maxc, streamEntryPtr, streamCnt, stream, relaxedRules);

                /* Check upper center adjacent cell */
                checkStreamIntxns(currentStreamPtr, streamId, row + 1, col,     maxc, streamEntryPtr, streamCnt, stream, relaxedRules);

                /* Check upper right adjacent cell */
                checkStreamIntxns(currentStreamPtr, streamId, row + 1, col + 1, maxc, streamEntryPtr, streamCnt, stream, relaxedRules);

                /* Check center right adjacent cell */
                checkStreamIntxns(currentStreamPtr, streamId, row,     col + 1, maxc, streamEntryPtr, streamCnt, stream, relaxedRules);

                /* Check lower right adjacent cell */
                checkStreamIntxns(currentStreamPtr, streamId, row - 1, col + 1, maxc, streamEntryPtr, streamCnt, stream, relaxedRules);

                /* Check lower center adjacent cell */
                checkStreamIntxns(currentStreamPtr, streamId, row - 1, col,     maxc, streamEntryPtr, streamCnt, stream, relaxedRules);

                /* Check lower left adjacent cell */
                checkStreamIntxns(currentStreamPtr, streamId, row - 1, col - 1, maxc, streamEntryPtr, streamCnt, stream, relaxedRules);

                /* Check center left adjacent cell */
                checkStreamIntxns(currentStreamPtr, streamId, row,     col - 1, maxc, streamEntryPtr, streamCnt, stream, relaxedRules);
            }
        }
    }

    for (iPass = 1; iPass <= maxPasses; iPass++) {
        int noDownstreamReach = 0;
        for (i = 0; i < streamCnt ; i++) {
            currentStreamPtr = streamEntryPtr + i;
            //printf("high: %f, low: %f\n", currentStreamPtr->maxElevation, currentStreamPtr->minElevation);
            if (currentStreamPtr->downstreamReach.streamId == 0)
                printf("no downstream reach for stream %d\n", currentStreamPtr->streamId);
            noDownstreamReach++;
        }

        if (noDownstreamReach <= 1)
            break;

        printf("Rechecking intersections for streams where a downstream reach was not found, pass %d.\n", iPass);
        for (row = 0; row < maxr; ++row) {
            for (col = 0; col < maxc; ++col) {
                index = col + row*maxc;

                /* If a basin name was provided on the command line, only process cells within the basin. */
                if (rnbasin != NULL) {
                    basinMaskValue = basin[index];
                    if (basinMaskValue == -2147483648)
                        continue;
                }

                streamId = stream[index];

                if (streamId != -2147483648) {
                    currentStreamPtr = findStreamEntry(streamId, &streamEntryPtr, &streamCnt);
                    if (currentStreamPtr->downstreamReach.streamId == 0) {
                        printf("Rechecking stream %d, min elev: %f, max elev: %f\n", streamId, currentStreamPtr->minElevation, currentStreamPtr->maxElevation);
                        int relaxedRules = 1;
                        checkStreamIntxns(currentStreamPtr, streamId, row + 1, col - 1, maxc, streamEntryPtr, streamCnt, stream, relaxedRules);
                        checkStreamIntxns(currentStreamPtr, streamId, row + 1, col,     maxc, streamEntryPtr, streamCnt, stream, relaxedRules);
                        checkStreamIntxns(currentStreamPtr, streamId, row + 1, col + 1, maxc, streamEntryPtr, streamCnt, stream, relaxedRules);
                        checkStreamIntxns(currentStreamPtr, streamId, row,     col + 1, maxc, streamEntryPtr, streamCnt, stream, relaxedRules);
                        checkStreamIntxns(currentStreamPtr, streamId, row - 1, col + 1, maxc, streamEntryPtr, streamCnt, stream, relaxedRules);
                        checkStreamIntxns(currentStreamPtr, streamId, row - 1, col,     maxc, streamEntryPtr, streamCnt, stream, relaxedRules);
                        checkStreamIntxns(currentStreamPtr, streamId, row - 1, col - 1, maxc, streamEntryPtr, streamCnt, stream, relaxedRules);
                        checkStreamIntxns(currentStreamPtr, streamId, row,     col - 1, maxc, streamEntryPtr, streamCnt, stream, relaxedRules);
                    }
                }
            }
        }
    }

    /* Now that all the downstream reaches have been defined, we must make one final pass through the stream array in
       order to determine all the upstream reaches, simply by check where the downstream reaches have been assigned
       and reverse mapping upstream reaches accordingly. Remember that we have only assigned one downstream reach to each
       reach, bu may assign multiple upstream reaches to each reach.
    */

    streamEntry *downstreamPtr = NULL;
    int downstreamId;

    printf("Assigning upstream reaches\n");
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
    fprintf(streamOutFile, "%i\n", streamCnt);
    double distance;
    double slopeValue;
    basinDivision *currentBasinDivisionPtr;
    streamIntxn *currentStreamIntxnPtr;

    G_begin_distance_calculations();

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

        for (j = 0; j < currentStreamPtr->basinDivisionCnt; ++j) {
            currentBasinDivisionPtr = (currentStreamPtr->basinDivisions) + j;
            //    //printf("j: %d, patch: %d, zone: %d, hill: %d\n", j, currentBasinDivisionPtr->patchId, currentBasinDivisionPtr->zoneId, currentBasinDivisionPtr->hillId);
            //    if ((currentBasinDivisionPtr->patchId < INT_MIN) || (currentBasinDivisionPtr->patchId > INT_MAX))
            //        printf("patchId value out of range %d\n", currentBasinDivisionPtr->patchId);
            //    if ((currentBasinDivisionPtr->zoneId  < INT_MIN) || (currentBasinDivisionPtr->zoneId  > INT_MAX))
            //        printf("zoneId value out of range\n");
            //    if ((currentBasinDivisionPtr->hillId  < INT_MIN) || (currentBasinDivisionPtr->hillId  > INT_MAX))
            //        printf("hillId value out of range\n");

            fprintf(streamOutFile, "    %d, %d, %d\n", currentBasinDivisionPtr->patchId, currentBasinDivisionPtr->zoneId, currentBasinDivisionPtr->hillId);
        }

        /* Now print out upstream reaches */
        fprintf(streamOutFile, "%d\n", currentStreamPtr->upstreamCnt);
        for (k = 0; k < currentStreamPtr->upstreamCnt; ++k) {
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
    printf("Done\n");

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
        newStreamPtr->downstreamReachMinElevation = DBL_MAX;
        newStreamPtr->downstreamReachMaxElevation = -DBL_MAX;
        found = 1;
        newStreamPtr->pixelCount = 0;
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

    /* The streamId was not found, so make a new entry for this stream */
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
        currentStreamPtr->downstreamReachMinElevation = DBL_MAX;
        currentStreamPtr->downstreamReachMaxElevation = -DBL_MAX;
        currentStreamPtr->downstreamReach.streamId = 0;
        currentStreamPtr->streamId = streamId;
        currentStreamPtr->pixelCount = 0;
        currentStreamPtr->upstreamCnt = 0;
        currentStreamPtr->basinDivisions = NULL;
    } else {
        printf("Error, streamId %d neither found nor newly allocated\n", streamId);
    }

    return currentStreamPtr;
}

int checkStreamIntxns(streamEntry *currentStreamPtr, int streamId, int nRow, int nCol, int maxc, streamEntry *streamEntryPtr, int streamCnt,
                      int *stream, int relaxedRules) {
    /*
      If an intersection is found, then modify the stream intersection structure for the current stream raster cell.

      The stream reaches in the 'stream' mapset are created such that a stream can have more than one upstream reach and
      only one downstream reach. A stream reach is "upstream" from another reach if it's max. elevation is higher than
      it's adjacent cell.
    */

    int index;
    int adjacentStreamId;
    int retStat = 0;
    streamEntry *adjacentStreamPtr = NULL;
    streamEntry *currentDownStreamPtr = NULL;

    // Raster cell that we are checking is out of bounds (off the map)
    if (nRow < 0 || nCol < 0)
        return 0;

    index = nCol + nRow*maxc;
    adjacentStreamId = stream[index];

    // The cell we are checking is not a stream
    if (adjacentStreamId == -2147483648)
        return 0;

    // The cell we are checing contains the current stream
    if (adjacentStreamId == streamId)
        return 0;

    // This cell must be another stream, so find it's stream entry
    adjacentStreamPtr = findStreamEntry(adjacentStreamId, &streamEntryPtr, &streamCnt);

    // Determine if the adjacent stream is downstream in relation to the current stream.
    //printf("adjacentStreamPtr->streamId: %d\n", adjacentStreamPtr->streamId);
    //printf("adjacentStreaPtr->minElevation: %f\n", adjacentStreamPtr->minElevation);
    //printf("adjacentStreamPtr->downstreamReachMinElevation: %f\n", adjacentStreamPtr->downstreamReachMinElevation);
    //printf("adjacentStreamPtr->downstreamReachMaxElevation: %f\n", adjacentStreamPtr->downstreamReachMaxElevation);

    if ( (adjacentStreamPtr->minElevation < currentStreamPtr->minElevation)
            && (currentStreamPtr->maxElevation > adjacentStreamPtr->maxElevation) )  {
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
    }
    else if (relaxedRules == 1) {
        if ( (adjacentStreamPtr->downstreamReachMinElevation <= currentStreamPtr->minElevation) &&
                currentStreamPtr->maxElevation > adjacentStreamPtr->downstreamReachMaxElevation)  {
            if (currentStreamPtr->downstreamReach.streamId == 0)
                addStreamIntxn(currentStreamPtr, adjacentStreamPtr, nRow, nCol, index);
            /* else if this stream has been added before, then replace the previous entry if the new one has a lower elevation. */
            else {
                currentDownStreamPtr = findStreamEntry(currentStreamPtr->downstreamReach.streamId, &streamEntryPtr, &streamCnt);
                if (adjacentStreamPtr->minElevation < currentDownStreamPtr->minElevation ) {
                    addStreamIntxn(currentStreamPtr, adjacentStreamPtr, nRow, nCol, index);
                }
            }
        }
    }

    //printf("Stream %d, elevation: %7.2f, Found intersecting stream %d, elevation %7.2f\n", streamId, currentStreamPtr->maxElevation, adjacentStreamId, adjacentStreamPtr->maxElevation);

    if (currentStreamPtr->downstreamReach.streamId == 0)
        return 0;
    else
        return 1;

}

int addStreamIntxn(streamEntry *currentStreamPtr, streamEntry *adjacentStreamPtr, int nRow, int nCol, int index) {

    printf("Stream %d: adding intxn to stream %d, row: %d, col: %d\n", currentStreamPtr->streamId, adjacentStreamPtr->streamId, nRow, nCol);
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

    currentStreamPtr->downstreamReachMinElevation = adjacentStreamPtr->minElevation;
    currentStreamPtr->downstreamReachMaxElevation = adjacentStreamPtr->maxElevation;
}

void addBasinDivision(streamEntry *streamPtr, int index, int* zone, int* hill, int* patch) {

    basinDivision* newBasinDivision;

    /* No basin divisions found for this stream entry, so add the division at the beginning of the array */
    if (streamPtr->basinDivisions == NULL) {
        //printf("adding initial division for stream %d\n", streamPtr->streamId);
        streamPtr->basinDivisions = (basinDivision *) malloc(sizeof(basinDivision));
        streamPtr->basinDivisions->zoneId  = zone[index];
        streamPtr->basinDivisions->hillId  = hill[index];
        streamPtr->basinDivisions->patchId = patch[index];
        streamPtr->basinDivisionCnt = 0;
        //printf("index: %d, stream: %d, zone: %d, hill %d, patch: %d\n", index, streamPtr->streamId, streamPtr->basinDivisions->zoneId, streamPtr->basinDivisions->hillId, streamPtr->basinDivisions->patchId);
    } else {
        newBasinDivision = findBasinDivision(streamPtr, index, zone, hill, patch);
        /* Basin division wasn't found, so add it */
        if (newBasinDivision == NULL) {
            (streamPtr->basinDivisionCnt)++;
            //printf("adding additional division %d for stream %d\n", streamPtr->basinDivisionCnt, streamPtr->streamId);
            streamPtr->basinDivisions = (basinDivision *) realloc(streamPtr->basinDivisions, sizeof(basinDivision) * (streamPtr->basinDivisionCnt));
            newBasinDivision = streamPtr->basinDivisions + ((streamPtr->basinDivisionCnt) - 1);
            newBasinDivision->zoneId = zone[index];
            newBasinDivision->hillId = hill[index];
            newBasinDivision->patchId = patch[index];
            //printf("index: %d, stream: %d, zone: %d, hill %d, patch: %d\n", index, streamPtr->streamId, newBasinDivision->zoneId, newBasinDivision->hillId, newBasinDivision->patchId);
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

    zoneId  = zone[index];
    hillId  = hill[index];
    patchId = patch[index];

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
        printf("Adding upstream id %d to stream %d\n", upstreamId, downstreamPtr->streamId);
        downstreamPtr->upstreamReaches = (streamIntxn *) malloc(sizeof(streamIntxn));
        (downstreamPtr->upstreamCnt)++;
        downstreamPtr->upstreamReaches->streamId = upstreamId;
        downstreamPtr->upstreamReaches->row = row;
        downstreamPtr->upstreamReaches->col = col;
    } else {
        printf("adding upstream id %d to stream %d\n", upstreamId, downstreamPtr->streamId);
        (downstreamPtr->upstreamCnt)++;
        downstreamPtr->upstreamReaches = (streamIntxn *) realloc(downstreamPtr->upstreamReaches, sizeof(streamIntxn) * (downstreamPtr->upstreamCnt));

        currentUpstreamIntxnPtr = (downstreamPtr->upstreamReaches) + ((downstreamPtr->upstreamCnt) - 1);
        currentUpstreamIntxnPtr->streamId = upstreamId;
        currentUpstreamIntxnPtr->row = row;
        currentUpstreamIntxnPtr->col = col;
    }
}
