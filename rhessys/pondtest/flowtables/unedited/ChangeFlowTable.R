# Edit Flow Table based on Table

# 1 - Read in Data files

# Read in Explicit rounting table created by hand / with GRASS

rtab <- read.csv(file="roadpnos.csv", header=T)

# Read in surface flow table
flow <- read.table(file="pondtest_surface.flow", header=F, sep=",")
flow.t <- as.data.frame(matrix(data=-99999,nrow=dim(flow)[1],ncol=11))

# Convert flow table into a usable data frame
for(i in seq(1:dim(flow)[1])){
	row1 <- as.data.frame(strsplit(as.character(flow[i,1])," "))
	row1 <- subset(row1, row1!="")
	if(dim(row1)[1]<5) {
		flow.t[i,1] <- -99999
		for(ii in seq(1:dim(row1)[1])){
			flow.t[i,(ii+1)] <- as.numeric(as.character(row1[ii,1]))
		}
	} else {
		for( ii in seq(1:dim(row1)[1])){
			flow.t[i,ii] <- as.numeric(as.character(row1[ii,1]))
		}
	}
}


#flow.t <- flow.t3
y <- dim(flow.t)[1]

nrv <- c(0)

for(i in seq(1:dim(rtab)[1])){
	for (ii in seq(1:y)){
		if(flow.t[ii,1]==rtab$PatchNo[i]) {
			if(rtab$RoadSeg[i]==0) {
				npats <- flow.t[ii,11]
				if(npats >= 2) {
					nrv <- c(nrv,c((ii+2):(ii+npats)))
				}
				flow.t[(ii+1),2] <- rtab$PatchRouteTo[i]
				flow.t[(ii+1),3] <- rtab$ZoneNo[i]
				flow.t[(ii+1),4] <- rtab$HillNo[i]
				flow.t[(ii+1),5] <- 1 # gamma of one
				flow.t[ii,11]    <- 1 # number of patches (just one) 
				flow.t[ii,9]     <- rtab$LandType[i]				
			} else {
				npats <- flow.t[ii,11]
				if(npats >= 2) {
					nrv <- c(nrv,c((ii+2):(ii+npats+1)))
				} else {
					nrv <- c(nrv,(ii+2))
				}
				flow.t[(ii+1),2] <- rtab$PatchRouteTo[i]
				flow.t[(ii+1),3] <- rtab$ZoneNo[i]
				flow.t[(ii+1),4] <- rtab$HillNo[i]
				flow.t[(ii+1),5] <- 1 # gamma 	
				flow.t[ii,11]    <- 1 # number of patches (just one) 	
				flow.t[ii,9]     <- rtab$LandType[i]		
			}
				
			break	
		}
	}
}

nrv <- nrv[-1]

flow.t <- flow.t[-nrv,]

flow.t[flow.t==-99999] <- NA

#flow.t[,9][flow.t[,9]==2]<-0 # LANDTYPE = ROADS SHOULD BE PROPERLY ROUTED IN ABOVE CODE - RHESSYS IS HAVING A PROBLEM READING THE FLOW TABLE SO JUST CHANGE TO 0 
write.table(flow.t, file="../pondtest_surface_new.flow",sep="  ", row.names=FALSE, col.names=FALSE, na="")









### for subsurface flow table:
# Read in subsurface flow table
flow <- read.table(file="pondtest_subsurface.flow", header=F, sep=",")
flow.t <- as.data.frame(matrix(data=-99999,nrow=dim(flow)[1],ncol=11))

# Convert flow table into a usable data frame
for(i in seq(1:dim(flow)[1])){
	row1 <- as.data.frame(strsplit(as.character(flow[i,1])," "))
	row1 <- subset(row1, row1!="")
	if(dim(row1)[1]<5) {
		flow.t[i,1] <- -99999
		for(ii in seq(1:dim(row1)[1])){
			flow.t[i,(ii+1)] <- as.numeric(as.character(row1[ii,1]))
		}
	} else {
		for( ii in seq(1:dim(row1)[1])){
			flow.t[i,ii] <- as.numeric(as.character(row1[ii,1]))
		}
	}
}


#flow.t <- flow.t3
y <- dim(flow.t)[1]

flow.save <- flow.t

flow.t <- flow.save
nrv <- c(0)
# NOW, jsut converted the "drainage_type" field based on the previous stuff.
# and delete the extra row for road patches
for(i in seq(1:dim(rtab)[1])){
	for (ii in seq(1:y)){
		if(flow.t[ii,1]==rtab$PatchNo[i]) { 
			if(rtab$RoadSeg[i]==0) { # Chnage drainage_type to SCM
				flow.t[ii,9]     <- rtab$LandType[i]				
			} else {
				npats <- flow.t[ii,11]
				nrv <- c(nrv,(ii+npats+1)) # remove the road routing row in the flowtable
				flow.t[ii,9]     <- rtab$LandType[i]		
			}
				
			break	

		}

	}
}
nrv <- nrv[-1]

flow.t <- flow.t[-nrv,]

flow.t[flow.t==-99999] <- NA

#flow.t[,9][flow.t[,9]==2]<-0 # LANDTYPE = ROADS SHOULD BE PROPERLY ROUTED IN ABOVE CODE - RHESSYS IS HAVING A PROBLEM READING THE FLOW TABLE SO JUST CHANGE TO 0 
write.table(flow.t, file="../pondtest_subsurface_new.flow",sep="  ", row.names=FALSE, col.names=FALSE, na="")








