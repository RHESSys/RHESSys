#' GIS_read
#'
#' Read in GIS maps. Returns a spatial data frame containing all maps. 2/14/18.
#' @param read_in Character vector of maps to be read in by the chosen method.
#' @param type GIS type to be used. Options are GRASS(will auto-detect GRASS6 or GRASS7) GRASS6, GRASS7, or raster.
#' @param typepars Parameters needed based on GIS type used. For GRASS GIS type, typepars is a
#' vector of 5 character strings. GRASS GIS parameters: gisBase, home, gisDbase, location, mapset.
#' Example parameters are included in an example script included in this package. See initGRASS help
#' for more info on parameters. For raster type, typepars is a file path to a folder containing the rasters indicated in read_in.
#' @return Returns a LargeSpatialGridDataFrame containing all the maps indicated in read_in
#' @author Will Burke

GIS_read = function(read_in,type,typepars) {

  maps_in = read_in

  if (type == "GRASS") {
    # determine which version of GRASS is being used automatically
    if (unlist(gregexpr("7.", typepars[1])) > unlist(gregexpr("6.", typepars[1]))) {
      type = "GRASS7"
    } else if (unlist(gregexpr("7.", typepars[1])) < unlist(gregexpr("6.", typepars[1]))) {
      type = "GRASS6"
    } else{stop("Cannot determine GRASS version, set type explicitly to GRASS6 or GRASS7")}
  }

  # ---------- spatial read in ----------
  # GRASS 6.4.4 ----------
  if (type == "GRASS6") {
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

    mask = paste("g.copy rast=",maps_in[1],",MASK",sep="") #set mask
    if (.Platform$OS.type == "unix") {
      system(mask) #mac
    } else {
      shell(mask) #windows
    }
    #read in maps
    readmap = readRAST(maps_in)
  } #end GRASS 6 spatial data

  # GRASS 7 ----------
  if (type == "GRASS7") {
    if(!require("rgrass7")){install.packages("rgrass7")}
    if(!require("rgdal")){install.packages("rgdal")}
    library(rgrass7)
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

    mask = paste("g.copy rast=",maps_in[1],",MASK",sep="")#set mask
    if (.Platform$OS.type == "unix") {
      system(mask) #mac
    } else {
      shell(mask) #windows
    }
    #read in maps ----------
    readmap = readRAST(maps_in)
  } #end GRASS 7 spatial data

  # Raster spatial data ----------
  if (type == "raster" | type == "Raster" | type =="RASTER") {
    if(!require("raster")){install.packages("raster")}
    library(raster)

    # import rasters - using spatialgriddataframe format for consistancy
    ct = 0
    for (name in maps_in){
      ct = ct+1
      file = list.files(path=typepars, pattern = paste("^",name,"$",sep =""),full.names = TRUE)

      if (length(file) == 0){ # check for file (name listed in template + path in typepars) since list.files doesn't throw an error on no return
        stop(paste("No file named:",name,"at path:",typepars))}
      if(length(file) > 1){ # can only be one file for each name in maps_in
        stop(paste("multiple files containing name:",name,"check directory:",typepars))}
      read = as(raster(file),"SpatialGridDataFrame") # read in map
      names(read) = name # fix names, names with dots get treated as suffixes and removeed by default
      if(!exists("readmap")){readmap = read} # initalize readmap if not already

      if(!identicalCRS(read,readmap)){stop(paste("Projection of",name,"doesn't match"))} # check projections match
      if(sum(read@grid@cellcentre.offset) != sum(readmap@grid@cellcentre.offset) | # check cell centers match
         sum(read@grid@cellsize) != sum(readmap@grid@cellsize) | # check sell sizes match
         sum(read@grid@cells.dim) != sum(readmap@grid@cells.dim) ){ # check cell dimensions match
        stop(paste("Grid topology (cell size, cell center offset, dimensions) of",name,"doesn't match"))}

      readmap = cbind(readmap,read) # append current map to map dataframe
    }
  } # end raster spatial data

  return(readmap)

}
