# Analyize patch data. cell_length is size of cell in meters. smooth_flag is a boolean that smoothes jagged
# patch borders, do not use when some cells are unique patches.
patch_data_analysis<-function(raw_patch_data,raw_patch_elevation_data,
                              raw_hill_data,raw_basin_data,raw_zone_data,
                              raw_slope_data,raw_stream_data,raw_road_data,
                              cell_length,road_width,smooth_flag){

  area_conv<-cell_length*cell_length   #metters^2 per patch (need actual number)
  data_list<-build_unique_patch(raw_basin_data,raw_hill_data,raw_zone_data,raw_patch_data)
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

  xy_data<-patch_centroid_calc(patch_data,length(patches),cell_length)

  flw_struct<-data.frame(patches,xy_data,patch_mean_elev,patch_mean_slope,patch_landtype,patch_roadtype)
  colnames(flw_struct)<-c("Number","Centroidx","Centroidy","Area","Centroidz","Mean_Slope","Landtype","Roadtype")
  flw_struct<-cbind(flw_struct,id_data)

  patch_borders<-matrix(0,nrow=length(patches),ncol=length(patches))
  patch_borders<-find_border_row(patch_data,patches,patch_borders) #find  neigbors
  if (smooth_flag==TRUE) {
    patch_borders<-find_border_correction(patch_data,patches,patch_borders) #correct for staircase diagonals
  }
  lst<-build_list(flw_struct,patch_borders,cell_length)

  lst<-fix_all_pits(lst)   #fill all pits

  lst<-find_stream(lst,road_width) # if there are roads, find the streams that are near

  return(lst)
}
