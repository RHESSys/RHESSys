/*------------------------------------------------------------------------------------------------------------------------------*/
/* 																*/
/*					construct_fire_grid								*/
/*																*/
/*	construct_fire_grid.c - creates a raster grid of patches to be passed 
/*	to WMFire 													*/
/*																*/
/*	NAME														*/
/*	construct_fire_grid.c - creates a raster grid							*/
/*																*/
/*	SYNOPSIS													*/
/*	struct fire_grid *construct_fire_grid(					*/
/*					base_station_file_name,						*/
/*					start_date,									*/
/*					duration									*/
/*					column_index);								*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*	Allocates memory for a rectangular raster grid.  Navigates the hierarchical 	*/
/* 	spatial structure in RHESSys and assigns patches to the grid cells with with 	*/
/* 	they overlap.  Calculates the area of overlap between the patch and the 	*/
/* 	associated grid cell(s).
/*																*/
/*																*/
/*	PROGRAMMER NOTES											*/
/* 	M Kennedy June 27, 2012										*/
/*	Assumes patches are square.  No guarantee of performance otherwise		*/
/* Updated May 16, 2013 to allow for a raster grid of patch id's be used to create	*/
/* the fire grid. gives a better approximation of irregularly-shaped patches		*/
/*-----------------------------------------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rhessys.h"
// local function definition:
double calc_patch_area_in_grid(double curMinX,double curMinY,double curMaxX,double curMaxY,double cellMaxX,
							double cellMaxY,double cellMinX,double cellMinY,double cell_res);

struct fire_object **construct_patch_fire_grid (struct world_object *world, struct command_line_object *command_line,struct fire_default def)

{
	/*--------------------------------------------------------------*/
	/*	Local function definition.									*/
	/*--------------------------------------------------------------*/

	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	struct patch_fire_object **fire_grid;
	struct patch_object *patch;
	int  b,h,p, z, i, j, k;
	double maxx, maxy, minx, miny, tmp,halfSideLength,curMinX,curMinY,curMaxX,curMaxY, cell_res;
	maxx=-10000; // depends on the origin for the coordinates, this was set for HJA centered at 0,0
	minx=-10000;
	maxy=-10000;
	miny=-10000;
	int grid_dimX,grid_dimY;
// first navigate the hierarchical spatial structure to access each patch.
// find the min and max X and Y coordinates
	// for debugging, write out all of the relevant patch information

	cell_res =  command_line[0].fire_grid_res; /* grid resolution */
