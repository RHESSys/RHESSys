#' file_make_net_2d
#'
#' Reads in flow table and makes 2d diagram of flow routing
#' @param flw_file Flow table to generate diagram
#' @author Daniel Nash
#' @export
#'
file_make_net_2d<-function(flw_file){
  flw<-read_in_flow(flw_file)
  make_net_2d_pic(flw)
}
