# call fill_pit on all pits
fix_all_pits<-function(flw){
  pits<-pit_list(flw)   #find all pits

  if (length(pits)==1){
    return(flw)        #no pits
  }
  pits<-pits[-1]   #throw out lowest pit
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
