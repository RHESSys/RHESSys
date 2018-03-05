#' Aspatial Patch Generation
#'
#' @author Will Burke

aspatial_patches = function(asprules,asp_mapdata) {

  asprules = "testrules.rules"
  asp_mapdata = 3

  # --------- Read rules file ----------
  con = file(asprules, open ="r") # commect to file
  readrules = readLines(con) # read file, default reads entire file, line by line
  close(con)

  rules_trim = trimws(readrules)
  rules_trim = rules_trim[!rules_trim==""]
  #rules_head = rules_trim[0:(min(which(startsWith(rules_trim,"ID")))-1)] # separate header
  rules_head = rules_trim[which(startsWith(rules_trim,"#"))]
  #rules_trim = rules_trim[min(which(startsWith(rules_trim,"ID"))):length(rules_trim)] #
  rules_trim = rules_trim[-which(startsWith(rules_trim,"#"))]
  id_ind = which(startsWith(rules_trim,"ID")) # index IDs
  patch_ind = c(which(startsWith(rules_trim,"_patch")), which(startsWith(rules_trim,"_Patch"))) # index patches
  strata_ind = c(which(startsWith(rules_trim,"_stratum")), which(startsWith(rules_trim,"_canopy_strata"))) # index strata
  rule_split = strsplit(rules_trim,"[ \t]+") # split strings at tabs and spaces
  splitAt <- function(x, pos) unname(split(x, cumsum(seq_along(x) %in% pos)))

  # new method
  ID_level_split = splitAt(rule_split,c(id_ind,patch_ind,strata_ind)) # split rules by IDs, patches, and strata
  rule_list2 = splitAt(ID_level_split,seq(1,length(ID_level_split),3)) # split again by rule IDs - list structure: ID-id,patch,strata-statevars for each
  names(rule_list2) = paste("rule_",sapply(lapply(rule_list2,"[[",1),"[[",1)[2,],sep = "")
  id_map_unq = unique(asp_mapdata)[[1]] # get rule IDs from map/input

  rm(rules_trim,rules_head,id_ind,patch_ind,strata_ind,rule_split)

  # ---------- VERSION 2  build rulevars based on rules and map ----------

  id_map_unq_tags = paste("rule_",id_map_unq,sep = "") # all map IDs + tags for referencing in code

  rulevars2 = as.list(rep(0,length(id_map_unq)))
  strata_index = as.list(rep(0,length(id_map_unq)))
  names(rulevars2) = id_map_unq_tags
  names(strata_index) = id_map_unq_tags

  for (id_tag in id_map_unq_tags) { # iterate through rule IDs
    aspatial_count = as.numeric(rule_list2[[id_tag]][[1]][[which(sapply(rule_list2[[id_tag]][[1]],"[[",1)=="subpatch_count")]][2])
    rulevars2[[id_tag]] = as.list(rep(0,aspatial_count)) # add list for each aspatial patch
    statevar_len = c(length(rule_list2[[id_tag]][[2]]),length(rule_list2[[id_tag]][[3]])) # count of statevars for patch,strata
    if(statevar_len[1]>1){patch_ind = 2:statevar_len[1]} else {patch_ind = NULL} # index of patches
    if(statevar_len[2]>1){strata_ind = 2:statevar_len[2]} else {strata_ind = NULL} # index of strata
    strata_ct = as.integer(rule_list2[[id_tag]][[3]][[1]][2]) # get strata count

    for (asp in 1:aspatial_count) { # iterate through aspatial patches/subpatches
      rulevars2[[id_tag]][[asp]] = as.list(rep(0,sum(statevar_len)-2)) # add list w length = number of state vars

      for(line in patch_ind){ # iteratre through lines for patch state vars
        asp_sep = which(rule_list2[[id_tag]][[2]][[line]]=="|")
        asp_ind = c(asp_sep[1]-1,asp_sep+1)
        rulevars2[[id_tag]][[asp]][[line-1]] = as.list(rep(0,1)) # make empty list for each patch state var
        if(rule_list2[[id_tag]][[2]][[line]][2] == "value") { #use value
          rulevars2[[id_tag]][[asp]][[line-1]][[1]] = as.double(rule_list2[[id_tag]][[2]][[line]][asp_ind[asp]])
        } else if(rule_list2[[id_tag]][[2]][[line]][2] == "dvalue") { #integer value
          rulevars2[[id_tag]][[asp]][[line-1]][[1]] = as.integer(rule_list2[[id_tag]][[2]][[line]][asp_ind[asp]])
        } else { print(paste("Unexpected 2nd element on line",line))}

        names(rulevars2[[id_tag]][[asp]])[[line-1]] = rule_list2[[id_tag]][[2]][[line]][1]
      }

      for(line in strata_ind){ # iterate through lines for strata state vars
        asp_sep = which(rule_list2[[id_tag]][[3]][[line]]=="|")
        asp_sep = c(2,asp_sep,length(rule_list2[[id_tag]][[3]][[line]])+1)
        asp_ind = matrix(nrow=aspatial_count,ncol=strata_ct)
        for(x in 1:aspatial_count){ # sort out aspatial patch and statum index
          if(length((asp_sep[x]+1):(asp_sep[x+1]-1)) == strata_ct){
            asp_ind[x,] = (asp_sep[x]+1):(asp_sep[x+1]-1)
          } else if (length((asp_sep[x]+1):(asp_sep[x+1]-1)) == 1){
            asp_ind[x,] = rep(asp_sep[x]+1,strata_ct)
          }
        }

        line_strata = max(patch_ind) + line - 2 # output line for strat vars
        rulevars2[[id_tag]][[asp]][[line_strata]] = as.list(rep(0,strata_ct)) # make empty list with size of strata num
        names(rulevars2[[id_tag]][[asp]])[[line_strata]] = rule_list2[[id_tag]][[3]][[line]][1]

        for(s in 1:strata_ct){
          if(rule_list2[[id_tag]][[3]][[line]][2] == "value") { #use value
            rulevars2[[id_tag]][[asp]][[line_strata]][[s]] = as.double(rule_list2[[id_tag]][[3]][[line]][asp_ind[asp,s]])
          } else if(rule_list2[[id_tag]][[3]][[line]][2] == "dvalue") { #integer value
            rulevars2[[id_tag]][[asp]][[line_strata]][[s]] = as.integer(rule_list2[[id_tag]][[3]][[line]][asp_ind[asp,s]])
          } else { print(paste("Unexpected 2nd element on line",line))}
        }
      } # end line strata itr
    } # end asp itr
    strata_index[[id_tag]] = min(strata_ind) # index to pass on of where strata starts
  } # end rule ID itr

  lret = list(rulevars2,strata_index)
  return(lret)

} #end function
