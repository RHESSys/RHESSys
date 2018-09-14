# Returns a boolean vector of all nodes that can be reached from a given node.
# child_list is a boolen vector of all nodes, initially all FALSE
find_children<-function(flw,start_node,child_list){
  dwnstrm_nodes<-flw[[start_node]]$Neighbors[flw[[start_node]]$Gamma_i>0]  #find all downstream nodes
  len<-length(dwnstrm_nodes)
  if(sum(child_list[dwnstrm_nodes])==len){     #we have seen all of the nodes children
    return(child_list)
  }else {
    child_list[dwnstrm_nodes]<-TRUE
    for (i in dwnstrm_nodes){
      child_list<-find_children(flw,i,child_list)
    }
    return(child_list)
  }
}
