
# start reading data here
tmp1 = read.table("../scripts/caln/str1", skip=1)
tmp2 = read.table("../scripts/caln/str2", skip=1)
#tmp3 = read.table("../scripts/caln/str3", skip=1)
#tmp4 = read.table("../scripts/caln/str4", skip=1)
#tmp5 = read.table("../scripts/caln/str5", skip=1)
#tmp6 = read.table("../scripts/caln/str6", skip=1)
#tmp7 = read.table("../scripts/caln/str7", skip=1)
#tmp8= read.table("../scripts/caln/str8", skip=1)
#tmp9 = read.table("../scripts/caln/str9", skip=1)
#tmp10 = read.table("../scripts/caln/str10", skip=1)
#results=cbind(tmp,tmp2,tmp3,tmp4, tmp5, tmp6, tmp7, tmp8, tmp9, tmp10)
results=as.data.frame(cbind(tmp1, tmp2))
results$date = seq.dates(from="10/1/1979", length=length(results$V1))

# you must already have a file called obs, with a date, and year, month,day # columns
# ncals = number of calibrations
ncal = ncol(results)-1
day3filter = rep(1/3,times=3)

lc = ncol(results)
lr = nrow(results)
tmp = apply(results[,1:(lc-1)],2,filter, filter=day3filter, method="convolution", sides=2)
tmp2 = as.data.frame(tmp[2:(lr-1),])
tmp2$date = results[2:(lr-1),"date"]

result3d = tmp2

lr = nrow(sagestr)
tmp = filter(sagestr$mm, filter=day3filter, method="convolution", sides=2)
tmp2 = as.data.frame(tmp[2:(lr-1)])
colnames(tmp2) = c("mm")
obs3d = tmp2
obs3d$date = sagestr$date[2:(lr-1)]

tmp1 = read.table("../scripts/caln/par1")
tmp2 = read.table("../scripts/caln/par2")
#tmp3 = read.table("../scripts/caln/par3")
#tmp4 = read.table("../scripts/caln/par4")
#tmp5 = read.table("../scripts/caln/par5")
#tmp6 = read.table("../scripts/caln/par6")
#tmp7 = read.table("../scripts/caln/par7")
#tmp8= read.table("../scripts/caln/par8")
#tmp9 = read.table("../scripts/caln/par9")
#tmp10 = read.table("../scripts/caln/par10")
#stats3d = rbind(tmp, tmp2, tmp3, tmp4,tmp5, tmp6, tmp7,tmp8,tmp9,tmp10)
stats3d = as.data.frame(rbind(tmp1, tmp2))
colnames(stats3d) = c("m","k","pa","po","gw1","gw2","rd")

result3d$year = as.numeric(as.character(years(result3d$date)))
 result3d$month = as.numeric(months(result3d$date))
 result3d$day = as.numeric(days(result3d$date))
 result3d$wy = ifelse((result3d$month >= 10), result3d$year+1, result3d$year)
stats3d$row = seq(from=1, length=length(stats3d$m))

tmp = merge(result3d, obs3d, by=c("date"))
result3d = tmp

# correction due to year with looks like overestimation of storm event
tmp = subset(result3d, result3d$wy != 1986)
tmp2 = apply(tmp[,2:(ncal+1)],2, nse, o=tmp$mm)
stats3d$nse.sh = tmp2

tmp2 = apply(tmp[,2:(ncal+1)],2, nselog, o=tmp$mm)
stats3d$nselog.sh = tmp2


tmp = apply(result3d[,2:(ncal+1)],2, nse, o=result3d$mm)
stats3d$nse = tmp

tmp = apply(result3d[,2:(ncal+1)], 2, nselog, o=result3d$mm)
stats3d$nselog = tmp

tmp = apply(result3d[,2:(ncal+1)],2, rmse, o=result3d$mm)
stats3d$rmse = tmp

tmp = apply(result3d[,2:(ncal+1)], 2, cor, y=result3d$mm)
tmp2 = tmp*tmp
stats3d$r2 = tmp2

tmp = apply(result3d[,2:(ncal+1)], 2, sum)
tmp2 = (tmp-sum(result3d$mm))/(sum(result3d$mm))*100
stats3d$perr.total = tmp2