//	printf("cell res: %lf\n",cell_res);

	if(def.n_rows==-1) // then we construct the grid
	{
		for (b=0; b< world[0].num_basin_files; ++b) {
		 for (h=0; h< world[0].basins[b][0].num_hillslopes; ++h) {
		   for (z=0; z< world[0].basins[b][0].hillslopes[h][0].num_zones; ++z) {
				tmp = world[0].basins[b][0].hillslopes[h][0].zones[z][0].aspect; /* aspect */
		    for (p=0; p< world[0].basins[b][0].hillslopes[h][0].zones[z][0].num_patches; ++p) {
				patch = world[0].basins[b][0].hillslopes[h][0].zones[z][0].patches[p];
		//	    fprintf(patchout,"%d\t%lf\t%lf\t%lf\n",patch[0].ID,patch[0].x,patch[0].y,patch[0].area);
			    
				halfSideLength=sqrt(patch[0].area)/2; // assuming a square patch, this is half the length of each side
				curMinX=patch[0].x-halfSideLength; // the min x value for this patch is the center x value minus half the side length
				curMaxX=patch[0].x+halfSideLength;// the max x value for this patch is the center x value plus half the side length
				curMinY=patch[0].y-halfSideLength;// the min y value for this patch is the center y value minus half the side length
				curMaxY=patch[0].y+halfSideLength;// the max y value for this patch is the center y value plus half the side length
				if(minx>-10000){ // if the minimum x value has already been assigned
					if(curMinX<minx) // compare the global min to the current min
						minx=curMinX;
				}
				else  // otherwise initialize the minimum with the current
					minx=curMinX;
				if(maxx>-10000){
					if(curMaxX>maxx)
						maxx=curMaxX;
				}
				else 
					maxx=curMaxX;
				if(miny>-10000){ // if the minimum y value has already been assigned
					if(curMinY<miny) // compare the global min to the current min
						miny=curMinY;
				}
				else  // otherwise initialize the minimum with the current
					miny=curMinY;
				if(maxy>-10000){// if the maximum y value has already been assigned
					if(curMaxY>maxy)// compare the global max to the current max
						maxy=curMaxY;
				}
				else // otherwise initialize the maximum with the current
					maxy=curMaxY;	 
		} /* end patches */
		}
		}
		}
	//	fclose(patchout);
//		printf("exited first navigation of patch structure in construct_fire_grid: maxX %lf  maxY %lf  minX %lf  minY %lf cell_res %lf\n",maxx,maxy,minx,miny,command_line[0].fire_grid_res);
		int minXpix,maxXpix,minYpix,maxYpix;
		double cellMinX, cellMaxX,cellMinY,cellMaxY,areaPatchInGrid;
		/*now we calculate the grid dimensions and allocate memory for the grid*/
	// add a small buffer on each edge for rounding issues
		minx=minx-cell_res/100;
		miny=miny-cell_res/100;
		maxx=maxx+cell_res/100;
		maxy=maxy+cell_res/100;
		
		
		
		grid_dimX=ceil((maxx-minx)/cell_res);
		grid_dimY=ceil((maxy-miny)/cell_res);
//		printf("grid dimensions: x %d  y %d minX %lf minY %lf maxX %lf maxY %lf\n",grid_dimX,grid_dimY,minx,miny,maxx,maxy);
	  // now we have to reorient the max and min X and Y for the new grid.  If we assume the orientation of the grid
	  // st the lower left is as minX and minY, then we just need to add to maxX and maxY
		maxx=minx+grid_dimX*cell_res;
		maxy=miny+grid_dimY*cell_res;
		fire_grid=(struct patch_fire_object **) malloc(grid_dimY*sizeof(struct patch_fire_object *)); // first allocate the rows
		for(i=0;i<grid_dimY;i++) // for each row, allocate the columns
			fire_grid[i]=(struct patch_fire_object *) malloc(grid_dimX*sizeof(struct patch_fire_object ));
	//	printf("allocate the fire grid\n");
			
		// then initialize values: e.g., 0's 
         #pragma omp parallel for                                                //160628LML
         for(int i=0;i<grid_dimX;i++){
            for(int j=0;j<grid_dimY;j++){
				fire_grid[j][i].occupied_area=0;
				fire_grid[j][i].num_patches=0;
				fire_grid[j][i].tmp_patch=0;	
				fire_grid[j][i].elev=0;
			}
		}
		world[0].num_fire_grid_row = grid_dimY;
		world[0].num_fire_grid_col = grid_dimX;
//		printf("after initializing the values: rows, columns: %d\t%d\n",world[0].num_fire_grid_row,world[0].num_fire_grid_col);
	// now we have to tally the number of patches that overlap each grid cell for allocation
	// of the patch and area arrays, then one more navigation to calculate the areas and
	// assign the patches to each grid cell
		for (b=0; b< world[0].num_basin_files; ++b) {
         #pragma omp parallel for                                                //160628LML
         for (int h=0; h< world[0].basins[b][0].num_hillslopes; ++h) {
           for (int z=0; z< world[0].basins[b][0].hillslopes[h][0].num_zones; ++z) {
                double tmp = world[0].basins[b][0].hillslopes[h][0].zones[z][0].aspect; /* aspect */
            for (int p=0; p< world[0].basins[b][0].hillslopes[h][0].zones[z][0].num_patches; ++p) {
                struct patch_object *patch = world[0].basins[b][0].hillslopes[h][0].zones[z][0].patches[p];
                double halfSideLength=sqrt(patch[0].area)/2; // assuming a square patch, this is half the length of each side
                double curMinX=patch[0].x-halfSideLength; // the min x value for this patch is the center x value minus half the side length
                double curMaxX=patch[0].x+halfSideLength;// the max x value for this patch is the center x value plus half the side length
                double curMinY=patch[0].y-halfSideLength;// the min y value for this patch is the center y value minus half the side length
                double curMaxY=patch[0].y+halfSideLength;// the max y value for this patch is the center y value plus half the side length

	      // Now translate the x-coordinate to grid indices--
                double minXpix=floor((curMinX-minx)/cell_res); // the left-most column that overlaps the current pixel
                double maxXpix=ceil((curMaxX-minx)/cell_res); // the right-most column that overlaps the current pixel
                double maxYpix=grid_dimY-floor((curMinY-miny)/cell_res);// the top-most row that overlaps the current pixel, reoriented so 0,0 is at the top
                double minYpix=grid_dimY-ceil((curMaxY-miny)/cell_res);// the bottom-most row that overlaps the current pixel, reoriented so 0,0 is at the top
	// check if the patch occupies just 1 grid cell
				if(minXpix==maxXpix) // the width of the patch is within a single column of pixels
					maxXpix=maxXpix+1; // so the i,j loop will work--it should just loop to the current pixel
				if(minYpix==maxYpix) // the width of the patch is within a single column of pixels
					maxYpix=maxYpix+1; // so the i,j loop will work--it should just loop to the current pixel
	    // so now we loop through each pixel pair starting with minX and minY, and ending with < maxX and maxY (In C)
	    // in the case that the patch overlaps with >1 pixel in both directions
                for(int i =minXpix; i<maxXpix; i++){
                    for(int j =minYpix; j<maxYpix; j++) {
                        #pragma omp critical
						fire_grid[j][i].num_patches=fire_grid[j][i].num_patches+1; // tally the number of patches that occupy each grid cell
                    }
				}
		} /* end patches */
		}
		}
		}	

	//	printf("after tallying the number of patches per grid cell\n");
		// allocate the arrays for each grid cell
        #pragma omp parallel for                                                 //160628LML
        for(int i=0;i<grid_dimX;i++){
            for(int j=0;j<grid_dimY;j++){
	//printf("number of patches: col %d row %d num patches %d\n",i,j,fire_grid[j][i].num_patches); 			
				fire_grid[j][i].patches=(struct patch_object **) malloc(fire_grid[j][i].num_patches*sizeof(struct patch_object *)); // looks like we're going to need to navigate the patch hierarchy three times.  the second time is simply to tally the number of patches in each grid
				fire_grid[j][i].prop_patch_in_grid=(double *) malloc(fire_grid[j][i].num_patches*sizeof(double)); // looks like we're going to need to navigate the patch hierarchy three times.  the second time is simply to tally the number of patches in each grid
				fire_grid[j][i].prop_grid_in_patch=(double *) malloc(fire_grid[j][i].num_patches*sizeof(double)); // looks like we're going to need to navigate the patch hierarchy three times.  the second time is simply to tally the number of patches in each grid
                for(int k=0;k<fire_grid[j][i].num_patches;k++){
					fire_grid[j][i].prop_grid_in_patch[k]=0;
					fire_grid[j][i].prop_patch_in_grid[k]=0;
				}
			}			
		}			

	//	printf("after allocating the arrays for each grid cell\n");
		// now we can calculate areas with one final navigation
		for (b=0; b< world[0].num_basin_files; ++b) {
		 for (h=0; h< world[0].basins[b][0].num_hillslopes; ++h) {
		   for (z=0; z< world[0].basins[b][0].hillslopes[h][0].num_zones; ++z) {
				tmp = world[0].basins[b][0].hillslopes[h][0].zones[z][0].aspect; /* aspect */
		    for (p=0; p< world[0].basins[b][0].hillslopes[h][0].zones[z][0].num_patches; ++p) {
				patch = world[0].basins[b][0].hillslopes[h][0].zones[z][0].patches[p];
			    
				halfSideLength=sqrt(patch[0].area)/2; // assuming a square patch, this is half the length of each side
				curMinX=patch[0].x-halfSideLength; // the min x value for this patch is the center x value minus half the side length
				curMaxX=patch[0].x+halfSideLength;// the max x value for this patch is the center x value plus half the side length
				curMinY=patch[0].y-halfSideLength;// the min y value for this patch is the center y value minus half the side length
				curMaxY=patch[0].y+halfSideLength;// the max y value for this patch is the center y value plus half the side length

	      // Now translate the x-coordinate to grid indices--
				minXpix=floor((curMinX-minx)/cell_res); // the left-most column that overlaps the current pixel
				maxXpix=ceil((curMaxX-minx)/cell_res); // the right-most column that overlaps the current pixel
				maxYpix=grid_dimY-floor((curMinY-miny)/cell_res); // to orient the grid st 0,0 is the top left corner, this gives the bottom row 
				minYpix=grid_dimY-ceil((curMaxY-miny)/cell_res);// to orient the grid st 0,0 is the top left corner, this gives the top row
	// check if the patch occupies just 1 grid cell
				if(minXpix==maxXpix) // the width of the patch is within a single column of pixels
					maxXpix=maxXpix+1; // so the i,j loop will work--it should just loop to the current pixel
				if(minYpix==maxYpix)
					maxYpix=maxYpix+1;
	//			printf("calculating overlap. minXpix %d minYpix %d maxXpix %d max Ypix %d\n",minXpix,minYpix,maxXpix,maxYpix);
				if(minXpix<0||minYpix<0)
	//				printf("out of bounds! curMinX %lf curMinY %lf minXpix %d minYpix %d grid_dimX %d grid_dimY %d\n",
																//curMinX,curMinY,minXpix,minYpix,grid_dimX,grid_dimY);


	   // so now we loop through each pixel pair starting with minX and minY, and ending with < maxX and maxY (In C)
	    // in the case that the patch overlaps with >1 pixel in both directions
                #pragma omp parallel for                                         //160628LML
                for(int i =minXpix; i<maxXpix; i++) {
                    for(int j =minYpix; j<maxYpix; j++){
						fire_grid[j][i].patches[fire_grid[j][i].tmp_patch]=patch; // this is the current patch in the loop
                        double cellMinX=i*cell_res+minx;
                        double cellMaxY=maxy-j*cell_res;
                        double cellMaxX=(i+1)*cell_res+minx ;
                        double cellMinY=maxy-(j+1)*cell_res;
                        double areaPatchInGrid=calc_patch_area_in_grid(curMinX,curMinY,curMaxX,curMaxY,cellMaxX,cellMaxY,cellMinX,cellMinY,cell_res);
	//					if(areaPatchInGrid<0)
	//						printf("error, areaPatchInGrid <0\n");
						fire_grid[j][i].occupied_area=fire_grid[j][i].occupied_area+areaPatchInGrid;
		// calculate the total area of the current patch that overlaps with the current grid cell
						fire_grid[j][i].prop_grid_in_patch[fire_grid[j][i].tmp_patch]=areaPatchInGrid/patch[0].area; // the proportion of patch area occupied by this grid					
						fire_grid[j][i].prop_patch_in_grid[fire_grid[j][i].tmp_patch]=areaPatchInGrid; // the grid area occupied by this patch, to be calculated as a proportion once 
																					//	the total occupied area of the grid is found
						fire_grid[j][i].tmp_patch++;
					}				
				}	 
		} /* end patches */
		}
		}
		}	
	//	printf("after calculating area of overlap between patches and grid cells\n");
	/* finally, navigate the grid cells one more time to calculate the proportions of each patch in each
		grid cell, given the total area occupied in each grid cell calculated above. */
        #pragma omp parallel for                                                 //160628LML
        for(int i=0;i<grid_dimX;i++){
            for(int j=0;j<grid_dimY;j++){
				if(fire_grid[j][i].occupied_area>0){
                    for(int k=0;k<fire_grid[j][i].num_patches;k++)
						fire_grid[j][i].prop_patch_in_grid[k]=fire_grid[j][i].prop_patch_in_grid[k]/fire_grid[j][i].occupied_area;
				}
			}
		}
	}// end if there is no raster patch file
	
	else // then we just read in the raster structure
	{
		// allocate the fire grid
		printf("reading patch raster structure\n");
		
		grid_dimX=def.n_cols;
		grid_dimY=def.n_rows;

		
		fire_grid=(struct patch_fire_object **) malloc(grid_dimY*sizeof(struct patch_fire_object *)); // first allocate the rows
        #pragma omp parallel for                                                 //160628LML
        for(int i=0;i<grid_dimY;i++) // for each row, allocate the columns
			fire_grid[i]=(struct patch_fire_object *) malloc(grid_dimX*sizeof(struct patch_fire_object ));
		printf("allocate the fire grid\n");
			
		// then initialize values: e.g., 0's 
        #pragma omp parallel for                                                 //160628LML
         for(int i=0;i<grid_dimX;i++){
            for(int j=0;j<grid_dimY;j++){
				fire_grid[j][i].occupied_area=0;
				fire_grid[j][i].num_patches=0;
				fire_grid[j][i].tmp_patch=0;					
//				patch_grid[j][i]=-1;
			}
		}
		world[0].num_fire_grid_row = grid_dimY;
		world[0].num_fire_grid_col = grid_dimX;
		
		printf("Rows: %d Cols: %d\n",world[0].num_fire_grid_row,world[0].num_fire_grid_col);

		int tmpPatchID;
		FILE *patchesIn;
		patchesIn=fopen("../auxdata/patchGrid.txt","r");
		// for now do away with the header
		for(i=0; i<grid_dimY;i++){
			for(j=0;j<grid_dimX;j++){
				tmpPatchID=-9999;
				fscanf(patchesIn,"%d\t",&tmpPatchID);
//				printf("Current patch id: %d, X: %d  Y: %d\n",tmpPatchID,i,j);
				if(tmpPatchID>=0){ // then find the corresponding patch and allocate it--only one patch per grid cell!
//					printf("numPatches 0: %d\n",fire_grid[j][i].num_patches);	
			//		printf("Current patch id: %d, X: %d  Y: %d\n",tmpPatchID,j,i);
					fire_grid[i][j].num_patches=1;
//					printf("numPatches 1: %d\n",fire_grid[j][i].num_patches);
					fire_grid[i][j].patches=(struct patch_object **) malloc(fire_grid[i][j].num_patches*sizeof(struct patch_object *));
					fire_grid[i][j].prop_patch_in_grid=(double *) malloc(fire_grid[i][j].num_patches*sizeof(double)); 
					fire_grid[i][j].prop_grid_in_patch=(double *) malloc(fire_grid[i][j].num_patches*sizeof(double)); 
//					printf("allocated patch array\n");
					for (b=0; b< world[0].num_basin_files; ++b) {
						for (h=0; h< world[0].basins[b][0].num_hillslopes; ++h) {
							for (z=0; z< world[0].basins[b][0].hillslopes[h][0].num_zones; ++z) {
								tmp = world[0].basins[b][0].hillslopes[h][0].zones[z][0].aspect; /* aspect */
								for (p=0; p< world[0].basins[b][0].hillslopes[h][0].zones[z][0].num_patches; ++p) {
									patch = world[0].basins[b][0].hillslopes[h][0].zones[z][0].patches[p];
									if(patch[0].ID==tmpPatchID)
									{
										//printf("now filling in array--found a match!\n");
										fire_grid[i][j].patches[0]=patch;
										//printf("patch1\n");
										fire_grid[i][j].occupied_area=cell_res*cell_res;
										//printf("patch2: area, cell res %lf, %lf\n",patch[0].area,cell_res);
										fire_grid[i][j].prop_grid_in_patch[0]=(cell_res*cell_res)/patch[0].area; // the proportion of this patch in this cell
										//printf("patch3\n");
										fire_grid[i][j].prop_patch_in_grid[0]=1;// the whole cell is occupied this patch
										//printf("array filled\n");
									}										
										
								}
							}
						}
					}
				}					

			}
		}
		fclose(patchesIn);
		printf("assigning dem\n");
		FILE *demIn;
		demIn=fopen("../auxdata/DemGrid.txt","r");
		// for now do away with the header, so this file has no header
		for(i=0; i<grid_dimY;i++){
			for(j=0;j<grid_dimX;j++){				
				fscanf(demIn,"%lf\t",&fire_grid[i][j].elev);
			}
		}
		fclose(demIn);
		printf("done assigning dem\n");
	}
	// for debugging, write out the fire grid and patches
