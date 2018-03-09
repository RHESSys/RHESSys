#' World Gen
#'
#' Generates world files for use in RHESSys based on input template and maps,
#' currently includes functionality for GRASS GIS and raster data, and works on both unix and windows. 1/23/18.
#' @param template Template file used to generate worldfile for RHESSys. Generic strucutre is:
#' <state variable> <operator> <value/map>. Levels are difined by lines led by "_", structured
#' <levelname> <map> <count>. Whitespace and tabs are ignored.  Maps referred to must be supplied
#' by your chosen method of data input(GRASS or raster), set using the "type" arguement.
#' @param worldfile Name and path of worldfile to be created.
#' @param type Input file type to be used. "GRASS" will attempt to autodetect the
#' version of GRASS GIS being used (6.x or 7.x).  GRASS GIS type can also be set explicitly to "GRASS6" or "GRASS7".
#' "Raster" type will use rasters in GeoTiff or equivalent format, with file names matching those indicated in the template.
#' @param typepars Parameters needed based on input data type used. For GRASS GIS type, typepars is a
#' vector of 5 character strings. GRASS GIS parameters: gisBase, home, gisDbase, location, mapset.
#' Example parameters are included in an example script included in this package. See initGRASS help
#' for more info on parameters.  If using raster type, typepars should be a string indicating the path to a folder
#' containing the raster files that are referenced by the template. See help for the Raster function (in the Raster package)
#' for additional information and supported filetypes.
#' @param overwrite Overwrite existing worldfile. FALSE is default and prompts a menu if worldfile already exists.
#' @param asprules The path and filename to the rules file.  Using this argument enables aspatial patches.
#' @seealso \code{\link{initGRASS}}, \code{\link{readRAST}}, \code{\link{Raster}}
#' @author Will Burke

