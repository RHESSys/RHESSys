# run_RHESSysPreprocess
# Will Burke 3/5/18
devtools::install_github("RHESSys/RHESSysPreprocessing")
detach("package:RHESSysPreprocessing", unload = TRUE)
remove.packages("RHESSysPreprocessing")
devtools::install_github("laurencelin/RHESSysPreprocessing", ref = "develop")

# ------------
# Assuming you installed the RHESSysPreprocessing package, you will need to load it
library(RHESSysPreprocessing)


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
template = "worldfiles/templates/template.testcase"


# Name
# ----
# Set the name and path for all function outputs.
# Suffixes of .world, .flow, and .meta will be appended to the worldfile, flowtable, and metadata files respectively.
name = "w8TC"

# Overwrite
# ---------
# TRUE/FALSE if an existing worldfile and flowtable be overwritten.
overwrite =  TRUE

# Streams
# -------
# Streams map to be used in creation of the flowtable - this is just the name of the map, to be found via the method indicated with "type"
streams = "streams50.tiff"

# Optional Flowtable Spatial Data
# -------------------------------
# These maps are optional inputs in flowtable creation
# roads = "roads_map"
# impervious = "impervious_map"
# roofs = "roofs_map"

# Header
# ------
# TRUE/FALSE to produce a header file. Header file will be have same name(and location) set by "name", with the ".hdr" suffix.
header = TRUE

# Finally, run the function.  Depending on size, it may take a minute or two.
RHESSysPreprocess(
  template = template,
  name = name,
  type = type,
  typepars = typepars,
  streams = streams,
  overwrite = overwrite,
  header = header)

