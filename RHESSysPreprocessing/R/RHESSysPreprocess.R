#' RHESSysPreprocess
#'
#' Preforms RHESSys Preprocessing, comprise of two main steps: generating a world file, and generating a flow table.
#' World file is generated via world_gen.R, and flow table is generated via CreateFlownet.R.
#' @param template Template file used to generate worldfile for RHESSys. Generic strucutre is:
#' <state variable> <operator> <value/map>. Levels are difined by lines led by "_", structured
#' <levelname> <map> <count>. Whitespace and tabs are ignored.  Maps referred to must be supplied
#' by your chosen method of data input(GRASS or raster), set using the "type" arguement.
#' @param name The base name (and potentially, path as well) to be used for your ouput files.
#' This will create a world file called "<name>.world", and a flow table called "<name>.flow".
#' @param type Input file type to be used.  Default is "GRASS" which will attempt to autodetect the
#' version of GRASS GIS being used (6.x or 7.x).  GRASS GIS type can also be set explicitly to "GRASS6" or "GRASS7".
#' "Raster" type will use rasters in GeoTiff or equivalent format (see Raster package), with file names
#' matching those indicated in the template.
#' @param typepars Parameters needed based on input data type used. For GRASS GIS type, typepars is a
#' vector of 5 character strings. GRASS GIS parameters: gisBase, home, gisDbase, location, mapset.
#' Example parameters are included in an example script included in this package. See initGRASS help
#' for more info on parameters.  If using raster type, typepars should be a string indicating the path to a folder
#' containing the raster files that are referenced by the template. See help for the Raster function (in the Raster package)
#' for additional information and supported filetypes.
#' @param overwrite Overwrite existing worldfile. FALSE is default and prompts a menu if worldfile already exists.
#' @param asprules The path and filename to the rules file.  Using this argument enables aspatial patches.
#' @param wrapper wrapper is an internal arguement that indivates to world_gen if it should output the required data to
#' memory(if being run in congunction with CreateFlownet), or to files, if being fun separately.
#' @seealso \code{\link{initGRASS}}, \code{\link{readRAST}}, \code{\link{Raster}}
#' @author Will Burke

RHESSysPreprocess = function(template,
                             name,
                             type = 'Raster',
                             typepars,
                             overwrite = FALSE,
                             streams = NULL,
                             roads = NULL,
                             impervious = NULL,
                             roofs = NULL,
                             asprules = NULL,
                             meta = TRUE,
                             wrapper = TRUE) {

  # check name input - remove prefix or sufixes
  basename = basename(name)
  if (startsWith(basename, "World.") |
      startsWith(basename, "world.")) {
    basename = substr(basename, 7, nchar(basename))
  } else if (endsWith(basename, ".world")) {
    basename = substr(basename, 0, nchar(basename) - 6)
  } else if (startsWith(basename, "Flow.") |
             startsWith(basename, "flow.")) {
    basename = substr(basename, 6, nchar(basename))
  } else if (endsWith(basename, ".flow")) {
    basename = substr(basename, 0, nchar(basename) - 5)
  }
  name_clean = file.path(dirname(name), basename)
  worldfile = name_clean
  cfname = name_clean

  rm(name, basename) # cleanup to help debugging

  # run world_gen
  world_gen_out = world_gen(template,
                            worldfile,
                            type,
                            typepars,
                            overwrite,
                            asprules,
                            wrapper)

  world_typepars = world_gen_out[[3]]
  world_cfmaps = world_gen_out[[2]]
  world_asp_list = world_gen_out[[1]]


  # run CreateFlownet
  print("Begin CreateFlownet")
  CreateFlownet(cfname,
                type,
                readin = world_cfmaps,
                typepars = world_typepars,
                asp_list = world_asp_list,
                streams,
                roads,
                impervious,
                roofs,
                wrapper)

if(meta){
  build_meta(
    name_clean,
    template,
    worldfile,
    cfname,
    type,
    typepars,
    world_cfmaps,
    streams,
    roads,
    impervious,
    roofs
  )
}

} # end function
