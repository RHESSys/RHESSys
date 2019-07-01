#' CreateFlownet
#'
#' Creates the flow networkd table used by RHESSys
#' @param flownet_name The name of the flow network file to be created.  Will be coerced to have ".flow" extension if not already present.
#' @param readin readin indicates which maps to be used. If CreateFlowmet.R is run it's own, this should point to the template. Otherwise,
#' if run inside of RHESSysPreprocess, readin will use the map data from world_gen.R, Streams map, and other optional maps, still need to
#' be specified.
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
#' @param parallel TRUE/FALSE flag to build a flowtable for use in the hilllslope parallelized version of RHESSys. Console may output warnings of
#' automated actions taken to make hillslope parallelization possible, or errors indicating fatal problems in hillslope parallelization.
#' @param d4 TRUE/FALSE flag to determine the logic used when finding neighbors in flow table creation. FALSE uses d8 routing, looking at all eight
#' neighboring cells. TRUE uses d4 routing, looking at only cardinal directions, not diagonals.
#' @param make_stream The maximum distance (cell lengths) away from an existing stream that a patch can be automatically coerced to be a stream.
#' Setting to TRUE will include patches at any distance. This is needed for hillslope parallelization, as all hillslopes must have an outlet stream patch.
#'  Default is 4.
#' @author Will Burke
#' @export


CreateFlownet = function(flownet_name,
                         readin = NULL,
                         type = "raster",
                         typepars = NULL,
                         asp_list = NULL,
                         streams = NULL,
                         overwrite = FALSE,
                         roads = NULL,
                         road_width = NULL,
                         impervious = NULL,
                         roofs = NULL,
                         wrapper = FALSE,
                         parallel = TRUE,
                         make_stream = 4,
                         d4 = FALSE){

  # ------------------------------ Read and check inputs ------------------------------
  cfbasename = basename(flownet_name) # Coerce .flow extension
  if (startsWith(cfbasename,"Flow.") | startsWith(cfbasename,"flow.")) {
    cfbasename = paste(substr(cfbasename,6,nchar(cfbasename)),".flow",sep = "")
  } else if (!endsWith(cfbasename,".flow")) {
    cfbasename = paste(cfbasename,".flow",sep = "")
  }
  flownet_name = file.path(dirname(flownet_name),cfbasename)

  if (!is.logical(overwrite)) {stop("overwrite must be logical")} # check overwrite inputs
  if (file.exists(flownet_name) & overwrite == FALSE) {stop(noquote(paste("Flowtable",flownet_name,"already exists.")))}

  if (!wrapper & is.character(readin)) { #if run outside of rhessyspreprocess.R, and if readin is character. readin is the template (and path)
    template_list = template_read(template)
    map_info = template_list[[5]]
    cfmaps = rbind(map_info,c("cell_length","none"), c("streams","none"), c("roads","none"), c("impervious","none"),c("roofs","none"))
  } else if (wrapper | (!wrapper & is.matrix(readin))) { # map info is passsed directly from world gen - either in wrapper or outside of wrapper and readin is matrix
    cfmaps = readin
  }

  # Check for streams map, menu allows input of stream map
  if (is.null(streams) & (cfmaps[cfmaps[,1] == "streams",2] == "none" | is.na(cfmaps[cfmaps[,1] == "streams",2]))) {
    t = menu(c("Specify map","Abort function"),
             title = "Missing stream map. Specify one now, or abort function and edit cf_maps file?")
    if (t == 2) {stop("Function aborted")}
    if (t == 1) {
      streams = readline("Stream map:")
    }
  }
  # only add stream map to cfmaps if it's not there already
  if ((cfmaps[cfmaps[,1] == "streams",2] == "none" | is.na(cfmaps[cfmaps[,1] == "streams",2]))) {
    cfmaps[cfmaps[,1] == "streams",2] = streams
  }

  # add road, impervious, and roofs to maps to get
  if (!is.null(roads)) {
    cfmaps[cfmaps[,1] == "roads",2] = roads
    if (is.null(road_width)) {stop("If using roads, road width cannot be 0.")}
  }
  if (!is.null(impervious)) {cfmaps[cfmaps[,1] == "impervious",2] = impervious}
  if (!is.null(roofs)) {cfmaps[cfmaps[,1] == "roofs",2] = roofs}

  maps_in = unique(cfmaps[cfmaps[,2] != "none" & cfmaps[,1] != "cell_length",2])

  # ------------------------------ Use GIS_read to get maps ------------------------------
  readmap = GIS_read(maps_in, type, typepars, map_info = cfmaps)
  map_ar_clean = as.array(readmap)
  dimnames(map_ar_clean)[[3]] = colnames(readmap@data)

  raw_patch_data = map_ar_clean[, , cfmaps[cfmaps[, 1] == "patch", 2]]
  raw_patch_elevation_data = map_ar_clean[, , unique(cfmaps[cfmaps[, 1] == "z", 2])]
  raw_hill_data = map_ar_clean[, , cfmaps[cfmaps[, 1] == "hillslope", 2]]
  raw_basin_data = map_ar_clean[, , cfmaps[cfmaps[, 1] == "basin", 2]]
  raw_zone_data = map_ar_clean[, , cfmaps[cfmaps[, 1] == "zone", 2]]
  raw_slope_data = map_ar_clean[, , unique(cfmaps[cfmaps[, 1] == "slope", 2])]
  raw_stream_data = map_ar_clean[, , cfmaps[cfmaps[, 1] == "streams", 2]]
  cell_length = readmap@grid@cellsize[1]

  # Roads
  raw_road_data = NULL
  if (!is.null(roads)) {raw_road_data = map_ar_clean[, ,cfmaps[cfmaps[,1] == "roads",2]]}

  # Roofs and impervious is not yet implemented - placeholders for now -----
  if (!is.null(roofs) | !is.null(impervious)) {print("Roofs and impervious are not yet working",quote = FALSE)}
  raw_roof_data = NULL
  if (!is.null(roofs)) {raw_roof_data = map_ar_clean[, ,cfmaps[cfmaps[,1] == "roofs",2]]}
  raw_impervious_data = NULL
  if (!is.null(impervious)) {raw_impervious_data = map_ar_clean[, ,cfmaps[cfmaps[,1] == "impervious",2]]}

  # ----- SMOOTH FLAG STILL NEEDS TO BE LOOKED AT AGAIN -----
  smooth_flag = FALSE

  # ------------------------------ Make flownet list ------------------------------
  print("Building flowtable",quote = FALSE)
  CF1 = patch_data_analysis(
    raw_patch_data = raw_patch_data,
    raw_patch_elevation_data = raw_patch_elevation_data,
    raw_basin_data = raw_basin_data,
    raw_hill_data = raw_hill_data,
    raw_zone_data = raw_zone_data,
    raw_slope_data = raw_slope_data,
    raw_stream_data = raw_stream_data,
    raw_road_data = raw_road_data,
    road_width = road_width,
    cell_length = cell_length,
    smooth_flag = smooth_flag,
    d4 = d4,
    parallel = parallel,
    make_stream = make_stream)

  # ------------------------------ Multiscale routing/aspatial patches ------------------------------
  if (!is.null(asp_list)) {
    CF1 = multiscale_flow(CF1 = CF1, map_ar_clean = map_ar_clean, cfmaps = cfmaps, asp_list = asp_list)
  }

  # ---------- Flownet list to flow table file ----------
  print("Writing flowtable",quote = FALSE)
  make_flow_table(flw = CF1, output_file = flownet_name, parallel = parallel)

  print(paste("Created flowtable:",flownet_name),quote = FALSE)

}
