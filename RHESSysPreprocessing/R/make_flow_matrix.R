# Make a matix where the indices are the patch ID's and the values are the proportion of flow between patches
make_flow_matrix<-function(flw){    #flw is a list of patches made by function read_in_flow
  num_patches<-length(flw)
  flw_mat<-matrix(0,num_patches,num_patches)
  src_p<-c()    #initialize sor and target
  tar_p<-c()
  for (p_i in 1:num_patches){
    num_neighbors<-flw[[p_i]]$numberofadjacentpatches
    
    for (j in 1:num_neighbors){
      n_id<-flw[[p_i]]$neighbors[[j]][1]
      n_flow<-flw[[p_i]]$neighbors[[j]][5]
      flw_mat[p_i,n_id]<-n_flow
    }
  }
  for (p_i in 1:num_patches){
    neigbors<-which(flw_mat[p_i,]!=0)
    src_p<-c(src_p,rep(p_i,length(neigbors)))
    tar_p<-c(tar_p,neigbors)
  }
  out_frame<-data.frame(as.character(src_p),as.character(tar_p))
  return(flw_mat)
  #return(out_frame)
}
#