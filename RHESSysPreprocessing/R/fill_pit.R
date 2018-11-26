# Make a recursive function that "fills pits". If all gamma's for a patch are zero, find the lowest
# neighbor and make the patch flow to that neighbor. Then, go to that neighbor, and make sure that it
# has non zero gamma's other than the one I just made. If not, call "fill_pit" again on the new patch.
# repeat until no non zero gamma's.

#Inputs are,
# flw: a list of all patches and their data,
# n_s: a starting node
# n_f: an ending node (initially, n_s=n_f means there is no starting node, a pit).
# history: a variable to store the history of the pit fill, to prevent circles
# parallel: passing along the parallel flag

fill_pit<-function(flw,n_s,n_f,parallel=FALSE,history=NULL){

  # print(paste(flw[[n_s]]$PatchID,"to",flw[[n_f]]$PatchID)) # for debugging

  history = unique(c(history, c(n_s,n_f))) # add nodes to history

  if (n_s!=n_f){     #remove starting node
    n_s_index<-which(flw[[n_f]]$Neighbors==n_s)  #where is the starting node (n_s)?
    gamma_clean<-flw[[n_f]]$Gamma_i[-n_s_index]   #remove gamma from neighbor n_s
    neighbor_clean<-flw[[n_f]]$Neighbors[-n_s_index]  #remove neighbor n_s
  } else { # starting node is a pit
    gamma_clean<-flw[[n_f]]$Gamma_i
    neighbor_clean<-flw[[n_f]]$Neighbors
  }

  # account for pit fill history - prevent flow in circles - should combine with first ifelse, maybe only for parallel version?
  neighbor_clean = neighbor_clean[!neighbor_clean %in% history]
  gamma_clean = gamma_clean[!neighbor_clean %in% history]

  if(parallel & length(neighbor_clean)==0){ # if no neighbors, escape function to the while loop
    # print(paste("Ended pit fill attempt unsuccessfully for patch:",flw[[n_f]]$PatchID,"hillslope:",flw[[n_f]]$HillID))
    return(history)
  }

  # ----- hard error on no new neighbors -----
  # if(length(neighbor_clean)==0){
  #   stop(noquote(paste("Patch",flw[[n_f]]$PatchID,"has only 1 neighbor and cannot route to the stream.")))
  # }

  # ----- warning on no new neighbors - report and continue -----
  # if(length(neighbor_clean)==0){
  #   print(paste("Ended pit fill attempt unsuccessfully for patch:",flw[[n_f]]$PatchID,"hillslope:",flw[[n_f]]$HillID))
  #   return(flw)
  # }

  # takes a list of nodes, returns their elevations
  elev_clean <- c()
  for (i in neighbor_clean) {
    elev_clean <- c(elev_clean,flw[[i]]$Centroidz)
  }

  if (sum(gamma_clean) != 0) {
    return(flw)  #new patch has non zero gamma's and is not a pit.
  }
  # if we get here, node has 0 gammas and is a pit.
  new_node_clean <- which.min(elev_clean) # find index of neighbor with smallest uphill elevation
  new_neighbor <- neighbor_clean[new_node_clean] #find ID of that neighbor
  new_node_i <- which(flw[[n_f]]$Neighbors == new_neighbor) # find location in list of that neighbor
  flw[[n_f]]$Gamma_i[new_node_i] <- 1 #new node is now downhill from n_f
  new_node_n_f <- which(flw[[new_neighbor]]$Neighbors == n_f) # find location of n_f in new_neighbor
  flw[[new_neighbor]]$Gamma_i[new_node_n_f] <- 0 #n_f is now uphill from new_neighbor
  new_gamma_tot <- sum(flw[[new_neighbor]]$Gamma_i)

  if (new_gamma_tot != 0) {
    flw[[new_neighbor]]$Gamma_i <- flw[[new_neighbor]]$Gamma_i/new_gamma_tot   # normalize new gamma's
    return(flw) # new_node is not a pit
  }
  # ----- exit pit fill if parallel and patch is stream -----
  if (parallel & flw[[new_neighbor]]$Landtype == 1) {
    return(flw)
  }
  flw <- fill_pit(flw = flw, n_s = n_f, n_f = new_neighbor, parallel = parallel, history = history) #call fill_pit on new node

  return(flw) # this is dumb i think and would never get called
}
#
