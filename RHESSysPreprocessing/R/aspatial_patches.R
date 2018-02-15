#' Aspatial Patch Generation
#'
#' @author Will Burke

aspatial_patches = function(asprules,statevars,asp_mapdata,levlist,tempclean) {

  # --------- Read rules file ----------

  con = file(asprules, open ="r") # commect to file
  readrules = readLines(con) # read file, default reads entire file, line by line
  close(con)

  rtrim = trimws(readrules)
  rtrim = rtrim[!rtrim==""]
  idind1 = which(startsWith(rtrim,"ID")) # index IDs
  rhead = rtrim[1:idind1[1] - 1] # get header
  rsplit = strsplit(rtrim[idind1[1]:length(rtrim)],"[ \t]+") # split values, ignoring header
  idind2 = which(startsWith(rtrim[idind1[1]:length(rtrim)],"ID")) # get index for IDs w/o header
  splitAt <- function(x, pos) unname(split(x, cumsum(seq_along(x) %in% pos)))
  rules_list = splitAt(rsplit,idind2) #split list again so each ID is a top level list element
  IDname = unlist(lapply(lapply(rules_list,"[[",1),"[",2))
  names(rules_list) = IDname # names so i don't get confused

  #rlevind = which(startsWith(trimws(readrules), "_")) # find levels in rules
  #this is ugly but whatever

  idmap = as.numeric(unique(asp_mapdata)[[1]]) # get rule IDs from map/input
  rlevin = matrix(nrow = length(idmap),ncol = 2)
  for (id in idmap) {
    ct = 0
    for (l in 1:length(rules_list[[id]])) {
      if( startsWith(rules_list[[id]][[l]][1],"_") ) {
        ct = ct + 1
        rlevin[id,ct] = l}
    }
  }

  for (id in idmap){
    lineind = c((rlevin[id,1]+1):(rlevin[id,2]-1),(rlevin[id,2]+1):length(rules_list[[id]]))
    for (line in lineind) {
      aspind = which(rules_list[[id]][[line]]==",")

    }
  }


  # ---------- build rulevars based on rules and map ----------

  rulevars = as.list(rep(0,length(idmap)))

  for (id in idmap) {

    lineind = c((rlevin[id,1]+1):(rlevin[id,2]-1),(rlevin[id,2]+1):length(rules_list[[id]]))
    asp_ct = as.numeric(rules_list[[id]][[which(lapply(rules_list[[id]],"[[",1)=="subpatch_count")]][2])
    rulevars[[id]] = as.list(rep(0,asp_ct))

    for (asp in 1:asp_ct) {

      rulevars[[id]][[asp]] = as.list(rep(0,length(lineind)))
      lpos = 0

      for (line in lineind) {
        lpos = lpos +1

        #curindex = sum(line > rlevin[id,]) # index based on current row relative to levels
        #levagg = levlist[1:curindex] # aggregate by levels above curren row

        aspind = which(rules_list[[id]][[line]]==",")
        aspind = c(2,aspind)

        if(line > rlevin[id,2]) {
          stratum = 1:rules_list[[id]][[rlevin[id,2]]][2]# get number of stratum
          rulevars[[id]][[asp]][[lpos]] = as.list(rep(0,length(stratum)))
        } else {
          rulevars[[id]][[asp]][[lpos]] = as.list(rep(0,1))
          stratum = 1
        }

        if(line == rlevin[id,2]+1) { sindex = c(lpos, rules_list[[id]][[line-1]]) }

        names(rulevars[[id]][[asp]])[[lpos]] <- rules_list[[id]][[line]][1]

        for (s in stratum) {

          pos = aspind[asp] + s

            if(rules_list[[id]][[line]][2] == "value") { #use value
              rulevars[[id]][[asp]][[lpos]][s] = as.double(rules_list[[id]][[line]][pos])
            } else if(rules_list[[id]][[line]][2] == "dvalue") { #integer value
              rulevars[[id]][[asp]][[lpos]][s] = as.integer(rules_list[[id]][[line]][pos])

            # } else if(rules_list[[id]][[line]][2] == "aver") { #average
            #   maptmp = as.vector(t(mapdf[rules_list[[id]][[line]][pos]]))
            #   rulevars[[id]][[asp]][[lpos]][s] = aggregate(maptmp, by = levagg, FUN = "mean")
            # } else if(rules_list[[id]][[line]][2] == "mode") { #median
            #   maptmp = as.vector(t(mapdf[rules_list[[id]][[line]][pos]]))
            #   rulevars[[id]][[asp]][[lpos]][s] = aggregate(maptmp, by = levagg, FUN = "median")
            # } else if(rules_list[[id]][[line]][2] == "area") { #only for state var area
            #   rulevars[[id]][[asp]][[lpos]][s] = aggregate(cellarea, by = levagg, FUN = "sum")

            } else { print(paste("Unexpected 2nd element on line",line)) }

        }
      }
    }
  }

  # ---------- Replicate existing patch and strata state variables for each aspatial patch ----------
  #DO THIS IN world_gen when building world file
  #

  lret = list(rulevars,sindex)
  return(lret)

} #end function
