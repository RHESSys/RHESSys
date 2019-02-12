RHESSys Preprocessing
Will Burke 2/15/18
Updated 1/14/19

-----Compatibility-----
Developed originally using R Version 3.4.0. May be incompatible with older R versions
Compatible with raster data - tested with GeoTIFF and ASCII, but all standard GDAL formats should be supported. See https://www.gdal.org/formats_list.html
Previously was compatible with GRASS GIS - versions 6.4.x and 7.x. Those GIS input methods are no longer being actively tested but may still work.

-----Contents-----
Package contains a few main components:
RHESSysPreprocess.R				An all-in-one function that runs both world_gen.R and CreateFlownet.R
world_gen.R						Creates a worldfile for use in RHESSys (replaces g2w)
CreateFlownet.R					Creates a RHESSys flowtable (replaces cf)

Other functions of note that can be used on their own:
template_read.R					Reads a template file and produces a clean template in list form, indices of levels and variables, and maps.
GIS_read.R						Reads an input type of GIS (GRASS or raster) and produces a LargeSpatialGridDataFrame containing all maps.
build_meta.R					Builds a metadata file, automatically generates most fields.
convert_flowtable.R				Converts an old flowtable to a hillslope parallelized flowtable. This is both a format change as well as checking and rerouting flow taht crosses hillslopes.
read_in_flow.RHESSys			Reads in a flow table and creates a R list format of the flowtable that is more useful for manipulation/analysis.

-----Installation-----
Installation/loading of this package can be done a few ways:
1) Install package (easiest)
	- Open RHESSysPreprocessing.Rproj
	- Under the "Build" menu, select "Install and restart". This installs the package effectively like it's an R package downloaded from CRAN.
	- Close the package (under "File" - "Close Project")
	- Load the package via: library(RHESSysPreprocessing)
2) Source the functions
	- For each function needed, use source() to load it. This will present problems if dependent functions are not loaded.
3) Run from within the RHESSysPreprocessing project
	- Open RHESSysPreprocessing.Rproj
	- Under the "Build" menu, select "Load All"

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

-----Parallelization-----
To run the hillslope parallelized version of RHESSys (develop branch - Dec, 2018), you must have a hillslope parallelized flowtable.
This can be done either by re-generating the flowtable from original inputs (preferred), or converting an existing flowtable. 
Currently (1/14/19) the develop branch should automatically produce a hilslope parallelized flowtable.
Generate from original inputs:
	- Setup RHESSysPreprocess.R or CreateFlownet.R as normal (see above instructions and included example R scripts)
	- Set arguemnt "parallel" to TRUE
	- Set "make_stream" argument if desired (default is 4, can be set to any integer, or TRUE)
		- The "make_stream" argument defines the distance from an existing stream that the outlet of a hillslope can be set to be a stream.
		Since all hilslopes must have stream outlets, if a hillslope outlet exists outside of the distance threshold set by "make_stream",
		an error will occur and indicae the problem hillslope/outlet patch of that hillslope. This typically occurs as an artifact of how watershed
		analysis is done, and hillslopes are created, which sometimes results in fragmented or very small/skinny hillslopes, far away from streams.
		- If you don't care about fragmented hillslopes, set "make_stream" to TRUE, and all hillslope outlets will become stream patches,
		and will route to the stream correctly.
Convert from existing:
	- Use the convert_flowtable.R function and set your existing flowtable as the input.
	- See above notes on the "make_stream" argument, which functions the same in this case.

Additional information on each function can be found in the R documentation by using help(RHESSysPreprocess), help(world_gen), help(CreateFlownet) etc.
Additional information can also be found on the RHESSys Github wiki page https://github.com/RHESSys/RHESSys/wiki

If you encounter bugs, let me know wburke@bren.ucsb.edu