#' GIS_read
#'
#' Read in GIS maps. Returns a spatial data frame containing all maps. 6/6/17.
#' @param type GIS type to be used. Currently only supports GRASS GIS.
#' @param readin Template file or vector containing character strings of map names.
#' @param typepars Parameters needed based on GIS type used. For Grass GIS type, typepars is a
#' vector of 5 character strings. GRASS GIS parameters: gisBase, home, gisDbase, location, mapset.
#' Example parameters are included in an example script included in this package. See initGRASS help
#' for more info on parameters.
#' @author Will Burke

GIS_read = function(type,readin,typepars) {

  if ((startsWith(basename(readin[1]),"template.") | startsWith(basename(readin[1]),"Template.") | endsWith(basename(readin[1]),".template") ) ) { # if starts or ends with "template"
    template = readin

    # ---------- Read in template ----------
    con = file(template, open ="r") # commect to file
    read = readLines(con) # read file, default reads entire file, line by line
    close(con)

    tempclean = strsplit(trimws(read),"[ \t]+") # remove leading/trailing whitespaces, split strings by spaces or tabs
    levindex = which(startsWith(trimws(read), "_")) # find lines that start w/ "_", get row nums of levels
    levmaps = lapply(tempclean[levindex],"[",2)# level map names, for use in GRASS
    tempindex = levindex[2]:length(tempclean)
    tempindex = tempindex[! tempindex %in% levindex] #make index for template, excluding def files and levels

    # get all maps - looks at last element of each row and checks if it's a number, if not num, gets that element, excluding "area"
    maps = vector()
    mapindex = vector()
    for (i in tempindex) {
      if (suppressWarnings(all(is.na(as.numeric( tempclean[[i]][3]  ))))) {
        maps[i] = tempclean[[i]][3]
        mapindex[i] = i }
      if (length(tempclean[[i]]) == 5 ) {
        maps[i] = tempclean[[i]][5]
        mapindex[i] = i }}

    maps =unique(maps[!is.na(maps)]) # get rid of NAs, get rid of duplicates
    mapindex = mapindex[!is.na(mapindex)] # index of rows w/ maps
    maps_in = c(unlist(levmaps), maps) # combine level maps and all other maps into vector
  }
  else if (is.character(readin)) { #if 'maps' is characters, and not a template, assume it's names of maps
    maps_in = readin
  }

  # ---------- Spatial data ----------
  # GRASS GIS spatial data ----------
  if (type == "GRASS") {

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

return(readmap)

}
