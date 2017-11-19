# takes a list of nodes, returns their elevations
elev_list<-function(flw,n_list){
  out_list<-c()
  for (i in n_list) {
    out_list<-c(out_list,flw[[i]]$Centroidz)
  }
  return(out_list)
}
#
