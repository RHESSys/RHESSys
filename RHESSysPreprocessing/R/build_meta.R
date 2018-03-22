#' build_meta
#'
#' Create documentation/metadata for RHESSys Preprocessing. 2/27/18
#' @param name Name and path for metadata. ".meta" will be appended automatically.
#' @return Writes metadata with name and path indicated in name
#' @author Will Burke
#'

# Include: spatial data used for each input, original source? , author, computer used, date time, worldfile and flownet produced

build_meta = function(name,
                      world = NULL,
                      flow = NULL,
                      template,
                      type,
                      typepars,
                      cf_maps,
                      streams = NULL,
                      roads = NULL,
                      impervious = NULL,
                      roofs = NULL,
                      asp_rule = NULL) {

  #---------- Build table ----------

  table_name = paste(name,".meta",sep="")

  template_list = template_read(template)

  vars = c(
    "Project Name",
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
    "mapset",
    "world",
    "basin",
    "zone",
    "hillslope",
    "patch",
    "strata",
    "streams",
    "roads",
    "impervious",
    "roofs",
    "asp_rule",
    "z",
    "slope",
    "aspect",
    "e_horizon",
    "w_horizon",
    "gw.storage",
    "veg_parm_ID",
    "soil_parm_ID",
    "landuse_parm_ID"
  )

  meta_out = matrix(ncol = 2, nrow = length(vars))
  meta_out[,1] = vars
  colnames(meta_out) = c("varname", "value")
  rownames(meta_out) = vars

  #----------- System info ----------
  get_sys = Sys.info()

  meta_out["Author", 2] = get_sys["user"]
  meta_out["Computer", 2] = get_sys["nodename"]
  meta_out["Datetime", 2] = as.character(Sys.time())

  #---------- Map info ----------

  meta_out["Project Name",2] = basename(name)
  meta_out["Worldfile",2] = world
  meta_out["Template",2] = template
  meta_out["Flowtable",2] = flow

  if (type == "Raster" | type == "raster" | type == "RASTER") {
    meta_out["GIS Type", 2] = "Raster"
    meta_out["Map Path", 2] = paste(getwd(),typepars,sep="")
  } else{
    meta_out["GIS Type", 2] = type
    meta_out["gisBase", 2] = typepars[1]
    meta_out["home", 2] = typepars[2]
    meta_out["gisDbase", 2] = typepars[3]
    meta_out["location", 2] = typepars[4]
    meta_out["mapset", 2] = typepars[5]
  }

  if(!is.null(roads)){meta_out["roads",2] = roads}
  if(!is.null(impervious)){meta_out["impervious",2] = impervious}
  if(!is.null(roofs)){meta_out["roofs",2] = roofs}
  if(!is.null(streams)){meta_out["streams",2] = streams}
  if(!is.null(asp_rule)){meta_out["asp_rule",2] = asp_rule}

    for (i in which(meta_out[,1] =="z"):length(meta_out[,1])) {
    if(meta_out[i,1] %in% template_list[[5]][,1]){
      meta_out[i,2] = template_list[[5]][template_list[[5]][,1]==meta_out[i,1],2]
    } else if(sum(meta_out[i,1] == template_list[[2]])==1) {
      meta_out[i,2] = template_list[[1]][[which(meta_out[i,1] == template_list[[2]])]][3]
      }
  }

  write.table(meta_out,file = table_name,row.names = FALSE)

  print(paste("Created metadata:",table_name),quote=FALSE)

}
