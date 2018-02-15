# run_RHESSysPreprocess
# Will Burke 2/15/18

# This is a script showing how the RHESSysPreprocess.R function should be run. Replace below where needed.

# First, set your working directory.
# Set the path below to that directory:
setwd("~/Documents/examplefolder")

# Next, choose your method of data input. "GRASS" will (attempt) to autodetect the version of GRASS.
# If the code cannot detect your version, you can specify "GRASS6" for GRASS 6.x and "GRASS7" for GRASS 7.x
# If you are having trouble setting up/using GRASS 6.x, GRASS 7.x may be easier to install and runs faster.
type = "GRASS"

# Alternatively, if you have spatial data that is already output into rasters (GeoTiff or equivalent) you can set type to "raster"
type = "raster"
# This method will typically be faster, and may be more stable is you are encountering GRASS GIS-realated issues.

# IF USING GRASS GIS DATA INPUT- Set your GRASS GIS environment using the variables listed below.
# Your gisbase is the location of GRASS GIS on your system, typically:
gisbase = "/Applications/GRASS-6.4.app/Contents/MacOS"
# for windows sysstems this will be something like: "D:\\Program Files\\GRASS GIS 6.4.4"

# Home is the directory where a gisrc file will be created. This can be any directory and shouldn't matter.
home = "~/Documents"

# Your gisdb is the location of your GRASS GIS database - it ie the folder that contains your GRASS GIS projects
gisdb = "~/Documents/mygrassgisprojects"

# Your location is the name of your GRASS GIS location of interest. It will be inside of your gisdb folder.
location = "MyGRASSGISlocation"

# Lastly, indicate your mapset in your GRASS GIS location.  By default this is "PERMANENT" but you may have made additional mapsets.
mapset = "PERMANENT"

# These GIS environment variables are combined into a vector.
typepars = c(gisbase,home,gisdb,location,mapset)

# IF USING RASTER DATA INPUT
# Set typepars to a folder containing your raster spatial data.  Each raster should be named the same as it is referenced in the template file
typepars = "/input_rasters"

# template is the path and name of your template file.
template = "/templates/example.template"

# name for worldfile and flowtable ( .world and .flow suffixes will be appended) to be made
name = "/output/my_watershed"

# overwrite is a TRUE/FALSE indicating if you want to overwrite an existing worldfile and flowtable
overwrite = FALSE

# stream map to be used in creation of the flowtable - this is just the name of the map, to be found via the method indicated with "type"
stream = "my_watershed_streams"

# optional maps for use in flowtable creation
# roads = "roads_map"
# impervious = "impervious_map"
# roofs = "roofs_map"

# Finally, run the function.  Depending on size, it may take a minute or two.
RHESSysPreprocess(
  template=template,
  name=name,
  type=type,
  typepars=typepars,
  overwrite=overwrite,
  streams = stream,
  roads = NULL,
  impervious = NULL,
  roofs = NULL)
