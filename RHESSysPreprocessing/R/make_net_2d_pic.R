# input flow table to create a 2d diagram of flow routing
make_net_2d_pic<-function(flw){
  ntp<-make_flow_table(flw)
  coords<-make_coord_table(flw)
  dtp=graph.adjacency(ntp,mode="directed",weighted = TRUE,diag = FALSE)
  plot.igraph(dtp,layout=coords[,1:2],edge.width=E(dtp)$weight*2,edge.arrow.size=0.2,vertex.size=5)
}