/*	FILE *gridout;

	gridout=fopen("FireGridPatchCheckOccupiedArea.txt","w");
	for(i=0;i<grid_dimY;i++){
		for(j=0;j<grid_dimX;j++){
			fprintf(gridout,"%lf\t",fire_grid[i][j].occupied_area);
		}
		fprintf(gridout,"\n");
	}
	fclose(gridout);

	gridout=fopen("FireGridPatchCheckPatches.txt","w");
	for(i=0;i<grid_dimY;i++){
		for(j=0;j<grid_dimX;j++){
			if(fire_grid[i][j].occupied_area>0){
			for(k=0;k<fire_grid[i][j].num_patches;k++)
			{	
				patch = fire_grid[i][j].patches[k];
				fprintf(gridout,"%d",patch[0].ID);
			}
			fprintf(gridout,"\t");
		}
		}
		fprintf(gridout,"\n");
	}
	fclose(gridout);
	
	gridout=fopen("FireGridCheckPatchesAndAreas.txt","w");
	fprintf(gridout,"IDY\tIDX\tID\tPropPatchInGrid\tPropGridInPatch\tOccupiedArea\n");
	for(i=0;i<grid_dimY;i++){
		for(j=0;j<grid_dimX;j++){
			if(fire_grid[i][j].occupied_area>0){
			for(k=0;k<fire_grid[i][j].num_patches;k++)
			{		    	
				patch = fire_grid[i][j].patches[k];
				fprintf(gridout,"%d\t%d\t%d\t%lf\t%lf\t%lf\n",i,j,patch[0].ID,fire_grid[i][j].prop_patch_in_grid[k],fire_grid[i][j].prop_grid_in_patch[k],fire_grid[i][j].occupied_area);
			}
		}
		else
			fprintf(gridout,"%d\t%d\t-1\t-1\t-1\t-1\n",i,j);
		}
	}
	fclose(gridout);
	
	gridout=fopen("RhessyDEMOut.txt","w");
	for(i=0;i<grid_dimY;i++){
		for(j=0;j<grid_dimX;j++){
				fprintf(gridout,"%lf\t",fire_grid[i][j].elev);
		}
		fprintf(gridout,"\n");
	}
	fclose(gridout);*/

	/* done allocating fire grid, return to RHESSys*/
	return(fire_grid);	
}

