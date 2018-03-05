#' build_meta
#'
#' Create documentation/metadata for RHESSys Preprocessing. 2/27/18
#' @param name Name and path for metadata. ".meta" will be appended automatically.
#' @return Writes metadata with name and path indicated in name
#' @author Will Burke
#'

# Include: spatial data used for each input, original source? , author, computer used, date time, worldfile and flownet produced

build_meta = function(name,
                      template,
                      worldfile,
                      flownet,
                      type,
                      typepars,
                      cf_maps,
                      streams,
                      roads=NULL,
                      impervious=NULL,
                      roofs=NULL) {

  table_name = paste(name,".meta",sep="")

  #---------- Build table ----------

  vars = c(
    "Template",
    "Worldfile",
    "Flowtable",
    "Author",
    "Computer",
    "Datetime",
    "GIS Type",
    "Map Path",
    "gisBase",
    "home",
    "gisDbase",
    "location",
    "mapset"
  )

  # "World",
  # "Basin",
  # "Zone",
  # "Hillslope",
  # "Patch",
  # "Strata",
  #
  # "Elevation",
  # "Slope",
  # "Aspect",
  # "Vegetation",
  # "Soils",
  # "Groundwater Storage",
  # "East Horizon",
  # "West Horizon",
  # "Rule",
  # "Streams",
  # "Roads",
  # "Impervious",
  # "Roofs",

  meta_out = matrix(ncol = 2, nrow = length(vars))
  meta_out[, 1] = vars
  colnames(meta_out) = c("varname", "value")
  rownames(meta_out) = vars

  #----------- Sytem info ----------
  get_sys = Sys.info()

  meta_out["Author", 2] = get_sys["user"]
  meta_out["Computer", 2] = get_sys["nodename"]
  meta_out["Datetime", 2] = as.character(Sys.time())

  as.character(Sys.time())
  #write.table(Sys.time(),"",row.names = FALSE)

  #---------- Map info ----------

  meta_out["Worldfile",2] = worldfile
  meta_out["Template",2] = template
  meta_out["Flowtable",2] = flownet

  if (type == "Raster" | type == "raster" | type == "RASTER") {
    meta_out["GIS Type", 2] = "Raster"
    meta_out["Map Path", 2] = paste(getwd(),typepars,sep="")
  } else{
    meta_out["Type", 2] = type
    meta_out["gisBase", 2] = typepars[1]
    meta_out["home", 2] = typepars[2]
    meta_out["gisDbase", 2] = typepars[3]
    meta_out["location", 2] = typepars[4]
    meta_out["mapset", 2] = typepars[5]
  }

  # meta_out["World", 2] = cf_maps[cf_maps[, 1] == "world", 2]
  # meta_out["Basin", 2] = cf_maps[cf_maps[, 1] == "basin", 2]
  # meta_out["Zone", 2] = cf_maps[cf_maps[, 1] == "zone", 2]
  # meta_out["Hillslope", 2] = cf_maps[cf_maps[, 1] == "hillslope", 2]
  # meta_out["Patch", 2] = cf_maps[cf_maps[, 1] == "patch", 2]
  # meta_out["Strata", 2] = cf_maps[cf_maps[, 1] == "strata", 2]

  # meta_out["Rule", 2] = cf_maps[cf_maps[, 1] == "asp_rule", 2]
  # meta_out["Streams", 2] = cf_maps[cf_maps[, 1] == "stream", 2]
  # meta_out["Roads", 2] = cf_maps[cf_maps[, 1] == "roads", 2]
  # meta_out["Impervious", 2] = cf_maps[cf_maps[, 1] == "impervious", 2]
  # meta_out["Roofs", 2] = cf_maps[cf_maps[, 1] == "roofs", 2]
  # #meta_out["Vegetation", 2] = cf_maps[cf_maps[, 1] == "vegetation", 2]
  # meta_out["Elevation", 2] = cf_maps[cf_maps[, 1] == "z", 2]
  # meta_out["Groundwater Storage", 2] = cf_maps[cf_maps[, 1] == "gw.storage", 2]
  # meta_out["Slope", 2] = cf_maps[cf_maps[, 1] == "slope", 2]
  # meta_out["Aspect", 2] = cf_maps[cf_maps[, 1] == "aspect", 2]
  # meta_out["East Horizon", 2] = cf_maps[cf_maps[, 1] == "e_horizon", 2]
  # meta_out["West Horizon", 2] = cf_maps[cf_maps[, 1] == "w_horizon", 2]
  # meta_out["Soils", 2] = cf_maps[cf_maps[, 1] == "soil_parm_ID", 2]

  meta_out = rbind(meta_out,cf_maps[cf_maps[,2]!="cell_length",])

  meta_out[meta_out[,1]=="streams",2] = streams

  if(!is.null(roads)){meta_out["roads",2] = roads}
  if(!is.null(impervious)){meta_out["impervious",2] = impervious}
  if(!is.null(roofs)){meta_out["roofs",2] = roofs}

  write.table(meta_out,file = table_name,row.names = FALSE)

  print(paste("Created Metadata:",table_name))

}
