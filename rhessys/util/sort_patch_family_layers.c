/*--------------------------------------------------------------*/
/*                                                              */ 
/*		sort_patch_family_layers								*/
/*                                                              */
/*  NAME                                                        */
/*		sort_patch_family_layers								*/
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/*  sort_patch_family_layers( struct patch_object *patch)		*/
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

void sort_patch_family_layers( struct patch_family_object *patch_family, int *rec)
{
	/*--------------------------------------------------------------*/
	/*  Local function declaration                                  */
	/*--------------------------------------------------------------*/
	int key_compare(const void *,  const void *);
	void	*alloc( 	size_t, char *, char *);
	/*--------------------------------------------------------------*/
	/*  Local variable definition.                                  */
	/*--------------------------------------------------------------*/
	int s, i, j,k;
	int list_bottom;
	int maxstemcID, maxleafcID;
	double cover_fraction;
	double maxstemc, maxleafc;
	struct canopy_strata_object *stratum;
	/*--------------------------------------------------------------*/
	/*	free current layer structure				*/
	/*--------------------------------------------------------------*/
	for ( i=0 ; i<patch_family[0].num_layers ; i++ ) {
		free(patch_family[0].layers[i].strata);
		patch_family[0].layers[i].count = 0;
	}
	/*--------------------------------------------------------------*/
	/*		Establish index of next free list entry.	*/
	/*--------------------------------------------------------------*/
	patch_family[0].num_layers = 0;
	list_bottom = 0;
	/*--------------------------------------------------------------*/
	/*	Determine the	unique height layers in the patch_family	*/
	/*--------------------------------------------------------------*/
	for( i=0; i<patch_family[0].num_canopy_strata ; i++ ){
		/*--------------------------------------------------------------*/
		/*		Check if this height alread exists.		*/
		/*--------------------------------------------------------------*/
		j = 0;
		while( (j<list_bottom) && (patch_family[0].canopy_strata[i][0].epv.height!=
			(patch_family[0].layers[j]).height) ){
			j++;
		}
		/*--------------------------------------------------------------*/
		/*		If we did not find this height in the list	*/
		/*		add it to the bottom of the list.		*/
		/*		otherwise just increment the count of layers	*/
		/*		at height "height".				*/
		/*--------------------------------------------------------------*/
		if ( j >= list_bottom ){
			(patch_family[0].layers[list_bottom]).height =
				patch_family[0].canopy_strata[i][0].epv.height;
			(patch_family[0].layers[list_bottom]).count = 1;
			list_bottom++;
		}
		else {
			(patch_family[0].layers[j]).count++;
		}
	}
	/*--------------------------------------------------------------*/
	/*	Define number of unique layers in this patch.		*/
	/*--------------------------------------------------------------*/
	patch_family[0].num_layers = list_bottom;
	/*--------------------------------------------------------------*/
	/*	Now sort the layer list into descending order.		*/
	/*--------------------------------------------------------------*/
	qsort(
		(void *) patch_family[0].layers,
		(size_t) patch_family[0].num_layers,
		sizeof(struct layer_object),
		key_compare);
	/*--------------------------------------------------------------*/
	/*	Now construct a list of pointers to strata at each	*/
	/*	height layer						*/
	/*--------------------------------------------------------------*/
	for ( i=0 ; i<patch_family[0].num_layers ; i++ ){
		/*--------------------------------------------------------------*/
		/*		Allocate the list for layer i			*/
		/*--------------------------------------------------------------*/
		patch_family[0].layers[i].strata = (long *)
			alloc(patch_family[0].layers[i].count*sizeof(long),
			"patch_family[0].layers[i].strata",
			"construct_patch_family");
		/*--------------------------------------------------------------*/
		/*		Reset the cover_fraction accumulator		*/
		/*--------------------------------------------------------------*/
		cover_fraction = 0.0;
		/*--------------------------------------------------------------*/
		/*	assign a bottom of layer				*/
		/*--------------------------------------------------------------*/
		if (i != patch_family[0].num_layers - 1)
			patch_family[0].layers[i].base = patch_family[0].layers[i+1].height;
		else
			patch_family[0].layers[i].base = 0.0;
		/*--------------------------------------------------------------*/
		/*		Find all strata with height matching layer i	*/
		/*--------------------------------------------------------------*/
		k = 0;
		for ( j=0 ; j<patch_family[0].num_canopy_strata; j++ ){
			/*--------------------------------------------------------------*/
			/*			check if this stratum has layer i height*/
			/*--------------------------------------------------------------*/
			if ( patch_family[0].canopy_strata[j][0].epv.height
				==(patch_family[0].layers[i]).height){
			/*--------------------------------------------------------------*/
			/*		Add the stratum index to the layer if it matches*/
			/*--------------------------------------------------------------*/
				patch_family[0].layers[i].strata[k] = j;
				k++;
				/*--------------------------------------------------------------*/
				/*		Keep a running total of the cover fraction in	*/
				/*		this layer to check that it adds to 1.0		*/
				/*--------------------------------------------------------------*/
				cover_fraction += patch_family[0].canopy_strata[j][0].cover_fraction;
			}
		}

	}
	return;
}
