/*--------------------------------------------------------------*/
/* 																*/
/*					construct_fire_grid							*/
/*																*/
/*	construct_fire_grid.c - creates a raster grid of patches	*/
/*	to be passed to WMFire 										*/
/*																*/
/*	NAME														*/
/*	construct_fire_grid.c - creates a raster grid				*/
/*																*/
/*	SYNOPSIS													*/
/*	struct fire_grid *construct_fire_grid(						*/
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
/* 	associated grid cell(s).									*/
/*																*/
/*																*/
/*	PROGRAMMER NOTES											*/
/* 	M Kennedy June 27, 2012										*/
/*	Assumes patches are square.  No guarantee of performance otherwise				*/
/* 	Updated May 16, 2013 to allow for a raster grid of patch id's be used to create	*/
/* 	the fire grid. gives a better approximation of irregularly-shaped patches		*/
/*--------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "rhessys.h"
#include <string.h>

// local function definition:
/*double calc_patch_area_in_grid(double curMinX,double curMinY,double curMaxX,double curMaxY,double cellMaxX,
							double cellMaxY,double cellMinX,double cellMinY,double cell_res);*/

struct fire_object **construct_patch_fire_grid (struct world_object *world, struct command_line_object *command_line,struct fire_default def)

{
	/*--------------------------------------------------------------*/
	/*	Local function definition.									*/
	/*--------------------------------------------------------------*/
	struct patch_object *find_patch(int patch_ID, int zone_ID, int hill_ID,
									struct basin_object *basin);

	struct patch_family_object *find_patch_family(int patch_family_ID, int zone_ID, int hill_ID,
												  struct basin_object *basin);

	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/

	//MCK: time process
	clock_t start, end;
	double time_used;
	start = clock();
	struct patch_fire_object **fire_grid;
	struct patch_object *patch;
	struct patch_family_object *patch_family;

	int b, h, p, z, i, j, k, pf;
	//	double maxx, maxy, minx, miny, tmp,halfSideLength,curMinX,curMinY,curMaxX,curMaxY, cell_res;
	double cell_res, tmp;
	/*	maxx=-10000; // depends on the origin for the coordinates, this was set for HJA centered at 0,0
	minx=-10000;
	maxy=-10000;
	miny=-10000;*/
	int grid_dimX, grid_dimY;

	//typedef struct node_fire_wui_dist *node;
	//	node *tmp_node; // for wui linked list

	cell_res =  command_line[0].fire_grid_res; //  grid resolution 
	//	printf("cell res: %lf\n",cell_res);

