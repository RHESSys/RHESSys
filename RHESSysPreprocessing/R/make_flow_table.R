# Make flow table. Inputs are flw: list of flow data, output_file: name of output file
make_flow_table<-function(flw,output_file){
  list_length<-length(flw)
  elev_order<-matrix(0,list_length,2)  #make table where where first col is decreasing elev, second is unique patch#
  for (i in 1:list_length){
    elev_order[i,1]<-flw[[i]]$Centroidz
    elev_order[i,2]<-flw[[i]]$Number
  }
  elev_order<-elev_order[order(elev_order[,1],decreasing = TRUE),]  #sort elev in decreasing order
  sink(output_file)    #write to "output_file"
  cat(list_length)
  cat("\n")
  for (i_count in 1:list_length){
    i<-elev_order[i_count,2]    # write in descending order by elevation
    num_neighbors<-length(flw[[i]]$Neighbors)
    out_string<-c(flw[[i]]$PatchID,flw[[i]]$ZoneID,flw[[i]]$HillID,flw[[i]]$Centroidx,flw[[i]]$Centroidy,
                  flw[[i]]$Centroidz,flw[[i]]$Area,flw[[i]]$Area,flw[[i]]$Landtype,flw[[i]]$TotalG,num_neighbors)
    cat(out_string)
    cat("\n")
    for (j in 1:num_neighbors){
      cat("\t")
      n_j<-flw[[i]]$Neighbors[j]
      out_string<-c(flw[[n_j]]$PatchID,flw[[n_j]]$ZoneID,flw[[n_j]]$HillID,flw[[i]]$Gamma_i[j])
      cat(out_string)
      cat("\n")
    }
  }
  sink()
}