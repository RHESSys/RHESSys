# find patch borders by row.
# patch0 is patch data with NA replaced by 0, patch_number is a vector of ordered patch numbers
# border_count is an array, initailly 0 but will be filled with the number of times patch i 
# touches patch j. the diagonal will be the number of times patch i touches anything.
find_border_row<-function(patch0,patch_number,border_count){
  p_rows_less_1<-nrow(patch0)-1
  p_cols_less_1<-ncol(patch0)-1
  for (i in 1:p_rows_less_1) {
    for (j in 1:p_cols_less_1){
      if ((patch0[i,j]*patch0[i,j+1]*patch0[i+1,j]!=0)){  # nothing is 0
        if (patch0[i,j]!=patch0[i,j+1]){  # rt. side boundary
          patch_ij<-which(patch_number==patch0[i,j])   #index of patch i,j
          patch_ij1<-which(patch_number==patch0[i,j+1])  #index of patch i,j+1
          
          # increment border_count
          border_count[patch_ij,patch_ij1]<-border_count[patch_ij,patch_ij1]+1
          border_count[patch_ij1,patch_ij]<-border_count[patch_ij1,patch_ij]+1
          border_count[patch_ij,patch_ij]<-border_count[patch_ij,patch_ij]+1
          border_count[patch_ij1,patch_ij1]<-border_count[patch_ij1,patch_ij1]+1
        }
        if (patch0[i,j]!=patch0[i+1,j]){  #lower boundary
          patch_ij<-which(patch_number==patch0[i,j])   #index of patch i,j
          patch_i1j<-which(patch_number==patch0[i+1,j])  #index of patch i+1,j
          
          # increment border_count
          border_count[patch_ij,patch_i1j]<-border_count[patch_ij,patch_i1j]+1
          border_count[patch_i1j,patch_ij]<-border_count[patch_i1j,patch_ij]+1
          border_count[patch_ij,patch_ij]<-border_count[patch_ij,patch_ij]+1
          border_count[patch_i1j,patch_i1j]<-border_count[patch_i1j,patch_i1j]+1
        }
      }
    }
  }
  return(border_count)
}