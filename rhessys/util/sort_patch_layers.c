/*--------------------------------------------------------------*/
/*                                                              */ 
/*		sort_patch_layers									*/
/*                                                              */
/*  NAME                                                        */
/*		sort_patch_layers									*/
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/*  sort_patch_layers( struct patch_object *patch)					*/
/*                                                              */
/*  OPTIONS                                                     */
/*                                                              */
/*  DESCRIPTION                                                 */
/*                                                              */
/*	sorts canopy_stratum within a patch by height into	*/
/*	different layers					*/
/*								*/
/*  PROGRAMMER NOTES                                            */
/*                                                              */
/*                                                              */
/*                                                              */
/*--------------------------------------------------------------*/

#include <stdio.h>
#include "rhessys.h"

void sort_patch_layers( struct patch_object *patch)
{
	/*--------------------------------------------------------------*/
	/*  Local function declaration                                  */
	/*--------------------------------------------------------------*/
	int key_compare(void *,  void *);
	void	*alloc( 	size_t, char *, char *);
	/*--------------------------------------------------------------*/
	/*  Local variable definition.                                  */
	/*--------------------------------------------------------------*/
	int i, j,k;
	int list_bottom;
	double cover_fraction;
	/*--------------------------------------------------------------*/
	/*	free current layer structure				*/
	/*--------------------------------------------------------------*/
	for ( i=0 ; i<patch[0].num_layers ; i++ ) {
		free(patch[0].layers[i].strata);
		patch[0].layers[i].count = 0;
	}
	/*--------------------------------------------------------------*/
	/*		Establish index of next free list entry.	*/
	/*--------------------------------------------------------------*/

	patch[0].num_layers = 0;
	list_bottom = 0;
	/*--------------------------------------------------------------*/
	/*	Determine the	unique height layers in the patch	*/
	/*--------------------------------------------------------------*/
	for( i=0; i<patch[0].num_canopy_strata ; i++ ){
		/*--------------------------------------------------------------*/
		/*		Check if this height alread exists.		*/
		/*--------------------------------------------------------------*/
		j = 0;
		while( (j<list_bottom) && (patch[0].canopy_strata[i][0].epv.height!=
			(patch[0].layers[j]).height) ){
			j++;
		}
		/*--------------------------------------------------------------*/
		/*		If we did not find this height in the list	*/
		/*		add it to the bottom of the list.		*/
		/*		otherwise just increment the count of layers	*/
		/*		at height "height".				*/
		/*--------------------------------------------------------------*/
		if ( j >= list_bottom ){
			(patch[0].layers[list_bottom]).height =
				patch[0].canopy_strata[i][0].epv.height;
			(patch[0].layers[list_bottom]).count = 1;
			list_bottom++;
		}
		else {
			(patch[0].layers[j]).count++;
		}
	}
	/*--------------------------------------------------------------*/
	/*	Define number of unique layers in this patch.		*/
	/*--------------------------------------------------------------*/
	patch[0].num_layers = list_bottom;
	/*--------------------------------------------------------------*/
	/*	Now sort the layer list into descending order.		*/
	/*--------------------------------------------------------------*/
	qsort(
		(void *) patch[0].layers,
		(size_t) patch[0].num_layers,
		sizeof(struct layer_object),
		key_compare);
	/*--------------------------------------------------------------*/
	/*	Now construct a list of pointers to strata at each	*/
	/*	height layer						*/
	/*--------------------------------------------------------------*/
	for ( i=0 ; i<patch[0].num_layers ; i++ ){
		/*--------------------------------------------------------------*/
		/*		Allocate the list for layer i			*/
		/*--------------------------------------------------------------*/
		patch[0].layers[i].strata = (long *)
			alloc(patch[0].layers[i].count*sizeof(long),
			"patch[0].layers[i].strata",
			"construct_patch");
		/*--------------------------------------------------------------*/
		/*		Reset the cover_fraction accumulator		*/
		/*--------------------------------------------------------------*/
		cover_fraction = 0.0;
		/*--------------------------------------------------------------*/
		/*	assign a bottom of layer				*/
		/*--------------------------------------------------------------*/
		if (i != patch[0].num_layers - 1)
			patch[0].layers[i].base = patch[0].layers[i+1].height;
		else
			patch[0].layers[i].base = 0.0;
		/*--------------------------------------------------------------*/
		/*		Find all strata with height matching layer i	*/
		/*--------------------------------------------------------------*/
		k = 0;
		for ( j=0 ; j<patch[0].num_canopy_strata; j++ ){
			/*--------------------------------------------------------------*/
			/*			check if this stratum has layer i height*/
			/*--------------------------------------------------------------*/
			if ( patch[0].canopy_strata[j][0].epv.height
				==(patch[0].layers[i]).height){
			/*--------------------------------------------------------------*/
			/*		Add the stratum index to the layer if it matches*/
			/*--------------------------------------------------------------*/
				patch[0].layers[i].strata[k] = j;
				k++;
				/*--------------------------------------------------------------*/
				/*		Keep a running total of the cover fraction in	*/
				/*		this layer to check that it adds to 1.0		*/
				/*--------------------------------------------------------------*/
				cover_fraction += patch[0].canopy_strata[j][0].cover_fraction;
			}
		}
		/*--------------------------------------------------------------*/
		/*		Report a fatal error if the cover fraction for	*/
		/*		this layer does not add to 1.0			*/
		/*--------------------------------------------------------------*/
		if ( cover_fraction > 1.0 ){
			/*printf( "\nFATAL ERROR: in sort_patch_layers cover fraction of layer height %f greater than 1.0\n",
				patch[0].layers[i].height);
			printf("\n for patch %d, cover fraction %lf\n", patch[0].ID, cover_fraction);
			patch[0].layers[i].null_cover = 0.0;*/
		}
		else {
			patch[0].layers[i].null_cover = 1.0 - cover_fraction;
		}
	}
	return;
}
