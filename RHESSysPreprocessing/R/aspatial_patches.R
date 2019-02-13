#' Aspatial Patch Generation
#'
#' @author Will Burke

aspatial_patches = function(asprules,asp_mapdata) {

  # ---------- Read rules file ----------
  con = file(asprules, open = "r") # commect to file
  readrules = readLines(con) # read file, default reads entire file, line by line
  close(con)

  # ---------- Parse into list of all rules ----------
  rules_in_trim = trimws(readrules)
  rules_in_trim = rules_in_trim[!rules_in_trim == ""]
  #rules_head = rules_in_trim[0:(min(which(startsWith(rules_in_trim,"ID")))-1)] # separate header
  rules_header = rules_in_trim[which(startsWith(rules_in_trim,"#"))] # this is for later use potentially, not being kept currently
  #rules_in_trim = rules_in_trim[min(which(startsWith(rules_in_trim,"ID"))):length(rules_in_trim)]
  rules_in_trim = rules_in_trim[-which(startsWith(rules_in_trim,"#"))]
  id_ind = which(startsWith(rules_in_trim,"ID")) # index IDs
  patch_ind = c(which(startsWith(rules_in_trim,"_patch")), which(startsWith(rules_in_trim,"_Patch"))) # index patches
  strata_ind = c(which(startsWith(rules_in_trim,"_stratum")), which(startsWith(rules_in_trim,"_canopy_strata"))) # index strata
  rule_split = strsplit(rules_in_trim,"[ \t]+") # split strings at tabs and spaces
  splitAt <- function(x, pos) unname(split(x, cumsum(seq_along(x) %in% pos)))
  id_split = splitAt(rule_split,c(id_ind,patch_ind,strata_ind)) # split rules by IDs, patches, and strata
  rule_list = splitAt(id_split,seq(1,length(id_split),3)) # split again by rule IDs -
  # list structure: top level list -id info (ID and subpatch count),patch,strata. second -statevars and values for each
  names(rule_list) = paste("rule_",sapply(lapply(rule_list,"[[",1),"[[",1)[2,],sep = "")

  # ---------- Build output data object(s) of patch and strata statevars ----------

  # STRUCTURE CHANGE
  # List of rule IDs
  # Each rule is a list, containing:
  # patch data frame, rows are attributes, cols are subpatches
  # strata list,containing
  # strata data frames (num = patch ct) - rows are statevars, cols are strata

  # ---------- build rulevars based on rules and map data ----------
  map_ids = unique(asp_mapdata)[[1]] # get rule IDs from map/input
  map_id_tags = paste("rule_",map_ids,sep = "") # all map IDs concated w tags for referencing/reading in code

  asp_vars = as.list(rep(0,length(map_ids))) # highest level list of the different rules
  # strata_index = as.list(rep(0,length(map_ids))) # get rid of in this version i think
  names(asp_vars) = map_id_tags
  # names(strata_index) = map_id_tags

  for (id_tag in map_id_tags) { # iterate through rule IDs

    subpatch_ct = as.numeric(rule_list[[id_tag]][[1]][[which(sapply(rule_list[[id_tag]][[1]],"[[",1) == "subpatch_count")]][2])
    patch_var_list = rule_list[[id_tag]][[2]] # get patch vars
    if (length(patch_var_list) == 1) { # if no vars, just header, make NULL
      patch_var_list = NULL
    } else {
      patch_var_list = patch_var_list[-which(sapply(patch_var_list, "[[", 1) == "_patch")]
    }
    patch_df = as.data.frame(matrix(ncol = (subpatch_ct + 1), nrow = length(patch_var_list))) # data frame for sub patch state vars
    names(patch_df) = c("state_var", paste("patch_",1:subpatch_ct,sep = ""))
    patch_df$state_var = sapply(patch_var_list, "[[", 1) # add state var names - assumes first item is state var name - pretty safe assumption

    if (!is.null(patch_var_list)) {
      for (var in 1:length(patch_var_list)) { # go through every patch level variable

        if (patch_var_list[[var]][[2]] == "value" | patch_var_list[[var]][[2]] == "dvalue") { # check if second element is equation/modifier
          patch_vars = patch_var_list[[var]][3:length(patch_var_list[[var]])] # extract everything else
        } else {
          patch_vars = patch_var_list[[var]][2:length(patch_var_list[[var]])]
        }
        # ----- Parsing -----
        num_index = which(suppressWarnings(!is.na(as.numeric(patch_vars)))) # positions of numbers
        if (length(num_index) > subpatch_ct) {
          stop(paste(id_tag,"patch variable",var,"contains more values than (aspatial) patches"))
        }
        if (length(num_index) == subpatch_ct | length(num_index) == 1) { # if it's the right number of values or 1 value
          patch_df[var,2:length(patch_df[var,])] = as.numeric(patch_vars[num_index])
        } else if (length(num_index) < subpatch_ct) { # messier if using | to denote where to fill in w base template
          var_split = splitAt(patch_vars, which(patch_vars == "|"))
          patch_df[var,2] = as.numeric(var_split[[1]])
          for (i in 2:length(var_split)) {
            if (length(var_split[[i]]) == 1) {var_split[[i]][2] = NA}
          }
          patch_df[var,3:length(patch_df[var,])] = sapply(var_split[2:length(var_split)], "[[", 2)
        }
      }
    }

    # ----- Strata -----
    strata_var_list = rule_list[[id_tag]][[3]] # get strata vars
    strata_ct = as.integer(strata_var_list[[1]][2]) # get strata count
    if (length(strata_var_list) == 1) { # if no vars, just header
      strata_var_list = NULL
    } else {
      strata_var_list = strata_var_list[-which(sapply(strata_var_list, "[[", 1) == "_canopy_strata")]
    }

    strata_df = as.data.frame(matrix(ncol = (strata_ct + 1), nrow = length(strata_var_list))) # data frame for strata
    names(strata_df) = c("state_var", paste("canopy_strata_",c(1:strata_ct),sep = ""))
    strata_df$state_var = sapply(strata_var_list, "[[", 1) # add state var names
    strata_list = replicate(subpatch_ct,strata_df,simplify = FALSE) # num of df = strata
    names(strata_list) =  paste("patch_",1:subpatch_ct,sep = "") # this has to be done again later since the names get lost in lapply

    if (!is.null(strata_var_list)) {
      for (var in 1:length(strata_var_list)) { # go through strata state vars

        if (strata_var_list[[var]][[2]] == "value" | strata_var_list[[var]][[2]] == "dvalue") { # check if second element is equation/modifier
          strata_vars = strata_var_list[[var]][3:length(strata_var_list[[var]])] # extract everything else
        } else {
          strata_vars = strata_var_list[[var]][2:length(strata_var_list[[var]])]
        }
        num_index = which(suppressWarnings(!is.na(as.numeric(strata_vars)))) # positions of numbers
        if (length(num_index) > subpatch_ct * strata_ct) {
          stop(paste(id_tag,"strata variable",var,"contains more values than (aspatial) patches"))
        }

        if (length(num_index) == subpatch_ct * strata_ct) { # if it's the right number of values
          strata_list = lapply(seq_along(strata_list), FUN = function(x,y) {y[[x]][var,2:length(y[[x]][var,])] = strata_vars[num_index][c(x * 2 - 1, x * 2)] ; return(y[[x]])}, y = strata_list)
        } else if ((length(num_index) == strata_ct | length(num_index) == 1) & !any(strata_vars == "|")) { # if its the same number of values as strata or just 1 value
          strata_list = lapply(seq_along(strata_list), FUN = function(x,y) {y[[x]][var,2:length(y[[x]][var,])] = strata_vars[num_index] ; return(y[[x]])}, y = strata_list)
        } else if (length(num_index) < subpatch_ct * strata_ct) { # messier if using | to denote where to fill in w base template
          var_split = splitAt(strata_vars, which(strata_vars == "|")) # split the values by |

          if (length(var_split) != subpatch_ct) {stop(paste(id_tag,"strata variable",var,"doesn't have correct number of values or | separators for subpatches"))}

          var_split = lapply(var_split, function(x) {
            if (x[1] == "|" & length(x) == 1) {x = rep(NA, strata_ct)
            } else if (x[1] == "|" & length(x) == 2) {x = rep(x[2], strata_ct)
            } else if (x[1] == "|" & length(x) == 3) {x = x[2:3]
            }
            return(x)
          })

          strata_list = lapply(seq_along(strata_list), FUN = function(x,y) {y[[x]][var,2:length(y[[x]][var,])] = as.numeric(var_split[[x]]) ; return(y[[x]])}, y = strata_list)

        }

        # for (s in 1:subpatch_ct) { # for each (sub)patch
        #   # ----- Parsing -----
        #   # if only given strata values for 1 subpatch, replicate to others, unless | are used to denote empty subpatches
        #   # if only 1 value is given for 2 strata, the 1 value is replicated. NA can be input to instead use the template value
        #   if (length(num_index) == subpatch_ct * strata_ct) { # if it's the right number of values
        #     strata_list[[s]][var,2:length(strata_list[[s]][var,])] = as.numeric(strata_vars[num_index[c(s * 2 - 1, s * 2)]])
        #   } else if ((length(num_index) == strata_ct | length(num_index) == 1) & !any(strata_vars == "|")) { # if its the same number of values as strata or just 1 value
        #     strata_list[[s]][var,2:length(strata_list[[s]][var,])] = as.numeric(strata_vars[num_index])
        #   } else if (length(num_index) < subpatch_ct * strata_ct) { # messier if using | to denote where to fill in w base template
        #     var_split = splitAt(strata_vars, which(strata_vars == "|")) # split the values by |
        #
        #     if (length(var_split) != subpatch_ct) {stop(paste(id_tag,"strata variable",var,"doesn't have correct number of values for subpatches"))}
        #
        #     for (i in 1:length(var_split)) { # iterate through the split value list
        #       if ((length(var_split[[i]]) == strata_ct | length(var_split[[i]]) == 1) & var_split[[i]][1] != "|") { # values == num strata, no | , use values - this is the first item in the split list
        #         strata_list[[s]][var,2:(strata_ct + 1)] = as.numeric(var_split[[1]])
        #       } else if (length(var_split[[i]]) == strata_ct + 1 & var_split[[i]][1] == "|") { # values = num strata +1, first is |
        #         strata_list[[s]][var,2:(strata_ct + 1)] = as.numeric(var_split[[1]][2:(strata_ct + 1)])
        #       }else if (length(var_split[[i]]) == 1 & var_split[[i]][1] == "|" ) { # if only "|" char, make both strata NA
        #         strata_list[[s]][var,2:(strata_ct + 1)] = NA
        #       } else if (length(var_split[[i]]) == 2 & var_split[[i]][1] == "|") { # if only 1 value, replicate for both strata
        #         strata_list[[s]][var,2:(strata_ct + 1)] = var_split[[i]][2]
        #       }
        #     }
        #     #strata_list[[s]][var,2:length(strata_list[[s]][var,])] = sapply(var_split[2:length(var_split)], "[[", 2)
        #   }
        #
        # } # end subpatch loop

      }
    }

    # ----- combine data frames into asp_vars list -----

    names(strata_list) =  paste("patch_",1:subpatch_ct,sep = "")

    asp_vars[[id_tag]] = list(patch_df, strata_list)
    names(asp_vars[[id_tag]]) = c("patch_level_vars", "strata_level_vars")

  } # end rule ID itr


  # ----- old (working) loops to build aspatial output -----
  # ---------- build rulevars based on rules and map data ----------
  # map_ids = unique(asp_mapdata)[[1]] # get rule IDs from map/input
  # map_id_tags = paste("rule_",map_ids,sep = "") # all map IDs concated w tags for referencing/reading in code
  #
  # asp_vars = as.list(rep(0,length(map_ids))) # empty list, to be output
  # strata_index = as.list(rep(0,length(map_ids))) #
  # names(asp_vars) = map_id_tags
  # names(strata_index) = map_id_tags
  #
  # for (id_tag in map_id_tags) { # iterate through rules/IDs
  #   subpatch_ct = as.numeric(rule_list[[id_tag]][[1]][[which(sapply(rule_list[[id_tag]][[1]],"[[",1) == "subpatch_count")]][2]) # count of patches within family/aspatial patches
  #   asp_vars[[id_tag]] = as.list(rep(0,subpatch_ct)) # add list for each aspatial patch
  #   statevar_len = c(length(rule_list[[id_tag]][[2]]),length(rule_list[[id_tag]][[3]])) # count of statevars for patch,strata
  #   if (statevar_len[1] > 1) {patch_ind = 2:statevar_len[1]} else {patch_ind = NULL} # index of patches
  #   if (statevar_len[2] > 1) {strata_ind = 2:statevar_len[2]} else {strata_ind = NULL} # index of strata
  #   strata_ct = as.integer(rule_list[[id_tag]][[3]][[1]][2]) # get strata count
  #
  #   for (asp in 1:subpatch_ct) { # iterate through aspatial patches/subpatches
  #     asp_vars[[id_tag]][[asp]] = as.list(rep(0,sum(statevar_len) - 2)) # add list w length = number of state vars
  #
  #     for (line in patch_ind) { # iteratre through lines for patch state vars
  #       asp_sep = which(rule_list[[id_tag]][[2]][[line]] == "|")
  #       asp_ind = c(asp_sep[1] - 1,asp_sep + 1)
  #       asp_vars[[id_tag]][[asp]][[line - 1]] = as.list(rep(0,1)) # make empty list for each patch state var
  #       if (rule_list[[id_tag]][[2]][[line]][2] == "value") { #use value
  #         asp_vars[[id_tag]][[asp]][[line - 1]][[1]] = as.double(rule_list[[id_tag]][[2]][[line]][asp_ind[asp]])
  #       } else if (rule_list[[id_tag]][[2]][[line]][2] == "dvalue") { #integer value
  #         asp_vars[[id_tag]][[asp]][[line - 1]][[1]] = as.integer(rule_list[[id_tag]][[2]][[line]][asp_ind[asp]])
  #       } else {print(paste("Unexpected 2nd element on line",line))}
  #
  #       names(asp_vars[[id_tag]][[asp]])[[line - 1]] = rule_list[[id_tag]][[2]][[line]][1]
  #     }
  #
  #     for (line in strata_ind) { # iterate through lines for strata state vars
  #       asp_sep = which(rule_list[[id_tag]][[3]][[line]] == "|")
  #       asp_sep = c(2,asp_sep,length(rule_list[[id_tag]][[3]][[line]]) + 1)
  #       asp_ind = matrix(nrow = subpatch_ct,ncol = strata_ct)
  #       for (x in 1:subpatch_ct) { # sort out aspatial patch and statum index
  #         if (length((asp_sep[x] + 1):(asp_sep[x + 1] - 1)) == strata_ct) {
  #           asp_ind[x,] = (asp_sep[x] + 1):(asp_sep[x + 1] - 1)
  #         } else if (length((asp_sep[x] + 1):(asp_sep[x + 1] - 1)) == 1) {
  #           asp_ind[x,] = rep(asp_sep[x] + 1,strata_ct)
  #         }
  #       }
  #
  #       line_strata = max(patch_ind) + line - 2 # output line for strat vars
  #       asp_vars[[id_tag]][[asp]][[line_strata]] = as.list(rep(0,strata_ct)) # make empty list with size of strata num
  #       names(asp_vars[[id_tag]][[asp]])[[line_strata]] = rule_list[[id_tag]][[3]][[line]][1]
  #
  #       for (s in 1:strata_ct) {
  #         if (rule_list[[id_tag]][[3]][[line]][2] == "value") { #use value
  #           asp_vars[[id_tag]][[asp]][[line_strata]][[s]] = as.double(rule_list[[id_tag]][[3]][[line]][asp_ind[asp,s]])
  #         } else if (rule_list[[id_tag]][[3]][[line]][2] == "dvalue") { #integer value
  #           asp_vars[[id_tag]][[asp]][[line_strata]][[s]] = as.integer(rule_list[[id_tag]][[3]][[line]][asp_ind[asp,s]])
  #         } else {print(paste("Unexpected 2nd element on line",line))}
  #       }
  #     } # end line strata itr
  #   } # end asp itr
  #   strata_index[[id_tag]] = min(strata_ind) # index to pass on of where strata starts
  # } # end rule ID itr


  # ---------- Output ----------

  return(asp_vars)

  #lret = list(asp_vars,strata_index)
  #return(lret)

} #end function
