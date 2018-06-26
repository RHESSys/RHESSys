# find 2d distance between two nodes
d2_node_dist<-function(flw,n1,n2){
  dist<-sqrt((flw[[n1]]$Centroidx-flw[[n2]]$Centroidx)^2+(flw[[n1]]$Centroidy-flw[[n2]]$Centroidy)^2)
  return(dist)
}
