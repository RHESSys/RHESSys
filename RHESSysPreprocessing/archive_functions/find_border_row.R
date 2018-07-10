# find patch borders by row.
# Daniel Nash 2017
# Edited by Will Burke 10/16/17

# patch_data is patch data with NA replaced by 0, patches is a vector of ordered patch numbers
# patch_borders is an array, initailly 0 but will be filled with the number of times patch i
# touches patch j. the diagonal will be the number of times patch i touches anything.
find_border_row<-function(patch_data,patches,patch_borders){

  #iterate through
  p_rows<-nrow(patch_data)
  p_cols<-ncol(patch_data)

  for (i in 1:p_rows) { # loop through all rows and cols of input patch data
    for (j in 1:p_cols){

      if (i < p_rows & j < p_cols){ # if both current row and col are less than max
        if ((patch_data[i,j]*patch_data[i,j+1]!=0)){  # patch itself, and +1 in cols dir aren't 0
          if (patch_data[i,j]!=patch_data[i,j+1]){  # rt. side boundary
            patch_ij<-which(patches==patch_data[i,j])   #index of patch i,j
            patch_ij1<-which(patches==patch_data[i,j+1])  #index of patch i,j+1

            # increment patch_borders
            patch_borders[patch_ij,patch_ij1]<-patch_borders[patch_ij,patch_ij1]+1
            patch_borders[patch_ij1,patch_ij]<-patch_borders[patch_ij1,patch_ij]+1
            patch_borders[patch_ij,patch_ij]<-patch_borders[patch_ij,patch_ij]+1
            patch_borders[patch_ij1,patch_ij1]<-patch_borders[patch_ij1,patch_ij1]+1
          }
        }
        if ((patch_data[i,j]*patch_data[i+1,j]!=0)){ # patch itself and +1 in row dir
          if (patch_data[i,j]!=patch_data[i+1,j]){  #lower boundary
            patch_ij<-which(patches==patch_data[i,j])   #index of patch i,j
            patch_i1j<-which(patches==patch_data[i+1,j])  #index of patch i+1,j

            # increment patch_borders
            patch_borders[patch_ij,patch_i1j]<-patch_borders[patch_ij,patch_i1j]+1
            patch_borders[patch_i1j,patch_ij]<-patch_borders[patch_i1j,patch_ij]+1
            patch_borders[patch_ij,patch_ij]<-patch_borders[patch_ij,patch_ij]+1
            patch_borders[patch_i1j,patch_i1j]<-patch_borders[patch_i1j,patch_i1j]+1
          }
        }
      } # end if not max row and col

      if (j == p_cols & i!=p_rows){ # final col exception
        if ((patch_data[i,j]*patch_data[i+1,j]!=0)){  # patch itself, and +1 in either directions aren't 0
          if (patch_data[i,j]!=patch_data[i+1,j]){  #lower boundary
            patch_ij<-which(patches==patch_data[i,j])   #index of patch i,j
            patch_i1j<-which(patches==patch_data[i+1,j])  #index of patch i+1,j

            # increment patch_borders
            patch_borders[patch_ij,patch_i1j]<-patch_borders[patch_ij,patch_i1j]+1
            patch_borders[patch_i1j,patch_ij]<-patch_borders[patch_i1j,patch_ij]+1
            patch_borders[patch_ij,patch_ij]<-patch_borders[patch_ij,patch_ij]+1
            patch_borders[patch_i1j,patch_i1j]<-patch_borders[patch_i1j,patch_i1j]+1
          }
        }
      }

      if (i==p_rows & j!=p_cols){ # final row exception
        if ((patch_data[i,j]*patch_data[i,j+1]!=0)){  # patch itself, and +1 in either directions aren't 0
          if (patch_data[i,j]!=patch_data[i,j+1]){  # rt. side boundary
            patch_ij<-which(patches==patch_data[i,j])   #index of patch i,j
            patch_ij1<-which(patches==patch_data[i,j+1])  #index of patch i,j+1

            # increment patch_borders
            patch_borders[patch_ij,patch_ij1]<-patch_borders[patch_ij,patch_ij1]+1
            patch_borders[patch_ij1,patch_ij]<-patch_borders[patch_ij1,patch_ij]+1
            patch_borders[patch_ij,patch_ij]<-patch_borders[patch_ij,patch_ij]+1
            patch_borders[patch_ij1,patch_ij1]<-patch_borders[patch_ij1,patch_ij1]+1
          }
        }
      }

    } # for j
  } # for i

  return(patch_borders)
}
#
