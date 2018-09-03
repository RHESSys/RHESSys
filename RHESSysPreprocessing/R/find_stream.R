# Searches through nodes, if a node has a road, it finds the nearest stream in the nodes descendants.
# If there are no streams it uses the final basin (sum of gamma ==0). Once the stream is found, the
# landtype is changed to 2, and the roadtype variable gets the node number.
find_stream<-function(flw,road_width){
  len_flw<-length(flw)
  for (i in 1:len_flw){
    if ((flw[[i]]$Roadtype==1)&(sum(flw[[i]]$Gamma_i)!=0)){

      # returns a list of all nodes that can be reached from start_node
        len<-length(flw)
        clist_bool<-rep(FALSE,len)
        clist_bool<-find_children(flw,i,clist_bool)
        if (sum(clist_bool)==0){   #if there are no downstream nodes, return 0
          desc = 0
        } else {    # otherwise, return a list of all downstream nodes.
          desc<-seq.int(1,len)[clist_bool]
        }

      strm_chld<-c()  #list of descendants with a stream or bottom
      strm_dist<-c()  # 2d distance to each stream
      for (j in desc) {
        if ((flw[[j]]$Landtype==1)|(sum(flw[[j]]$Gamma_i)==0)) { #has a stream or is bottom
          strm_chld<-c(strm_chld,j)
          # find 2d distance between two nodes
          strm_dist<-c(strm_dist,sqrt((flw[[i]]$Centroidx-flw[[j]]$Centroidx)^2+(flw[[i]]$Centroidy-flw[[j]]$Centroidy)^2))
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
