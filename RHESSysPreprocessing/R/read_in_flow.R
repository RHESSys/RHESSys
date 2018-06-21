#' read_in_flow
#'
#' Read in flow table file and return a R list.
#' @param input_file Flow table to be read in.
#' @author Daniel Nash
#' @export

# function to read in a flow table and return a list.
read_in_flow<-function(input_file){
  num_patches<-scan(file=input_file,nlines=1,quiet = TRUE)  #read in number of patches
  patch_list<-matrix(0,num_patches,3)  #make list of unique patch ID's
  flw<-list()
  line_count<-1  #where are we in the file
  for (p_i in 1:num_patches){       #p_i is which patch we are on
    pct = round((p_i/num_patches)*100,digits = 2)
    print(paste(pct,"%"))
    y<-scan(file=input_file,nlines=1,skip=line_count,quiet = TRUE) #read in a line of patch data, 11th num is munber of neighbors
    line_count<-line_count+1      #move to next line

    neighbor_list<-list()
    for (j in 1:y[11]) {      #read in neighbor data
      n_y<-scan(file=input_file,nlines=1,skip=line_count,quiet = TRUE)  #read in neighbor data, patchID,zoneID,hillID, proportion of flow
      line_count<-line_count+1      #move to next line
      neighbor_list[[j]]<-c(0,n_y)    #add 0 as place holder for unique patch id
    }

    #build list
    flw[[p_i]]<-list(patchID=y[1],
                     zoneID=y[2],
                     hillID=y[3],
                     centroidx=y[4],
                     centroidy=y[5],
                     centroidz=y[6],
                     accumulatedarea=y[7],
                     area=y[8],
                     landtype=y[9],
                     totalG=y[10],
                     numberofadjacentpatches=y[11],
                     neighbors=neighbor_list)
    #neighbors will contain:unique patch id, patchID,zoneID,hillID, proportion of flow

    patch_list[p_i,]<-c(y[1],y[2],y[3])  #patchID,zoneID,hillID, for patch p_i

  }

  for (p_i in 1:num_patches){ #calculate unique patch ID for all neighbors
    num_neighbors<-flw[[p_i]]$numberofadjacentpatches

    for (j in 1:num_neighbors){
      patch3_id<-flw[[p_i]]$neighbors[[j]][2:4]   #patch3_id contains patch, zone and hill info for each neighbor
      n_id<-which(patch_list[,1]==patch3_id[1]&patch_list[,2]==patch3_id[2]
                  &patch_list[,3]==patch3_id[3])   #find id of neighboring patch
      flw[[p_i]]$neighbors[[j]][1]<-n_id   #add id to neighbor data
    }
  }

  return(flw)
}
#
