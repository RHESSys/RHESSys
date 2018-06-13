/*--------------------------------------------------------------*/
/*                                                              */
/*      construct_beetle_defaults                               */
/*                                                              */
/*      construct_beetle_defaults.c - makes beetle default      */
/*                              model objects.                  */
/*                                                              */
/*      NAME                                                    */
/*      construct_beetle_defaults.c - makes beetle default      */
/*                                    objects.                  */
/*                                                              */
/*      SYNOPSIS                                                */
/*      struct beetle_default *construct_beetle_defaults(       */
/*                        num_default_files,                    */
/*                        default_files,                        */
/*                        beetlespread_flag,                    */
/*                                                              */
/*      OPTIONS                                                 */
/*                                                              */
/*      DESCRIPTION                                             */
/*      NR 2018-4-19                                            */
/*                                                              */
/*      PROGRAMMER NOTES                                        */
/*                                                              */
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "rhessys.h"
#include "params.h"

struct spinup_default *construct_beetle_defaults(
        int     num_default_files,
        char    **default_files,
        struct command_line_object *command_line)

{
        /*--------------------------------------------------------------*/
        /*      Local function definition.                              */
        /*--------------------------------------------------------------*/
        void    *alloc( size_t, char *, char *);

        /*--------------------------------------------------------------*/
        /*      Local variable definition.                              */
        /*--------------------------------------------------------------*/
        int     i;
        int strbufLen = 256;
        int filenameLen = 1024;
        int paramCnt = 0;
        char    strbuf[strbufLen];
        char    outFilename[filenameLen];
        double  ftmp, soil;
        FILE    *default_file;
        char    *newrecord;
        char    record[MAXSTR];
        struct  beetle_default    *default_object_list;
        param *paramPtr = NULL;

        /*--------------------------------------------------------------*/
        /*      Allocate an array of default objects.                   */
        /*--------------------------------------------------------------*/
        default_object_list   = (struct beetle_default *)
                alloc(num_default_files *
                sizeof(struct beetle_default),"default_object_list",
                "construct_beetle_defaults");


        /*--------------------------------------------------------------*/
        /*      Loop through the default files list.                    */
        /*--------------------------------------------------------------*/
        for (i=0 ; i<num_default_files; i++){
                /*--------------------------------------------------------------*/
                /*              Try to open the ith default file.               */
                /*--------------------------------------------------------------*/
                printf("\nReading %s\n", default_files[i]);
                paramCnt = 0;
                if (paramPtr != NULL)
                    free(paramPtr);

                paramPtr = readParamFile(&paramCnt, default_files[i]);

                /*--------------------------------------------------------------*/
                /*              read the ith default file into the ith object   */
                /*--------------------------------------------------------------*/
                default_object_list[i].ID = getIntParam(&paramCnt, &paramPtr, "beetle_default_ID", "%d", 1, 1); // new param name
                /*--------------------------------------------------------------*/
                /*              assign parameters in  default and read the      */
                /*   optional parameter specification                           */
                /*--------------------------------------------------------------*/
                default_object_list[i].attack_mortality = getDoubleParam(&paramCnt, &paramPtr, "attack_mortality", "%lf", 0.95, 1); // the default mortality is 95%
		printf("mortality: %d\n",default_object_list[i].attack_mortality);
                default_object_list[i].year_delay = getDoubleParam(&paramCnt, &paramPtr, "year_delay", "%lf", 5, 1); // the default delay year is 5
		printf("snag_year_delay: %d\n",default_object_list[i].year_delay);
                default_object_list[i].half_life = getDoubleParam(&paramCnt, &paramPtr, "half_life", "%lf", 10, 1); // the default delay year is 10
		printf("snag_half_life: %d\n",default_object_list[i].half_life);
                default_object_list[i].n_rows=getIntParam(&paramCnt, &paramPtr, "n_rows", "%d", -1, 1);
                printf("n_rows: %d\n",default_object_list[i].n_rows);
                default_object_list[i].n_cols=getIntParam(&paramCnt, &paramPtr, "n_cols", "%d", -1, 1);
                printf("n_cols: %d\n",default_object_list[i].n_cols);
                default_object_list[i].year_attack=getIntParam(&paramCnt, &paramPtr, "year_attack", "%d", 1991, 1);
                printf("year_attack: %d\n",default_object_list[i].year_attack);
                default_object_list[i].beetle_in_buffer =getIntParam(&paramCnt, &paramPtr, "beetle_in_buffer", "%d", 0, 1);
                printf("beetle_in_buffer: %d\n",default_object_list[i].beetle_in_buffer);
                default_object_list[i].leaf_year_delay = getDoubleParam(&paramCnt, &paramPtr, "leaf_year_delay", "%lf", 3, 1); // the default delay year is 3
		printf("leaf_year_delay: %d\n",default_object_list[i].leaf_year_delay);
                default_object_list[i].leaf_half_life = getDoubleParam(&paramCnt, &paramPtr, "leaf_half_life", "%lf", 2, 1); // the default delay year is 2
		printf("leaf_half_life: %d\n",default_object_list[i].leaf_half_life);

                /*--------------------------------------------------------------*/
                /*              Close the ith default file.                     */
                /*--------------------------------------------------------------*/

                memset(strbuf, '\0', strbufLen);
                strcpy(strbuf, default_files[i]);
                char *s = strbuf;
                char *y = NULL;
                char *token = NULL;
                char filename[256];

                // Store filename portion of path in 't'
                while ((token = strtok(s, "/")) != NULL) {
                    // Save the latest component of the filename
                    strcpy(filename, token);
                    s = NULL;
                }

                // Remove the file extension, if one exists
                memset(strbuf, '\0', strbufLen);
                strcpy(strbuf, filename);
                free(s);
                s = strbuf;
                token = strtok(s, ".");
                if (token != NULL) {
                    strcpy(filename, token);
                }

                memset(outFilename, '\0', filenameLen);

                // Concatenate the output prefix with the filename of the input .def file
                // and "_stratum.params"
                if (command_line[0].output_prefix != NULL) {
                    strcat(outFilename, command_line[0].output_prefix);
                    if (filename != NULL) {
                        strcat(outFilename, "_");
                        strcat(outFilename, filename);
                    }
                    strcat(outFilename, "_beetle.params");
                }
                else {
                    if (filename != NULL) {
                        strcat(outFilename, "_");
                        strcat(outFilename, filename);
                    }
                    strcat(outFilename, "beetle.params");
                }

            printParams(paramCnt, paramPtr, outFilename);
        } /*end for*/
        return(default_object_list);
} /*end construct_beetle_defaults*/



