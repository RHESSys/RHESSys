#' RHESSysPreprocess
#'
#' Preforms RHESSys Preprocessing, comprised of two main steps: generating a world file, and generating a flow table.
#' World file is generated via world_gen.R, and flow table is generated via CreateFlownet.R.
#' @param template Template file used to generate worldfile for RHESSys. Generic strucutre is:
#' <state variable> <operator> <value/map>. Levels are difined by lines led by "_", structured
#' <levelname> <map> <count>. Whitespace and tabs are ignored.  Maps referred to must be supplied
#' by your chosen method of data input(GRASS or raster), set using the "type" arguement.
#' @param name The base name (and potentially, path as well) to be used for your ouput files.
#' This will create a world file called "<name>.world", and a flow table called "<name>.flow".
#' @param type Input file type to be used. Default is raster. "Raster" type will use rasters
#' in GeoTiff or equivalent format (see Raster package), with file names  matching those indicated in the template.
#' ASCII is supported, but 0's cannot be used as values for data. "GRASS" will attempt to autodetect the version of
#' GRASS GIS being used (6.x or 7.x).  GRASS GIS type can also be set explicitly to "GRASS6" or "GRASS7".
#' @param typepars Parameters needed based on input data type used. If using raster type, typepars should be a string
#' indicating the path to a folder containing the raster files that are referenced by the template.
#' For GRASS GIS type, typepars is a vector of 5 character strings. GRASS GIS parameters: gisBase, home, gisDbase, location, mapset.
#' Example parameters are included in an example script included in this package. See initGRASS help
#' for more info on parameters.
#' @param streams Streams map to be used in building the flowtable.
#' @param overwrite Overwrite existing worldfile. FALSE is default and prompts a menu if worldfile already exists.
#' @param roads Roads map, an optional input for flowtable creation.
#' @param impervious Impervious map, an optional input for flowtable creation.
#' @param roofs Roofs map, an optional input for flowtable creation.
#' @param header TRUE/FALSE flag for the creation of a header file. Will have same name (and location) as "name" argument, but with ".hdr" suffix.
#' @param asprules The path and filename to the rules file.  Using this argument enables aspatial patches.
#' @param meta TRUE/FALSE flag for the creation of a metadata file. Still in dev.
#' @param parallel TRUE/FALSE flag to build a flowtable for use in the hilllslope parallelized version of RHESSys. Console may output warnings of
#' automated actions taken to make hillslope parallelization possible, or errors indicating fatal problems in hillslope parallelization.
#' @param d4 TRUE/FALSE flag to determine the logic used when finding neighbors in flow table creation. FALSE uses d8 routing, looking at all eight
#' neighboring cells. TRUE uses d4 routing, looking at only cardinal directions, not diagonals.
#' @param make_stream The maximum distance (cell lengths) away from an existing stream that a patch can be automatically coerced to be a stream.
#' Setting to TRUE will include patches at any distance. This is needed for hillslope parallelization, as all hillslopes must have an outlet stream patch.
#' Default is 4.
#' @seealso \code{\link{initGRASS}}, \code{\link{readRAST}}, \code{\link{raster}}
#' @author Will Burke
#' @export

# ---------- Function start ----------
RHESSysPreprocess = function(template,
                             name,
                             type = 'Raster',
                             typepars,
                             streams = NULL,
                             overwrite = FALSE,
                             roads = NULL,
                             impervious = NULL,
                             roofs = NULL,
                             asprules = NULL,
                             header = FALSE,
                             meta = FALSE,
                             wrapper = TRUE,
                             parallel = TRUE,
                             d4 = FALSE,
                             make_stream = 4) {

  # ---------- Check Inputs ----------
  if (!file.exists(template)) { # check if template exists
    print(paste("Template does not exist or is not located at specified path:",template),quote = FALSE)
  }

  basename = basename(name) # check Name
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

  if (!dir.exists(dirname(name))) { # check if output dir exists, menu to create
    t = menu(
      c("Yes", "No [Exit]"),
      title = paste("Ouput directory path:",dirname(name),"is not valid. Create folder(s)?"))
    if (t == 1) {
      dir.create(dirname(name), recursive = TRUE)
    }
    if (t == 2) {
      stop("RHESSysPreprocess.R exited without completing")
    }
  }

  if (!type %in% c("Raster", "RASTER", "raster", "GRASS", "GRASS6", "GRASS7")) { # check if type is valid
    stop(noquote(paste("Type '", type, "' not recognized.", sep = "")))
  }

  if (!is.logical(overwrite)) { # check overwrite type
    stop("Overwrite must be logical")
  }

  # ---------- Run world_gen ----------
  print("Begin world_gen.R",quote = FALSE)

  if (file.exists(worldfile) & overwrite == FALSE) { # check for worldfile overwrite
    t = menu(c("Yes", "No [Exit]"), title = noquote(paste(
      "Worldfile", worldfile, "already exists. Overwrite?"
    )))
    if (t == 2) {
      stop("RHESSysPreprocess.R exited without completing")
    }
  }

  world_gen_out = world_gen(template = template,
                            worldfile = worldfile,
                            type = type,
                            typepars = typepars,
                            overwrite = overwrite,
                            header = header,
                            asprules = asprules,
                            wrapper = wrapper)

  readin = world_gen_out[[1]]
  asp_list = world_gen_out[[2]]

  # ---------- Run CreateFlownet ----------
  print("Begin CreateFlownet.R",quote = FALSE)

  if (file.exists(cfname) & overwrite == FALSE) { # check for flownet overwrite
    t = menu(c("Yes", "No [Exit]"), title = noquote(paste(
      "Flowtable", cfname, "already exists. Overwrite?"
    )))
    if (t == 2) {
      stop("RHESSysPreprocess.R exited without completing")
    }
  }

  CreateFlownet(name = cfname,
                readin = readin,
                type = type,
                typepars = typepars,
                asp_list = asp_list,
                streams = streams,
                overwrite = overwrite,
                roads = roads,
                impervious = impervious,
                roofs = roofs,
                wrapper = wrapper,
                parallel = parallel,
                make_stream = make_stream,
                d4 = d4)

  # ---------- Run build_meta ----------
  # if (meta) {
  #   build_meta(
  #     name = name_clean,
  #     world = worldfile,
  #     flow = cfname,
  #     template = template,
  #     type = type,
  #     typepars = typepars,
  #     cf_maps = readin,
  #     streams = streams,
  #     roads = roads,
  #     impervious = impervious,
  #     roofs = roofs,
  #     asp_rule = asprules
  #   )
  # }

} # end function
