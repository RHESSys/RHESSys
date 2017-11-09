# Find the average row and column of each patch, Variables are, patch_data: all pathes w unique id and NA's replaced
# by 0, num_patches: number of unique pathces, cell_length: size of individual cells.
patch_centroid_calc<-function(patch_data,num_patches,cell_length){
  patch_coord<-matrix(0,nrow=num_patches,ncol=3)   #x,y positions of each patch, third col is total count.
  for (i in 1:nrow(patch_data)) {
    for (j in 1:ncol(patch_data)) {
      if(patch_data[i,j]!=0){
        patch_coord[patch_data[i,j],1]=patch_coord[patch_data[i,j],1]+j      # sum of x coordinate of cell
        patch_coord[patch_data[i,j],2]=patch_coord[patch_data[i,j],2]+i      # sum of y coordinate of cell
        patch_coord[patch_data[i,j],3]=patch_coord[patch_data[i,j],3]+1      # count of cells
      }
    }
  }
  patch_coord[,1:2]<-patch_coord[,1:2]/patch_coord[,3]
  patch_coord[,3]<-cell_length^2*patch_coord[,3]
  
  return(patch_coord)
}