# make a list of pits (nodes whose gamma's sum to 0), list in ascending order
pit_list<-function(flw){
  num_patch<-length(flw)
  x<-c()
  hght<-c()
  for (i in 1:num_patch){
    if (sum(flw[[i]]$Gamma_i)==0){
      x<-c(x,i)
      hght<-c(hght,flw[[i]]$Centroidz)
    }
  }
  x<-x[order(hght)]
  return(x)
}
