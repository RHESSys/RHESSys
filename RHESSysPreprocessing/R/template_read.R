#' template_read
#'
#' Reads in a RHESSys template file, produces a clean list, variable name vector, indices of levels and variables
#' @param template template name and path to be read in
#' @param asp_check flag for if aspatial patches are being used/if the asp_rule map needs to be identified.
#' @return Returns a list containing (in order): template in list form, variable name vector, index of levels,
#' index of variables, full table of maps and associated variables
#' @author Will Burke

template_read = function(template){

  # ---------- Read in template ----------
  con = file(template, open ="r") # commect to file
  read = readLines(con) # read file, default reads entire file, line by line
  close(con)

  trim = trimws(read)
  trimlines = trim[trim!=""]
  head = trimlines[1:min(which(startsWith(trimlines, "_")))]
  template_clean = strsplit(trimlines,"[ \t]+") # remove whitespaces, split strings by spaces or tabs

  var_names = unlist(lapply(template_clean,"[[",1)) #all names of state variables in template

  level_index = which(startsWith(var_names, "_")) # find lines that start w/ "_", get row nums of levels
  level_maps = lapply(template_clean[level_index],"[",2)# level map names, for use in GRASS
  var_index = level_index[2]:length(template_clean)
  var_index = var_index[! var_index %in% level_index] #make index for template, excluding def files and levels

  # Find all maps - in addiiion to level maps
  maps_all = vector()
  maps_index = vector()
  for (i in var_index) {
    if ( suppressWarnings(all(is.na(as.numeric( template_clean[[i]][3]))))  & length(template_clean[[i]]) != 2) {
      maps_all[i] = template_clean[[i]][3]
      maps_index[i] = i }
    if (length(template_clean[[i]]) == 5 ) {
      maps_all[i] = template_clean[[i]][5]
      maps_index[i] = i }}

  maps_index = maps_index[!is.na(maps_index)] # index of rows w/ maps
  map_names = sapply(template_clean[maps_index], function(x) x[1])
  map_info = cbind(c("world","basin","hillslope","zone","patch","strata", map_names),c(unlist(level_maps),maps_all[!is.na(maps_all)]))
  colnames(map_info) = c("MapName","Map")
  map_info = unique(map_info)

  template_list = list(template_clean,var_names,level_index,var_index,map_info,head)
  return(template_list)

}
