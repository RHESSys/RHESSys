# run_RHESSysPreprocess
# Will Burke 3/5/18

# Instructions
# ------------
# This is an example script showing how the RHESSysPreprocess.R function should be run.
# 1) Install or source the RHESSysPreprocessing package
# 2) Copy this script, and edit where indicated.
# 3) Run the RHESSysPreprocess.R function at the bottom.
# 4) The funciton will produce:
#       - worldfile
#       - flowtable
#       - metadata (if not supressed)

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
# You will need to select your method of geospatial data input.  This is the means by which the spatial data referenced in your template

# Currently there are a two supported methods:
# 1) Raster - spatial data in any raster format supported by R GDAL will be read in from a folder.
# 2) GRASS GIS - GRASS 6 or 7, spatial data will be imported from the specified GRASS location and mapset.

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

# GRASS GIS
# ---------
# To import sptial data using GRASS GIS:
# 1) Set type to "GRASS". GRASS 6.x amd GRASS 7.x are supported. Setting type to "GRASS" will autodetect your version.
# Alternatively, you can set it to "GRASS6" or "GRASS7" explicitly.
type = "GRASS"
# 2) Set typepars to your GRASS GIS environment. The five environment variables are listed below:
# -- gisbase is the location of GRASS GIS on your system, typically:
gisbase = "/Applications/GRASS-6.4.app/Contents/MacOS"
# For windows sysstems this will be something like: "D:\\Program Files\\GRASS GIS 6.4.4"
# -- Home is the directory where a gisrc file will be created. This can be any directory and shouldn't matter.
home = "~/Documents"
# -- gisdb is the location of your GRASS GIS database - it ie the folder that contains your GRASS GIS projects
gisdb = "~/Documents/mygrassgisprojects"
# -- Location is the name of your GRASS GIS location of interest. It will be inside of your gisdb folder.
location = "MyLocation"
# Lastly, indicate your mapset in your GRASS GIS location.  By default this is "PERMANENT" but you may have made additional mapsets.
mapset = "PERMANENT"

# These GIS environment variables are combined into a vector.
typepars = c(gisbase,home,gisdb,location,mapset)


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

# Finally, run the function.  Depending on size, it may take a minute or two.
RHESSysPreprocess(
  template = template,
  name = name,
  type = type,
  typepars = typepars,
  streams = streams,
  overwrite = overwrite,
  header = header)