/*------------------------------------------------------------------------------------------------------------------------------*/
/* 																*/
/*					calc_patch_area_in_grid							*/
/*																*/
/*	construct_fire_grid.c - creates a raster grid of patches to be passed 
/*	to WMFire 													*/
/*																*/
/*	NAME														*/
/*	construct_fire_grid.c - creates a raster grid							*/
/*																*/
/*	SYNOPSIS													*/
/*	double calc_patch_area_in_grid(					*/
/*					double curMinX,double curMinY,double curMaxX,double curMaxY,double cellMaxX,
							double cellMaxY,double cellMinX,double cellMinY,double cell_res);								*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*	Calculates the area of overlap between the patch and the 	*/
/* 	associated grid cell.
/*																*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*	Assumes patches are square.  No guarantee of performance otherwise		*/
/*-----------------------------------------------------------------------------------------------------------------------------*/
double calc_patch_area_in_grid(double curMinX,double curMinY,double curMaxX,double curMaxY,double cellMaxX,
							double cellMaxY,double cellMinX,double cellMinY,double cell_res)
{
	double areaPatchInGrid;
	/* what follows is an ugly set of if/else statements that determine the orientation of the 
	square patch to the grid cell so the area of overlap can be calculated*/
	if(curMinX<=cellMinX&curMaxX>=cellMaxX&curMinY<=cellMinY&curMaxY>=cellMaxY) // then 100% of the grid is occupied by this patch, complete overlap
		areaPatchInGrid=cell_res*cell_res; // then the area is simply the area of the pixel  
	else if(curMinX<=cellMinX&curMaxX>=cellMaxX) {// we span the entire horizonal length of the pixel, but not simultaneously the vertical length
		if(curMinY<=cellMinY) // enter from the bottom
			areaPatchInGrid=(curMaxY-cellMinY)*cell_res;
		else if(curMaxY>=cellMaxY)// enter from the top
			areaPatchInGrid=(cellMaxY-curMinY)*cell_res;
		else // enter in the middle of the cell in the y-direction
			areaPatchInGrid=(curMaxY-curMinY)*cell_res;
	}
	else if(curMinX<=cellMinX){ // enter in the left side of the pixel
		if(curMinY<=cellMinY&curMaxY>=cellMaxY) // span the entire vertical length of the left side of the pixel
			areaPatchInGrid=cell_res*(curMaxX-cellMinX);
		else if(curMinY<=cellMinY) // enter in the lower left corner
			areaPatchInGrid=(curMaxY-cellMinY)*(curMaxX-cellMinX);
		else if(curMaxY>=cellMaxY)// enter in the upper left corner
			areaPatchInGrid=(cellMaxY-curMinY)*(curMaxX-cellMinX);
		else // enter in the middle of the left side in the y-direction
			areaPatchInGrid=(curMaxY-curMinY)*(curMaxX-cellMinX);
	}
	else if(curMaxX>=cellMaxX){// enter in the right side of the pixel
		if(curMinY<=cellMinY&curMaxY>=cellMaxY) // span the entire vertical length of the right side of the pixel
			areaPatchInGrid=cell_res*(cellMaxX-curMinX);
		else if(curMinY<=cellMinY) // enter in the lower right corner
			areaPatchInGrid=(curMaxY-cellMinY)*(cellMaxX-curMinX);
		else if(curMaxY>=cellMaxY)// enter in the upper right corner
			areaPatchInGrid=(cellMaxY-curMinY)*(cellMaxX-curMinX);
		else // enter in the middel of the right side in the y-direction
			areaPatchInGrid=(curMaxY-curMinY)*(cellMaxX-curMinX);
	}      
	else{ // the patch falls within the pixel borders in the x-direction
		if(curMinY<=cellMinY&curMaxY>=cellMaxY) // spans the entire vertical length of the cell, probably not possible assuming square patches...
			areaPatchInGrid=cell_res*(curMaxX-curMinX);
		else if(curMinY<=cellMinY) // this means we've entered this cell in the bottom middle
			areaPatchInGrid=(curMaxY-cellMinY)*(curMaxX-curMinX);
		else if(curMaxY>=cellMaxY)// we entered in the upper middle
			areaPatchInGrid=(cellMaxY-curMinY)*(curMaxX-curMinX);
		else // the patch falls within the pixel border in both the x and the y-directions
			areaPatchInGrid=(curMaxY-curMinY)*(curMaxX-curMinX);
	}
//	printf("Area patch in grid: %lf\t",areaPatchInGrid);
	return areaPatchInGrid;
}

