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
#' @param streams Streams map to be used in building the flowtable.
#' @param overwrite Overwrite existing worldfile. FALSE is default and prompts a menu if worldfile already exists.
#' @param roads Roads map, an optional input for flowtable creation.
#' @param impervious Impervious map, an optional input for flowtable creation.
#' @param roofs Roofs map, an optional input for flowtable creation.
#' @param header TRUE/FALSE flag for the creation of a header file. Will have same name (and location) as "name" argument, but with ".hdr" suffix.
#' @param asprules The path and filename to the rules file.  Using this argument enables aspatial patches.
#' @param meta TRUE/FALSE flag for the creation of a metadata file. Still in dev.
#' @seealso \code{\link{initGRASS}}, \code{\link{readRAST}}, \code{\link{raster}}
#' @author Will Burke
#' @export

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
                             meta = TRUE,
                             wrapper = TRUE,
                             parallel = FALSE) {

  # ---------- Check Inputs ----------
  if (!file.exists(template)) { # check if template exists
    print(paste("Template does not exist or is not located at specified path:",template),quote=FALSE)
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
  print("Begin world_gen.R",quote=FALSE)

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

  typepars = world_gen_out[[3]]
  readin = world_gen_out[[2]]
  asp_list = world_gen_out[[1]]

  # ---------- Run CreateFlownet ----------
  print("Begin CreateFlownet.R",quote=FALSE)

  if (file.exists(cfname) & overwrite == FALSE) { # check for flownet overwrite
    t = menu(c("Yes", "No [Exit]"), title = noquote(paste(
      "Flowtable", cfname, "already exists. Overwrite?"
    )))
    if (t == 2) {
      stop("RHESSysPreprocess.R exited without completing")
    }
  }

  CreateFlownet(cfname = cfname,
                type = type,
                readin = readin,
                typepars = typepars,
                asp_list = asp_list,
                streams = streams,
                roads = roads,
                impervious = impervious,
                roofs = roofs,
                wrapper = wrapper,
                parallel = parallel)

  # ---------- Run build_meta ----------
  if(meta){
    build_meta(
      name = name_clean,
      world = worldfile,
      flow = cfname,
      template = template,
      type = type,
      typepars = typepars,
      cf_maps = world_cfmaps,
      streams = streams,
      roads = roads,
      impervious = impervious,
      roofs = roofs,
      asp_rule = asprules
    )
  }

} # end function
