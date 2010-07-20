library(chron)


cal.wyd = function(x) {
	tmp = aggregate(x$yd, by=list(x$year), max)
	colnames(tmp) = c("year","n")
	tmp$year = as.integer(as.character(tmp$year))
	x$wyd=0
	tmp2 = subset(tmp, tmp$n == 365)
	new = ifelse( (x$year %in% tmp2$year), ifelse(x$yd >= 274,x$yd-273, x$yd+91), ifelse(x$yd >=275, x$yd-274, x$yd+92))
	new
}


mkdate = function(x) {
x$date = as.Date(paste(x$year, x$month, x$day, sep="-"))
x$wy = ifelse(x$month >=10, x$year+1, x$year)
x$yd = as.integer(format(as.Date(x$date), format="%j"))
x$wyd = cal.wyd(x)
x
}


read_rhessys_met = function(prename) {

 tname = paste(prename,".tmin",sep="")
 clim = read.table(tname,,skip=1, header=F)
 colnames(clim) = c("tmin")
 tname = paste(prename,".tmax",sep="")
 tmp = read.table(tname,skip=1, header=F)
 clim$tmax = tmp$V1
 tmp = scan(file=tname, what="%d%d%d%d",n=4)
 tmp2 = sprintf("%s/%s/%s", tmp[2], tmp[3], tmp[1])
 clim$date = seq.dates(from=tmp2, length=length(clim$tmin))

 tname = paste(prename,".rain",sep="")
 tmp = read.table(tname,skip=1, header=F)
 
 tmp$rain = tmp$V1*1000.0


 tmp2 = scan(file=tname, what="%d%d%d%d",n=4)
 tmp3 = sprintf("%s/%s/%s", tmp2[2], tmp2[3], tmp2[1])
  tmp$date = seq.dates(from=tmp3, length=length(tmp$rain))

 new = merge(clim, tmp[,c("rain","date")], by=c("date"), all=TRUE)

 clim = new
  rm(tmp, tmp2,tname,tmp3,new)
 clim$year = as.numeric(as.character(years(clim$date)))
 clim$month = as.numeric(months(clim$date))
 clim$day = as.numeric(days(clim$date))
 clim$wy = ifelse((clim$month >= 10), clim$year+1, clim$year)
 clim
}


nselog = function(m,o) {
m = log(m+0.0001)
o=(o+0.0001)
err = m-o
 ns = 1-var(err)/var(o)
 ns
 }

nse = function(m,o) {
err = m-o
 ns = 1-var(err)/var(o)
 ns
 }



rmse = function(m,o) {
        err = m-o
        total = sum(err**2)
        n = length(m)
        rmse = sqrt(total)/n*100/mean(o)
        rmse
}



running_mean = function(n,x) {
tmp = rep(0, times=n)
lc = length(x)
new = rep(0, times=lc)
for (i in 1:lc) {
tmp[2:n]=tmp[1:(n-1)]
tmp[1]=x[i]
tmp3 = subset(tmp, tmp > 0.01)
tmp2 = ifelse(length(tmp3)==0, max(tmp), mean(tmp3)) 
new[i] = (tmp2)
}
new
}

running_median = function(n,x) {
tmp = rep(0, times=n)
lc = length(x)
new = rep(0, times=lc)
for (i in 1:lc) {
tmp[2:n]=tmp[1:(n-1)]
tmp[1]=x[i]
new[i] = median(tmp)
}
new
}


mth.names = c("Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec")
mth.wy.names = 
c("Nov","Dec","Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct")

midmonthday = c(015,046,074,105,135,166,196,227,258,288,319,349)

mthwyd = c(107,136,166,197,227,258,288,319,350,16,46,77)



# function definition for moving window of NSE 
nsmult = function(of,mf,df,len) {
begin = floor(len %/% 2 + 1)
final = floor(length(mf) - len/2 - 1)
rnse = rep(0.0, times=(final-begin+1)) 
rdate = seq.dates(from="1/1/1911", length=(final-begin+1))
rdate = as.Date(rdate)
for (i in begin:final) {

	start = i - len %/% 2;
	rdate[i-begin+1] = df[i];	
	end = i+len/2-1;
	rnse[i-begin+1] = nse(o=of[start:end], m=mf[start:end])
}
rnse = data.frame(rdate,rnse)
colnames(rnse) = c("date","nse")
rnse
}

spherical_mean = function(x) {
 alpharad = x*pi/180.0 
 sinalpha = sin(alpharad)
 cosalpha = cos(alpharad)
 n = length(x)
 mean = atan2(1/n*sum(sinalpha), 1/n*sum(cosalpha))
 #convert back to degrees
 mean = mean * 180.0/pi
 mean = ifelse(mean < 0.0, 360+mean, mean)
 mean
}



