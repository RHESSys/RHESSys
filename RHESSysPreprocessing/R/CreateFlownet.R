#' CreateFlownet
#'
#' Creates the flow network used by RHESSys
#' @param cfname The name of the flow network file to be created.  Will be coerced to have ".flow" extension if not already present.
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


CreateFlownet = function(cfname,
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
                         parallel = FALSE,
                         make_stream = NULL,
                         d4 = FALSE){

  # ------------------------------ Read and check inputs ------------------------------
  cfbasename = basename(cfname) # Coerce .flow extension
  if (startsWith(cfbasename,"Flow.") | startsWith(cfbasename,"flow.")) {
    cfbasename = paste(substr(cfbasename,6,nchar(cfbasename)),".flow",sep="")
  } else if(!endsWith(cfbasename,".flow")) {
    cfbasename = paste(cfbasename,".flow",sep="")
  }
  cfname = file.path(dirname(cfname),cfbasename)

  if(!is.logical(overwrite)) {stop("overwrite must be logical")} # check overwrite inputs
  if(file.exists(cfname) & overwrite == FALSE) {stop(noquote(paste("Flowtable",cfname,"already exists.")))}

  if(!wrapper & is.character(readin)){ #if run outside of rhessyspreprocess.R, and if readin is character. readin is the template (and path)
    template_list = template_read(template)
    map_info = template_list[[5]]
    cfmaps = rbind(map_info,c("cell_length","none"), c("streams","none"), c("roads","none"), c("impervious","none"),c("roofs","none"))
  } else if(wrapper | (!wrapper & is.matrix(readin))){ # map info is passsed directly from world gen - either in wrapper or outside of wrapper and readin is matrix
    cfmaps = readin
  }

  # Check for streams map, menu allows input of stream map
  if(is.null(streams) & (cfmaps[cfmaps[,1]=="streams",2]=="none"|is.na(cfmaps[cfmaps[,1]=="streams",2]))) {
    t = menu(c("Specify map","Abort function"),
             title="Missing stream map. Specify one now, or abort function and edit cf_maps file?")
    if(t==2) {stop("Function aborted")}
    if(t==1) {
      streams = readline("Stream map:")
    }
  }
  if((cfmaps[cfmaps[,1]=="streams",2]=="none"|is.na(cfmaps[cfmaps[,1]=="streams",2]))){ # only add stream map to cfmaps if it's not there already
    cfmaps[cfmaps[,1]=="streams",2] = streams
  }

  if(!is.null(roads)){cfmaps[cfmaps[,1]=="roads",2]=roads}
  if(!is.null(impervious)){cfmaps[cfmaps[,1]=="impervious",2]=impervious}
  if(!is.null(roofs)){cfmaps[cfmaps[,1]=="roofs",2]=roofs}

  maps_in = unique(cfmaps[cfmaps[,2]!="none" & cfmaps[,1]!="cell_length",2])

  # ------------------------------ Use GIS_read to get maps ------------------------------
  readmap = GIS_read(maps_in, type, typepars, map_info = cfmaps)
  map_ar_clean = as.array(readmap)
  dimnames(map_ar_clean)[[3]] = colnames(readmap@data)

  raw_patch_data = map_ar_clean[, ,cfmaps[cfmaps[,1]=="patch",2]]
  raw_patch_elevation_data = map_ar_clean[, ,unique(cfmaps[cfmaps[,1]=="z",2])]
  raw_hill_data = map_ar_clean[, ,cfmaps[cfmaps[,1]=="hillslope",2]]
  raw_basin_data = map_ar_clean[, ,cfmaps[cfmaps[,1]=="basin",2]]
  raw_zone_data = map_ar_clean[, ,cfmaps[cfmaps[,1]=="zone",2]]
  raw_slope_data = map_ar_clean[, ,unique(cfmaps[cfmaps[,1]=="slope",2])]
  raw_stream_data = map_ar_clean[, ,cfmaps[cfmaps[,1]=="streams",2]]
  cell_length = readmap@grid@cellsize[1]

  raw_road_data = NULL
  if(!is.null(roads)){raw_road_data = map_ar_clean[, ,cfmaps[cfmaps[,1]=="roads",2]]}

  # Roofs and impervious is not yet implemented - placeholders for now -----
  if(!is.null(roofs)|!is.null(impervious)){print("Roofs and impervious are not yet working",quote = FALSE)}
  raw_roof_data = NULL
  if(!is.null(roofs)){raw_roof_data = map_ar_clean[, ,cfmaps[cfmaps[,1]=="roofs",2]]}
  raw_impervious_data = NULL
  if(!is.null(impervious)){raw_impervious_data = map_ar_clean[, ,cfmaps[cfmaps[,1]=="impervious",2]]}

  # ----- SMOOTH FLAG STILL NEEDS TO BE LOOKED AT AGAIN -----
  smooth_flag = FALSE

  # ------------------------------ Make flownet list ------------------------------
  print("Building flowtable",quote=FALSE)
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
    cell_length=cell_length,
    smooth_flag=smooth_flag,
    d4 = d4,
    parallel = parallel,
    make_stream = make_stream)

  # ------------------------------ Multiscale routing/aspatial patches ------------------------------
  if(!is.null(asp_list)){
    # import: existing flownet, asp/rule map, rule list with proportionate areas
    asp_map = map_ar_clean[, ,cfmaps[cfmaps[,1]=="asp_rule",2]]
    patch_ID = unlist(lapply(CF1, "[[",9)) # patch IDs from cf1
    numbers = unlist(lapply(CF1, "[[",1)) # flow list numbers
    rulevars = asp_list[[1]] # subset rules by ID
    CF2 = list() # empty list for new flow list

    for(p in raw_patch_data[!is.na(raw_patch_data)] ){ # iterate through physical patches
      id = asp_map[which(raw_patch_data==p)] # get rule ID for patch p
      id = unique(id)
      if(length(id)>1){stop(paste("multiple aspatial rules for patch",p))} # if multiple rules for a single patch
      asp_count = length(rulevars[[id]]) # get number of aspatial patches for current patch

      for(asp in 1:asp_count){ #iterate through aspatial patches
        # Add all aspatial patches
        CF2 = c(CF2,CF1[which(patch_ID==p)])
        CF2[[length(CF2)]]$PatchID = CF2[[length(CF2)]]$PatchID * 100 + asp # aspatial patch ID is old patch ID *100 + aspatial number
        CF2[[length(CF2)]]$Number = CF2[[length(CF2)]]$Number * 100 + asp # same modification to number
        CF2[[length(CF2)]]["PatchFamilyID"] = CF1[[which(patch_ID==p)]]$PatchID # retain old patch ID as patch family ID XXXXXXXXX IF CF2 DOESNT WORK REMOVE THIS
        CF2[[length(CF2)]]$Area = CF2[[length(CF2)]]$Area * rulevars[[id]][[asp]]$pct_family_area[[1]] # change area

        #STILL NEED:
        # TOTAL GAMMA?

        # Changes for each neighbor
        old_nbrs = CF2[[length(CF2)]]$Neighbors
        new_nbrs = vector(mode="numeric")
        old_gammas = CF2[[length(CF2)]]$Gamma_i
        new_gammas = vector(mode="numeric")
        old_slope = CF2[[length(CF2)]]$Slope
        new_slope = vector(mode="numeric")
        old_boarder = CF2[[length(CF2)]]$Boarder
        new_boarder = vector(mode="numeric")

        for(nbr in old_nbrs){ # loop through old neighbors - neighbors are numbers not patches
          nbr_patch = patch_ID[numbers==nbr]
          nbr_id = asp_map[which(raw_patch_data==nbr_patch)]
          nbr_id = unique(nbr_id)
          if(length(nbr_id)>1){stop(paste("multiple aspatial rules for patch",nbr_patch))} # if multiple rules for a single patch
          nbr_asp_ct =length(rulevars[[nbr_id]])
          gamma = old_gammas[which(old_nbrs==nbr)]
          new_slope = c(new_slope,rep(old_slope[old_nbrs[nbr]],nbr_asp_ct))
          new_boarder = c(new_boarder,rep(old_boarder[old_nbrs[nbr]],nbr_asp_ct))

          for(nbr_asp in 1:nbr_asp_ct){ # for each asp for each neighbor
            new_nbrs = c(new_nbrs,nbr*100+nbr_asp) # use same convention as above
            new_gammas = c(new_gammas, gamma * rulevars[[nbr_id]][[nbr_asp]][["pct_family_area"]][[1]] )

          }
        }
        CF2[[length(CF2)]]$Neighbors = new_nbrs
        CF2[[length(CF2)]]$Gamma_i = new_gammas
        CF2[[length(CF2)]]$Slope = new_slope
        CF2[[length(CF2)]]$Boarder = new_boarder

      } # end aspatial patch loop
    } # end spatial patch loop
    CF1 = CF2
  } # end multiscale routing

  # ---------- Flownet list to flow table file ----------
  print("Writing flowtable",quote=FALSE)
  make_flow_table(flw = CF1, output_file = cfname, parallel = parallel)

  print(paste("Created flowtable:",cfname),quote=FALSE)

}

