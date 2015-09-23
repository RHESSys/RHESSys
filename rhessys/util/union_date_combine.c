
/*-----------------------------------------------------------------------------
 *  union_date_combine
 *
 *  NAME
 *  union_date_combine
 *
 *  INPUT
 *  struct date ** master_hourly_date
 *
 *  OUTPUT
 *  struct date * union_date
 *
 *  DESCRIPTION
 *  Create the sequnce of date which is the union of all date seqs from the master_hourly_date
 *
 *  LOCAL FUNCTION
 *  void *alloc(size_t, char *, char *);
 *  long julday(struct date);
 *  struct date * date_oper_union(struct date * dateseq1,
 *				  struct date * dateseq2);
 *-----------------------------------------------------------------------------*/
#include<stdio.h>
#include<stdlib.h>
#include "rhessys.h"
 
struct date * union_date_combine(struct world_object * world,
			struct date ** master_hourly_date){
  /*-----------------------------------------------------------------------------
   *  Local function definition
   *-----------------------------------------------------------------------------*/
  void *alloc(size_t,char *, char * );
  long julday(struct date);
  struct date *date_oper_union(struct date * dateseq1,
			      struct date * dateseq2);


  /*-----------------------------------------------------------------------------
   *  Local variable definition
   *-----------------------------------------------------------------------------*/
  int i;
  struct date *result;
  struct date *result_final;
  /*-----------------------------------------------------------------------------
   *  Initiliaze the local variables
   *-----------------------------------------------------------------------------*/
  //result = NULL;
  //result_final= NULL;
  /*-----------------------------------------------------------------------------
   *  1. 
   *-----------------------------------------------------------------------------*/

   result = date_oper_union(master_hourly_date[0],master_hourly_date[1]);
    if(world[0].num_base_stations>=2){
      for(i=1;i<world[0].num_base_stations;i++){
	result_final = date_oper_union(result,master_hourly_date[i]);
	free(result);
	result=result_final;
	result_final = NULL;
      }
    }
  for(i=0;i<world[0].num_base_stations;i++){
    free(master_hourly_date[i]);
  }
  free(master_hourly_date);
  return(result);


}
