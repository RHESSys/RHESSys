#' World Gen
#'
#' Generates world files for use in RHESSys based on input template and maps,
#' currently includes functionality for GRASS GIS, and works on both unix and windows. 5/16/17.
#' @param template Template file used to generate worldfile for RHESSys. Generic strucutre is:
#' <state variable> <operator> <value/map>. Levels are difined by lines led by "_", structured
#' <levelname> <map> <count>. Whitespace and tabs are ignored.
#' @param worldfile Name and path of worldfile to be created.
#' @param type GIS type to be used. Currently only supports GRASS GIS.
#' @param typepars Parameters needed based on GIS type used. For Grass GIS type, typepars is a
#' vector of 5 character strings. GRASS GIS parameters: gisBase, home, gisDbase, location, mapset.
#' Example parameters are included in an example script included in this package. See initGRASS help
#' for more info on parameters.
#' @param overwrite Overwrite existing worldfile. FALSE is default and prompts a menu if worldfile already exists.
#' @param asprules The path and filename to the rules file.  Using this argument enables aspatial patches.
#' @seealso \code{\link{initGRASS}}, \code{\link{readRAST}}
#' @author Will Burke

# ---------- Function start ----------
world_gen = function(template, worldfile, type = 'GRASS', typepars, overwrite=FALSE, asprules=NULL) {

  timer = proc.time() #start timer

  if(!require("circular")){install.packages("circular")} #check for circular package, install if not present
  library(circular)

  # ---------- Check inputs ----------
  if (!file.exists(template)) {
    print(paste("template does not exist or is not located at specified path:",template),quote=FALSE) #check if template exists
  }

  worldname = basename(worldfile)# Coerce .world extension
  if (startsWith(worldname,"World.") | startsWith(worldname,"world.")) {
    worldname = paste(substr(worldname,7,nchar(worldname)),".world",sep="")
    worldfile = paste(substr(worldfile, 1, (nchar(dirname(worldfile))+1)),worldname,sep="")
  } else if(!endsWith(worldname,".world")) {
    worldname = paste(worldname,".world",sep="")
    worldfile = paste(substr(worldfile, 1, (nchar(dirname(worldfile))+1)),worldname,sep="")
  }

  if (!is.logical(overwrite)) {stop("overwrite must be logical")} # check overwrite inputs
  if (file.exists(worldfile) & overwrite == FALSE) {
    t = menu(c("Yes","No"),title=paste("Worldfile",worldfile,"already exists. Overwrite?"))
    if (t==2) {stop("world_gen exited without completing")}
  }

  if (!is.null(asprules)) {asp_check = TRUE} else {asp_hcheck = FALSE} # check for aspatial patches
  if (asp_check) { if(!file.exists(asprules) ) {asp_check=FALSE}}

  # ---------- Read in template ----------
  con = file(template, open ="r") # commect to file
  read = readLines(con) # read file, default reads entire file, line by line
  close(con)

  readtrim = trimws(read)
  tempclean = strsplit(readtrim,"[ \t]+") # remove leading/trailing whitespaces, split strings by spaces or tabs
  levindex = which(startsWith(readtrim, "_")) # find lines that start w/ "_", get row nums of levels
  levmaps = lapply(tempclean[levindex],"[",2)# level map names, for use in GRASS
  tempindex = levindex[2]:length(tempclean)
  tempindex = tempindex[! tempindex %in% levindex] #make index for template, excluding def files and levels

  # Find all maps
  mapsall = vector()
  mapindex = vector()
  for (i in tempindex) {
    if ( suppressWarnings(all(is.na(as.numeric( tempclean[[i]][3]))))  & length(tempclean[[i]]) != 2) {
      mapsall[i] = tempclean[[i]][3]
      mapindex[i] = i }
    if (length(tempclean[[i]]) == 5 ) {
      mapsall[i] = tempclean[[i]][5]
      mapindex[i] = i }}

  maps = unique(mapsall[!is.na(mapsall)]) # get rid of NAs, get rid of duplicates
  mapindex = mapindex[!is.na(mapindex)] # index of rows w/ maps
  maps_in = c(unlist(levmaps), maps) # combine level maps and all other maps into vector

  if (asp_check) { # if using aspatial patches, get rules value or map
    asp_map = tempclean[[which(startsWith(readtrim,"asp_rule"))]][3]
    if (is.character(asp_map)) {
    maps_in = c(maps_in,asp_map) }}

  mapnames = sapply(tempclean[mapindex], function(x) x[1])
  mapinfo = cbind(c("world","basin","hillslope","zone","patch","strata", mapnames),c(unlist(levmaps),mapsall[!is.na(mapsall)]))
  colnames(mapinfo) = c("Mapname","Map")
  mapinfo = unique(mapinfo)

  # ---------- Spatial data ----------
  # GRASS GIS spatial data ----------
  if (type == "GRASS") {

    if(!require("spgrass6")){install.packages("spgrass6")}
    if(!require("rgdal")){install.packages("rgdal")}
    library(spgrass6)
    library(rgdal)

    initGRASS( # Initialize GRASS environment
      gisBase = typepars[1],
      home = typepars[2],
      gisDbase = typepars[3],
      location = typepars[4],
      mapset = typepars[5],
      override = TRUE)

    region = paste("g.region rast=",maps_in[1]," -d", sep = '') #set region to map used for world level
    if (.Platform$OS.type == "unix") {
      system(region) #mac
    } else {
      shell(region) #windows
    }

    #set mask - throws warning if mask is already set, but code still runs
    mask = paste("g.copy rast=",maps_in[1],",MASK",sep="")
    if (.Platform$OS.type == "unix") {
      system(mask) #mac
    } else {
      shell(mask) #windows
    }
    #read in maps ----------
    readmap = readRAST(maps_in) #may get large in memory
  } #end GRASS spatial data

  # Raster spatial data ----------
  if (type == "raster") {

    if(!require("raster")){install.packages("raster")}
    library(raster)

    read = SpatialGridDataFrame()
    ct = 0

    for (name in maps_in){
      path = list.files(typepars, pattern = name)
      if (length(path) > 1){
        print(paste("multiple files named:",name ))}
      else {
        ct = ct+1
        read[[ct]] = raster(path)
      }
    }
    print("this doesn't work yet")
    stop("world_gen function stopped")
    # to add: check that spatial data covers same area. Downscale cell sizes to smallest common size
  }

  #process map data  ----------
  mapdf = as.data.frame(readmap) #make data frame for ease of use
  cellarea = readmap@grid@cellsize[1] * readmap@grid@cellsize[2] # get cell area - need for area operator
  cellarea = rep(cellarea, length(mapdf[,6]))

  # structure to iterate through levels ---- input object with unique ID's for each unit at each level, will iterate through
  levstrct = unname(data.matrix(mapdf[1:6], length(mapdf[[6]])))
  levlist = list()
  for (i in 1:6) {levlist[i] = list(levstrct[,i])}

  # ---------- Build R list containing values based on template and maps ----------
  statevars = list() # set up list to store state variables
  stratum = 1:tempclean[[levindex[6]]][3] # stratum vector

  # this section could use refining -_-
  for (i in tempindex) {
    curindex = sum(i > levindex) # index based on current row relative to levels
    levagg = levlist[1:curindex] # aggregate by levels above curren row

    for (s in stratum) {
      if (length(s) > 1 & i > levindex[6]) {

        if(tempclean[[i]][2] == "value") { #use value
          statevars[[i]][s] = as.double(tempclean[[i]][3])
        } else if(tempclean[[i]][2] == "dvalue") { #integer value
          statevars[[i]][s] = as.integer(tempclean[[i]][3])
        } else if(tempclean[[i]][2] == "aver") { #average
          maptmp = as.vector(t(mapdf[tempclean[[i]][3]]))
          statevars[[i]][s] = aggregate(maptmp, by = levagg, FUN = "mean")
        } else if(tempclean[[i]][2] == "mode") { #median
          maptmp = as.vector(t(mapdf[tempclean[[i]][3]]))
          statevars[[i]][s] = aggregate(maptmp, by = levagg, FUN = "median")
        }

      } else {

        if(tempclean[[i]][2] == "value") { #use value
          statevars[[i]] = as.double(tempclean[[i]][3])
        } else if(tempclean[[i]][2] == "dvalue") { #integer value
          statevars[[i]] = as.integer(tempclean[[i]][3])
        } else if(tempclean[[i]][2] == "aver") { # average
          maptmp = as.vector(t(mapdf[tempclean[[i]][3]]))
          statevars[[i]] = aggregate(maptmp, by = levagg, FUN = "mean")
        } else if(tempclean[[i]][2] == "mode") { #median
          maptmp = as.vector(t(mapdf[tempclean[[i]][3]]))
          statevars[[i]] = aggregate(maptmp, by = levagg, FUN = "median")
        } else if(tempclean[[i]][2] == "eqn") {
          # only for horizons old version -- use mean, without multiplying moving forward
          maptmp = as.vector(t(mapdf[tempclean[[i]][5]]))
          statevars[[i]] = aggregate(maptmp, by = levagg, FUN = "mean")
          statevars[[i]][,"x"] = statevars[[i]][,"x"] * as.numeric(tempclean[[i]][3])
        } else if(tempclean[[i]][2] == "spavg") { #spherical average
          maptmp = as.vector(t(mapdf[tempclean[[i]][3]]))
          maptmpangles = circular(maptmp,units="degrees",type="angles",modulo="2pi",template="geographics")
          #avgangles = aggregate(maptmpangles, by = levagg, FUN = "mean.circular")[,"x"]
          #avgangles = circular(avgangles,units="degrees",type="angles",modulo="2pi",template="geographics")
          statevars[[i]] = aggregate(maptmpangles, by = levagg, FUN = "mean.circular")
        } else if(tempclean[[i]][2] == "area") { #only for state var area
          statevars[[i]] = aggregate(cellarea, by = levagg, FUN = "sum")
        } else { print(paste("Unexpected 2nd element on line",i)) }

      }
    }
  }

  if(asp_check) {statevars = aspatial_patches(asprules,statevars,asp_map)} # aspatial patch processing

  # ---------- Build world file ----------
  print("Begin writing world file",quote=FALSE)
  # create/open file
  sink(worldfile)

  # world - no state variables
  world = unique(levstrct[,1])
  cat(world,"\t\t\t","world_ID\n",sep="")
  num_basins = length(unique(levstrct[,2]))
  cat(num_basins,"\t\t\t","num_basins\n",sep="")

  basin = unique(levstrct[,2])
  for (b in basin) { #basins
    cat("\t",b,"\t\t\t", "basin_ID\n",sep="")
    for (i in (levindex[2]+1):(levindex[3]-1)) {
      if (length(statevars[[i]]) >1) {
        var = statevars[[i]][statevars[[i]][2]==b ,"x"]
      } else { var = statevars[[i]] }
      varname = tempclean[[i]][1]
      cat("\t",var,"\t\t\t",varname,"\n",sep="")
    }
    hillslopes = unique(levstrct[levstrct[,2]==b, 3])
    cat("\t",length(hillslopes),"\t\t\t","num_hillslopes\n",sep="")

    for (h in hillslopes) { #hillslopes
      cat("\t\t",h,"\t\t\t", "hillslope_ID\n",sep="")
      for (i in (levindex[3]+1):(levindex[4]-1)) {
        if (length(statevars[[i]]) >1) {
          var = statevars[[i]][statevars[[i]][2]==b & statevars[[i]][3]==h ,"x"]
        } else { var = statevars[[i]] }
        varname = tempclean[[i]][1]
        cat("\t\t",var,"\t\t\t",varname,"\n",sep="")
      }
      zones = unique(levstrct[levstrct[,3]==h & levstrct[,2]==b, 4])
      cat("\t\t",length(zones),"\t\t\t","num_zones\n",sep="")

      for (z in zones) { #zones
        cat("\t\t\t",z,"\t\t\t", "zone_ID\n",sep="")
        for (i in (levindex[4]+1):(levindex[5]-1)) {
          if (length(statevars[[i]]) >1) {
            var = statevars[[i]][statevars[[i]][2]==b & statevars[[i]][3]==h & statevars[[i]][4]==z ,"x"]
          } else { var = statevars[[i]] }
          varname = tempclean[[i]][1]
          cat("\t\t\t",var,"\t\t\t",varname,"\n",sep="")
        }
        patches = unique(levstrct[levstrct[,4]==z & levstrct[,3]==h & levstrct[,2]==b, 5])
        cat("\t\t\t",length(patches),"\t\t\t","num_patches\n",sep="")

        for (p in patches) { #patches
          cat("\t\t\t\t",p,"\t\t\t", "patch_ID\n",sep="")
          for (i in (levindex[5]+1):(levindex[6]-1)) {
            if (length(statevars[[i]]) >1) {
              var = statevars[[i]][statevars[[i]][2]==b & statevars[[i]][3]==h & statevars[[i]][4]==z & statevars[[i]][5]==p ,"x"]
            } else { var = statevars[[i]] }
            varname = tempclean[[i]][1]
            cat("\t\t\t\t",var,"\t\t\t",varname,"\n",sep="")
          }
          cat("\t\t\t\t",length(stratum),"\t\t\t","num_stratum\n",sep="")

          for (s in stratum) { #stratum
            cat("\t\t\t\t\t", s,"\t\t\t", "stratum_ID\n",sep="")
            for (i in (levindex[6]+1):length(tempclean)) {
              if (length(statevars[[i]]) >1) {  ########## make sure this works @@@@@@@@@@@@@
                var = statevars[[i]][statevars[[i]][ncol(statevars[[i]])-1]==p ,"x"]
              } else { var = statevars[[i]] }
              varname = tempclean[[i]][1]
              cat("\t\t\t\t\t",var,"\t\t\t",varname,"\n",sep="")
            }
          }
        }
      }
    }
  }
  sink()

  print(paste("Created worldfile:",worldfile),quote=FALSE)

  #----------Create cf_maps for use in cf -----------
  cfmaps = rbind(mapinfo[1:6,],mapinfo[mapinfo[,1] == "z",],
                 mapinfo[mapinfo[,1] == "slope",],
                 c("cell_length",readmap@grid@cellsize[1]),
                 c("stream","none"), c("roads","none"), c("impervious","none"),c("roofs","none"))

  f = file.create("cf_maps",overwrite=TRUE)
  write.table(cfmaps,"cf_maps",sep="\t\t",row.names=FALSE,quote = FALSE)

  #worldname2 = substr(worldname,1,(nchar(worldname)-6))
  f2 = save(typepars, file = "typepars")

  print(paste("Total function time:", round((proc.time() - timer)[3],digits=1),"seconds"),quote=FALSE)

} # end function

