# call fill_pit on all pits
fix_all_pits<-function(flw){

  # ----- find all pits -----
  # make a list of pits (nodes whose gamma's sum to 0), list in ascending order
  num_patch<-length(flw)
  x<-c()
  hght<-c()
  for (i in 1:num_patch){
    if (sum(flw[[i]]$Gamma_i)==0){
      x<-c(x,i) # vector of pit patch numbers
      hght<-c(hght,flw[[i]]$Centroidz) # vector of pit heights
    }
  }
  pits<-x[order(hght)] # pit patch numbers ordered by elevation

  if (length(pits)==1){
    return(flw)        #no pits
  }
  pits<-pits[-1]   #throw out lowest pit - this is the outlet
  num_pits<-length(pits)

  for (j in 1:num_pits){
    i<-pits[j]   #fill pits
    if (sum(flw[[i]]$Gamma_i)==0){
      flw<-fill_pit(flw,i,i)
    }
  }
  return(flw)
}
#
