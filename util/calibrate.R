calibrate = function(runs, str_obs, firsttag=1, filterdays=3) {

# str_obs is a data.frame with two column named date and mm,
# where mm is the precip normalized by basin area.

	# Combine all streamflow data from multiple runs into a single
	# streamflow table
	for (i in firsttag:runs) {
		str_tmp = read.table( paste("str", i, sep="") )
		par_tmp = read.table( paste("par", i, sep="") )
		if (i != firsttag) {
			str_results = cbind(str_results, str_tmp)
			stats = rbind(stats, par_tmp)
		} else {
			str_results = str_tmp
			stats = par_tmp
		}
	}
	
	# Add lables to the stats table
 	colnames(stats) = c("m","k","gw1","gw2")

	# Find the starting date from one of the RHESSys basin daily output files
	basin_file = paste("sen", firsttag, "_basin.daily", sep="")
	basin_table = read.table(basin_file, skip=1)
	start_day = basin_table[1,1]
	start_month = basin_table[1,2]
	start_year = basin_table[1,3]
	start_date = paste(start_month, start_day, start_year, sep="/")

	# Add a date column to the results
	str_results$date = seq.dates(from=start_date, length=length(str_results$V1))

	# ncals = number of calibrations
 	ncal = ncol(str_results) - 1

 	filter = rep(1/filterdays,times=filterdays)
	rear_offset = floor(filterdays / 2 )
 	lc = ncol(str_results)
 	lr = nrow(str_results)
 	tmp = apply(str_results[,1:(lc - rear_offset)], 2, filter, filter=filter, method="convolution", sides=2)
 	tmp2 = as.data.frame(tmp[2:(lr - rear_offset),])
 	tmp2$date = str_results[2:(lr - rear_offset),"date"]
 
 	filtered_str = tmp2
 
 	lr = nrow(str_obs)
 	tmp = filter(str_obs$mm, filter=filter, method="convolution", sides=2)
 	tmp2 = as.data.frame(tmp[2:(lr - rear_offset)])
 	colnames(tmp2) = c("mm")
 	filtered_obs = tmp2
 	filtered_obs$date = str_obs$date[2:(lr - rear_offset)]

 
 	filtered_str$year = as.numeric(as.character(years(filtered_str$date)))
 	filtered_str$month = as.numeric(months(filtered_str$date))
 	filtered_str$day = as.numeric(days(filtered_str$date))
 	filtered_str$wy = ifelse((filtered_str$month >= 10), filtered_str$year+1, filtered_str$year)
 	stats$row = seq(from=1, length=length(stats$m))
 
 	tmp = merge(filtered_str, filtered_obs, by=c("date"))
 	filtered_str = tmp
 
 	tmp2 = apply(tmp[,2:(ncal+1)],2, nselog, o=tmp$mm)
 	stats$nselog.sh = tmp2
 
 	tmp = apply(filtered_str[,2:(ncal+1)],2, nse, o=filtered_str$mm)
 	stats$nse = tmp
 
 	tmp = apply(filtered_str[,2:(ncal+1)], 2, nselog, o=filtered_str$mm)
 	stats$nselog = tmp
# 
# 	tmp = apply(filtered_str[,2:(ncal+1)],2, rmse, o=filtered_str$mm)
# 	stats$rmse = tmp
# 
# 	tmp = apply(filtered_str[,2:(ncal+1)], 2, cor, y=filtered_str$mm)
# 	tmp2 = tmp*tmp
# 	stats$r2 = tmp2
# 
# 	tmp = apply(filtered_str[,2:(ncal+1)], 2, sum)
# 	tmp2 = (tmp-sum(filtered_str$mm))/(sum(filtered_str$mm))*100
# 	stats$perr.total = tmp2
# 
# 	filtered_str.mth = as.data.frame(matrix(nrow=12, ncol=ncal, 0.0))
# 	for (i in 1:ncal) {
# 		tmp = aggregate(filtered_str[,i+1], by=list(filtered_str$month), FUN=mean)
# 		filtered_str.mth[,i] = tmp$x
# 	}
# 
# 	tmp = aggregate(filtered_str$mm, by=list(filtered_str$month), FUN=mean)
# 	filtered_str.mth$obs = tmp$x
# 	tmp = apply(filtered_str.mth[,1:ncal],2, cor, y=filtered_str.mth$obs)
# 	stats$mth.cor = tmp
# 
# 
# 	filtered_str = mkdate(filtered_str)
# 	filtered_str.wyd = as.data.frame(matrix(nrow=366, ncol=ncal, 0.0))
# 	for (i in 1:ncal) {
# 		tmp = aggregate(filtered_str[,i+1], by=list(filtered_str$wyd), FUN=mean)
# 		filtered_str.wyd[,i] = tmp$x
# 	}
# 
# 	tmp = aggregate(filtered_str$mm, by=list(filtered_str$wyd), FUN=mean)
# 	filtered_str.wyd$obs = tmp$x
# 	tmp = apply(filtered_str.wyd[,1:ncal],2, cor, y=filtered_str.wyd$obs)
# 	stats$wyd.cor = tmp
# 
# 
# 	tmp = range(filtered_str$wy)
# 	nwy = tmp[2]-tmp[1]+1
# 
# 	filtered_str.min = as.data.frame(matrix(nrow=nwy, ncol=ncal, 0.0))
# 	for (i in 1:ncal) {
# 		tmp = aggregate(filtered_str[,i+1], by=list(filtered_str$wy), FUN=min)
# 		filtered_str.min[,i] = tmp$x
# 	}
# 
# 	tmp = aggregate(filtered_str$mm, by=list(filtered_str$wy), FUN=min)
# 	filtered_str.min$obs = tmp$x
# 	tmp = apply(filtered_str.min[,1:ncal],2, cor, y=filtered_str.min$obs)
# 	stats$min.cor = tmp
# 	tmp = apply(filtered_str.min[,1:ncal], 2, sum)
# 	tmp2 = (tmp-sum(filtered_str.min$obs))/(sum(filtered_str.min$obs))*100
# 	stats$min.perr = tmp2
# 
# 
# 	tmp = apply(filtered_str.mth[,1:ncal],2, nse, o=filtered_str.mth$obs)
# 	stats$mth.nse = tmp
# 
# 
# 	filtered_str.wy = as.data.frame(matrix(nrow=nwy, ncol=ncal, 0.0))
# 
# 	for (i in 1:ncal) {
# 		tmp = aggregate(filtered_str[,i+1], by=list(filtered_str$wy), FUN=sum)
# 		filtered_str.wy[,i] = tmp$x
# 	}
# 
# 	tmp = aggregate(filtered_str$mm, by=list(filtered_str$wy), FUN=sum)
# 	filtered_str.wy$obs = tmp$x
# 	filtered_str.wy$wy = as.numeric(as.character(tmp$Group.1))
# 
# 	tmp = apply(filtered_str.wy[,(1:ncal)],2,nse,o=filtered_str.wy$obs)
# 	stats$wy.nse = tmp
# 	tmp = apply(filtered_str.wy[,(1:ncal)],2,cor,y=filtered_str.wy$obs)
# 	stats$wy.cor = tmp
# 
# 
# 
# 	filtered_str.mwy = as.data.frame(matrix(nrow=nwy*12, ncol=ncal, 0.0))
# 
# 	for (i in 1:ncal) {
# 		tmp = aggregate(filtered_str[,i+1], by=list(filtered_str$wy, filtered_str$month), FUN=mean)
# 		filtered_str.mwy[,i] = tmp$x
# 	}
# 
# 	tmp = aggregate(filtered_str$mm, by=list(filtered_str$wy, filtered_str$month), FUN=mean)
# 	filtered_str.mwy$obs = tmp$x
# 	filtered_str.mwy$wy = as.numeric(as.character(tmp$Group.1))
# 	filtered_str.mwy$month = as.numeric(tmp$Group.2)
# 
# 	filtered_str.aug = subset(filtered_str.mwy, filtered_str.mwy$month==8)
# 
# 	tmp = apply(filtered_str.aug[,(1:ncal)],2,nse,o=filtered_str.aug$obs)
# 	stats$aug.nse = tmp
# 
# 	tmp = apply(filtered_str.aug[,(1:ncal)],2,cor,y=filtered_str.aug$obs)
# 	stats$aug.cor = tmp

	# Assign various useful structures to the global scope
	assign("stats", stats, envir=.GlobalEnv)
 }
