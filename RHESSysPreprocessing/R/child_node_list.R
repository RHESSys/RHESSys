# returns a list of all nodes that can be reached from start_node
child_node_list<-function(flw,start_node){
  len<-length(flw)
  clist_bool<-rep(FALSE,len)
  clist_bool<-find_children(flw,start_node,clist_bool)
  if (sum(clist_bool)==0){   #if there are no downstream nodes, return 0
    return(0)
  } else {    # otherwise, return a list of all downstream nodes.
    clist<-seq.int(1,len)[clist_bool]
    return(clist)
  }
}
