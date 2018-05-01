# input flow table to create a 3d diagram of flow routing
make_net_3d_pic<-function(flw){
  ntp<-make_flow_table(flw)
  coords<-make_coord_table(flw)
  dtp=graph.adjacency(ntp,mode="directed",weighted = TRUE,diag = FALSE)
  rglplot(dtp,layout=coords[,1:3],edge.width=E(dtp)$weight*2,edge.arrow.size=0.2,vertex.size=5)
}