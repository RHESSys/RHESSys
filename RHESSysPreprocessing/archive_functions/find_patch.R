#
find_patch<-function(flw,patch_num){
  num_p<-length(flw)
  for (i in 1:num_p){
    if (flw[[i]]$PatchID==patch_num){
      return(i)
    }
  }
}
#