result3d.mth = as.data.frame(matrix(nrow=12, ncol=ncal, 0.0))
for (i in 1:ncal) {
tmp = aggregate(result3d[,i+1], by=list(result3d$month), FUN=mean)
result3d.mth[,i] = tmp$x
}
tmp = aggregate(result3d$mm, by=list(result3d$month), FUN=mean)
result3d.mth$obs = tmp$x
tmp = apply(result3d.mth[,1:ncal],2, cor, y=result3d.mth$obs)
stats3d$mth.cor = tmp


result3d = mkdate(result3d)
result3d.wyd = as.data.frame(matrix(nrow=366, ncol=ncal, 0.0))
for (i in 1:ncal) {
tmp = aggregate(result3d[,i+1], by=list(result3d$wyd), FUN=mean)
result3d.wyd[,i] = tmp$x
}
tmp = aggregate(result3d$mm, by=list(result3d$wyd), FUN=mean)
result3d.wyd$obs = tmp$x
tmp = apply(result3d.wyd[,1:ncal],2, cor, y=result3d.wyd$obs)
stats3d$wyd.cor = tmp


tmp = range(result3d$wy)
nwy = tmp[2]-tmp[1]+1

result3d.min = as.data.frame(matrix(nrow=nwy, ncol=ncal, 0.0))
for (i in 1:ncal) {
tmp = aggregate(result3d[,i+1], by=list(result3d$wy), FUN=min)
result3d.min[,i] = tmp$x
}
tmp = aggregate(result3d$mm, by=list(result3d$wy), FUN=min)
result3d.min$obs = tmp$x
tmp = apply(result3d.min[,1:ncal],2, cor, y=result3d.min$obs)
stats3d$min.cor = tmp
tmp = apply(result3d.min[,1:ncal], 2, sum)
tmp2 = (tmp-sum(result3d.min$obs))/(sum(result3d.min$obs))*100
stats3d$min.perr = tmp2



tmp = apply(result3d.mth[,1:ncal],2, nse, o=result3d.mth$obs)
stats3d$mth.nse = tmp



result3d.wy = as.data.frame(matrix(nrow=nwy, ncol=ncal, 0.0))

for (i in 1:ncal) {
tmp = aggregate(result3d[,i+1], by=list(result3d$wy), FUN=sum)
result3d.wy[,i] = tmp$x
}

tmp = aggregate(result3d$mm, by=list(result3d$wy), FUN=sum)
result3d.wy$obs = tmp$x
result3d.wy$wy = as.numeric(as.character(tmp$Group.1))

tmp = apply(result3d.wy[,(1:ncal)],2,nse,o=result3d.wy$obs)
stats3d$wy.nse = tmp
tmp = apply(result3d.wy[,(1:ncal)],2,cor,y=result3d.wy$obs)
stats3d$wy.cor = tmp



result3d.mwy = as.data.frame(matrix(nrow=nwy*12, ncol=ncal, 0.0))

for (i in 1:ncal) {
tmp = aggregate(result3d[,i+1], by=list(result3d$wy, result3d$month), FUN=mean)
result3d.mwy[,i] = tmp$x
}

tmp = aggregate(result3d$mm, by=list(result3d$wy, result3d$month), FUN=mean)
result3d.mwy$obs = tmp$x
result3d.mwy$wy = as.numeric(as.character(tmp$Group.1))
result3d.mwy$month = as.numeric(tmp$Group.2)

result3d.aug = subset(result3d.mwy, result3d.mwy$month==8)

tmp = apply(result3d.aug[,(1:ncal)],2,nse,o=result3d.aug$obs)
stats3d$aug.nse = tmp

tmp = apply(result3d.aug[,(1:ncal)],2,cor,y=result3d.aug$obs)
stats3d$aug.cor = tmp

#April Streamflows
result3d.apr = subset(result3d.mwy, result3d.mwy$month==4)

tmp = apply(result3d.apr[,(1:ncal)],2,nse,o=result3d.apr$obs)
stats3d$apr.nse = tmp

tmp = apply(result3d.apr[,(1:ncal)],2,cor,y=result3d.apr$obs)
stats3d$apr.cor = tmp