# ---------- Function start ----------
world_gen = function(template, worldfile, type = 'Raster', typepars, overwrite=FALSE, header = FALSE, asprules=NULL, wrapper = FALSE) {

  # ---------- Check inputs ----------
  if (!file.exists(template)) {
    print(paste("template does not exist or is not located at specified path:",template),quote=FALSE) #check if template exists
  }

  worldname = basename(worldfile)# Coerce .world extension
  if (startsWith(worldname,"World.") | startsWith(worldname,"world.")) {
    worldname = paste(substr(worldname,7,nchar(worldname)),".world",sep="")
  } else if(!endsWith(worldname,".world")) {
    worldname = paste(worldname,".world",sep="")
  }
  worldfile = file.path(dirname(worldfile),worldname)

  if (!is.logical(overwrite)) {stop("overwrite must be logical")} # check overwrite inputs
  if (file.exists(worldfile) & overwrite == FALSE) {
    t = menu(c("Yes","No"),title=paste("Worldfile",worldfile,"already exists. Overwrite?"))
    if (t==2) {stop("world_gen exited without completing")}
  }

  if (!is.null(asprules)) {asp_check = TRUE} else {asp_check = FALSE} # check for aspatial patches
  if (asp_check) { if(!file.exists(asprules) ) {asp_check=FALSE}}

  if (wrapper==FALSE){ # only run if function run alone
  fpath = ".extra_files" # hidden folder to store files later if needed
  dir.create(fpath,showWarnings = FALSE)
  }

  # ---------- Read in template ----------
  template_list = template_read(template)

  template_clean = template_list[[1]]
  var_names = template_list[[2]]
  level_index = template_list[[3]]
  var_index = template_list[[4]]
  map_info = template_list[[5]]
  head = template_list[[6]]
  maps_in = unique(map_info[,2])

  if (asp_check) { # if using aspatial patches, get rules value or map
    if(sum(var_names=="asp_rule")<1){
      stop("Missing asp_rule state variable in template")}}

  # ---------- spatial read in ----------
  read_maps = GIS_read(maps_in,type,typepars)

  # process map data  ----------
  map_df = as.data.frame(read_maps) #make data frame for ease of use
  cellarea = read_maps@grid@cellsize[1] * read_maps@grid@cellsize[2] # get cell area - need for area operator
  cellarea = rep(cellarea, length(map_df[,6]))

  # structure to iterate through levels ---- input object with unique ID's for each unit at each level, will iterate through
  w_map = map_info[map_info[,1]=="world",2]
  b_map = map_info[map_info[,1]=="basin",2]
  h_map = map_info[map_info[,1]=="hillslope",2]
  z_map = map_info[map_info[,1]=="zone",2]
  p_map = map_info[map_info[,1]=="patch",2]
  s_map = map_info[map_info[,1]=="strata",2]

  levels = unname(data.matrix(map_df[c(w_map,b_map,h_map,z_map,p_map,s_map)], length(map_df[p_map]) ))

  # ----------- Aspatial Patch Processing --> NAS INTRODUCED BY COERCION HERE -----

  lret = NULL
  if (asp_check) {
    asp_map = template_clean[[which(var_names=="asp_rule")]][3]
    if (!is.numeric(asp_map)) {
      asp_mapdata = map_df[asp_map]
    } else if (is.numeric(asp_map)) {
      asp_mapdata = asp_map
    }
    lret = aspatial_patches(asprules, asp_mapdata)
    rulevars = lret[[1]]
    strata_index = lret[[2]]
  }
  if(wrapper==FALSE){f = save(lret, file = paste(fpath,"/lret",sep = ""))}

  # ---------- Build list containing values based on template and maps ----------
  statevars = vector("list",length(template_clean))

  for (i in var_index) {
    level_agg = as.list(data.frame(levels[,1:sum(i > level_index)]))
    if (i > level_index[6]) { strata = 1:template_clean[[level_index[6]]][3] # for stratum level of template
    } else{strata = 1}
    for (s in strata) {
      if(template_clean[[i]][2] == "value") { #use value
        statevars[[i]][[s]] = as.double(template_clean[[i]][2+s])
      } else if(template_clean[[i]][2] == "dvalue") { #integer value
        statevars[[i]][[s]] = as.integer(template_clean[[i]][2+s])
      } else if(template_clean[[i]][2] == "aver") { #average
        maptmp = as.vector(t(map_df[template_clean[[i]][2+s]]))
        statevars[[i]][[s]] = aggregate(maptmp, by = level_agg, FUN = "mean")
      } else if(template_clean[[i]][2] == "mode") { #median
        maptmp = as.vector(t(map_df[template_clean[[i]][2+s]]))
        statevars[[i]][[s]] = aggregate(maptmp, by = level_agg, FUN = "median")
      } else if(template_clean[[i]][2] == "eqn") { # only for horizons old version -- use normal mean in future
        maptmp = as.vector(t(map_df[template_clean[[i]][5]]))
        statevars[[i]][[s]] = aggregate(maptmp, by = level_agg, FUN = "mean")
        statevars[[i]][[s]][,"x"] = statevars[[i]][[s]][,"x"] * as.numeric(template_clean[[i]][3])
      } else if(template_clean[[i]][2] == "spavg") { #spherical average
        maptmp = as.vector(t(map_df[template_clean[[i]][3]]))
        rad = (maptmp * pi)/(180) #convert to radians
        sin_avg = aggregate(sin(rad), by = level_agg, FUN = "mean") #avg sin
        cos_avg = aggregate(cos(rad), by = level_agg, FUN = "mean") #avg cos
        aspect_rad = atan2(sin_avg[,"x"],cos_avg[,"x"]) # sin and cos to tan
        aspect_deg = (aspect_rad * 180) / (pi) #rad to deg
        for (a in 1:length(aspect_deg)){
          if(aspect_deg[a]<0) { aspect_deg[a] = 360 + aspect_deg[a]}}
        statevars[[i]][[s]] = cos_avg
        statevars[[i]][[s]][,"x"] = aspect_deg
      } else if(template_clean[[i]][2] == "area") { #only for state var area
        statevars[[i]][[s]] = aggregate(cellarea, by = level_agg, FUN = "sum")
      } else { print(paste("Unexpected 2nd element on line",i)) }
    }
  }



  # ---------- Build world file ----------
  stratum = 1:template_clean[[level_index[6]]][3] #make sure correct number of stratum
  # create/open file
  sink(worldfile)

  # world - no state variables
  world = unique(levels[,1])
  cat(world,"\t\t\t","world_ID\n",sep="")
  num_basins = length(unique(levels[,2]))
  cat(num_basins,"\t\t\t","num_basins\n",sep="")

  basin = unique(levels[,2])
  for (b in basin) { #basins
    cat("\t",b,"\t\t\t", "basin_ID\n",sep="")
    for (i in (level_index[2]+1):(level_index[3]-1)) {
      if (length(statevars[[i]][[1]]) >1) {
        var = statevars[[i]][[1]][statevars[[i]][[1]][2]==b ,"x"]
      } else { var = statevars[[i]][[1]] }
      varname = template_clean[[i]][1]
      cat("\t",var,"\t\t\t",varname,"\n",sep="")
    }
    hillslopes = unique(levels[levels[,2]==b, 3])
    cat("\t",length(hillslopes),"\t\t\t","num_hillslopes\n",sep="")

    for (h in hillslopes) { #hillslopes
      cat("\t\t",h,"\t\t\t", "hillslope_ID\n",sep="")
      for (i in (level_index[3]+1):(level_index[4]-1)) {
        if (length(statevars[[i]][[1]]) >1) {
          var = statevars[[i]][[1]][statevars[[i]][[1]][2]==b & statevars[[i]][[1]][3]==h ,"x"]
        } else { var = statevars[[i]][[1]] }
        varname = template_clean[[i]][1]
        cat("\t\t",var,"\t\t\t",varname,"\n",sep="")
      }
      zones = unique(levels[levels[,3]==h & levels[,2]==b, 4])
      cat("\t\t",length(zones),"\t\t\t","num_zones\n",sep="")

      for (z in zones) { #zones
        cat("\t\t\t",z,"\t\t\t", "zone_ID\n",sep="")
        for (i in (level_index[4]+1):(level_index[5]-1)) {
          if (length(statevars[[i]][[1]]) >1) {
            var = statevars[[i]][[1]][statevars[[i]][[1]][2]==b & statevars[[i]][[1]][3]==h & statevars[[i]][[1]][4]==z ,"x"]
          } else { var = statevars[[i]][[1]] }
          varname = template_clean[[i]][1]
          cat("\t\t\t",var,"\t\t\t",varname,"\n",sep="")
        }
        patches = unique(levels[levels[,4]==z & levels[,3]==h & levels[,2]==b, 5])
        cat("\t\t\t",length(patches),"\t\t\t","num_patches\n",sep="") #Should this be the number of spatial patches?????????~~~~~~~~~~

        #---------- Start aspatial patches and stratum ----------
        if(asp_check) {
          if(is.data.frame(asp_mapdata)){
            levels = cbind(levels,unname(as.matrix(asp_mapdata)))
          } else if (is.numeric(asp_mapdata)) {
            levels = cbind(levels, rep(asp_mapdata,length(levels[,1])) )
          }

          for (p in patches){ #iterate through spatial patches
            ruleid = unique(levels[(levels[,5]==p & levels[,4]==z & levels[,3]==h & levels[,2]==b),7])
            if(length(ruleid)!=1){stop("something's wrong with the ruleid")}
            asp_index = 1:length(rulevars[[ruleid]])

            for (asp in asp_index){ #iterate through aspatial patches, 1-n for each spatial patch
              pnum = (p*100) + asp
              cat("\t\t\t\t",pnum,"\t\t\t", "patch_ID\n",sep="")
              if (p>1) {cat("\t\t\t\t",p,"\t\t\t", "patch_family\n",sep="")}

              rvpind = 1:(strata_index[[1]]-1)
              rvindex1 = which(! names(rulevars[[ruleid]][[asp]][rvpind]) %in% var_names[var_index])#include patch state vars from rulevars that aren't in template
              for (i in rvindex1) {
                var = rulevars[[ruleid]][[asp]][[i]][[1]]
                varname = names(rulevars[[ruleid]][[asp]][i])
                cat("\t\t\t\t",var,"\t\t\t",varname,"\n",sep="")
              }

              for (i in (level_index[5]+1):(level_index[6]-1)) { #iterate through template-based state variables
                if (length(statevars[[i]][[1]]) >1) {
                  var = statevars[[i]][[1]][statevars[[i]][[1]][2]==b & statevars[[i]][[1]][3]==h & statevars[[i]][[1]][4]==z & statevars[[i]][[1]][5]==p ,"x"]
                } else { var = statevars[[i]][[1]] }
                varname = template_clean[[i]][1]
                if(varname %in% names(rulevars[[ruleid]][[asp]])) {var = rulevars[[ruleid]][[asp]][[varname]][[1]] } # if variable is in rulevars, replace with rulevars version
                if(template_clean[[i]][1] =="area"){ var = var * rulevars[[ruleid]][[asp]][["pct_family_area"]][[1]] } # variable is area, adjust for pct_family_area
                cat("\t\t\t\t",var,"\t\t\t",varname,"\n",sep="")
              }

              rvsind = strata_index[[1]]:length(rulevars[[ruleid]][[asp]])
              rvslen = unlist(lapply(rulevars[[ruleid]][[asp]][rvsind],length))

              #if(sum(ifelse(length(stratum) != rvslen,TRUE,FALSE)) > 0 ) {
                #warning("Varying numbers of stratum in template and rules document. Values will be replicated to fill in missing strata.")
              #}

              cat("\t\t\t\t",length(stratum),"\t\t\t","num_stratum\n",sep="")

              for (s in stratum) { #stratum
                cat("\t\t\t\t\t", s,"\t\t\t", "stratum_ID\n",sep="")

                rvindex2 = which(! names(rulevars[[ruleid]][[asp]][rvsind]) %in% var_names[var_index])#include strata state vars from rulevars that aren't in template
                for (i in rvindex2) {
                  var = rulevars[[ruleid]][[asp]][[i]][[s]]
                  varname = names(rulevars[[ruleid]][[asp]][i])
                  cat("\t\t\t\t",var,"\t\t\t",varname,"\n",sep="")
                }

                for (i in (level_index[6]+1):length(template_clean)) {
                  if (length(statevars[[i]][[s]]) >1) {  ########## make sure this works @@@@/// is this needed???
                    var = statevars[[i]][[1]][statevars[[i]][ncol(statevars[[i]])-1]==p ,"x"]
                  } else { var = statevars[[i]][[s]] }
                  varname = template_clean[[i]][1]
                  if(varname %in% names(rulevars[[ruleid]][[asp]])) {var = rulevars[[ruleid]][[asp]][[varname]][[s]] } # if variable is in rulevars, replace with rulevars version
                  cat("\t\t\t\t\t",var,"\t\t\t",varname,"\n",sep="")

                }
              }
            }
          } # end aspatial patches ----------

        } else { # start standard patches and stratum ----------
          for (p in patches) { #patches
            cat("\t\t\t\t",p,"\t\t\t", "patch_ID\n",sep="")
            for (i in (level_index[5]+1):(level_index[6]-1)) {
              if (length(statevars[[i]][[1]]) >1) {
                var = statevars[[i]][[1]][statevars[[i]][[1]][2]==b & statevars[[i]][[1]][3]==h & statevars[[i]][[1]][4]==z & statevars[[i]][[1]][5]==p ,"x"]
              } else { var = statevars[[i]][[1]] }
              varname = template_clean[[i]][1]
              cat("\t\t\t\t",var,"\t\t\t",varname,"\n",sep="")
            }
            cat("\t\t\t\t",length(stratum),"\t\t\t","num_stratum\n",sep="")

            for (s in stratum) { #stratum
              cat("\t\t\t\t\t", s,"\t\t\t", "stratum_ID\n",sep="")
              for (i in (level_index[6]+1):length(template_clean)) {
                if (length(statevars[[i]][[s]]) >1) {  ########## make sure this works @@@@/// is this needed???
                  var = statevars[[i]][[1]][statevars[[i]][ncol(statevars[[i]])-1]==p ,"x"]
                } else { var = statevars[[i]][[s]] }
                varname = template_clean[[i]][1]
                cat("\t\t\t\t\t",var,"\t\t\t",varname,"\n",sep="")
              }
            }
          }# end non aspatial patch +stratum

        }
      }
    }
  }

  sink()

  print(paste("Created worldfile:",worldfile),quote=FALSE)

  if (header) {
    headfile = paste(substr(worldfile,0,(nchar(worldfile)-5)),"hdr",sep = "")
    write(head,file = headfile)
    print(paste("Created header file:",headfile),quote=FALSE)
  }

  #----------Output parameters for CreateFlownet-----------
  cfmaps = rbind(map_info,
                 c("cell_length",read_maps@grid@cellsize[1]),
                 c("streams","none"), c("roads","none"), c("impervious","none"),c("roofs","none"))

  # map_info[1:6,],map_info[map_info[,1] == "z",],
  # map_info[map_info[,1] == "slope",],
  # map_info[map_info[,1] == "asp_rule",]

  if (wrapper == FALSE) {
    f = file.create(paste(fpath, "/cf_maps", sep = ""))
    write.table(cfmaps, "cf_maps", sep = "\t\t", row.names = FALSE, quote = FALSE)
    f = save(typepars, file = paste(fpath, "/typepars", sep = ""))
  } else if (wrapper == TRUE) {
    world_gen_out = list(lret, cfmaps, typepars)
    return(world_gen_out)
  }

} # end function
