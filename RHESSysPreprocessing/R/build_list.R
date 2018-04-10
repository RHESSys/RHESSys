# build list for output. Turn border count into probabilities and lists of neighbors
build_list<-function(flw_struct,patch_borders,cell_length){
  num_rows<-length(flw_struct$Number)
  neighbors<-list()
  for (i in 1:num_rows){
    neighbor_index<-which((patch_borders[i,]>0)) #find neighbors
    neighbor_index<-neighbor_index[-which(neighbor_index==i)] #remove self from neigbor list

    tp_perimeter<-cell_length*patch_borders[i,neighbor_index]
    tp_neighbors<-flw_struct$Number[neighbor_index]
    tp_xi<-flw_struct$Centroidx[i]     #patch i x position
    tp_yi<-flw_struct$Centroidy[i]     #patch i y position
    tp_zi<-flw_struct$Centroidz[i]     #patch i y position
    tp_xj<-flw_struct$Centroidx[neighbor_index]   #list of neighbors x positions
    tp_yj<-flw_struct$Centroidy[neighbor_index]   #list of neighbors y positions
    tp_zj<-flw_struct$Centroidz[neighbor_index]   #list of neighbors z positions
    dist<-cell_length*sqrt((tp_xj-tp_xi)^2+(tp_yj-tp_yi)^2)   #list of distances to neighbors
    slope_i<-(tp_zi-tp_zj)/dist

    tp_gamma<-tp_perimeter*slope_i
    #   slope_i<-tp_perimeter*slope_i
    perim_sum<-sum(tp_perimeter[tp_gamma>0]) #sum downslope boarders
    tp_gamma[tp_gamma<0]<-0
    gamma_tot<-sum(tp_gamma)

    if (gamma_tot!=0){
      tp_gamma<-tp_gamma/gamma_tot
      tp_TotalG<-(gamma_tot/perim_sum)*flw_struct$Area[i]
    } else {
      tp_TotalG<--max(slope_i)*flw_struct$Area[i]  #if all upslope, take slope from closest neighbor in height
    }

    neighbors[[i]]<-list(Number=flw_struct$Number[i],
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
  return(neighbors)
}
#
