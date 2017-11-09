# Make unique ID for each patch. Return patch matrix of unique id's and table 
# of basin, hill, zone and patch data whose index is the unique patch id.
build_unique_patch<-function(basin.m,hill.m,zone.m,patch.m){
  p_rows<-nrow(patch.m)
  p_cols<-ncol(patch.m)
  unique_patch<-matrix(0,p_rows,p_cols)   #patch matrix with unique patch id's
  id_table<-list()
  unique_id<-0
  for (i in 1:p_rows) {
    for (j in 1:p_cols) {
      if(!is.na(patch.m[i,j])) {  #patch i,j is not NA
        test_id<-c(basin.m[i,j],hill.m[i,j],zone.m[i,j],patch.m[i,j]) #vector of id info
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
  return(list(id_table,unique_patch))
}