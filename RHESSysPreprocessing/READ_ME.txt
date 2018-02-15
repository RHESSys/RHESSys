RHESSys Preprocessing
Will Burke 2/15/18

-----Compatibility-----
Developed using R Version 3.4.0. May be incompatible with older R versions
Compatible with GRASS GIS - versions 6.4.x and 7.x. Also compatible with raster data - tested with GeoTIFF format.

-----Contents-----
Package contains a few main components:
RHESSysPreprocess.R			An all-in-one function that runs both world_gen.R and CreateFlownet.R
world_gen.R					Creates a worldfile for use in RHESSys (replaces g2w)
CreateFlownet.R				Creates a RHESSys flowtable (replaces cf)

Additionally, the package contains a variety of functions that constitute the above functions. Of note are:
template_read.R				Reads a template file and produces a clean template in list form, indices of levels and variables, and maps.
GIS_read.R					Reads an input type of GIS (GRASS or raster) and produces a LargeSpatialGridDataFrame containing all maps.

-----Instructions-----
Prerequisites: 
	- Input spatial data must be generated first. This includes level maps (world, basin, etc.) and maps of aspect, slope, horizon, etc.
	- Template - template must point to the spatial data you want to use 
	- Additional maps for flowtable - stream map, and potentially roofs, roads, and impervious area must be generated as well.
1) Method 1: All-in-one
	- Open the set-up/example run script "run_RHESSysPreprocess.R"
	- Follow the instructions included in the script
	- Script will the RHESSysPreprocess.R, and produce a worldfile and flowtable.
2) Method 2: run separately
	2a) Create worldfile
		- Open the set-up/example script "run_world_gen.R"
		- Follow the commented directions to run world_gen.R
		- Script will produce a worldfile.  Additionally, a hidden folder ".extra_files" will be produced to store files needed for CreateFlownet.R
	2b) Create flowtable
		- Open the set-up/example script "run_CreateFlownet.R" 
		- Follow the commented instructions to rune CreateFlownet.R
		- Script will produce a flowtable

Additional information on each function can be found in the R documentation by using help(RHESSysPreprocess), help(world_gen) or help(CreateFlownet).
Additional information can also be found (soonish) on the RHESSys Github wiki page https://github.com/RHESSys/RHESSys/wiki

If you encounter bugs,let me know wburke@bren.ucsb.edu