# Make a recursive function that "fills pits". If all gamma's for a patch are zero, find the lowest 
# neighbor and make the patch flow to that neighbor. Then, go to that neighbor, and make sure that it
# has non zero gamma's other than the one I just made. If not, call "fill_pit" again on the new patch.
# repeat until no non zero gamma's. Inputs are, flw: a list of all patches and their data, n_s a starting
# node, n_f: an ending node (initially, n_s=n_f means there is no starting node, a pit).
fill_pit<-function(flw,n_s,n_f){
  #  print(c(n_s,n_f))
  if (n_s!=n_f){     #remove starting node 
    n_s_index<-which(flw[[n_f]]$Neighbors==n_s)  #where is the starting node (n_s)?
    gamma_clean<-flw[[n_f]]$Gamma_i[-n_s_index]   #remove gamma from neighbor n_s
    neighbor_clean<-flw[[n_f]]$Neighbors[-n_s_index]  #remove neighbor n_s
    slope_clean<-flw[[n_f]]$Slopes[-n_s_index]   #remove slope from neighbor n_s
  } else { # starting node is a pit
    gamma_clean<-flw[[n_f]]$Gamma_i
    neighbor_clean<-flw[[n_f]]$Neighbors
    slope_clean<-flw[[n_f]]$Slopes
  }
  if (sum(gamma_clean)!=0){
    return(flw)  #new patch has non zero gamma's and is not a pit.
  }
  #  if we get here, node has 0 gammas and is a pit.
  new_node_clean<-which.max(slope_clean) # find index of neighbor with smallest uphill slope
  new_neighbor<-neighbor_clean[new_node_clean] #find ID of that neighbor
  new_node_i<-which(flw[[n_f]]$Neighbors==new_neighbor) # find location in list of that neighbor
  flw[[n_f]]$Gamma_i[new_node_i]<-1 #new node is now downhill from n_f
  new_node_n_f<-which(flw[[new_neighbor]]$Neighbors==n_f) # find location of n_f in new_neighbor
  flw[[new_neighbor]]$Gamma_i[new_node_n_f]<-0 #n_f is now uphill from new_neighbor
  new_gamma_tot<-sum(flw[[new_neighbor]]$Gamma_i)
  
  if (new_gamma_tot!=0){
    flw[[new_neighbor]]$Gamma_i<-flw[[new_neighbor]]$Gamma_i/new_gamma_tot   # normalize new gamma's
    return (flw) # new_node is not a pit
  }
  flw<-fill_pit(flw,n_f,new_neighbor) #call fill_pit on new node
  return (flw)
}