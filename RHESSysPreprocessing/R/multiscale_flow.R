# multiscale_flow
# function that transforms flowtable into multiscale flow table
# Will Burke 1/16/19

multiscale_flow = function(CF1, map_ar_clean, cfmaps, asp_list){
  
  # Inputs: exisiting flow table (list), map arrays, map list, aspatial/multiscale rule list 
  
  # ----- Variable setup -----
  asp_map = map_ar_clean[, ,cfmaps[cfmaps[,1] == "asp_rule",2]] # matrix of aspatial rules
  patch_ID = unlist(lapply(CF1, "[[",9)) # patch IDs from cf1
  numbers = unlist(lapply(CF1, "[[",1)) # flow list numbers
  raw_patch_data = map_ar_clean[, , cfmaps[cfmaps[, 1] == "patch", 2]] # get patch matrix inside the function
  rulevars = asp_list[[1]] # get rules - state variable overrides
  CF2 = list() # empty list for new flow list
  
  # ----- iterate through (spatial) patches -----
  for (p in raw_patch_data[!is.na(raw_patch_data)]) { 
    id = unique(asp_map[which(raw_patch_data == p)]) # get unique rule ID for patch p
    
    if (length(id) > 1) {stop(paste("multiple aspatial rules for patch",p))} # if multiple rules for a single patch
    asp_count = length(rulevars[[id]]) # get number of aspatial patches for current patch
    
    # ----- iterate through aspatial patches -----
    for (asp in 1:asp_count) { 
      
      CF2 = c(CF2,CF1[which(patch_ID == p)]) # copy (aspatial) patch values from old patch (family)
      CF2[[length(CF2)]]$PatchID = CF2[[length(CF2)]]$PatchID * 100 + asp # aspatial patch ID is old patch ID *100 + aspatial number
      CF2[[length(CF2)]]$Number = CF2[[length(CF2)]]$Number * 100 + asp # same modification to number
      CF2[[length(CF2)]]["PatchFamilyID"] = CF1[[which(patch_ID == p)]]$PatchID # retain old patch ID as patch family ID XXXXXXXXX IF CF2 DOESNT WORK REMOVE THIS
      CF2[[length(CF2)]]$Area = CF2[[length(CF2)]]$Area * rulevars[[id]][[asp]]$pct_family_area[[1]] # change area
      
      # Changes for each neighbor
      old_nbrs = CF2[[length(CF2)]]$Neighbors
      new_nbrs = vector(mode = "numeric")
      old_gammas = CF2[[length(CF2)]]$Gamma_i
      new_gammas = vector(mode = "numeric")
      old_slope = CF2[[length(CF2)]]$Slope
      new_slope = vector(mode = "numeric")
      old_boarder = CF2[[length(CF2)]]$Boarder
      new_boarder = vector(mode = "numeric")
      
      for (nbr in old_nbrs) { # loop through old neighbors - neighbors are numbers not patches
        nbr_patch = patch_ID[numbers == nbr]
        nbr_id = asp_map[which(raw_patch_data == nbr_patch)]
        nbr_id = unique(nbr_id)
        if (length(nbr_id) > 1) {stop(paste("multiple aspatial rules for patch",nbr_patch))} # if multiple rules for a single patch
        nbr_asp_ct = length(rulevars[[nbr_id]])
        gamma = old_gammas[which(old_nbrs == nbr)]
        new_slope = c(new_slope,rep(old_slope[old_nbrs[nbr]],nbr_asp_ct))
        new_boarder = c(new_boarder,rep(old_boarder[old_nbrs[nbr]],nbr_asp_ct))
        
        for (nbr_asp in 1:nbr_asp_ct) { # for each asp for each neighbor
          new_nbrs = c(new_nbrs,nbr*100 + nbr_asp) # use same convention as above
          new_gammas = c(new_gammas, gamma * rulevars[[nbr_id]][[nbr_asp]][["pct_family_area"]][[1]] )
          
        }
      }
      CF2[[length(CF2)]]$Neighbors = new_nbrs
      CF2[[length(CF2)]]$Gamma_i = new_gammas
      CF2[[length(CF2)]]$Slopes = new_slope
      CF2[[length(CF2)]]$Boarder = new_boarder
      
      for(i in 1:length(CF2[[length(CF2)]])){
        if(is.na(CF2[[length(CF2)]])[[i]]) {stop("shouldn't have NAs")}
        if(length(CF2[[length(CF2)]][[i]])==0) {stop("shouldn't have numeric(0)'s")}
      }

      
    } # end aspatial patch loop
  } # end spatial patch loop
  
  
  return(CF2)
}