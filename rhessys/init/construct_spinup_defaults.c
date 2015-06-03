/*--------------------------------------------------------------*/
/*                                                              */
/*      construct_spinup_defaults                               */
/*                                                              */
/*      construct_spinup_defaults.c - makes spinup default      */
/*                                    objects.                  */
/*                                                              */
/*      NAME                                                    */
/*      construct_spinup_defaults.c - makes spinup default      */
/*                                    objects.                  */
/*                                                              */
/*      SYNOPSIS                                                */
/*      struct spinup_default *construct_spinup_defaults(       */
/*                        num_default_files,                    */
/*                        default_files,                        */
/*                        grow_flag,                            */
/*                                                              */
/*      OPTIONS                                                 */
/*                                                              */
/*      DESCRIPTION                                             */
/*                                                              */
/*      PROGRAMMER NOTES                                        */
/*                                                              */
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "rhessys.h"
#include "params.h"

struct spinup_default *construct_spinup_defaults(
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
        struct  spinup_default    *default_object_list;
        param *paramPtr = NULL;
        
        /*--------------------------------------------------------------*/
        /*      Allocate an array of default objects.                   */
        /*--------------------------------------------------------------*/
        default_object_list   = (struct spinup_default *)
                alloc(num_default_files *
                sizeof(struct spinup_default),"default_object_list",
                "construct_spinup_defaults");
        
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
                default_object_list[i].ID = getIntParam(&paramCnt, &paramPtr, "spinup_default_ID", "%d", 1, 1); // new param name
                /*--------------------------------------------------------------*/
                /*              assign parameters in  default and read the      */
                /*   optional parameter specification                           */
                /*--------------------------------------------------------------*/
                default_object_list[i].tolerance = getDoubleParam(&paramCnt, &paramPtr, "tolerance", "%lf", 0.005, 1);
                default_object_list[i].max_years = getDoubleParam(&paramCnt, &paramPtr, "max_years", "%lf", 500, 1);
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
                    strcat(outFilename, "_spinup.params");
                } 
                else {
                    if (filename != NULL) {
                        strcat(outFilename, "_");
                        strcat(outFilename, filename);
                    }
                    strcat(outFilename, "spinup.params");
                }
        
            printParams(paramCnt, paramPtr, outFilename);
        } /*end for*/
        return(default_object_list);
} /*end construct_spinup_defaults*/