	if(def.n_rows!=-1) // then we just read in the raster structure
	{
		// allocate the fire grid
		if (command_line[0].verbose_flag <= -7) {
			printf("Reading patch raster structure\n");
		}

		grid_dimX = def.n_cols;
		grid_dimY = def.n_rows;

		world[0].num_fire_grid_row = grid_dimY;
		world[0].num_fire_grid_col = grid_dimX;

		if (command_line[0].verbose_flag <= -7) {
			printf("Allocate the fire grid\n");
		}

		fire_grid = (struct patch_fire_object **)malloc(grid_dimY * sizeof(struct patch_fire_object *)); // first allocate the rows
		// for each row, allocate the columns
		for (i = 0; i < grid_dimY; i++)	{
			fire_grid[i] = (struct patch_fire_object *)malloc(grid_dimX * sizeof(struct patch_fire_object));
		}

		if (command_line[0].verbose_flag <= -7) {
			printf("ncol %d | ", grid_dimX);
			printf("nrow %d \n", grid_dimY);
		}

		/*if(def.include_wui==1)
		{	
			double tmp.dists[def.nWUI];// intended to initialize an array of distances to all wuis in a watershed
			for(w=0;w<def.nWUI;w++)
				tmp.dists[w]=0;	
		}*/

		int tmpPatchID, tmpZoneID, tmpHillID;	// for reading in
		int curPatchID, curZoneID; 				// for find_patch

		// including locally only since we don't need paths elsewhere
		char firegrid_dem_file[FILEPATH_LEN], firegrid_patch_file[FILEPATH_LEN], firegrid_zone_file[FILEPATH_LEN], firegrid_hill_file[FILEPATH_LEN];
		FILE *demIn;
		FILE *patchesIn;
		FILE *zoneIn;
		FILE *hillIn;

		// copy filename prefix
		strncpy(firegrid_dem_file, world[0].fire_grid_prefix[0], FILEPATH_LEN);
		strncpy(firegrid_patch_file, world[0].fire_grid_prefix[0], FILEPATH_LEN);
		strncpy(firegrid_zone_file, world[0].fire_grid_prefix[0], FILEPATH_LEN);
		strncpy(firegrid_hill_file, world[0].fire_grid_prefix[0], FILEPATH_LEN);
		if (command_line[0].verbose_flag <= -7) {
			printf("Fire grid path prefix: %s\n",firegrid_dem_file);
		}

		// add suffixes/extensions
		strcat(firegrid_dem_file, ".dem");
		strcat(firegrid_patch_file, ".patch");
		strcat(firegrid_zone_file, ".zone");
		strcat(firegrid_hill_file, ".hillslope");

		if (command_line[0].verbose_flag <= -7) {
			printf("Files with suffix: %s, %s, %s, %s\n",firegrid_dem_file, firegrid_patch_file, firegrid_zone_file, firegrid_hill_file);
		}

		
		// open file connections
		if ((demIn = fopen(firegrid_dem_file, "r")) == NULL)
		{
			fprintf(stderr, "FATAL ERROR:  Cannot open DEM fire grid file %s\n", firegrid_dem_file);
			exit(EXIT_FAILURE);
		}
		if ((patchesIn = fopen(firegrid_patch_file, "r")) == NULL)
		{
			fprintf(stderr, "FATAL ERROR:  Cannot open patch fire grid file %s\n", firegrid_patch_file);
			exit(EXIT_FAILURE);
		}
		if ((zoneIn = fopen(firegrid_zone_file, "r")) == NULL)
		{
			fprintf(stderr, "FATAL ERROR:  Cannot open zone fire grid file %s\n", firegrid_zone_file);
			exit(EXIT_FAILURE);
		}
		if ((hillIn = fopen(firegrid_hill_file, "r")) == NULL)
		{
			fprintf(stderr, "FATAL ERROR:  Cannot open hillslope fire grid file %s\n", firegrid_hill_file);
			exit(EXIT_FAILURE);
		}

		// Start building patch fire grid
		for (i = 0; i < grid_dimY; i++)
		{
			for (j = 0; j < grid_dimX; j++)
			{
				fire_grid[i][j].occupied_area = 0;
				fire_grid[i][j].num_patches = 0;
				fire_grid[i][j].tmp_patch = 0;

				//if(def.include_wui==1)
				//		fire_grid[j][i].wui_dists=tmp.dists; intended to be distance to wuis
				//				patch_grid[j][i]=-1;
				//			}
				//		}
				//		world[0].num_fire_grid_row = grid_dimY;
				//		world[0].num_fire_grid_col = grid_dimX;
				//		printf("Rows: %d Cols: %d\n",world[0].num_fire_grid_row,world[0].num_fire_grid_col);
				//		int tmpPatchID;
				//		FILE *patchesIn;
				//		patchesIn=fopen(command_line[0].firegrid_patch_filename,"r");
				/*		if(def.include_wui==1) // then readin the wui LUT
						{
							FILE *wuiIn;
							wuiIn=fopen(command_line[0].wui_lut_filename,"r");
						}*/
				//patchesIn=fopen("../auxdata/patchGrid.txt","r");
				// for now do away with the header
				//		for(i=0; i<grid_dimY;i++){
				//			for(j=0;j<grid_dimX;j++){
				// first initialize the fire grid
				//	fire_grid[j][i].occupied_area=0;
				//		fire_grid[j][i].num_patches=0;
				//	fire_grid[j][i].tmp_patch=0;
				// now point to the corresponding patch
				
				// Check map/grid values at current i & j
				tmpPatchID = -9999;
				fscanf(patchesIn, "%d\t", &tmpPatchID);

				tmpZoneID = -9999;
				fscanf(zoneIn, "%d\t", &tmpZoneID);

				tmpHillID = -9999;
				fscanf(hillIn, "%d\t", &tmpHillID);
				
				fscanf(demIn, "%lf\t", &fire_grid[i][j].elev);
				
				//		printf("read dem \n");
				// here read in the wuiLUT,
				/*if(def.include_wui==1)
				{
					double dummy1,dummy2;
					fscanf(wuiIn,"%d\t%d\t,&dumm1,&dummy2); // throw away the first two elements of each row ? or 3 , depending on LUT structure
					for(int w=0;w<def.nWUI;w++)
					{
						fscanf(wuiIn,"%d\t",&fire_grid[i][j].wui_dists[w]);
					}					
				}
				*/
				if (command_line[0].verbose_flag <= -7) {
					printf("--------------------\n");
					printf("Current grid patch id: %d, X: %d  Y: %d\n",tmpPatchID,i,j);
					printf("Zone: %d | Hill: %d | elev: %lf\n",tmpZoneID,tmpHillID,fire_grid[i][j].elev);
				}

				if (tmpPatchID >= 0)
				{	
					// if valid, find the corresponding patch/patch family
					curPatchID = tmpPatchID;
					curZoneID = tmpZoneID;
					if (command_line[0].verbose_flag <= -7) {
						printf("Valid patch id: %d, X: %d  Y: %d\n", curPatchID, j, i);
					}

					//1	#pragma omp parallel for
					//             for(i=0; i<grid_dimY;i++){
					//		 #pragma omp parallel for
					//                 for(j=0;j<grid_dimX;j++){
					//			if(fire_grid[i][j].num_patches==1)
					//			{
					
					fire_grid[i][j].occupied_area = cell_res * cell_res; // this grid cell is 100% occupied - lets assume this is still true

					for (b = 0; b < world[0].num_basin_files; ++b)
					{
						if (command_line[0].multiscale_flag == 1)
						{
							// assuming 1 patch family per grid, curpatch is family not patch here
							fire_grid[i][j].patch_families = (struct patch_family_object **)malloc(sizeof(struct patch_family_object *));
							//printf("Finding patch family %d\n", curPatchID);
							patch_family = find_patch_family(curPatchID, curZoneID, tmpHillID, world[0].basins[b]);
							fire_grid[i][j].patch_families[0] = patch_family;
							fire_grid[i][j].num_patches = patch_family[0].num_patches_in_fam;

							fire_grid[i][j].patches = (struct patch_object **)malloc(fire_grid[i][j].num_patches * sizeof(struct patch_object *));
							fire_grid[i][j].prop_patch_in_grid = (double *)malloc(fire_grid[i][j].num_patches * sizeof(double));
							fire_grid[i][j].prop_grid_in_patch = (double *)malloc(fire_grid[i][j].num_patches * sizeof(double));

							fire_grid[i][j].prop_grid_in_patch[0] = (cell_res * cell_res) / patch_family[0].area;
							// iterate through patches in family and set prop_patch_in_grid to patch area/fam area
							if (command_line[0].verbose_flag <= -7) { printf("Patches:"); }

							for (p = 0; p < patch_family[0].num_patches_in_fam; ++p)
							{
								fire_grid[i][j].patches[p] = patch_family[0].patches[p];
								fire_grid[i][j].prop_patch_in_grid[p] = fire_grid[i][j].patches[p][0].family_pct_cover;
								if (command_line[0].verbose_flag <= -7) { printf(" %d",fire_grid[i][j].patches[p][0].ID); }
							}
							if (command_line[0].verbose_flag <= -7) { printf("\n");}
						}
						else
						{
							fire_grid[i][j].num_patches = 1;

							fire_grid[i][j].patches = (struct patch_object **)malloc(fire_grid[i][j].num_patches * sizeof(struct patch_object *));
							fire_grid[i][j].prop_patch_in_grid = (double *)malloc(fire_grid[i][j].num_patches * sizeof(double));
							fire_grid[i][j].prop_grid_in_patch = (double *)malloc(fire_grid[i][j].num_patches * sizeof(double));

							patch = find_patch(curPatchID, curZoneID, tmpHillID, world[0].basins[b]);
							fire_grid[i][j].patches[0] = patch; // assign the current patch to this grid cell
							//printf("patch2: area, cell res %lf, %lf\n",patch[0].area,cell_res);
							fire_grid[i][j].prop_grid_in_patch[0] = (cell_res * cell_res) / patch[0].area; // the proportion of this patch in this cell
							//printf("patch3\n");
							fire_grid[i][j].prop_patch_in_grid[0] = 1; // the whole cell is occupied this patch
						}

						//	printf("array filled\n");
						// if we have found the correct patch, stop looking
					}
					
					/*if(patch[0].wuiID>=0&&def.include_wui==1) // then see if this pixel is within salience distance of this wui patch, and if it is add this patch to the pixel linked list
									{
										if(fire_grid[i][j].wui_dists[patch[0].wuiID]<=3)
										{
											tmp_node=(node)malloc(sizeof(struct node_fire_wui_dist));
											tmp_node.dist=fire_grid[i][j].wui_dists[patch[0].wuiID];
											tmp_node.patches[0]=patch;
											tmp_node.next=NULL;
											if(fire_grid[i][j].patch_wui_dist[0]==NULL)
												fire_grid[i][j].patch_wui_dist[0]=tmp_node);
											else
											{
												cur_node=fire_grid[i][j].patch_wui_dist[0].next;
												while(cur_node->next!=NULL)
												{
													cur_node=cur_node.next;
												}
												cur_node->next=tmp_node;
											}	
											// then update linked list arr index 0; this patch is within first salience distance
										}
										else
										{
											if(fire_grid[i][j].wui_dists[patch[0].wuiID]<=5)
											{
												tmp_node=(node)malloc(sizeof(struct node_fire_wui_dist));
												tmp_node.dist=fire_grid[i][j].wui_dists[patch[0].wuiID];
												tmp_node.patches[0]=patch;
												tmp_node.next=NULL;
												if(fire_grid[i][j].patch_wui_dist[1]==NULL)
													fire_grid[i][j].patch_wui_dist[1]=tmp_node);
												else
												{
													cur_node=fire_grid[i][j].patch_wui_dist[1].next;
													while(cur_node->next!=NULL)
													{
														cur_node=cur_node.next;
													}
													cur_node->next=tmp_node;
												}	
												// then update linked list arr index 1; this patch is within second salience distance
											}
											else if(fire_grid[i][j].wui_dists[patch[0].wuiID]<=10)
											{
												tmp_node=(node)malloc(sizeof(struct node_fire_wui_dist));
												tmp_node.dist=fire_grid[i][j].wui_dists[patch[0].wuiID];
												tmp_node.patches[0]=patch;
												tmp_node.next=NULL;
												if(fire_grid[i][j].patch_wui_dist[2]==NULL)
													fire_grid[i][j].patch_wui_dist[2]=tmp_node);
												else
												{
													cur_node=fire_grid[i][j].patch_wui_dist[2].next;
													while(cur_node->next!=NULL)
													{
														cur_node=cur_node.next;
													}
													cur_node->next=tmp_node;
													se(patchesIn);
												}	
												// then update linked list arr index 2; this patch is within third salience distance
											}
										}
									}*/
				}
			}
		}

//			}
//		}
/*		if(def.include_wui==1) // then readin the wui LUT
			fclose(wuiIn);
*/		
		//		FILE *demIn;
		//		demIn=fopen(command_line[0].firegrid_dem_filename,"r");
		//		demIn=fopen("../auxdata/DemGrid.txt","r");
				// for now do away with the header, so this file has no header
		//		for(i=0; i<grid_dimY;i++){
		//for(j=0;j<grid_dimX;j++){				
		//				fscanf(demIn,"%lf\t",&fire_grid[i][j].elev);
		//printf("Which column? %d \n",i);

		//close file connections
		fclose(patchesIn);
		fclose(demIn);
		fclose(hillIn);
		fclose(zoneIn);

		printf("--------------------\n");

/*		if(def.include_wui==1) // then readin the wui LUT
		{
			FILE *wuiIn;
			wuiIn=fopen("../auxdata/WUILUT.txt","r");
			// for now do away with the header, so this file has no header // a placeholder, replace with a linked list of wui blocks for each grid cell with distance to wui block
			for(i=0; i<grid_dimY;i++){
				for(j=0;j<grid_dimX;j++){				
					fscanf(wuiIn,"%d\t",&fire_grid[i][j].wui_flag);
				}
			}
			fclose(wuiIn);
			printf("done assigning wui\n");
		}*/
	}
	else
	{
		fprintf(stderr,"******\nNo patch grid file! Create a 30 m raster grid with patch ids\n********");
		fire_grid = NULL;
		exit(EXIT_FAILURE);
	}
	// for debugging, write out the fire grid and patches
	/*
	// comment out for now.
	FILE *gridout;

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
	fclose(gridout);

*/
	/* done allocating fire grid, return to RHESSys*/
	end=clock();
	time_used=((double) (end-start))/CLOCKS_PER_SEC;
	printf("***** Done allocating fire grid\nTime: %f  *****\n",time_used);
	return(fire_grid);	
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
			fire_grid[i][j].burn=0; // if burned, WMFire will replace with the p_s value
			fire_grid[i][j].fuel_veg=0; // for one of the fuel loading options
			fire_grid[i][j].fuel_litter=0; // for
			fire_grid[i][j].fuel_moist=100;
			fire_grid[i][j].soil_moist=100;
			fire_grid[i][j].temp=0;
			//fire_grid[i][j].df=0 // placeholder for linked list of WUI's within salience distance of pixel, initialized at zero
							// this would be an array of length=number of WUI blocks within <=10 km of this cell,
							// with the distance associated with each WUI block. WMFire will replace these zeroes if
							// a fire occurs in this pixel
							// or a pointer to the to wui block
	
		}
	}

	return(fire_grid);
}
