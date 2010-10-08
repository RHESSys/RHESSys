randomParams <- function(
	prefix, 
	rows, 
	procs=1, 
	firsttag=1, 
	rfunc=runif, 
	seed=FALSE,
	mrange=c(0,1),
	Krange=c(1,400),
	gw1range=c(0,0.6),
	gw2range=c(0,0.5)
	) 
{
	if (seed != FALSE) {
		set.seed(seed)
	}

	row_per_proc = round(rows / procs)

	for (tag in firsttag:(firsttag+procs-1) ) {
		m <- rfunc(row_per_proc,0,1)
		K <- rfunc(row_per_proc,1,400)
		gw1 <- rfunc(row_per_proc,0,0.6)
		gw2 <- rfunc(row_per_proc,0,0.5)
		table <- cbind(m,K,gw1,gw2)
		filename <- paste(prefix,tag,sep="")
		write.table(table, file=filename,col.names=FALSE,row.names=FALSE)
	}
}
