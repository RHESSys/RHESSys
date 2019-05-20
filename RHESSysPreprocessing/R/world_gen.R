#' World Gen
#'
#' Generates world files for use in RHESSys based on input template and maps,
#' currently includes functionality for GRASS GIS and raster data, and works on both unix and windows. 1/23/18.
#' @param template Template file used to generate worldfile for RHESSys. Generic strucutre is:
#' <state variable> <operator> <value/map>. Levels are difined by lines led by "_", structured
#' <levelname> <map> <count>. Whitespace and tabs are ignored.  Maps referred to must be supplied
#' by your chosen method of data input(GRASS or raster), set using the "type" arguement.
#' @param worldfile Name and path of worldfile to be created.
#' @param type Input file type to be used. Default is raster. "Raster" type will use rasters
#' in GeoTiff or equivalent format (see Raster package), with file names  matching those indicated in the template.
#' ASCII is supported, but 0's cannot be used as values for data. "GRASS" will attempt to autodetect the version of
#' GRASS GIS being used (6.x or 7.x).  GRASS GIS type can also be set explicitly to "GRASS6" or "GRASS7".
#' @param typepars Parameters needed based on input data type used. If using raster type, typepars should be a string
#' indicating the path to a folder containing the raster files that are referenced by the template.
#' For GRASS GIS type, typepars is a vector of 5 character strings. GRASS GIS parameters: gisBase, home, gisDbase, location, mapset.
#' Example parameters are included in an example script included in this package. See initGRASS help
#' for more info on parameters.
#' @param overwrite Overwrite existing worldfile. FALSE is default and prompts a menu if worldfile already exists.
#' @param asprules The path and filename to the rules file.  Using this argument enables aspatial patches.
#' @seealso \code{\link{raster}}
#' @author Will Burke
#' @export

