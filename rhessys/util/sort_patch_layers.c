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

void sort_patch_layers( struct patch_object *patch, int *rec)
{
	/*--------------------------------------------------------------*/
	/*  Local function declaration                                  */
	/*--------------------------------------------------------------*/
	int key_compare(void *,  void *);
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
		/*		corrected this by adding a very small amount to height of stratum       */
		/*         with most stem (tree) or leaf (grass) - if the same (which is likely) */
		/* 		simply pick the last in the list */
		/*--------------------------------------------------------------*/
		if (( cover_fraction > 1.0 ) && ( patch[0].layers[i].height > ZERO) && rec < 2){
		//	printf( "\n WARNING in sort_patch_layers cover fraction of layer height %f greater than 1.0\n",
		//		patch[0].layers[i].height);
	
			maxstemcID = 0;
			maxleafcID = 0;
			maxstemc = 0;
			maxleafc = 0;
			for ( s = 0; s < patch[0].layers[i].count; s++) {
				if (patch[0].canopy_strata[patch[0].layers[i].strata[s]][0].defaults[0][0].epc.veg_type == TREE) {
				if (patch[0].canopy_strata[patch[0].layers[i].strata[s]][0].cs.dead_stemc >  maxstemc)
					maxstemcID = s;;
				} 
				else {
				if (patch[0].canopy_strata[patch[0].layers[i].strata[s]][0].cs.leafc >  maxleafc)
					maxleafcID = s;;
				}
				}

			if (maxstemc > 0) {
				stratum=patch[0].canopy_strata[patch[0].layers[i].strata[maxstemcID]];
				stratum->cs.dead_stemc += 0.0001; 
				stratum->cdf.added_carbon += 0.0001;
				stratum->epv.height = stratum->defaults[0][0].epc.height_to_stem_coef
                                * pow ( (stratum->cs.live_stemc + stratum->cs.dead_stemc)/(stratum->cs.stem_density), stratum->defaults[0][0].epc.height_to_stem_exp);
				}
			else {
				stratum=patch[0].canopy_strata[patch[0].layers[i].strata[maxleafcID]];
                                stratum->cs.leafc += 0.0001;                                                      
				stratum->cdf.added_carbon += 0.0001;
                                stratum->epv.height = stratum->defaults[0][0].epc.height_to_stem_coef                    
                                * pow ( (stratum->cs.leafc), stratum->defaults[0][0].epc.height_to_stem_exp);
				}
					
				
			patch[0].layers[i].null_cover = 0.0;
		//	printf("\n for patch %d, cover fraction > %lf  fixed by  changing height for %d strata", patch[0].ID, cover_fraction,
		//			stratum->ID);
		//	printf("\n carbon will not balance for this day");

			/* recursively call patch layers to fix this - should always work because we are changing the height */
			rec += 1;
			sort_patch_layers(patch, *rec);
		}
		else {
			patch[0].layers[i].null_cover = 1.0 - cover_fraction;
		}
	}
	return;
}
