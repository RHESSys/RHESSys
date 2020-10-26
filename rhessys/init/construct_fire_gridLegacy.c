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
//	double maxx, maxy, minx, miny, tmp,halfSideLength,curMinX,curMinY,curMaxX,curMaxY, cell_res;
	double cell_res;
/*	maxx=-10000; // depends on the origin for the coordinates, this was set for HJA centered at 0,0
	minx=-10000;
	maxy=-10000;
	miny=-10000;*/
	int grid_dimX,grid_dimY;

	cell_res =  command_line[0].fire_grid_res; //  grid resolution 
//	printf("cell res: %lf\n",cell_res);


	if(def.n_rows!=-1) // then we just read in the raster structure
	{
		// allocate the fire grid
		printf("reading patch raster structure\n");
		
		grid_dimX=def.n_cols;
		grid_dimY=def.n_rows;

		
		fire_grid=(struct patch_fire_object **) malloc(grid_dimY*sizeof(struct patch_fire_object *)); // first allocate the rows
		for(i=0;i<grid_dimY;i++) // for each row, allocate the columns
			fire_grid[i]=(struct patch_fire_object *) malloc(grid_dimX*sizeof(struct patch_fire_object ));
		printf("allocate the fire grid\n");
			
		// then initialize values: e.g., 0's 
		 for(i=0;i<grid_dimX;i++){
			for(j=0;j<grid_dimY;j++){
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
		//		printf("Current patch id: %d, X: %d  Y: %d\n",tmpPatchID,i,j);
				if(tmpPatchID>=0){ // then find the corresponding patch and allocate it--only one patch per grid cell!
				//	printf("numPatches 0: %d\n",fire_grid[j][i].num_patches);	
			//		printf("Current patch id: %d, X: %d  Y: %d\n",tmpPatchID,j,i);
					fire_grid[i][j].num_patches=1;
					//printf("numPatches 1: %d\n",fire_grid[j][i].num_patches);
					fire_grid[i][j].patches=(struct patch_object **) malloc(fire_grid[i][j].num_patches*sizeof(struct patch_object *));
					fire_grid[i][j].prop_patch_in_grid=(double *) malloc(fire_grid[i][j].num_patches*sizeof(double)); 
					fire_grid[i][j].prop_grid_in_patch=(double *) malloc(fire_grid[i][j].num_patches*sizeof(double)); 
					//printf("allocated patch array, how about the world %d?\n",world[0].num_basin_files);
					for (b=0; b< world[0].num_basin_files; ++b) {
						for (h=0; h< world[0].basins[b][0].num_hillslopes; ++h) {
							for (z=0; z< world[0].basins[b][0].hillslopes[h][0].num_zones; ++z) {
								tmp = world[0].basins[b][0].hillslopes[h][0].zones[z][0].aspect; /* aspect */
								for (p=0; p< world[0].basins[b][0].hillslopes[h][0].zones[z][0].num_patches; ++p) {
									patch = world[0].basins[b][0].hillslopes[h][0].zones[z][0].patches[p]; //zone object has linked list to point to patch families to point to patches
				//					printf("is my world ok? %d\n",&patch[0].ID);
									if(patch[0].ID==tmpPatchID)
									{
				//						printf("now filling in array--found a match!\n");
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
		printf("assigning dem\n");

		if(def.include_wui==1)
		{
			FILE *wuiIn;
			wuiIn=fopen("../auxdata/WUIGrid.txt","r");
			// for now do away with the header, so this file has no header
			for(i=0; i<grid_dimY;i++){
				for(j=0;j<grid_dimX;j++){				
					fscanf(wuiIn,"%d\t",&fire_grid[i][j].wui_flag);
				}
			}
			fclose(wuiIn);
			printf("done assigning wui\n");
		}
	}
	
	else
	{
		fprintf(stderr,"******\nNo patch grid file! Create a 30 m raster grid with patch identifiers\n********");
		exit(EXIT_FAILURE);
	}
	// for debugging, write out the fire grid and patches
	
	// comment out for now.
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
	for(i=0;i<world[0].num_fire_grid_row;i++) // for each row, allocate the columns
		fire_grid[i]=(struct fire_object *) malloc(world[0].num_fire_grid_col*sizeof(struct fire_object ));
		
	// then initialize values: e.g., 0's 
	 for(i=0;i<world[0].num_fire_grid_row;i++){
		for(j=0;j<world[0].num_fire_grid_col;j++){
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