world_gen = function(template,
                     worldfile,
                     type = 'Raster',
                     typepars,
                     overwrite = FALSE,
                     header = FALSE,
                     unique_strata_ID = TRUE,
                     asprules = NULL,
                     wrapper = FALSE) {

  # -------------------- Input & Error Checking --------------------
  options(scipen = 999) # no scientific notation - prevents automatic conversion later

  if (!file.exists(template)) {
    print(paste("Template does not exist or is not located at specified path:",template),quote = FALSE) #check if template exists
  }

  if (!is.logical(overwrite)) {stop("overwrite must be logical")} # check overwrite inputs
  if (file.exists(worldfile) & overwrite == FALSE) {stop(noquote(paste("Worldfile",worldfile,"already exists.")))}

  if (!is.null(asprules)) {asp_check = TRUE} else {asp_check = FALSE} # check for aspatial patches
  if (asp_check) { if (!file.exists(asprules) ) {asp_check = FALSE}}

  # ----- File Name Check+Conversion -----
  worldname = basename(worldfile)# Coerce .world extension
  if (startsWith(worldname,"World.") | startsWith(worldname,"world.")) {
    worldname = paste(substr(worldname,7,nchar(worldname)),".world",sep = "")
  } else if (!endsWith(worldname,".world")) {
    worldname = paste(worldname,".world",sep = "")
  }
  worldfile = file.path(dirname(worldfile),worldname)

  # -------------------- Read in Template --------------------
  template_list = template_read(template)

  template_clean = template_list[[1]] # template in list form
  var_names = template_list[[2]] # names of template vars
  level_index = template_list[[3]] # index of level separators in template_clean/var_names
  var_index = template_list[[4]] # index of vars
  map_info = template_list[[5]] # tables of maps and their inputs/names in the template
  head = template_list[[6]] # header
  maps_in = unique(map_info[,2])

  # if using aspatial patches, check for aspatial rules state variable in template
  if (asp_check) {
    if (sum(var_names == "asp_rule") < 1) {
      stop(noquote("Missing asp_rule state variable in template"))
    }
  }

  # -------------------- Read in Maps --------------------
  read_maps = GIS_read(maps_in,type,typepars,map_info)

  # process map data
  if (length(read_maps@data[,1]) == 1){
    map_df = as.data.frame(read_maps@data) # works for 1 patch world
  } else {
    map_df = as.data.frame(read_maps) #make data frame for ease of use
  }

  cell_len = read_maps@grid@cellsize[1] # cell length for output
  cellarea = read_maps@grid@cellsize[1] * read_maps@grid@cellsize[2] # get cell area - need for area operator
  cellarea = rep(cellarea, length(map_df[,1]))
  rm(read_maps) # read_maps may be very large if maps are large

  # structure to iterate through spatial levels ---- matrix with unique ID's for each unit at each level
  w_map = map_info[map_info[,1] == "world",2]
  b_map = map_info[map_info[,1] == "basin",2]
  h_map = map_info[map_info[, 1] == "hillslope", 2]
  z_map = map_info[map_info[, 1] == "zone", 2]
  p_map = map_info[map_info[, 1] == "patch", 2]
  s_map = map_info[map_info[, 1] == "strata", 2]
  levels = unname(data.matrix(map_df[c(w_map,b_map,h_map,z_map,p_map,s_map)], length(map_df[p_map]) ))

  # -------------------- Aspatial Patch Processing --------------------
  rulevars = NULL
  if (asp_check) {
    asp_map = template_clean[[which(var_names == "asp_rule")]][3] # get rule map/value
    if (!is.numeric(asp_map)) { # if it's a map
      asp_mapdata = map_df[asp_map]
    } else if (is.numeric(asp_map)) { # if is a single number
      asp_mapdata = asp_map
    }
    rulevars = aspatial_patches(asprules = asprules, asp_mapdata = asp_mapdata)

    if (is.data.frame(asp_mapdata)) { # add ruleID to levels matrix
      levels = cbind(levels,unname(as.matrix(asp_mapdata)))
    } else if (is.numeric(asp_mapdata)) {
      levels = cbind(levels, rep(asp_mapdata,length(levels[,1])) )
    }
  }

  # -------------------- Additional Input Checking --------------------
  # Error checking for basestationID/number - it's a loop :/ but short so not a big deal
  # If n_basestations is 0 -> do NOT include base_station_ID
  # If n_basestations is > 0 -> must include base_station_ID, even if doing redefine w/ -9999
  n_basestations_index = which(var_names == "n_basestations")

  # get n basestations , check that strata are the same
  n_base = sapply(template_clean[n_basestations_index],FUN = function(x) as.integer(unique(x[3:length(x)])))
  if (length(n_base[[5]]) > 1) {stop(noquote("Canopy Strata n_basestations are inconsistent"))}
  n_base_nl = template_clean[n_basestations_index + 1] # get next line
  id_bad = n_base == 0 & !sapply(n_base_nl,is.null) # t/t for if n_base is 0 and next line IS base_station_ID
  id_bad[id_bad] = grepl("base_station_ID",unlist(lapply(n_base_nl[id_bad],"[[",1))) # get var names of next lines, update id_bad

  id_need = n_base > 0 & !sapply(n_base_nl,is.null) # t/f for if n_base is >0 and next line IS NOT base_station_ID
  id_need[id_need] = !grepl("base_station_ID",unlist(lapply(n_base_nl[id_need],"[[",1))) # get var names of next lines, update id_bad

  if (any(id_need)) {
    i_need = n_basestations_index[id_need]
    stop(noquote(paste("n_basestations on template line(s)",paste(i_need,collapse = ", "),
                       "is >0 & the following line is missing a base_station_ID.\n Either set n_basestations to 0 or add the base_station_ID.")))
  }

  if (any(id_bad)) {
    # indices to remove
    i_rm = n_basestations_index[id_bad]
    # removes from list and names vector regardless of if ID is -9999 or not
    template_clean = template_clean[-(i_rm + 1)]
    var_names = var_names[-(i_rm + 1)]
    # shift indices
    level_index[level_index > min(i_rm + 1)] = level_index[level_index > min(i_rm + 1)] - 1
    var_index = var_index[-which(var_index == i_rm + 1)]
    var_index[var_index > min(i_rm + 1)] = var_index[var_index > min(i_rm + 1)] - 1
    print(paste("n_basestations on template line(s)", paste(i_rm,collapse = ", "),
                "is 0. The base_station_ID on the following line has been omitted from the worldfile."),quote = FALSE)
  }

  # for (i in n_basestations_index) {
  #   if n_basestations is -9999 & if the next line is basestation ID
  #   if (template_clean[[i]][3] == -9999) {
  #     if (grepl("base_station_ID", template_clean[[i + 1]][1])) {
  #       # removes from list and names vector regardless of if ID is -9999 or not
  #       template_clean = template_clean[-(i + 1)]
  #       var_names = var_names[-(i + 1)]
  #       # shift indices
  #       level_index[level_index > (i + 1)] = level_index[level_index > (i + 1)] - 1
  #       var_index[var_index > (i + 1)] = var_index[var_index > (i + 1)] - 1
  #     }
  #   }
  # }

  # -------------------- Process Template + Maps --------------------
  # Build list based on operations called for by template
  statevars = vector("list",length(template_clean))

  for (i in var_index) {
    #level_agg = as.list(data.frame(levels[, 1:sum(i > level_index)]))
    if (nrow(levels) == 1) {
      level_agg = unname(split(levels[,i > level_index],f = seq_along(levels[,i > level_index])))
    } else {
      level_agg = as.list(data.frame(levels[,i > level_index]))
    }

    if (i > level_index[6]) {
      strata = 1:template_clean[[level_index[6]]][3] # for stratum level of template
    } else{
      strata = 1
    }

    for (s in strata) {
      # evalueate based on operator at 2nd element
      if (template_clean[[i]][2] == "value") { #use value
        if (suppressWarnings(all(is.na(as.numeric(template_clean[[i]][2 + s]))))) {
          stop(noquote(paste("\"",template_clean[[i]][2 + s],"\" on template line ",i," is not a valid value.",sep = "")))
        }
        statevars[[i]][[s]] = as.double(template_clean[[i]][2 + s])

      } else if (template_clean[[i]][2] == "dvalue") { #integer value
        statevars[[i]][[s]] = as.integer(template_clean[[i]][2 + s])

      } else if (template_clean[[i]][2] == "aver") { #average
        maptmp = as.vector(t(map_df[template_clean[[i]][2 + s]]))
        statevars[[i]][[s]] = aggregate(maptmp, by = level_agg, FUN = "mean")

      } else if (template_clean[[i]][2] == "mode") { #mode
        maptmp = as.vector(t(map_df[template_clean[[i]][2 + s]]))
        statevars[[i]][[s]] = aggregate(
          maptmp,
          by = level_agg,
          FUN = function(x) {
            ux <- unique(x)
            ux[which.max(tabulate(match(x, ux)))]
          }
        )

      } else if (template_clean[[i]][2] == "eqn") { # only for horizons old version -- use normal mean in future
        maptmp = as.vector(t(map_df[template_clean[[i]][5]]))
        statevars[[i]][[s]] = aggregate(maptmp, by = level_agg, FUN = "mean")
        statevars[[i]][[s]][, "x"] = statevars[[i]][[s]][, "x"] * as.numeric(template_clean[[i]][3])

      } else if (template_clean[[i]][2] == "spavg") { #spherical average
        maptmp = as.vector(t(map_df[template_clean[[i]][3]]))
        rad = (maptmp * pi) / (180) #convert to radians
        sin_avg = aggregate(sin(rad), by = level_agg, FUN = "mean") #avg sin
        cos_avg = aggregate(cos(rad), by = level_agg, FUN = "mean") #avg cos
        aspect_rad = atan2(sin_avg[, "x"], cos_avg[, "x"]) # sin and cos to tan
        aspect_deg = (aspect_rad * 180) / (pi) #rad to deg
        for (a in 1:length(aspect_deg)) {
          if (aspect_deg[a] < 0) {
            aspect_deg[a] = 360 + aspect_deg[a]
          }
        }
        statevars[[i]][[s]] = cos_avg
        statevars[[i]][[s]][, "x"] = aspect_deg
      } else if (template_clean[[i]][2] == "area") { #only for state var area
        statevars[[i]][[s]] = aggregate(cellarea, by = level_agg, FUN = "sum")

      } else {
        print(paste("Unexpected 2nd element on line", i))
      }
    }
  }

  # -------------------- Write World File --------------------

  print("Writing worldfile",quote = FALSE)
  stratum = 1:template_clean[[level_index[6]]][3] # count of stratum

  # ----- Progress Bar -----
  # Iterates at hillslope level, shouldnt slow code too much
  progress = 0
  pb = txtProgressBar(min = 0, max = 1,style = 3)
  setTxtProgressBar(pb,0)

  # create/open connection
  wcon = file(worldfile,open = "wt", encoding = "ASCII")

  # ----- World -----
  # No state variables at world level
  world = unique(levels[,1])
  writeChar(paste(world,"\t\t\t","world_ID\n",sep = ""),con = wcon, eos = NULL)
  writeChar(paste(length(unique(levels[,2])),"\t\t\t","num_basins\n",sep = ""),con = wcon,eos = NULL)
  basin = unique(levels[,2])

  # ----- Basin -----
  for (b in basin) {
    writeChar(paste("\t",b,"\t\t\t","basin_ID\n",sep = ""),con = wcon,eos = NULL)

    for (i in (level_index[2] + 1):(level_index[3] - 1)) {
      if (length(statevars[[i]][[1]]) > 1) {
        var = statevars[[i]][[1]][statevars[[i]][[1]][2] == b ,"x"]
      } else {var = statevars[[i]][[1]]}
      varname = template_clean[[i]][1]
      writeChar(paste("\t",var,"\t\t\t",varname,"\n",sep = ""),con = wcon,eos = NULL)
    }
    hillslopes = unique(levels[levels[,2] == b, 3])
    writeChar(paste("\t",length(hillslopes),"\t\t\t","num_hillslopes\n",sep = ""),con = wcon,eos = NULL)

    # ----- Hillslope -----
    for (h in hillslopes) {

      # Iterate progress bar
      progress = progress + 1
      setTxtProgressBar(pb,progress/length(unique(levels[,3])))

      writeChar(paste("\t\t",h,"\t\t\t","hillslope_ID\n",sep = ""),con = wcon,eos = NULL)

      for (i in (level_index[3] + 1):(level_index[4] - 1)) {
        if (length(statevars[[i]][[1]]) > 1) {
          var = statevars[[i]][[1]][statevars[[i]][[1]][2] == b & statevars[[i]][[1]][3] == h ,"x"]
        } else {var = statevars[[i]][[1]]}
        varname = template_clean[[i]][1]
        writeChar(paste("\t\t",var,"\t\t\t",varname,"\n",sep = ""),con = wcon,eos = NULL)
      }
      zones = unique(levels[levels[,3] == h & levels[,2] == b, 4])
      writeChar(paste("\t\t",length(zones),"\t\t\t","num_zones\n",sep = ""),con = wcon,eos = NULL)

      # ----- Zone -----
      for (z in zones) {
        writeChar(paste("\t\t\t",z,"\t\t\t","zone_ID\n",sep = ""),con = wcon,eos = NULL)

        for (i in (level_index[4] + 1):(level_index[5] - 1)) {
          if (length(statevars[[i]][[1]]) > 1) {
            var = statevars[[i]][[1]][statevars[[i]][[1]][2] == b & statevars[[i]][[1]][3] == h & statevars[[i]][[1]][4] == z ,"x"]
          } else {var = statevars[[i]][[1]]}
          varname = template_clean[[i]][1]
          writeChar(paste("\t\t\t",var,"\t\t\t",varname,"\n",sep = ""),con = wcon,eos = NULL)
        }

        #---------- Start multiscale (aspatial) patches and stratum ----------
        if (asp_check) {
          patches = unique(levels[levels[,4] == z & levels[,3] == h & levels[,2] == b, 5])
          asp_ct = sapply(rulevars, FUN = function(x) ncol(x[[1]]) - 1)
          total_patches = sum(asp_ct[levels[levels[,5] == patches & levels[,4] == z & levels[,3] == h & levels[,2] == b, 7]])

          writeChar(paste("\t\t\t",total_patches,"\t\t\t","num_patches\n",sep = ""),con = wcon,eos = NULL)

          # ----- Patches (spatial) -----
          for (p in patches) {
            ruleid = unique(levels[(levels[,5] == p & levels[,4] == z & levels[,3] == h & levels[,2] == b),7])
            if (length(ruleid) != 1) {stop("something's wrong with the ruleid")}
            asp_index = 1:(length(rulevars[[ruleid]]$patch_level_vars[1,]) - 1)

            # ----- Patches (non-spatial) -----
            for (asp in asp_index) {
              pnum = (p*100) + asp # adjust patch numbers here - adds two 0's, ie: patch 1 becomes patches 101, 102, etc.
              writeChar(paste("\t\t\t\t",pnum,"\t\t\t","patch_ID\n",sep = ""),con = wcon,eos = NULL)
              writeChar(paste("\t\t\t\t",p,"\t\t\t","patch_family\n",sep = ""),con = wcon,eos = NULL)

              asp_p_vars = which(!rulevars[[ruleid]]$patch_level_vars[,1] %in% var_names[var_index]) # get vars from aspatial not included in template

              for (i in asp_p_vars) {
                var = as.numeric(rulevars[[ruleid]]$patch_level_vars[i,asp + 1])
                varname = rulevars[[ruleid]]$patch_level_vars[i,1]
                if (is.na(var)) {stop(paste(varname,"cannot be NA since a default isn't specified in the template, please set explicitly in your rules file."))}
                writeChar(paste("\t\t\t\t",var,"\t\t\t",varname,"\n",sep = ""),con = wcon,eos = NULL)
              }

              for (i in (level_index[5] + 1):(level_index[6] - 1)) { #iterate through template-based state variables
                if (length(statevars[[i]][[1]]) > 1) {
                  var = statevars[[i]][[1]][statevars[[i]][[1]][2] == b & statevars[[i]][[1]][3] == h & statevars[[i]][[1]][4] == z & statevars[[i]][[1]][5] == p ,"x"]
                } else {var = statevars[[i]][[1]]}
                varname = template_clean[[i]][1]
                if (varname %in% rulevars[[ruleid]]$patch_level_vars[,1]) { # if variable is in rulevars, replace with rulevars version
                  if (!is.na(rulevars[[ruleid]]$patch_level_vars[rulevars[[ruleid]]$patch_level_vars[,1] == varname, asp + 1])) {
                    var = as.numeric(rulevars[[ruleid]]$patch_level_vars[rulevars[[ruleid]]$patch_level_vars[,1] == varname, asp + 1])
                  }
                }
                if (varname == "area") { # variable is area, adjust for pct_family_area
                  var = var * as.numeric(rulevars[[ruleid]]$patch_level_vars[rulevars[[ruleid]]$patch_level_vars[,1] == "pct_family_area",asp + 1])
                }
                writeChar(paste("\t\t\t\t",var,"\t\t\t",varname,"\n",sep = ""),con = wcon,eos = NULL)
              }

              asp_strata_ct = length(rulevars[[ruleid]]$strata_level_vars[[asp]][1,]) - 1
              strata_ct = asp_strata_ct

              writeChar(paste("\t\t\t\t",strata_ct,"\t\t\t","num_stratum\n",sep = ""),con = wcon,eos = NULL)

              # ----- Canopy Strata (for non-spatial patches) -----
              for (s in 1:strata_ct) {

                # Adds unique IDs for strataum intialized w/ same map as patches - appends 1 or 2 to the patch ID, ie patch 30 would have stratum 301 and 302
                if (unique_strata_ID) {
                  stratum_ID = unique(levels[levels[,5] == p & levels[,4] == z & levels[,3] == h & levels[,2] == b, 6]) * 10 + s
                } else {
                  stratum_ID = unique(levels[levels[,5] == p & levels[,4] == z & levels[,3] == h & levels[,2] == b, 6])
                }
                writeChar(paste("\t\t\t\t\t",stratum_ID,"\t\t\t","canopy_strata_ID\n",sep = ""),con = wcon,eos = NULL)

                # if template has 1 strata and rules have 2 - replicate existing values if missing
                # if template has 2 and asp rules only has 1 strata, missing values will use only the 1st strata of the template
                if (length(stratum) == 1 & asp_strata_ct == 2 & s == 2) {s2 = 1} else {s2 = s}

                asp_s_vars = which(!rulevars[[ruleid]]$strata_level_vars[[s]][,1] %in% var_names[var_index]) # get vars from aspatial not included in template
                for (i in asp_s_vars) {
                  var = as.numeric(rulevars[[ruleid]]$strata_level_vars[[asp]][i, s + 1])
                  varname = rulevars[[ruleid]]$strata_level_vars[[asp]][i, 1]
                  if (is.na(var)) {stop(paste(varname,"cannot be NA since a default isn't specified in the template, please set explicitly in your rules file."))}
                  writeChar(paste("\t\t\t\t",var,"\t\t\t",varname,"\n",sep = ""),con = wcon,eos = NULL)
                }

                for (i in (level_index[6] + 1):length(template_clean)) { # go through srata vars normally
                  if (length(statevars[[i]][[s2]]) > 1) { # its a map
                    var = statevars[[i]][[s2]][statevars[[i]][[s2]][2] == b & statevars[[i]][[s2]][3] == h & statevars[[i]][[s2]][4] == z & statevars[[i]][[s2]][5] == p ,"x"]
                  } else {var = statevars[[i]][[s2]]} # its a value
                  varname = template_clean[[i]][1]

                  if (varname %in% rulevars[[ruleid]]$strata_level_vars[[asp]][,1]) { # if variable is in rulevars, replace with rulevars version
                    if (!is.na(rulevars[[ruleid]]$strata_level_vars[[asp]][rulevars[[ruleid]]$strata_level_vars[[asp]][,1] == varname, s + 1])) { # make sure not NA
                      var = as.numeric(rulevars[[ruleid]]$strata_level_vars[[asp]][rulevars[[ruleid]]$strata_level_vars[[asp]][,1] == varname, s + 1])
                    }
                  }
                  writeChar(paste("\t\t\t\t\t",var,"\t\t\t",varname,"\n",sep = ""),con = wcon,eos = NULL)
                }
              }
            }
          } # end aspatial patches + stratum

          # ---------- Standard Patches + Stratum ----------
        } else {

          patches = unique(levels[levels[,4] == z & levels[,3] == h & levels[,2] == b, 5])
          writeChar(paste("\t\t\t",length(patches),"\t\t\t","num_patches\n",sep = ""),con = wcon,eos = NULL)

          # ----- Patches -----
          for (p in patches) {
            writeChar(paste("\t\t\t\t",p,"\t\t\t","patch_ID\n",sep = ""),con = wcon,eos = NULL)

            for (i in (level_index[5] + 1):(level_index[6] - 1)) {
              if (length(statevars[[i]][[1]]) > 1) {
                var = statevars[[i]][[1]][statevars[[i]][[1]][2] == b & statevars[[i]][[1]][3] == h & statevars[[i]][[1]][4] == z & statevars[[i]][[1]][5] == p ,"x"]
              } else {var = statevars[[i]][[1]]}
              varname = template_clean[[i]][1]
              writeChar(paste("\t\t\t\t",var,"\t\t\t",varname,"\n",sep = ""),con = wcon,eos = NULL)
            }
            writeChar(paste("\t\t\t\t",length(stratum),"\t\t\t","num_stratum\n",sep = ""),con = wcon,eos = NULL)

            # ----- Canopy Strata -----
            for (s in stratum) {

              # Adds unique IDs for strataum intialized w/ same map as patches - appends 1 or 2 to the patch ID, ie patch 30 would have stratum 301 and 302
              if (unique_strata_ID) {
                stratum_ID = unique(levels[levels[,5] == p & levels[,4] == z & levels[,3] == h & levels[,2] == b, 6]) * 10 + s
              } else {
                stratum_ID = unique(levels[levels[,5] == p & levels[,4] == z & levels[,3] == h & levels[,2] == b, 6])
              }
              writeChar(paste("\t\t\t\t\t",stratum_ID,"\t\t\t","canopy_strata_ID\n",sep = ""),con = wcon,eos = NULL)

              for (i in (level_index[6] + 1):length(template_clean)) {
                if (length(statevars[[i]][[s]]) > 1) { # if is a map
                  var = statevars[[i]][[s]][statevars[[i]][[s]][2] == b & statevars[[i]][[s]][3] == h & statevars[[i]][[s]][4] == z & statevars[[i]][[s]][5] == p ,"x"]
                } else {var = statevars[[i]][[s]]}
                varname = template_clean[[i]][1]
                writeChar(paste("\t\t\t\t\t",var,"\t\t\t",varname,"\n",sep = ""),con = wcon,eos = NULL)
              }

            } # end spatial statum
          }# end spatial patch
        } # end standard patches + stratum
      } # end zone
    } # end hillslope
  } # end basin

  close(wcon) # close file connection
  close(pb) # end progress bar connection

  print(paste("Created worldfile:",worldfile),quote = FALSE)

  # ---------------------- Write Header ----------------------
  if (header) {
    headfile = paste(substr(worldfile,0,(nchar(worldfile) - 5)),"hdr",sep = "")
    write(head,file = headfile)
    print(paste("Created header file:",headfile),quote = FALSE)
  }

  # ---------------------- Output for use in CreateFlownet ----------------------
  cfmaps = rbind(map_info,
                 c("cell_length",cell_len),
                 c("streams","none"), c("roads","none"), c("impervious","none"),c("roofs","none"))

  world_gen_out = list(cfmaps,rulevars)

  return(world_gen_out)

} # end function
