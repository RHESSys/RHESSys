# reads in flow table and makes 3d diagram of flow routing
file_make_net_3d<-function(flw_file){
  flw<-read_in_flow(flw_file)
  make_net_3d_pic(flw)
}