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
/*		-d print debug information 			*/
/*		-v print verbose information 			*/
/*                                                              */
/*  DESCRIPTION                                                 */
/*                                                              */
/*  revision:  1.0  12 June, 2012                               */
/*  PROGRAMMER NOTES                                            */
/*                                                              */
/*--------------------------------------------------------------*/

#include <float.h>
//#include <stdbool.h>
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
streamEntry* findHighestStream(streamEntry **streamEntryPtr, int *streamCnt);
streamEntry* findStreamEntry(int, streamEntry**, int*);
int  checkStreamIntxns(streamEntry*, int, int, int, int, streamEntry*, int, int*, bool);
void  addStreamIntxn(streamEntry*, streamEntry*, int, int, int);
void addBasinDivision(streamEntry*, int, int*, int*, int*);
basinDivision* findBasinDivision(streamEntry*, int, int*, int*, int*);
void addUpstreamIntxn(streamEntry*, streamEntry*, int, int);
bool printStream(streamEntry*, FILE*, streamEntry**, int*);

/* Global variables */
int    debug;
int    verbose;
float cellResolution;

int main(int argc, char *argv[])
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
    bool done = false;
    bool printed;
    char *pEnd;
    char *endPtr;
    double demValue;
    double distance;
    float inVal;
    int basinMaskValue;
    int	i;
    int j;
    int k;
    int index;
    int maxr, maxc;
    int row, col;
    int streamCnt = 0;
    int streamId;
    struct Cell_head basin_header;
    struct Cell_head dem_header;
    struct Cell_head hill_header;
    struct Cell_head ManningsN_header;
    struct Cell_head patch_header;
    struct Cell_head stream_header;
    struct Cell_head streamTopWidth_header;
    struct Cell_head streamBottomWidth_header;
    struct Cell_head streamDepth_header;
    struct Cell_head zone_header;

    streamEntry *nStreamPtr = NULL;
    streamEntry *streamEntryPtr = NULL;

    /* filenames for each image and file */
    char  name[MAXS], name2[MAXS];
    char* rnhill;
    char* rnzone;
    char* rnpatch;
    char* rndem;
    char* rnstream;
    char* rnbasin;
    char* rnManningsN;
    char* rnStreamTopWidth;
    char* rnStreamBottomWidth;
    char* rnStreamDepth;
    char* tmpStr;

    /* set pointers for images */

    double *dem;
    int    *patch;
    int    *hill;
    int    *zone;
    int	   *stream;
    int	   *basin;
    int    iPass;
    int    maxPasses;
    float  *slope;
    float  *ManningsN_map = NULL;
    float  *streamDepth_map = NULL;
    float  *streamTopWidth_map = NULL;
    float  *streamBottomWidth_map = NULL;

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
    maxPasses_opt->type = TYPE_INTEGER;
    maxPasses_opt->required = NO;
    maxPasses_opt->description = "maxPasses";

    /* Note that coor->answer is not given a default value. */
    struct Flag* verbose_flag  = G_define_flag();
    verbose_flag ->key = 'v';
    verbose_flag ->description = "print verbose information";

    struct Flag* debug_flag  = G_define_flag();
    debug_flag ->key = 'd';
    debug_flag ->description = "print debug info";

    // Parse GRASS arguments
    if (G_parser(argc, argv))
        exit(1);

    // Get values from GRASS arguments
    // Name for output files
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
    debug = debug_flag->answer;
    verbose = verbose_flag->answer;

    if (tmpStr == NULL) {
    	maxPasses = 1;
    } else {
    	maxPasses = atoi(tmpStr);
    }
    if (maxPasses == 0)
        maxPasses = 1;
       printf("Max passes: %d\n", maxPasses);
        printf("Max passes: %d\n", maxPasses);

    if (verbose)
        printf("Max passes: %d\n", maxPasses);

    if (rnbasin != NULL) {
        basin = (int*)raster2array(rnbasin, &basin_header, NULL, NULL, CELL_TYPE);
    }

    /* ManningsN can be specified on the command line as either a raster map name or a
       constant value.
     */

    if (rnManningsN != NULL) {
        inVal = strtof(rnManningsN, &pEnd);
        if (pEnd != rnManningsN) {
            ManningsN = inVal;
            if (verbose)
                printf("Using constant value from command line for ManningsN: %7.3f\n", ManningsN);
        } else {
            if (verbose)
                printf("Using map for ManningsN: %s\n", rnManningsN);
            ManningsN_map = (float*)raster2array(rnManningsN, &ManningsN_header, NULL, NULL, FCELL_TYPE);
        }
    }

    if (rnStreamTopWidth != NULL) {
        inVal = strtof(rnStreamTopWidth, &pEnd);
        if (pEnd != rnStreamTopWidth) {
            streamTopWidth = inVal;
            if (verbose)
                printf("Using constant value from command line for stream top width: %7.3f\n", streamTopWidth);
        } else {
            if (verbose)
                printf("Using map for top width: %s\n", rnStreamTopWidth);
            streamTopWidth_map = (float*)raster2array(rnStreamTopWidth, &streamTopWidth_header, NULL, NULL, FCELL_TYPE);
        }
    }

    if (rnStreamBottomWidth != NULL) {
        inVal = strtof(rnStreamBottomWidth, &pEnd);
        if (pEnd != rnStreamBottomWidth) {
            streamBottomWidth = inVal;
            if (verbose)
                printf("Using constant value from command line for stream bottom width: %7.3f\n", streamBottomWidth);
        } else {
            if (verbose)
                printf("Using map for bottom width: %s\n", rnStreamBottomWidth);
            streamBottomWidth_map = (float*)raster2array(rnStreamBottomWidth, &streamBottomWidth_header, NULL, NULL, FCELL_TYPE);
        }
    }

    if (rnStreamDepth != NULL) {
        inVal = strtof(rnStreamDepth, &pEnd);
        if (pEnd != rnStreamDepth) {
            streamDepth = inVal;
            if (verbose)
                printf("Using constant value from command line for stream depth: %7.3f\n", streamDepth);
        } else {
            if (verbose)
                printf("Using map for stream depth: %s\n", rnStreamDepth);
            streamDepth_map = (float*)raster2array(rnStreamDepth, &streamDepth_header, NULL, NULL, FCELL_TYPE);
        }
    }

    //printf("Reading input data for %s...\n\n", rndem);

    dem = (double*)raster2array(rndem, &dem_header, NULL, NULL, DCELL_TYPE);
    hill = (int*)raster2array(rnhill, &hill_header, NULL, NULL, CELL_TYPE);
    patch = (int*)raster2array(rnpatch, &patch_header, NULL, NULL, CELL_TYPE);
    stream = (int*)raster2array(rnstream, &stream_header, &maxr, &maxc, CELL_TYPE);
    zone = (int*)raster2array(rnzone, &zone_header, NULL, NULL, CELL_TYPE);

    //cellResolution = (dem_header.ew_res + dem_header.ns_res)/2.0;
    cellResolution = dem_header.ew_res;
    if (verbose)
        printf("DEM raster cell resolution: %7.2f\n", cellResolution);


    if (verbose) {
        printf("maxr: %d\n", maxr);
        printf("maxc: %d\n", maxc);
        printf("Cataloging streams...\n");
    }

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
                newStreamPtr->pixelCount = newStreamPtr->pixelCount + 1;

                addBasinDivision(newStreamPtr, index, zone, hill, patch);

                // Adding .5 to col, row causes position of center of cell to be returned.
                //xPos = G_col_to_easting((double) col + .5, &stream_header);
                //yPos = G_row_to_northing((double) row + .5, &stream_header);
                //printf("streamId: %d, xPos: %7.2f, yPos: %7.2f\n", streamId, xPos, yPos);

                demValue = dem[index];
                //printf("height: %6.2f\n", demValue);

                // Set max height if this DEM value is higher than current max.
                if (demValue > newStreamPtr->maxElevation) {
                    newStreamPtr->maxElevation = demValue;
                }

                // Set min height if this DEM value is lower than current min.
                if (demValue < newStreamPtr->minElevation) {
                    newStreamPtr->minElevation = demValue;
                }

                if (ManningsN_map != NULL) {
                    newStreamPtr->ManningsN += ManningsN_map[index];
                }

                if (streamTopWidth_map != NULL) {
                    newStreamPtr->streamTopWidth += streamTopWidth_map[index];
                }

                if (streamBottomWidth_map != NULL) {
                    newStreamPtr->streamBottomWidth += streamBottomWidth_map[index];
                }

                if (streamDepth_map != NULL) {
                    newStreamPtr->streamDepth += streamDepth_map[index];
                }
            }
        }
    }

    if (verbose)
        printf("Searching for stream intersections...\n");

    streamEntry *currentStreamPtr;
    streamEntry *adjacentStreamPtr;
    int nRow, nCol;
    int patchCnt = 0;
    bool relaxedRules;

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
            patchCnt += 1;
            if (streamId != -2147483648) {
                currentStreamPtr = findStreamEntry(streamId, &streamEntryPtr, &streamCnt);
                if (debug)
                    printf("Checking intersections for stream %d, min elev: %f, max elev: %f\n", streamId, currentStreamPtr->minElevation, currentStreamPtr->maxElevation);

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

                relaxedRules = false;
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

    /* Loop through streams, assign stream wide values */
    for (i = 0; i < streamCnt ; i++) {
        currentStreamPtr = streamEntryPtr + i;
        if (debug)
            printf("high: %f, low: %f\n", currentStreamPtr->maxElevation, currentStreamPtr->minElevation);

        //G_begin_distance_calculations();

        //distance = G_distance(currentStreamPtr->headXpos, currentStreamPtr->headYpos, currentStreamPtr->outletXpos, currentStreamPtr->outletYpos);

        // If the stream only contains one or two pixels, distance will be calculated to be 0.0.
        // If this is the case, then artificially set distance to the cell resolution, the width of one pixel.
        if (distance == 0.0 && currentStreamPtr->pixelCount == 2)
            distance = cellResolution;
        else
            distance = currentStreamPtr->pixelCount * cellResolution;

        // As of 6/4 we are calculating slope simply as ((max. elev. - min. elevation) / number of pixels in the stream reach).
        if (currentStreamPtr->maxElevation == currentStreamPtr->minElevation) {
            currentStreamPtr->slope = 0;
        } else {
            // Calculate slope: m = (y2 - y1)/(x2 - x1)
            currentStreamPtr->slope = (currentStreamPtr->maxElevation - currentStreamPtr->minElevation) / distance;
        }

        /* Calculate mean ManningsN */
        if (ManningsN_map != NULL) {
            currentStreamPtr->ManningsN = currentStreamPtr->ManningsN / currentStreamPtr->pixelCount;
        } else {
            currentStreamPtr->ManningsN = ManningsN;
        }

        /* Calculate mean stream top width */
        if (streamTopWidth_map != NULL) {
            currentStreamPtr->streamTopWidth = currentStreamPtr->streamTopWidth / currentStreamPtr->pixelCount;
        } else {
            currentStreamPtr->streamTopWidth = streamTopWidth;
        }

        /* Calculate mean stream bottom width */
        if (streamBottomWidth_map != NULL) {
            currentStreamPtr->streamBottomWidth = currentStreamPtr->streamBottomWidth / currentStreamPtr->pixelCount;
        } else {
            currentStreamPtr->streamBottomWidth = streamBottomWidth;
        }

        /* Calculate mean stream depth */
        if (streamDepth_map  != NULL) {
            currentStreamPtr->streamDepth = currentStreamPtr->streamDepth / currentStreamPtr->pixelCount;
        } else {
            currentStreamPtr->streamDepth = streamDepth;
        }
    }

    relaxedRules = true;
    for (iPass = 1; iPass <= maxPasses; iPass++) {
        if (debug)
            printf("pass: %d\n", iPass);
        int noDownstreamReach = 0;
        for (i = 0; i < streamCnt ; i++) {
            currentStreamPtr = streamEntryPtr + i;
            if (currentStreamPtr->downstreamReach.streamId == -1)
                noDownstreamReach++;
        }

        // There should always be at least one stream that is the outlet to the basin.
        if (noDownstreamReach == 1)
            break;

        if (debug)
            printf("%d streams have no outlet.\n", noDownstreamReach);

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
                    if (currentStreamPtr->downstreamReach.streamId == -1) {
                        if (debug)
                            printf("Rechecking stream %d, min elev: %f, max elev: %f\n", streamId, currentStreamPtr->minElevation, currentStreamPtr->maxElevation);
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

    int noDownstreamReach = 0;
    for (i = 0; i < streamCnt ; i++) {
        currentStreamPtr = streamEntryPtr + i;
        if (currentStreamPtr->downstreamReach.streamId == -1)
            noDownstreamReach++;
    }

    if (verbose)
        printf("Final count of stream reaches without an outlet: %d\n", noDownstreamReach);

    /* Now that all possible downstream reaches have been defined, we must make another pass through the stream array in
       order to determine all the upstream reaches, simply by check where the downstream reaches have been assigned
       and reverse mapping upstream reaches accordingly. Remember that we have only assigned one downstream reach to each
       reach, bu may assign multiple upstream reaches to each reach.
    */

    streamEntry *downstreamPtr = NULL;
    int downstreamId;

    //printf("Assigning upstream reaches\n");
    for (i = 0; i < streamCnt ; i++) {
        currentStreamPtr = streamEntryPtr + i;
        streamId = currentStreamPtr->streamId;
        row = currentStreamPtr->downstreamReach.row;
        col = currentStreamPtr->downstreamReach.col;

        // Print stream info for debugging purposes
        //printf("%d, %7.2f, %7.2f\n", streamId, currentStreamPtr->minElevation, currentStreamPtr->maxElevation);

        downstreamId = currentStreamPtr->downstreamReach.streamId;
        /* There still may be reaches that are not connected to any other stream. */
        if (downstreamId == -1)
            continue;

        /* Find the the stream reach entry that is downstream from the current stream. */
        downstreamPtr = findStreamEntry(downstreamId, &streamEntryPtr, &streamCnt);

        /* Now add the current stream as an upstream entry to the downstream reaches' entry */
        //printf("Checking stream %d for downstreamId %d\n", streamId, downstreamId);
        addUpstreamIntxn(downstreamPtr, currentStreamPtr, row, col);
    }

    /* Loop through all streams and print info out to a file */
    FILE *streamOutFile = fopen(output_name_opt->answer, "w");
    if (verbose)
        printf("Writing out file %s\n", output_name_opt->answer);

    fprintf(streamOutFile, "%i\n", streamCnt);
    //fprintf(streamOutFile, "%i\n", patchCnt);
    streamEntry *highestStreamPtr;

    /* The streams are printed out highest elevation to lowest. */
    while (done != true) {

        highestStreamPtr = findHighestStream(&streamEntryPtr, &streamCnt);

        if (highestStreamPtr == NULL) {
            break;
        }

        // Print stream info to output file specified on the command line.
        printed = printStream(highestStreamPtr, streamOutFile, &streamEntryPtr, &streamCnt);

        if (printed != true) {
            break;
        }

    }

    fclose(streamOutFile);
    if (verbose)
        printf("Program %s Done\n", argv[0]);

    return EXIT_SUCCESS;

    /* end cst.c */
}

/* Print stream info to a file. */
bool printStream(streamEntry *currentStreamPtr, FILE *streamOutFile, streamEntry **streamEntryPtr, int *streamCnt) {

    basinDivision *currentBasinDivisionPtr;
    streamIntxn *currentStreamIntxnPtr;
    streamEntry *downstreamReachPtr;

    if (currentStreamPtr->printed == true) {
        return false;
    } else {
        currentStreamPtr->printed = true;
    }

    if (debug)
        printf("Printing stream %d\n", currentStreamPtr->streamId);

	fprintf(streamOutFile, "\n%d %7.2f %7.2f %7.2f %7.4f %7.4f %7.2f\n", currentStreamPtr->streamId, currentStreamPtr->streamTopWidth, 
	currentStreamPtr->streamBottomWidth, currentStreamPtr->streamDepth, currentStreamPtr->slope, currentStreamPtr->ManningsN, 
	currentStreamPtr->pixelCount*cellResolution);
    fprintf(streamOutFile, "%d\n", currentStreamPtr->basinDivisionCnt);

    int j;
    for (j = 0; j < currentStreamPtr->basinDivisionCnt; ++j) {
        currentBasinDivisionPtr = (currentStreamPtr->basinDivisions) + j;
        fprintf(streamOutFile, "    %d %d %d\n", currentBasinDivisionPtr->patchId, currentBasinDivisionPtr->zoneId, currentBasinDivisionPtr->hillId);
    }

    /* Now print out upstream reaches */
    fprintf(streamOutFile, "%d\n", currentStreamPtr->upstreamCnt);
    int k;
    for (k = 0; k < currentStreamPtr->upstreamCnt; ++k) {
        currentStreamIntxnPtr = currentStreamPtr->upstreamReaches + k;
        fprintf(streamOutFile, "    %d\n", currentStreamIntxnPtr->streamId);
    }

    /* Now print out downstream reaches */
    if (currentStreamPtr->downstreamReach.streamId == -1) {
        fprintf(streamOutFile, "0\n");
    } else {
        fprintf(streamOutFile, "1\n");
        fprintf(streamOutFile, "    %d\n", currentStreamPtr->downstreamReach.streamId);
    }

    return true;
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
        found = 1;
        newStreamPtr->pixelCount = 0;
        newStreamPtr->printed = false;
        newStreamPtr->upstreamCnt = 0;
        newStreamPtr->downstreamReach.streamId = -1;
        newStreamPtr->basinDivisions = NULL;
        newStreamPtr->streamBottomWidth = 0.0;
        newStreamPtr->streamTopWidth = 0.0;
        newStreamPtr->streamDepth = 0.0;
        newStreamPtr->ManningsN = 0.0;

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
        currentStreamPtr->downstreamReach.streamId = -1;
        currentStreamPtr->streamId = streamId;
        currentStreamPtr->pixelCount = 0;
        currentStreamPtr->printed = false;
        currentStreamPtr->upstreamCnt = 0;
        currentStreamPtr->basinDivisions = NULL;
        currentStreamPtr->streamBottomWidth = 0.0;
        currentStreamPtr->streamTopWidth = 0.0;
        currentStreamPtr->streamDepth = 0.0;
        currentStreamPtr->ManningsN = 0.0;
    } else {
        printf("Error, streamId %d neither found nor newly allocated\n", streamId);
    }

    return currentStreamPtr;
}

int checkStreamIntxns(streamEntry *currentStreamPtr, int streamId, int nRow, int nCol, int maxc, streamEntry *streamEntryPtr, int streamCnt,
                      int *stream, bool relaxedRules) {
    /*
      If an intersection is found, then modify the stream intersection structure for the current stream raster cell.

      The stream reaches in the 'stream' mapset are created such that a stream can have more than one upstream reach and
      only one downstream reach. A stream reach is "upstream" from another reach if it's max. elevation is higher than
      it's adjacent cell.
    */

    int index;
    int adjacentStreamId;
    int downstreamReachCount = 1;
    int maxDownstreamReachCount = 10;
    double elevationDiffThreshold = 0.0;
    double streamMidPoint;
    streamEntry *adjacentStreamPtr = NULL;
    streamEntry *currentDownstreamPtr = NULL;

    // Raster cell that we are checking is out of bounds (off the map)
    if (nRow < 0 || nCol < 0)
        return 0;

    /* Does the current stream already have a downstream reach defined? */
    if (currentStreamPtr->downstreamReach.streamId != -1) {
        return 0;
        //currentDownstreamPtr = findStreamEntry(currentStreamPtr->downstreamReach.streamId, &streamEntryPtr, &streamCnt);
    } else {
        currentDownstreamPtr = NULL;
    }

    index = nCol + nRow*maxc;
    adjacentStreamId = stream[index];

    // The cell we are checking is not a stream
    if (adjacentStreamId == -2147483648)
        return 0;

    // The cell we are checking contains the current stream
    if (adjacentStreamId == streamId)
        return 0;

    // This cell must be another stream (i.e. not same stream or no stream), so find it's stream entry
    adjacentStreamPtr = findStreamEntry(adjacentStreamId, &streamEntryPtr, &streamCnt);

    currentDownstreamPtr = adjacentStreamPtr;
    if (debug)
        printf("Checking downstream connection to stream %d, min: %7.2f, max: %7.2f\n", currentStreamPtr->streamId, currentStreamPtr->minElevation, currentStreamPtr->maxElevation);

    for (downstreamReachCount = 1; downstreamReachCount <= maxDownstreamReachCount; downstreamReachCount++) {

        if (debug)
            printf("Trying downstream reach %d, min: %7.2f, max: %7.2f\n", currentDownstreamPtr->streamId, currentDownstreamPtr->minElevation, currentDownstreamPtr->maxElevation);

        /* Sanity check - are we looping? i.e. is the possible downstream reach actually upstream from current stream and we now have come back to ourselves */
        if (currentDownstreamPtr->streamId == currentStreamPtr->streamId) {
            if (debug)
                printf("Whoa, we are looping, skipping adjacent stream %d\n", adjacentStreamPtr->streamId);
            return 0;
        }

        /* Check if possible downstream midpoint is less than current stream min by at least the threshold value. */
        streamMidPoint = (currentDownstreamPtr->maxElevation) - ((currentDownstreamPtr->maxElevation - currentDownstreamPtr->minElevation)/2.0);

        /* First check if the candidate stream is actually upstream within our confidence interval for elevation. */
        if ((streamMidPoint - currentStreamPtr->maxElevation) > elevationDiffThreshold) {
            if (debug)
                printf("Skipping uphill stream %d\n", currentDownstreamPtr->streamId);
            return 0;
        }

        if ((currentStreamPtr->minElevation - streamMidPoint) >= elevationDiffThreshold) {
            /* Connect the adjacent stream reach to the current stream reach */
            addStreamIntxn(currentStreamPtr, adjacentStreamPtr, nRow, nCol, index);
            currentStreamPtr->downstreamReachMinElevation = currentDownstreamPtr->minElevation;
            return 1;
        }

        /* Haven't found a downstream reach that is low enough yet, so try the next down stream reach from the current downstream reach. */
        if (currentDownstreamPtr->downstreamReach.streamId != -1)
            currentDownstreamPtr = findStreamEntry(currentDownstreamPtr->downstreamReach.streamId, &streamEntryPtr, &streamCnt);
        else {
            return 0;
            if (debug)
                printf("No more downstream reaches\n");
        }
    }

    /* No suitable downstream reach found after max number of passes. */
    if (debug)
        printf("No suitable downstream reach found\n");
    return 0;

}

void addStreamIntxn(streamEntry *currentStreamPtr, streamEntry *adjacentStreamPtr, int nRow, int nCol, int index) {

    if (debug)
        printf("Stream %d: adding intxn to stream %d, row: %d, col: %d\n", currentStreamPtr->streamId, adjacentStreamPtr->streamId, nRow, nCol);

    currentStreamPtr->downstreamReach.streamId = adjacentStreamPtr->streamId;
    currentStreamPtr->downstreamReach.row = nRow;
    currentStreamPtr->downstreamReach.col = nCol;

}

void addBasinDivision(streamEntry *streamPtr, int index, int* zone, int* hill, int* patch) {

    basinDivision* newBasinDivision;

    /* No basin divisions found for this stream entry, so add the division at the beginning of the array */
    if (streamPtr->basinDivisions == NULL) {
        streamPtr->basinDivisions = (basinDivision *) malloc(sizeof(basinDivision));
        streamPtr->basinDivisions->zoneId  = zone[index];
        streamPtr->basinDivisions->hillId  = hill[index];
        streamPtr->basinDivisions->patchId = patch[index];
        streamPtr->basinDivisionCnt = 1;
    } else {
        newBasinDivision = findBasinDivision(streamPtr, index, zone, hill, patch);
        /* Basin division wasn't found, so add it */
        if (newBasinDivision == NULL) {
            (streamPtr->basinDivisionCnt)++;
            streamPtr->basinDivisions = (basinDivision *) realloc(streamPtr->basinDivisions, sizeof(basinDivision) * (streamPtr->basinDivisionCnt));
            newBasinDivision = streamPtr->basinDivisions + ((streamPtr->basinDivisionCnt) - 1);
            newBasinDivision->zoneId = zone[index];
            newBasinDivision->hillId = hill[index];
            newBasinDivision->patchId = patch[index];
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

        if ((currentBasinDivision->zoneId == zoneId ) &&
                (currentBasinDivision->hillId == hillId) &&
                (currentBasinDivision->patchId == patchId)) {
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
            if (debug)
                printf("upstreamId %d already added to stream %d\n", upstreamId, downstreamPtr->streamId);
            return;
        }
    }

    /* We made it to this point, so the upstreamId was not found and we have to add it.*/
    if (downstreamPtr->upstreamCnt == 0) {
        if (debug)
            printf("Adding upstream id %d to stream %d\n", upstreamId, downstreamPtr->streamId);
        downstreamPtr->upstreamReaches = (streamIntxn *) malloc(sizeof(streamIntxn));
        (downstreamPtr->upstreamCnt)++;
        downstreamPtr->upstreamReaches->streamId = upstreamId;
        downstreamPtr->upstreamReaches->row = row;
        downstreamPtr->upstreamReaches->col = col;
    } else {
        if (debug)
            printf("Adding upstream id %d to stream %d\n", upstreamId, downstreamPtr->streamId);
        (downstreamPtr->upstreamCnt)++;
        downstreamPtr->upstreamReaches = (streamIntxn *) realloc(downstreamPtr->upstreamReaches, sizeof(streamIntxn) * (downstreamPtr->upstreamCnt));

        currentUpstreamIntxnPtr = (downstreamPtr->upstreamReaches) + ((downstreamPtr->upstreamCnt) - 1);
        currentUpstreamIntxnPtr->streamId = upstreamId;
        currentUpstreamIntxnPtr->row = row;
        currentUpstreamIntxnPtr->col = col;
    }
}


streamEntry* findHighestStream(streamEntry **streamEntryPtr, int *streamCnt) {

    int i;
    double maxElevation = -DBL_MAX;
    streamEntry *highestStreamPtr = NULL;
    streamEntry *currentStreamPtr = NULL;

    for(i=0; i < *streamCnt; i++) {
        currentStreamPtr = *streamEntryPtr + i;

        if (currentStreamPtr->printed == false) {
            if (currentStreamPtr->maxElevation > maxElevation) {
                maxElevation = currentStreamPtr->maxElevation;
                highestStreamPtr = currentStreamPtr;
            }
        }
    }

    return highestStreamPtr;
}