/*-------------------------------------------------------------------------------------------------------*/
/* construct fire grid--constructs a second grid with only the		*/
/* information necessary to be passed to WMFire				*/
/* contained in the fire_object								*/
/*------------------------------------------------------------------------------------------------------*/
struct fire_object **construct_fire_grid(struct world_object *world)
{
	struct fire_object **fire_grid;
	int i,j;
	fire_grid=(struct fire_object **) malloc(world[0].num_fire_grid_row*sizeof(struct fire_object *)); // first allocate the rows
    #pragma omp parallel for                                                     //160628LML
    for(int i=0;i<world[0].num_fire_grid_row;i++) // for each row, allocate the columns
		fire_grid[i]=(struct fire_object *) malloc(world[0].num_fire_grid_col*sizeof(struct fire_object ));
		
	// then initialize values: e.g., 0's 
     #pragma omp parallel for                                                    //160628LML
     for(int i=0;i<world[0].num_fire_grid_row;i++){
        for(int j=0;j<world[0].num_fire_grid_col;j++){
			fire_grid[i][j].burn=0;
			fire_grid[i][j].fuel_veg=0;
			fire_grid[i][j].fuel_litter=0;
			fire_grid[i][j].fuel_moist=100;
			fire_grid[i][j].soil_moist=100;
			fire_grid[i][j].temp=0;
		}
	}

	return fire_grid;
}

