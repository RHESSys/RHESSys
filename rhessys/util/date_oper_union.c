
/*-----------------------------------------------------------------------------
 *  date_oper_union
 *
 *  NAME
 *  date_oper_union
 *
 *  INPUT
 *  struct date * dateseq1
 *  struct date * dateseq2
 *
 *  OUTPUT
 *  struct date * dateunion
 *-----------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "rhessys.h"
struct date * date_oper_union(struct date * dateseq1,
			      struct date * dateseq2){

  /*-----------------------------------------------------------------------------
   *  Local function definition
   *-----------------------------------------------------------------------------*/
  long julday(struct date);
  void *alloc(size_t,char *, char *);
  

  /*-----------------------------------------------------------------------------
   *  Local variable definition
   *-----------------------------------------------------------------------------*/
  int i,j,k;
  int num_interse,num_d1,num_d2,num_union;
  struct date *result;


  /*-----------------------------------------------------------------------------
   *  Initialize the local variables
   *-----------------------------------------------------------------------------*/
  num_interse = 0;
  num_d1 = 0;
  num_d2 = 0;
  num_union=0;
  i=0;

  /*-----------------------------------------------------------------------------
   *  Number of days for the seq1
   *-----------------------------------------------------------------------------*/
  while(dateseq1[i].year!=0){
    i++;
  }
  num_d1=i;
   /*-----------------------------------------------------------------------------
   *  Number of days for the seq2
   *-----------------------------------------------------------------------------*/
  i=0;
  while(dateseq2[i].year!=0){
    i++;
  }
  num_d2=i;
  /*-----------------------------------------------------------------------------
   *  Number of days for the result of union
   *-----------------------------------------------------------------------------*/
  for(i=0;i<num_d1;i++){
    for(j=0;j<num_d2;j++){
	if(julday(dateseq1[i])==julday(dateseq2[j])){
	  num_interse++;
	  break;
	}
    }
  }
  num_union = num_d1 + num_d2 - num_interse;


  /*-----------------------------------------------------------------------------
   *  Construct new seq to store the union
   *-----------------------------------------------------------------------------*/
  result = (struct date *)alloc((num_union+1) * sizeof(struct date),"date_union",
								"date_oper_union"); 
  /*Initialize*/ 
  if(julday(dateseq1[0])<=julday(dateseq2[0])){
    result[0] = dateseq1[0];
  }
  else{
    result[0] = dateseq2[0];
  }
   /*Loop through two seqs*/
  i=0;
  j=0;
  k=0;
  for(k=0;k<num_union;k++){
    if(i==num_d1){
      result[k] = dateseq2[j];
      j++;
    }
    else if(j==num_d2){
      result[k] = dateseq1[i];
      i++;
    }
    else{
      if(julday(dateseq1[i])<julday(dateseq2[j])){
	result[k]=dateseq1[i];
	i++;
      }
      else if(julday(dateseq1[i])>julday(dateseq2[j])){
	result[k]=dateseq2[j];
	j++;
      }
      else{
	result[k]=dateseq1[i];
	i++;
	j++;
      }
    }
  }
  result[num_union].year = 0;

  /*for(k=0;k<num_union;k++){
      printf("year = %d, month = %d, day = %d\n",result[k].year,
						result[k].month,
						result[k].day);
  }*/

  return(result);



}
