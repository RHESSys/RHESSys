# Searches through nodes, if a node has a road, it finds the nearest stream in the nodes descendants.
# If there are no streams it uses the final basin (sum of gamma ==0). Once the stream is found, the
# landtype is changed to 2, and the roadtype variable gets the node number.
find_stream<-function(flw,road_width){
  len_flw<-length(flw)
  for (i in 1:len_flw){
    if ((flw[[i]]$Roadtype==1)&(sum(flw[[i]]$Gamma_i)!=0)){
      desc<-child_node_list(flw,i)
      strm_chld<-c()  #list of descendants with a stream or bottom
      strm_dist<-c()  # 2d distance to each stream
      for (j in desc) {
        if ((flw[[j]]$Landtype==1)|(sum(flw[[j]]$Gamma_i)==0)) { #has a stream or is bottom
          strm_chld<-c(strm_chld,j)
          strm_dist<-c(strm_dist,d2_node_dist(flw,i,j))
        }
      }
      strm_chld<-strm_chld[order(strm_dist)] # order list by 2d distance
      flw[[i]]$Roadtype<-strm_chld[[1]]  #pick closest one
    }
  }
  for (i in 1:len_flw){
    if (flw[[i]]$Roadtype!=0){
      strm<-flw[[i]]$Roadtype
      flw[[i]]$Landtype<-2
      flw[[i]]$Roadtype<-c(flw[[strm]]$PatchID,flw[[strm]]$ZoneID,flw[[strm]]$HillID,road_width)
    }
  }
  return(flw)
}
