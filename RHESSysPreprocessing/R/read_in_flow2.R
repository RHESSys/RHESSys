#' read_in_flow2
#'
#' Read in flow table file and return a R list. Should be faster than the other one.
#' @param input_file Flow table to be read in.
#' @author William Burke
#' @export
#'
read_in_flow2<-function(input_file){
  print("Reading in flow table")
  num_patches<-scan(file=input_file,nlines=1,quiet = TRUE)  #read in number of patches
  flow_raw = readLines(input_file,warn = FALSE) # read in flow table
  flow_list = strsplit(flow_raw[2:length(flow_raw)],"\\s+") # split strings into list of char vectors
  p_ind = sapply(flow_list,length) == 12 # find patches

  print("Converting to list")
  flw = lapply(
    flow_list[p_ind],
    FUN = function(X) {
      list(PatchID = as.numeric(X[2]),
        ZoneID = as.numeric(X[3]),
        HillID = as.numeric(X[4]),
        Centroidx = as.numeric(X[5]),
        Centroidy = as.numeric(X[6]),
        Centroidz = as.numeric(X[7]),
        Accumulated_Area =
          as.numeric(X[8]),
        Area = as.numeric(X[9]),
        Landtype = as.numeric(X[10]),
        TotalG = as.numeric(X[11]),
        NumAdjacentPatches =
          as.numeric(X[12])
      )
    }
  )

  n_ind = sapply(flw,"[[",11)
  n_ind2 = rep(1:length(n_ind),n_ind)
  nbrs = split(as.numeric(sapply(flow_list[!p_ind],"[[",2)),n_ind2)
  nbr_gamma = split(as.numeric(sapply(flow_list[!p_ind],"[[",5)),n_ind2)

  p_num = sapply(flw,"[[",1)

  for(i in 1:length(flw)){
    flw[[i]]$Number = i
    flw[[i]]$Gamma_i = nbr_gamma[[i]]

    nbr_num = vector(mode = "numeric")
    for(n in 1:flw[[i]]$NumAdjacentPatches){
      nbr_num = c(nbr_num,which(p_num==nbrs[[i]][n]))
    }
    flw[[i]]$Neighbors = nbr_num
    print(i/length(flw))
  }

  return(flw)
  }
