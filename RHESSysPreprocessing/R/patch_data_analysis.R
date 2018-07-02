# patch_data_analysis
# Daniel Nash 2017
# Edited by Will Burke

# This function is the main component of the larger create_flownet function. Function finds neighbors,
# calculates flow, accounts of pits, roads, streams etc.
# cell_length is size of cell in meters. smooth_flag is a boolean that smoothes jagged patch borders

patch_data_analysis <-
  function(raw_patch_data,
           raw_patch_elevation_data,
           raw_hill_data,
           raw_basin_data,
           raw_zone_data,
           raw_slope_data,
           raw_stream_data,
           raw_road_data = NULL,
           cell_length,
           road_width = NULL,
           smooth_flag = FALSE,
           d4,
           parallel) {


  # -------------------- Error checking and NULL handling --------------------
  if(cell_length<=0){stop("Cell length is <=0")}
  if(is.null(raw_road_data)){raw_road_data = matrix(0,ncol = ncol(raw_patch_data),nrow = nrow(raw_patch_data))}
  if(is.null(road_width)){road_width=0}

  area_conv<-cell_length*cell_length   #meters^2 per patch (need actual number)

  # -------------------- build unique patch names --------------------
  # Make unique ID for each patch. Return patch matrix of unique id's and table
  # of basin, hill, zone and patch data whose index is the unique patch id.
  p_rows<-nrow(raw_patch_data)
  p_cols<-ncol(raw_patch_data)
  unique_patch<-matrix(0,p_rows,p_cols)   #patch matrix with unique patch id's
  id_table<-list()
  unique_id<-0
  for (i in 1:p_rows) {
    for (j in 1:p_cols) {
      if(!is.na(raw_patch_data[i,j])) {  #patch i,j is not NA
        test_id<-c(raw_basin_data[i,j],raw_hill_data[i,j],raw_zone_data[i,j],raw_patch_data[i,j]) #vector of id info
        new_point<-lapply(id_table,function(x) all(x==test_id)) # check if id info is in table
        new_point<-unlist(new_point)
        if (!sum(new_point)) { #if id info is not in table, add info to table
          unique_id<-unique_id+1
          id_table[[unique_id]]<-test_id
          unique_patch[i,j]<-unique_id
        }  else {
          unique_patch[i,j]<-which(new_point)  #if info is in table, add correct unique id to patch matrix
        }
      }
    }
  }

  id_table<-data.frame(do.call(rbind,id_table))
  colnames(id_table)<-c("Basin","Hill","Zone","Patch")
  data_list = list(id_table,unique_patch)

  id_data<-data_list[[1]]
  patch_data<-data_list[[2]]
  patch_elevation_data<-raw_patch_elevation_data
  patch_elevation_data[is.na(patch_elevation_data)]<-0   #Replace NA's with 0
  patch_slope_data<-raw_slope_data
  patch_slope_data[is.na(patch_slope_data)]<-0   #Replace NA's with 0
  patch_stream_data<-raw_stream_data
  patch_stream_data[is.na(patch_stream_data)]<-0   #Replace NA's with 0
  patch_road_data<-raw_road_data
  patch_road_data[is.na(patch_road_data)]<-0   #Replace NA's with 0
  y<-as.vector(patch_data)
  z<-as.vector(patch_elevation_data)
  w<-as.vector(patch_slope_data)
  x<-as.vector(patch_stream_data)
  x1<-as.vector(patch_road_data)
  patches<-as.vector(unique(y))     #find individual patches
  patches<-sort(patches)

  patch_mean_elev<-tapply(z,y,mean)  #create vector of patch elevation means
  patch_mean_elev<-as.vector(patch_mean_elev)

  patch_mean_slope<-tapply(w,y,mean)  #create vector of patch slope means
  patch_mean_slope<-as.vector(patch_mean_slope)

  patch_landtype<-tapply(x,y,max)  #create vector of 1's if there is a stream in the patch
  patch_landtype<-as.vector(patch_landtype)
  patch_landtype[patch_landtype>0]<-1

  patch_roadtype<-tapply(x,y,max)  #create vector of 1's if there is a road in the patch
  patch_roadtype<-as.vector(patch_roadtype)
  patch_roadtype[patch_roadtype>0]<-1


  if (patches[1]==0) {
    patches<-patches[-1]       #throw out patch 0
    patch_mean_elev<-patch_mean_elev[-1]     #throw out patch 0
    patch_mean_slope<-patch_mean_slope[-1]     #throw out patch 0
    patch_landtype<-patch_landtype[-1]     #throw out patch 0
    patch_roadtype<-patch_roadtype[-1]     #throw out patch 0
  }

  # -------------------- patch centroid calc --------------------
  # Find the average row and column of each patch, Variables are, patch_data: all pathes w unique id and NA's replaced
  # by 0, num_patches: number of unique pathces, cell_length: size of individual cells.
  num_patches = length(patches)
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
  xy_data = patch_coord

  flw_struct<-data.frame(patches,xy_data,patch_mean_elev,patch_mean_slope,patch_landtype,patch_roadtype)
  colnames(flw_struct)<-c("Number","Centroidx","Centroidy","Area","Centroidz","Mean_Slope","Landtype","Roadtype")
  flw_struct<-cbind(flw_struct,id_data)



  # -------------------- find neighbors cell by cell (previously find border row) --------------------
  # patch_data is patch data with NA replaced by 0, patches is a vector of ordered patch numbers
  # patch_borders is an array, initailly 0 but will be filled with the number of times patch i
  # touches patch j. the diagonal will be the number of times patch i touches anything.

  if(d4){ # d4 neighbor find start -----
    patch_borders<-matrix(0,nrow=length(patches),ncol=length(patches))

    #iterate through
    p_rows<-nrow(patch_data)
    p_cols<-ncol(patch_data)

    for (i in 1:p_rows) { # loop through all rows and cols of input patch data
      for (j in 1:p_cols){
        if (i < p_rows & j < p_cols){ # if both current row and col are less than max
          if ((patch_data[i,j]*patch_data[i,j+1] != 0)){  # if patch itself, and +1 in x dir aren't 0
            if (patch_data[i,j]!=patch_data[i,j+1]){  # if +1 in x dir is different patch
              patch_ij<-which(patches==patch_data[i,j])   #index of patch i,j
              patch_ij1<-which(patches==patch_data[i,j+1])  #index of patch i,j+1
              patch_borders[patch_ij,patch_ij1]<-patch_borders[patch_ij,patch_ij1]+1
              patch_borders[patch_ij1,patch_ij]<-patch_borders[patch_ij1,patch_ij]+1
              patch_borders[patch_ij,patch_ij]<-patch_borders[patch_ij,patch_ij]+1
              patch_borders[patch_ij1,patch_ij1]<-patch_borders[patch_ij1,patch_ij1]+1
            }
          }
          if ((patch_data[i,j]*patch_data[i+1,j]!=0)){ # if patch itself and +1 in y dir
            if (patch_data[i,j]!=patch_data[i+1,j]){  # if +1 in y dir is different patch
              patch_ij<-which(patches==patch_data[i,j])   #index of patch i,j
              patch_i1j<-which(patches==patch_data[i+1,j])  #index of patch i+1,j
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
              patch_borders[patch_ij,patch_ij1]<-patch_borders[patch_ij,patch_ij1]+1
              patch_borders[patch_ij1,patch_ij]<-patch_borders[patch_ij1,patch_ij]+1
              patch_borders[patch_ij,patch_ij]<-patch_borders[patch_ij,patch_ij]+1
              patch_borders[patch_ij1,patch_ij1]<-patch_borders[patch_ij1,patch_ij1]+1
            }
          }
        }

      } # for j
    } # for i
  }


  # -------------------- D8 neighbor search and border count --------------------
  if(!d4){
    patch_borders<-matrix(0,nrow=length(patches),ncol=length(patches))
    p_rows<-nrow(patch_data)
    p_cols<-ncol(patch_data)

    diag_border = 1/sqrt(2)

    for (i in 1:p_rows) { # loop through all rows and cols of input patch data
      for (j in 1:p_cols){
        if(patch_data[i,j]!=0){ # only look for neighbors if current cell is actually a patch

          if(j < p_cols){ # ----- all rows, all cols except last
            if(patch_data[i,j] != patch_data[i,j+1] & patch_data[i,j+1] != 0){ # east - is different patch and is not 0
              p1<-which(patches==patch_data[i,j])   #index of patch i,j
              p2<-which(patches==patch_data[i,j+1])  #index of patch i,j+1
              patch_borders[p1,p2]<-patch_borders[p1,p2]+1
              patch_borders[p2,p1]<-patch_borders[p2,p1]+1
              patch_borders[p1,p1]<-patch_borders[p1,p1]+1
              patch_borders[p2,p2]<-patch_borders[p2,p2]+1
            } # end east
          }
          if(i < p_rows & j < p_cols){ # ----- all rows/cols except last
            if(patch_data[i,j] != patch_data[i+1,j+1] & patch_data[i+1,j+1] != 0){ # southeast - is different patch and is not 0
              p1<-which(patches==patch_data[i,j])   #index of patch i,j
              p2<-which(patches==patch_data[i+1,j+1])  #index of patch i+1,j+1
              patch_borders[p1,p2]<-patch_borders[p1,p2]+diag_border
              patch_borders[p2,p1]<-patch_borders[p2,p1]+diag_border
              patch_borders[p1,p1]<-patch_borders[p1,p1]+diag_border
              patch_borders[p2,p2]<-patch_borders[p2,p2]+diag_border
            } # end southeast
          }
          if(i < p_rows){ # ----- all rows except last, all cols
            if(patch_data[i,j] != patch_data[i+1,j] & patch_data[i+1,j] != 0){ # south - is different patch and is not 0
              p1<-which(patches==patch_data[i,j])   #index of patch i,j
              p2<-which(patches==patch_data[i+1,j])  #index of patch i+1,j
              patch_borders[p1,p2]<-patch_borders[p1,p2]+1
              patch_borders[p2,p1]<-patch_borders[p2,p1]+1
              patch_borders[p1,p1]<-patch_borders[p1,p1]+1
              patch_borders[p2,p2]<-patch_borders[p2,p2]+1
            } # end south
          }
          if(i < p_rows & j > 1){ # ----- all rows except last, all cols except first
            if(patch_data[i,j] != patch_data[i+1,j-1] & patch_data[i+1,j-1] != 0){ # southwest - is different patch and is not 0
              p1<-which(patches==patch_data[i,j])   #index of patch i,j
              p2<-which(patches==patch_data[i+1,j-1])  #index of patch i+1,j
              patch_borders[p1,p2]<-patch_borders[p1,p2]+diag_border
              patch_borders[p2,p1]<-patch_borders[p2,p1]+diag_border
              patch_borders[p1,p1]<-patch_borders[p1,p1]+diag_border
              patch_borders[p2,p2]<-patch_borders[p2,p2]+diag_border
            } # end southwest
          }

        } # end if not 0
      } # end p_cols loop
    } # end p_rows loop
  } # end d8 if


  # ----- smooth flag, staircase diagonal correction ----- (Im not really sure how this works, so i haven't messed with it,
  # it may have an edge row error which occurs with single cell patches like the normal find_border_row had =Will)
  if (smooth_flag==TRUE) {
    patch_borders<-find_border_correction(patch_data,patches,patch_borders) #correct for staircase diagonals
  }

  # -------------------- build list --------------------
  # build list for output. Turn border count into probabilities and lists of neighbors
  num_rows<-length(flw_struct$Number)
  lst<-list()
  for (i in 1:num_rows){
    neighbor_index<-which((patch_borders[i,]>0)) #find neighbors
    neighbor_index<-neighbor_index[-which(neighbor_index==i)] #remove self from neigbor list

    if(parallel){ # ********** hillslope parallelization **********
      # remove neighbors that are in different hillslopes
      neighbor_index = neighbor_index[flw_struct[i,]$Hill == flw_struct[neighbor_index,]$Hill]
    }

    tp_perimeter<-cell_length*patch_borders[i,neighbor_index] # total perimeter in map units (meters,etc)
    tp_neighbors<-flw_struct$Number[neighbor_index]  # vector of neighboring patches
    tp_xi<-flw_struct$Centroidx[i]     #patch i x position
    tp_yi<-flw_struct$Centroidy[i]     #patch i y position
    tp_zi<-flw_struct$Centroidz[i]     #patch i y position
    tp_xj<-flw_struct$Centroidx[neighbor_index]   #list of neighbors x positions
    tp_yj<-flw_struct$Centroidy[neighbor_index]   #list of neighbors y positions
    tp_zj<-flw_struct$Centroidz[neighbor_index]   #list of neighbors z positions
    dist<-cell_length*sqrt((tp_xj-tp_xi)^2+(tp_yj-tp_yi)^2)   #list of distances to neighbors (centroid to centroid)
    slope_i<-(tp_zi-tp_zj)/dist
    if(any(dist==0)){ # if distance is 0 because of weird patch setup, maybe because zones cut patches exactly in half
      if(tp_zi == tp_zj[dist==0]){slope_i[dist==0] = 0 # if elev is the same set slope to 0 --check if this works in fill_pit
      } else{slope_i[dist==0] = (tp_zi-tp_zj[dist==0])/cell_length}
    }

    tp_gamma<-tp_perimeter*slope_i # slope * border length (m^3)
    perim_sum<-sum(tp_perimeter[tp_gamma>0]) #sum downslope boarders of patches with positive gammas
    tp_gamma[tp_gamma<0]<-0 # set negative gammas to 0
    gamma_tot<-sum(tp_gamma) # sum perim * slope of all neighbors

    if (gamma_tot!=0){ # if there's a downslope neighbor
      tp_gamma<-tp_gamma/gamma_tot # normalize gamma by total (% or proportion)
      tp_TotalG<-(gamma_tot/perim_sum)*flw_struct$Area[i] # gamma_tot/perim_sum = sum of slopes * area = volume
    } else {
      tp_TotalG<--max(slope_i)*flw_struct$Area[i]  #if all upslope, take slope from closest neighbor in height
    }

    lst[[i]]<-list(Number=flw_struct$Number[i],
                         Area=flw_struct$Area[i],
                         Centroidx=flw_struct$Centroidx[i],
                         Centroidy=flw_struct$Centroidy[i],
                         Centroidz=flw_struct$Centroidz[i],
                         BasinID=flw_struct$Basin[i],
                         HillID=flw_struct$Hill[i],
                         ZoneID=flw_struct$Zone[i],
                         PatchID=flw_struct$Patch[i],
                         Landtype=flw_struct$Landtype[i],
                         Roadtype=flw_struct$Roadtype[i],
                         Neighbors=tp_neighbors,
                         Border_permiter=tp_perimeter,
                         Slopes=tp_perimeter*slope_i,
                         TotalG=tp_TotalG,
                         Gamma_i=tp_gamma)
  }

  # -------------------- Pit filling --------------------
  lst = fix_all_pits(lst)

  # # ----- find all pits ----- make a list of pits (nodes whose gamma's sum to 0), list in ascending order
  # num_patch<-length(lst)
  # x<-c()
  # hght<-c()
  # for (i in 1:num_patch){
  #   if (sum(lst[[i]]$Gamma_i)==0){
  #     x<-c(x,i) # vector of pit patch numbers
  #     hght<-c(hght,lst[[i]]$Centroidz) # vector of pit heights
  #   }
  # }
  # pits<-x[order(hght)] # pit patch numbers ordered by elevation
  #
  # if (length(pits)==1){
  #   return(lst)        #no pits
  # }
  # pits<-pits[-1]   #throw out lowest pit - this is the outlet
  # num_pits<-length(pits)
  #
  # for (j in 1:num_pits){
  #   i<-pits[j]   #fill pits
  #   if (sum(lst[[i]]$Gamma_i)==0){
  #     lst<-fill_pit(lst,i,i)
  #   }
  # }

  # -------------------- streams and roads --------------------
  lst<-find_stream(lst,road_width) # if there are roads, find the streams that are near

  # ----- End function -----
  return(lst)
}
