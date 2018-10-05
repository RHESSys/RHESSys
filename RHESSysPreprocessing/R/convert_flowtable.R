#' convert_flowtable
#'
#' Convert a flowtable to hillslope parallelized format
#' @param input_file Flow table to be read in.
#' @param output_file Name for output file to be created.
#' @param make_stream The maximum distance (cell lengths) away from an existing stream that a patch can be automatically coerced to be a stream.
#' Setting to TRUE will include patches at any distance. This is needed for hillslope parallelization, as all hillslopes must have an outlet stream patch.
#'  Default is 4.
#' @author William Burke
#' @export
#'

convert_flowtable = function(input_file,output_file, make_stream = 4) {

  # ---------- Get list from flowtable ----------
  flow = read_in_flow(input_file,df = TRUE)

  flw = flow[[1]]
  flw_struct = flow[[2]]

  # ---------- Convert to hillslope routing ----------
  #
  # Add hillslope outlets
  no_stream_fix = NULL
  no_stream = NULL
  print_dist_fix = NULL
  print_dist = NULL

  min_hill_patch = stats::aggregate(flw_struct$Centroidz,by = list(flw_struct$Hill),FUN = which.min) # sum streams(landuse) by hillslope
  hill_no_outlets = matrix(0, nrow = length(unique(flw_struct$Hill)), ncol = 2)
  hill_no_outlets[,1] = unique(flw_struct$Hill)
  for (h in hill_no_outlets[,1]) {
    hill_no_outlets[hill_no_outlets[,1] == h,2] = flw_struct[flw_struct$Hill==h,][min_hill_patch[min_hill_patch[,1] == h,2],"Landtype"]
  }

  if(any(hill_no_outlets[,2]==0)){ # if there are any hillslopes without streams
    print("Correcting for hillslopes missing stream outlets.",quote = FALSE)
    streams = flw_struct[flw_struct$Landtype==1,] # make var of streams
    for(i in hill_no_outlets[hill_no_outlets[,2]==0,1]){
      hill_patches = flw_struct[flw_struct$Hill==i,] # get patches from problem hillslope
      min_patch = hill_patches[which.min(hill_patches$Centroidz),]# find lowest elevation patch
      dist2stream = sqrt(abs(streams$Centroidx - min_patch$Centroidx)^2 + abs(streams$Centroidy - min_patch$Centroidy)^2) # distance to streams from min elev patch
      if(is.numeric(make_stream)){ # if make_stream is a number
        if(min(dist2stream)<=make_stream){ # if within make_stream var distance of stream, make min patch a stream
          flw_struct[min_patch$Number,"Landtype"] = 1
          flw[[min_patch$Number]]$Landtype = 1
          no_stream_fix = c(no_stream_fix,min_patch$Number)
          print_dist_fix = c(print_dist_fix,min(dist2stream))
        } else if(min(dist2stream)>make_stream){ # if outside of make_stream distance threshold
          no_stream = c(no_stream,min_patch$Number)
          print_dist = c(print_dist,min(dist2stream))
        }
      } else if(make_stream){ # if TRUE
        flw_struct[min_patch$Number,"Landtype"] = 1
        flw[[min_patch$Number]]$Landtype = 1
        no_stream_fix = c(no_stream_fix,min_patch$Number)
        print_dist_fix = c(print_dist_fix,min(dist2stream))
      }

    }
  }

  if(!is.null(no_stream_fix)){
    print(paste(length(no_stream_fix),"hillslopes had their lowest elevation patches set to streams, at a max distance from existing streams of",
                max(print_dist_fix),"cell lengths."),quote = FALSE)
  }
  if(!is.null(no_stream)){ # output hillslopes that weren't corrected
    print(paste("The following outlet patches were outside of the",make_stream,"cell length distance threshold set by the 'make_stream' argument."),quote = F)
    print(cbind("Dist2Stream" = print_dist,flw_struct[no_stream,]))
    stop(noquote("The above hillslopes must have stream patch outlets, either increase the value of the 'make_stream' argument, or fix via GIS."))
  }

  # Correct out-of-hillslope neighbors
  for (i in 1:length(flw)) {
    diff_hill = !flw_struct[flw[[i]]$Neighbors,"HillID"] == flw[[i]]$HillID
    if (any(diff_hill)) {
      flw[[i]]$Neighbors = flw[[i]]$Neighbors[!diff_hill]
      flw[[i]]$Gamma_i = flw[[i]]$Gamma_i[!diff_hill]
    }
    if (sum(flw[[i]]$Gamma_i) != 1 & any(flw[[i]]$Gamma_i > 0)) {
      flw[[i]]$Gamma_i = flw[[i]]$Gamma_i / sum(flw[[i]]$Gamma_i) # renormalize gammas
    }
  }

  # ---------- Fix pits ----------
  flw = fix_all_pits(flw = flw,flw_struct = flw_struct,parallel = TRUE)

  # ---------- Create new flowtable ----------
  make_flow_table(flw = flw,output_file = output_file,parallel = TRUE)

}
