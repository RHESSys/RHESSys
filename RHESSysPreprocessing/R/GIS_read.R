#' GIS_read
#'
#' Read in GIS maps. Returns a spatial data frame containing all maps. 2/14/18.
#' @param maps_in Character vector of maps to be read in by the chosen method.
#' @param type GIS type to be used. Options are GRASS(will auto-detect GRASS6 or GRASS7) GRASS6, GRASS7, or raster.
#' @param typepars Parameters needed based on GIS type used. For GRASS GIS type, typepars is a
#' vector of 5 character strings. GRASS GIS parameters: gisBase, home, gisDbase, location, mapset.
#' Example parameters are included in an example script included in this package. See initGRASS help
#' for more info on parameters. For raster type, typepars is a file path to a folder containing the rasters indicated in read_in.
#' @return Returns a LargeSpatialGridDataFrame containing all the maps indicated in read_in
#' @author Will Burke
#' @export

GIS_read = function(maps_in,type,typepars,map_info) {

  # try to determine which version of GRASS is being used automatically
  if (type == "GRASS") {
    if (unlist(gregexpr("7.", typepars[1])) > unlist(gregexpr("6.", typepars[1]))) {
      type = "GRASS7"
    } else if (unlist(gregexpr("7.", typepars[1])) < unlist(gregexpr("6.", typepars[1]))) {
      type = "GRASS6"
    } else{stop("Cannot determine GRASS version, set type explicitly to GRASS6 or GRASS7")}
  }

  # ---------- Read in spatial data ----------
  print("Reading in maps",quote=FALSE)
  # GRASS 6.4.4 ----------
  if (type == "GRASS6") {
    spgrass6::initGRASS( # Initialize GRASS environment
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
    readmap = spgrass6::readRAST(maps_in)
  } #end GRASS 6 spatial data

  # GRASS 7 ----------
  if (type == "GRASS7") {
    rgrass7::initGRASS( # Initialize GRASS environment
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
    readmap = rgrass7::readRAST(maps_in)
  } #end GRASS 7 spatial data

  # Raster spatial data ----------
  if (type == "raster" | type == "Raster" | type =="RASTER") {

    # Get file paths and check files exist
    file_paths = vector(mode = "character")
    for (name in maps_in){
      file = list.files(path=typepars, pattern = paste("^",name,"$",sep =""),full.names = TRUE)
      if (length(file) == 0){ # check for file (name listed in template + path in typepars) since list.files doesn't throw an error on no return
        stop(paste("No file named:",name,"at path:",typepars))}
      if(length(file) > 1){ # can only be one file for each name in maps_in
        stop(paste("multiple files containing name:",name,"check directory:",typepars))}
      file_paths = c(file_paths,file)
    }

    # read in rasters
    # NATIVE DRIVERS -- may or may not actually help, future testing maybe
    read_stack = try(raster::stack(file_paths,native=TRUE))

    if(inherits(read_stack, "try-error")){ # automatic error handling, can be added to as errors are found -----

      if(attr(read_stack,"condition")$message == "different extent"){ # check/compare extents
        extents = list()
        for (i in file_paths){ # get extents
          extents[[which(file_paths==i)]] = extent(raster(i))
        }
        for(i in 1:length(unique(extents))){ # print maps and extents, should make outlier maps obvious
          m = maps_in[sapply(extents,FUN = function(x) x == unique(extents)[[i]])]
          print(paste("Maps:",paste(m,collapse = ", ")),quote=FALSE)
          print(paste("Have extent: xmin =",unique(extents)[[i]]@xmin,"xmax =",unique(extents)[[i]]@xmax,
                      "ymin =",unique(extents)[[i]]@ymin,"ymax =",unique(extents)[[i]]@ymax),quote=FALSE)
        }
      } # end if different extents

      # other types of errors go here

      stop("Something went wrong, see previous messages")
    } # end try error handling

    names(read_stack) = maps_in # names lose extensions by default, confused by "."s in names

    # Check projections (read_stack will error if proj is different, but arguments might be different) -----
    p = vector(mode="character",length=length(read_stack[1]))
    d = p
    for(i in 1:length(read_stack[1])){
      p[i] = raster::projection(read_stack[[i]])
      d[i] = attr(rgdal::GDALinfo(file_paths[i],silent = TRUE),which = "driver")
    }
    if(length(unique(p))>1 & exists("map_info")){ # if map_info is present, coerce world level projection, output text for overwritten projections
      for(i in which(p != raster::projection(read_stack[[map_info[map_info[,1]=="world",2]]]))){
        raster::projection(read_stack[[i]]) = raster::projection(read_stack[[map_info[map_info[,1]=="world",2]]])
        print(paste("Projection arguments for",names(read_stack[[i]]),"coerced to world level projection:",
                    raster::projection(read_stack[[map_info[map_info[,1]=="world",2]]])),quote=FALSE)
      }
    }
    if(length(unique(p))>1 & !exists("map_info")){
      print(paste("Differing projection arguments:",unique(p),"Potential conflicts."),quote=FALSE)
    }

    # if(is.na(raster::projection(read_stack))){
    #   print("Rasters are missing projection information. Shouldn't effect RHESSysPreprocess functions.")
    # }

    # Handling grass ascii 0's - get rid of 0's for background/NA -----
    # Ideeally this should be handled when reading in files, but I can't find where the default for nodata is set,
    # and strangely this is an ascii specific issue
    if(any(d == "AAIGrid")|any(d == "GRASSASCIIGrid")){
      # raster::values(read_stack)[apply(raster::values(read_stack)==0,FUN = all,MARGIN = 1)] = NA
      # print(paste("Background 0's converted to NA's"))

      # new fix - just set world map(usually basin) 0's to NA, less chance of confusion
      raster::values(read_stack[[map_info[map_info[,1]=="world",2]]])[raster::values(read_stack[[map_info[map_info[,1]=="world",2]]])==0] = NA
    }

    # Mask all maps by world level map -----
    if(exists("map_info")){ # if being run inside RHESSysPreprocess.R will always have map_info - just makes funciton more versitile
      read_stack = raster::mask(read_stack,read_stack[[map_info[map_info[,1]=="world",2]]])# mask by map used for world level
    }

    read_stack = raster::trim(read_stack) #get rid of extra background

    # Check for missing data (within world map mask) - no fix, just an error since I think this will break things if left unchecked
    if(exists("map_info") & sum(is.na(raster::values(read_stack)[!is.na(raster::values(read_stack[[map_info[map_info[,1]=="world",2]]])),
                                                                 !colnames(raster::values(read_stack)) %in% map_info[map_info[,1]=="streams",2]])) > 0){
      # Add in future? - which maps are missing data?
      stop("Missing data within bounds of world level map. Check you input maps.")
    }

    # Convert maps to SpatialGridDataFrame since world_gen.R expects that format
    readmap = as(read_stack,"SpatialGridDataFrame")

  } # end raster spatial data

  return(readmap)

}
