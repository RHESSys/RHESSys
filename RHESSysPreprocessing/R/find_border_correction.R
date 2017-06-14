# find correction for diagonals. Everytime two adjacent edges touch the same new patch, replace the 2 with 
# sqrt(2) in the border_count matrix.
# patch0 is patch data with NA replaced by 0, patch_number is a vector of ordered patch numbers
# border_count is an array, initailly 0 but will be filled with the number of times patch i 
# touches patch j. the diagonal will be the number of times patch i touches anything.
# 
find_border_correction<-function(patch0,patch_number,border_count){
  rt2crt<-1-sqrt(2)/2   #root 2 correction
  p_rows_less_1<-nrow(patch0)-1
  p_cols_less_1<-ncol(patch0)-1
  for (i in 2:p_rows_less_1) {
    for (j in 2:p_cols_less_1){
      #      4 cases, upper and right, right and lower, lower and left, and left and upper. 
      #     1st case, upper and right (i-1,j+1)
      i_offset<--1
      j_offset<-1
      if ((patch0[i,j]*patch0[i,j+j_offset]*patch0[i+i_offset,j]!=0)){  # nothing is 0
        if ((patch0[i,j]!=patch0[i,j+j_offset])&(patch0[i,j]!=patch0[i+i_offset,j])&
            (patch0[i,j+j_offset]==patch0[i+i_offset,j])){  #diagonal step found
          patch_ij<-which(patch_number==patch0[i,j])   #index of patch i,j
          patch_ij1<-which(patch_number==patch0[i,j+j_offset])  #index of patch i,j+j_offset
          # increment border_count
          border_count[patch_ij,patch_ij1]<-border_count[patch_ij,patch_ij1]-rt2crt
          border_count[patch_ij1,patch_ij]<-border_count[patch_ij1,patch_ij]-rt2crt
          border_count[patch_ij,patch_ij]<-border_count[patch_ij,patch_ij]-rt2crt
          border_count[patch_ij1,patch_ij1]<-border_count[patch_ij1,patch_ij1]-rt2crt
        }
      } 
      #     2nd case, right and lower (i+1,j+1)
      i_offset<-1
      j_offset<-1
      if ((patch0[i,j]*patch0[i,j+j_offset]*patch0[i+i_offset,j]!=0)){  # nothing is 0
        if ((patch0[i,j]!=patch0[i,j+j_offset])&(patch0[i,j]!=patch0[i+i_offset,j])&
            (patch0[i,j+j_offset]==patch0[i+i_offset,j])){  #diagonal step found
          patch_ij<-which(patch_number==patch0[i,j])   #index of patch i,j
          patch_ij1<-which(patch_number==patch0[i,j+j_offset])  #index of patch i,j+j_offset
          # increment border_count
          border_count[patch_ij,patch_ij1]<-border_count[patch_ij,patch_ij1]-rt2crt
          border_count[patch_ij1,patch_ij]<-border_count[patch_ij1,patch_ij]-rt2crt
          border_count[patch_ij,patch_ij]<-border_count[patch_ij,patch_ij]-rt2crt
          border_count[patch_ij1,patch_ij1]<-border_count[patch_ij1,patch_ij1]-rt2crt
        }
      } 
      #     3rd case, left and lower (i+1,j-1)
      i_offset<-1
      j_offset<--1
      if ((patch0[i,j]*patch0[i,j+j_offset]*patch0[i+i_offset,j]!=0)){  # nothing is 0
        if ((patch0[i,j]!=patch0[i,j+j_offset])&(patch0[i,j]!=patch0[i+i_offset,j])&
            (patch0[i,j+j_offset]==patch0[i+i_offset,j])){  #diagonal step found
          patch_ij<-which(patch_number==patch0[i,j])   #index of patch i,j
          patch_ij1<-which(patch_number==patch0[i,j+j_offset])  #index of patch i,j+j_offset
          # increment border_count
          border_count[patch_ij,patch_ij1]<-border_count[patch_ij,patch_ij1]-rt2crt
          border_count[patch_ij1,patch_ij]<-border_count[patch_ij1,patch_ij]-rt2crt
          border_count[patch_ij,patch_ij]<-border_count[patch_ij,patch_ij]-rt2crt
          border_count[patch_ij1,patch_ij1]<-border_count[patch_ij1,patch_ij1]-rt2crt
        }
      } 
      #     4th case, upper and left (i-1,j-1)
      i_offset<--1
      j_offset<--1
      if ((patch0[i,j]*patch0[i,j+j_offset]*patch0[i+i_offset,j]!=0)){  # nothing is 0
        if ((patch0[i,j]!=patch0[i,j+j_offset])&(patch0[i,j]!=patch0[i+i_offset,j])&
            (patch0[i,j+j_offset]==patch0[i+i_offset,j])){  #diagonal step found
          patch_ij<-which(patch_number==patch0[i,j])   #index of patch i,j
          patch_ij1<-which(patch_number==patch0[i,j+j_offset])  #index of patch i,j+j_offset
          # increment border_count
          border_count[patch_ij,patch_ij1]<-border_count[patch_ij,patch_ij1]-rt2crt
          border_count[patch_ij1,patch_ij]<-border_count[patch_ij1,patch_ij]-rt2crt
          border_count[patch_ij,patch_ij]<-border_count[patch_ij,patch_ij]-rt2crt
          border_count[patch_ij1,patch_ij1]<-border_count[patch_ij1,patch_ij1]-rt2crt
        }
      }
      
    }
  }
  return(border_count)
}