# Make flow table.
# Inputs are flw: list of flow data, output_file: name of output file, flag for parallelization at hillslope

make_flow_table<-function(flw,output_file,parallel){

  list_length<-length(flw)
  elev_order<-matrix(0,list_length,3)  #make table where where first col is decreasing elev, second is unique patch, 3rd is hillslope
  for (i in 1:list_length){
    elev_order[i,1]<-flw[[i]]$Centroidz
    elev_order[i,2]<-flw[[i]]$Number
    elev_order[i,3]<-flw[[i]]$HillID
  }

  # -----Output for hillslope parallelized flownet -----
  if(parallel){
    elev_order<-elev_order[order(elev_order[,3],elev_order[,1],decreasing = TRUE),]  #sort by hillslope, then elev
    hill_unique = unique(elev_order[,3])
    hill_count = length(hill_unique)

    pb = txtProgressBar(min=0,max=hill_count,style=3)
    ct=0

    sink(output_file)    #write to "output_file"
    cat(hill_count) # print number of hillslopes
    cat("\n")
    for(i_hill in hill_unique){
      ct=ct+1
      sink()
      setTxtProgressBar(pb,ct)
      sink(output_file,append = TRUE)

      patches = elev_order[elev_order[,3]==i_hill,2]
      p_count = length(patches)
      cat(paste(i_hill,p_count,sep = "\t")) #print hillslope ID and number of patches in that hillslope
      cat("\n")
      for(i in patches){
        num_neighbors<-length(flw[[i]]$Neighbors)
        out_string<-c(flw[[i]]$PatchID,flw[[i]]$ZoneID,flw[[i]]$HillID,flw[[i]]$Centroidx,flw[[i]]$Centroidy,
                      flw[[i]]$Centroidz,flw[[i]]$Area,flw[[i]]$Area,flw[[i]]$Landtype,flw[[i]]$TotalG,num_neighbors)
        cat(out_string)
        cat("\n")
        if(num_neighbors>0){
          for (j in 1:num_neighbors){
            cat("\t")
            n_j<-flw[[i]]$Neighbors[j]
            out_string<-c(flw[[n_j]]$PatchID,flw[[n_j]]$ZoneID,flw[[n_j]]$HillID,flw[[i]]$Gamma_i[j])
            cat(out_string)
            cat("\n")
          }
        }
        if (flw[[i]]$Landtype==2){
          cat("\t")
          cat(flw[[i]]$Roadtype)
          cat("\n")
        }
      } #patch loop

    } #hillslope loop
    sink()
    close(pb)

  } else{

    pb = txtProgressBar(min=0,max=list_length,style=3)

    elev_order<-elev_order[order(elev_order[,1],decreasing = TRUE),]  #sort elev in decreasing order
    sink(output_file)    #write to "output_file"
    cat(list_length)
    cat("\n")
    for (i_count in 1:list_length){
      sink()
      setTxtProgressBar(pb,i_count)
      sink(output_file,append = TRUE)

      if (list_length > 1) {
      i<-elev_order[i_count,2]    # write in descending order by elevation
      num_neighbors<-length(flw[[i]]$Neighbors)
      }
      else { i <- 1;
      num_neighbors<- 0;
      }

      out_string<-c(flw[[i]]$PatchID,flw[[i]]$ZoneID,flw[[i]]$HillID,flw[[i]]$Centroidx,flw[[i]]$Centroidy,
                    flw[[i]]$Centroidz,flw[[i]]$Area,flw[[i]]$Area,flw[[i]]$Landtype,flw[[i]]$TotalG,num_neighbors)
      cat(out_string)
      cat("\n")
      if (list_length > 1) {
      for (j in 1:num_neighbors){
        cat("\t")
        n_j<-flw[[i]]$Neighbors[j]
        out_string<-c(flw[[n_j]]$PatchID,flw[[n_j]]$ZoneID,flw[[n_j]]$HillID,flw[[i]]$Gamma_i[j])
        cat(out_string)
        cat("\n")
      }
      }
      if (flw[[i]]$Landtype==2){
        cat("\t")
        cat(flw[[i]]$Roadtype)
        cat("\n")
      }
    }

    sink()
    close(pb)
  }
}
#
