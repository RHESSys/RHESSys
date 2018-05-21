# make table of xyz coordinates for each patch in flow table
make_coord_table<-function(flw){
  num_patches<-length(flw)
  p_coord<-matrix(0,num_patches,3)
  for (p_i in 1:num_patches) {
    p_coord[p_i,1]<-flw[[p_i]]$centroidx
    p_coord[p_i,2]<-flw[[p_i]]$centroidy
    p_coord[p_i,3]<-flw[[p_i]]$centroidz
  }
  return(p_coord)
}
#