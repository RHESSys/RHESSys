#calibrate = function(runs, str_obs, firsttag=1, filterdays=3) {

# str_obs is a data.frame with two column named date and mm,
# where mm is the precip normalized by basin area, and date
# is a chron date created with seq.dates (very important!!!!).

	# Combine all streamflow data from multiple runs into a single
	# streamflow table, and copy parameters from multiple files into
	# the statistics output table
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
	
	# Row information for the statistics output table
 	stats$row = seq(from=1, length=length(stats$m))

	# Find the starting date from one of the RHESSys basin daily output files
	basin_file = paste("sen", firsttag, "_basin.daily", sep="")
	basin_table = read.table(basin_file, header=TRUE)
	start_day = basin_table[1,1]
	start_month = basin_table[1,2]
	start_year = basin_table[1,3]
	start_date = paste(start_month, start_day, start_year, sep="/")

	# Add a date column to the results
	str_results$date = seq.dates(from=start_date, length=length(str_results$V1))

	# ncals = number of calibrations
 	ncal = ncol(str_results) - 1

	# Generate values for the convolution filter
 	filter_weights = rep(1 / filterdays, times=filterdays)
	forward_lag = floor(filterdays / 2)
	backward_lag = floor( (filterdays - 1) / 2)

	# Filter the streamflow simulation predictions
 	lc = ncol(str_results)
 	lr = nrow(str_results)
	# (lc-1) so the date column of str_results is not filtered
 	tmp = apply(str_results[,1:(lc - 1)], 2, filter, filter=filter_weights, method="convolution", sides=2)
 	filtered_str = as.data.frame(tmp[(1 + backward_lag):(lr - forward_lag),])
 	filtered_str$date = str_results[(1 + forward_lag):(lr - forward_lag), "date"]

	# Filter the observed streamflow data using the same filter as was used for
	# the simulation streamflow output
 	lr = nrow(str_obs)
 	tmp = filter(str_obs$mm, filter=filter_weights, method="convolution", sides=2)
 	filtered_obs = as.data.frame(tmp[(1 + backward_lag):(lr - forward_lag)])
 	colnames(filtered_obs) = c("mm")
 	filtered_obs$date = str_obs$date[(1 + backward_lag):(lr - forward_lag)]

	# Add year, month, day, and water year to filtered stream data. 
 	filtered_str$year = as.numeric(as.character(years(filtered_str$date)))
 	filtered_str$month = as.numeric(months(filtered_str$date))
 	filtered_str$day = as.numeric(days(filtered_str$date))
	filtered_str$wy = ifelse((filtered_str$month >= 10), filtered_str$year+1, filtered_str$year)

	# Create a single table with streamflow data and the observed data
	filtered_str = merge(filtered_str, filtered_obs, by=c("date"))

	# Compute nse
	stats$nse = apply(filtered_str[,2:(ncal+1)],2, nse, o=filtered_str$mm)

	# Compute nselog
 	stats$nselog = apply(filtered_str[,2:(ncal+1)], 2, nselog, o=filtered_str$mm)

	# Compute rmse
 	stats$rmse = apply(filtered_str[,2:(ncal+1)],2, rmse, o=filtered_str$mm)

	# Compute cor
 	tmp = apply(filtered_str[,2:(ncal+1)], 2, cor, y=filtered_str$mm)
 	tmp2 = tmp*tmp
 	stats$r2 = tmp2

	# Compute total percent error
 	tmp = apply(filtered_str[,2:(ncal+1)], 2, sum, na.rm=TRUE)
 	stats$perr.total = (tmp-sum(filtered_str$mm, na.rm=TRUE))/(sum(filtered_str$mm, rm.na=TRUE))*100

#	filtered_str.mth = as.data.frame(matrix(nrow=12, ncol=ncal, 0.0))
#	for (i in 1:ncal) {
#		tmp = aggregate(filtered_str[,i+1], by=list(filtered_str$month), FUN=mean)
#		filtered_str.mth[,i] = tmp$x
#	}
#
#	tmp = aggregate(filtered_str$mm, by=list(filtered_str$month), FUN=mean)
#	filtered_str.mth$obs = tmp$x
#	tmp = apply(filtered_str.mth[,1:ncal],2, cor, y=filtered_str.mth$obs)
#	stats$mth.cor = tmp
#
#	filtered_str = mkdate(filtered_str)
#	filtered_str.wyd = as.data.frame(matrix(nrow=366, ncol=ncal, 0.0))
#	for (i in 1:ncal) {
#		tmp = aggregate(filtered_str[,i+1], by=list(filtered_str$wyd), FUN=mean)
#		filtered_str.wyd[,i] = tmp$x
#	}
#
#	tmp = aggregate(filtered_str$mm, by=list(filtered_str$wyd), FUN=mean)
#	filtered_str.wyd$obs = tmp$x
#	tmp = apply(filtered_str.wyd[,1:ncal],2, cor, y=filtered_str.wyd$obs)
#	stats$wyd.cor = tmp
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
#	stats$min.perr = tmp2
# 
# 	tmp = apply(filtered_str.mth[,1:ncal],2, nse, o=filtered_str.mth$obs)
# 	stats$mth.nse = tmp
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
#	assign("stats", stats, envir=.GlobalEnv)
#	assign("filtered_obs", filtered_obs, envir=.GlobalEnv)
#	assign("filtered_results", filtered_results, envir=.GlobalEnv)
# }
