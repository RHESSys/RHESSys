#' Build Redefine Template
#'
#' Builds a template file for use in a redefine world.  Allows for specification of which variables to override and replace
#' with an absolute or multiplier value. Should be paired with world_redefine function (which is essentially world_gen), to
#' create the actual redefine worldfile.
#' @param template Original template file used to generate your worldfile for RHESSys.
#' @param name Name of output redefine template to be created
#' @param replace DOESNT WORK YET -Indicates the variable-method-values to replace. All others will be given values of "-9999"
#' @param overwrite Overwrite existing redefine template? FALSE is default and prompts a menu if worldfile already exists.
#' @author Will Burke
#' @export

build_redefine_template = function(template, name, replace, overwrite) {

  template_list = template_read(template)

  template_clean = template_list[[1]] # template in list form
  #var_names = template_list[[2]] # names of template vars
  level_index = template_list[[3]] # index of level separators in template_clean/var_names
  var_index = template_list[[4]] # index of vars
  #map_info = template_list[[5]] # tables of maps and their inputs/names in the template
  #head = template_list[[6]] # header
  #maps_in = unique(map_info[,2])


  redef_template = template_clean

  # Set all vars to -9999
  for (i in var_index) {
    redef_template[[i]] = c(redef_template[[i]][1], "value", "-9999")
  }

  # Replace specific values here
  # NOT HERE YETTT

  # print template
  template_write = vector(mode = "character")
  for (i in min(level_index):length(redef_template)) {
    indent = rep(x = "\t", sum(level_index < i))
    template_write = c(template_write, indent, redef_template[[i]], "\n")
  }

  sink(name)
  cat(template_write)
  sink()

}
