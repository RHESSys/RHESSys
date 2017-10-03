#' Aspatial Patch Generation
#'
#' @author Will Burke

aspatial_patches = function(rules,statevars,asp_map) {

  # --------- Read subpatch rules file ----------

  con = file(rules, open ="r") # commect to file
  readrules = readLines(con) # read file, default reads entire file, line by line
  close(con)
  rtrim = trimws(readrules)
  idind1 = which(startsWith(rtrim,"ID")) # index IDs
  rhead = rtrim[1:(idindex[1] - 1)] # remove and store header above ID
  rtrim2 = rtrim[idindex[1]:length(rtrim)] # trim header
  idind2 = which(startsWith(rtrim2,"ID")) # index IDs
  rsplit = strsplit(rtrim2,"[ \t]+")
  splitAt <- function(x, pos) unname(split(x, cumsum(seq_along(x) %in% pos)))
  rules_list = splitAt(rsplit,idind2)
  rlevind = which(startsWith(trimws(readrules), "_")) # find levels in rules
  IDname = unlist(lapply(lapply(rules_list,"[[",1),"[",2))
  names(rules_list) = IDname

  # ---------- overwrite/modify statevars based on rules and map ----------


}
