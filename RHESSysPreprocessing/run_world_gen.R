# run_world_gen
# Will Burke 5/24/17

# This is a script showing how the world_gen function should be run. Replace below where needed.

# First, set your working directory.  Additional files world_gen makes will be located here (typepars, cf_maps)
# Set the path below to that directory:
setwd("~/Documents/examplefolder")

# Next, choose your method of GIS. "GRASS" Will (attempt) to autodetect the version of GRASS.
# Alternatively you can specify "GRASS6" for GRASS 6.x and "GRASS7" for GRASS 7.x
# If you are having trouble setting up/using GRASS 6.x, GRASS 7.x may be easier to install and runs faster.
type = "GRASS"

# Set your GRASS GIS environment using the variables listed below.

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

# template is the path and name of your template file.
template = "/templates/example.template"

# worldfile is the path and name of your worldfile
worldfile = "/worldfile/exmaple.world"

# overwrite is a TRUE/FALSE indicating if you want to overwrite an existing worldfile.
overwrite = FALSE

# Finally, run the function.  Depending on size, it may take a minute or two.
world_gen(template,worldfile,type,typepars,overwrite)

