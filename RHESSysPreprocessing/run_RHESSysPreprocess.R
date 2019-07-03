# run_RHESSysPreprocess
# Will Burke 3/5/18
# Last updated 4/20/19

# Instructions
# ------------
# This is an example script showing how the RHESSysPreprocess.R function should be run.
# 1) Install or source the RHESSysPreprocessing package
# 2) Copy this script, and edit where indicated.
# 3) Run the RHESSysPreprocess.R function at the bottom.
# 4) The funciton will produce:
#       - worldfile
#       - flowtable
#       - optionally: header
#       (metadata output is being worked on currently)

# Load Package
# ------------
# Assuming you installed the RHESSysPreprocessing package, you will need to load it
library(RHESSysPreprocessing)

# Filepaths
# ----------------
# This script uses relative filepaths. This means that it will look folders and files relative to your current working directory.
# If needed, set your current working directory to the folder of your project:
setwd("~/Documents/MyProject")
# This script also uses the "~", which is a shorthand method of navigating to your "home" user directory - typically the folder named for your username.

# Spatial Data
# ------------
# You will need to select your method of geospatial data input.  This is the means by which the spatial data referenced in your template.

# Currently there are a two methods:
# 1) Raster - spatial data in any raster format supported by R GDAL will be read in from a folder.
# 2) < NO LONGER BEING ACTIVELY SUPPORTED> GRASS GIS > - GRASS 6 or 7, spatial data will be imported from the specified GRASS location and mapset.

# NOTES:
# - Due to a variety of factors, spatial data import via the raster method is both more robust and faster.
# - Regardless of import method, good practice for spatial data should be followed.
# - Input data should have the same projections, extents, and cell sizes. THIS MAY RESULT IN ERRORS IF NOT FOLLOWED.

# Raster
# ------
# To import spatial data from a folder of rasters:
# 1) Set type to "raster"
type = "raster"
# 2) Set typepars to the path of the folder containing your rasters
typepars = "spatial_data"


# Template
# --------
# The worldfile template is the key document that outlines how your worldfile will be built.
# The template variable should point to the name and location of your template.
template = "/templates/example.template"


# Name
# ----
# Set the name and path for all function outputs.
# Suffixes of .world, .flow, and .meta will be appended to the worldfile, flowtable, and metadata files respectively.
name = "/output/my_watershed"

# Overwrite
# ---------
# TRUE/FALSE if an existing worldfile and flowtable be overwritten.
overwrite = FALSE

# Streams
# -------
# Streams map to be used in creation of the flowtable - this is just the name of the map, to be found via the method indicated with "type"
streams = "my_watershed_streams"

# Optional Flowtable Spatial Data
# -------------------------------
# These maps are optional inputs in flowtable creation
# roads = "roads_map"
# impervious = "impervious_map"
# roofs = "roofs_map"

# Header
# ------
# TRUE/FALSE to produce a header file. Header file will be have same name(and location) set by "name", with the ".hdr" suffix.
header = FALSE

# Parallelization
# ---------------
# Current (Dec 2018 and on) develop branch RHESSys is hillslope paralleized and requires a flowtable that is compatible.
# Parallelization must be set to TRUE (this is a default)
# parallel = TRUE

# The "make_stream" argument defines the distance from an existing stream that the outlet of a hillslope can be set to be a
# stream.Since all hilslopes must have stream outlets, if a hillslope outlet exists outside of the distance threshold set by
# "make_stream",an error will occur and indicae the problem hillslope/outlet patch of that hillslope. This typically occurs
# as an artifact of how watershed analysis is done, and hillslopes are created, which sometimes results in fragmented or very
# small/skinny hillslopes, far away from streams.
# "make_stream" can be set to any positive value, or TRUE to always set hillslope outlets to streams.
# Default is 4, which is meant to roughly account for the errors/aritifacts that might occur from GIS, without including any
# extreme outlying hillslopes
# make_stream = 4

# Finally, run the function.  Depending on size, it may take a minute or two.
RHESSysPreprocess(
  template = template,
  name = name,
  type = type,
  typepars = typepars,
  streams = streams,
  overwrite = overwrite,
  header = header)
