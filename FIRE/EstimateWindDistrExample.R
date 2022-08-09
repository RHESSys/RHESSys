##########
# R script to estimate wind distribution
# parameters necessary to run WMFire
# This requires wind speed (m/s)
# and wind direction

# Example
# read in the weather stream 
bc.met=read.csv("../ClimateDataWithWind/P301_v3_7_30min.csv") # m/s, degrees, rain in mm
# find the windspeed, we assume m/s
bc.wind.spd.ms=bc.met[!is.na(bc.met[,4]),4] # WSPD_(m/s)
# if wind direction in degrees, convert to rad
bc.wind.dir.deg=bc.met[!is.na(bc.met[,3]),3] # WDIR_(deg)

# Convert windspeed to radians if necessary
bc.wind.dir.rad=bc.wind.dir.deg*pi/180

# windspeeds tend to follow a bimodal distribution.
# the distribution fitting assumes the bimodal distribution
# is split at pi
# inspect the distribution of wind speeds and inspect 
# whether the bimodal distribution is split at pi
hist(bc.wind.dir.rad)
abline(v=pi)

# if not, apply a shift so it is split at pi
# this shift becomes the shift parameter for
# WMFire. If no shift then the parameter is zero
wind.shift=pi/2
# In this case the wind is shifted pi/2
bc.wind.dir.rad2=bc.wind.dir.rad+wind.shift
bc.wind.dir.rad3=bc.wind.dir.rad2
bc.wind.dir.rad3[bc.wind.dir.rad2>2*pi]=bc.wind.dir.rad2[bc.wind.dir.rad2>2*pi]-2*pi
hist(bc.wind.dir.rad3)

# requires the CircStats library       
library(CircStats)
# first estimate the proportion of windspeeds
# at either side of pi. This gives the relative
# occurrence of each distribution
p1a=length(bc.wind.dir.rad3[bc.wind.dir.rad3<=pi])/length(bc.wind.dir.rad3)
# Now estimate the parameters for each distribution
# on either side of pi
# these estimate the parameters for the von Mises distribution
ms1a=vm.ml(bc.wind.dir.rad3[bc.wind.dir.rad3<=pi])
ms2a=vm.ml(bc.wind.dir.rad3[bc.wind.dir.rad3>pi])
#
wind.shift #wind_shift in fire.def
p1a # p_rvm in fire.def
ms1a # mean1_rvm and kappa1_rvm in fire.def
ms2a # mean2_rvm and kappa2_rvm in fire.def

### check distribution by taking random draws
tmp1=rvm(100000*p1a,ms1a[,1],ms1a[,2])
tmp2=rvm(100000*(1-p1a),ms2a[,1],ms1a[,2])
tmpc=c(tmp1,tmp2)
tmpc=tmpc-wind.shift
tmpc[tmpc<0]=tmpc[tmpc<0]+2*pi
par(mfrow=c(1,2))
hist(bc.wind.dir.rad)
hist(tmpc)

### required to estimate windspeed distribution
library(fitdistrplus)
# assumes lognormal distribution
bc.wind.spd.lnorm=fitdist(bc.wind.spd.ms,distr = "lnorm")
bc.wind.spd.lnorm$estimate # shows mean_log_wind, sd_log_wind for WMFire
