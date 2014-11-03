# Compute depths, hourly, not just day time rain duration (for that see beaverdam/auxdata/MetData/Hourly)

####### FROM PART 1 ############
raw<-read.csv(file="hrlydata.csv", header=T)

hrly <- data.frame(year=c(1900),month=c(1),day=c(1),hr=c(1),rain=c(0))
newrow <-hrly

for(i in seq(1:length(raw$YEAR))) {
	for(a in seq(1:24)){

		newrow[1] <- raw$YEAR[i]
		newrow[2] <- raw$MO[i]
		newrow[3] <- raw$DA[i]
		newrow[4] <- a
		newrow[5] <- raw[i,(a+3)]
		hrly <- rbind(hrly,newrow)
	}
	
}

hrly <- subset(hrly, hrly$rain>0)
hrly$rain <- hrly$rain / 10 * 0.0254 # convert tenths of inches to meters


#hrly.sv <- hrly

hrly <- hrly.sv
####### FROM PART 1 ############

rawpt2 <- read.csv(file="../Hourlypt2/20110401to20120721.csv", header=T)

hrly2 <- rawpt2[,c(-1,-2,-4,-8)]
colnames(hrly2) <- c("year","month","day","hr","rain")
hrly2 <- subset(hrly2, hrly2$rain>0)

hrly2$rain <- hrly2$rain / 10 * 0.0254 # convert tenths of inches to meters

hrly <- rbind(hrly,hrly2)
hrly$rain <- round(hrly$rain, digits=6)
hrlycnt <- length(hrly$rain)

cat(hrlycnt, "\n", file="BD_hourly.rain", append=FALSE)
write.table(hrly, file="BD_hourly.rain", row.names=FALSE, col.names=FALSE, append=TRUE, sep=" ")


library(chron)
day <- data.frame(date=seq.dates(from="01/01/1949", to="07/21/2012"), dur=0,rain=0)
day$yr <- as.numeric(years(day$date))+1948
day$mo <- as.numeric(months(day$date))
day$da <- as.numeric(days(day$date))

for(i in seq(1:length(day$date))){
	this.yr <- day[i,4]
	this.mo <- day[i,5]
	this.day <- day[i,6]
	hrly.tmp <- subset(hrly, hrly$year==this.yr&hrly$month==this.mo&hrly$day==this.day)
	day$dur[i] <- dim(hrly.tmp)[1]
	day$rain[i] <- sum(hrly.tmp$rain)
}

cat(day$yr[1], day$mo[1], day$da[1], 1, "\n", file="BD_daily.daytime_rain_duration", append=FALSE)
write.table(day$dur, file="BD_daily.daytime_rain_duration", append=TRUE, col.names=F, row.names=F)

cat(day$yr[1], day$mo[1], day$da[1], 1, "\n", file="BD_daily.rain", append=FALSE)
write.table(day$rain, file="BD_daily.rain", append=TRUE, col.names=F, row.names=F)
