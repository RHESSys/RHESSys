#!/bin/bash
#######################
# INPUTS
# Location of landsat metadata files
LANDDIR=$1
# Local maps needed
DEM=$2
WSH=$3
# Prefix of landsat images. Each should end in the number representing its band
IMG_PREFIX=$4

# Define extinction coefficient variable (dependent on veg type) and maximum LAI
K=0.4
laimax=6 # taken from conifer.def
# Define ndvi value for background (ndvimin) and infinite veg (ndvimax)
ndvimax=0.8
ndvimin=0.05

# Generate empty mean stats file
echo   > mean.txt

#######################
# Get file listing
cd $LANDDIR
PREFS=`ls *_MTL.txt| awk -F/ '{print $1}' | cut -d "_" -f1-2 | sort -u`
# Make starting mean LAI file
echo "DT LAI_K LAI_W LAI_S LAI_M LAI_F"> mean.txt

#######################
# Loop for each specific landsat image (must be unique per date)
for PREF in $PREFS; do
	echo $PREF
	DT=`echo $PREF | tail -9c`
	echo $DT
	# Extract control file variables from metadata
	PROD=`grep -aw PROCESSING_SOFTWARE ${PREF}_MTL.txt | awk '{print $3}'`
	# Geometrical conditions:
	SENSID=`grep -aw SENSOR_ID ${PREF}_MTL.txt | awk '{print $3}'`
	echo SENSOR=$SENSID
	if [ "$SENSID" == '"ETM+"' ]; then
		GEOM=8
		else
			GEOM=7
	fi
	echo GEOMETRY CODE=$GEOM
	#
	# Month and Day:
	RAWDATE=`grep -aw ACQUISITION_DATE ${PREF}_MTL.txt | awk '{print $3}'`
	echo RAWDATE=$RAWDATE
	YEAR=`echo $RAWDATE | cut -d "-" -f1`
	MONTH=`echo $RAWDATE | cut -d "-" -f2`
	DAY=`echo $RAWDATE | cut -d "-" -f3`
	echo YEAR=$YEAR
	echo MONTH=$MONTH
	echo DAY=$DAY
	#
	# Time:
	RAWTIME=`grep -aw SCENE_CENTER_SCAN_TIME ${PREF}_MTL.txt | awk '{print $3}'`
	if [ ! -n "$RAWTIME" ]; then
		RAWTIME=17:30
	fi
	echo RAWTIME=$RAWTIME
	HOUR=`echo $RAWTIME|cut -d ":" -f1`
	MIN=`echo $RAWTIME|cut -d ":" -f2`
	echo HOUR=$HOUR
	echo MINUTE=$MIN
	TIMEDH=$(echo "$HOUR+($MIN/60)" | bc -l)
	echo TIME DECIMAL HOURS=$TIMEDH
	#
	# Atmospheric mode:
	if [ $MONTH -ge 4 ] && [ $MONTH -le 9 ]; then
		ATMOD=2
		else
			ATMOD=3
	fi
	echo ATMOSPHERIC MODE=$ATMOD

	#######################
	# Band-specific calculations
	for BANDNO in 3 4 5; do
		GISPREF=${IMG_PREFIX}${BANDNO}
		LMAX=`grep -aw LMAX_BAND$BANDNO ${PREF}_MTL.txt | awk '{print $3}'`
		LMIN=`grep -aw LMIN_BAND$BANDNO ${PREF}_MTL.txt | awk '{print $3}'`
		# Import landsat band into current location
		# Calculate radiance
		echo Calculating radiance for image date $DT and band $BANDNO
		# Processing software = LPGS	
		r.mapcalc "${GISPREF}.rad=(${LMAX}-${LMIN})/254*(${GISPREF}-1)+${LMIN}"
		echo finished
		#
		# Calculate centerpoint coordinates of image
		eval `g.region -ucgb rast=${GISPREF}.rad`
		echo CENTERPOINT LONGITUDE=$ll_clon
		echo CENTERPOINT LATITUDE=$ll_clat
		#
		# Calculate mean elevation
		g.region --overwrite save=current.reg
		g.region rast=${GISPREF}.rad
		eval `r.univar -g map=$DEM`
		g.region region=current.reg
		echo MEAN ELEV=$mean
		#
		# Mean elevation in -km
		ELEVKM=$(echo "-$mean/1000" | bc -l)
		echo ELEV KM=$ELEVKM
		#
		# Sensor and Band code:
		if [ $GEOM -eq 8 ]; then
			SENSBAND=$[60+$BANDNO]
			else
				SENSBAND=$[24+$BANDNO]
		fi
		echo SENSOR+BAND CODE=$SENSBAND
		# Create parameter file for i.atcorr
		echo Building parameter file...
		echo $GEOM
		echo $MONTH $DAY $TIMEDH $ll_clon $ll_clat
		echo $ATMOD
		echo 1
		echo 50
		echo $ELEVKM
		echo -1000
		echo $SENSBAND
		echo "$GEOM
$MONTH $DAY $TIMEDH $ll_clon $ll_clat
$ATMOD
1
50
$ELEVKM
-1000
$SENSBAND" > params.txt
		# Run atmospheric correction model
		# Set flag for pre or post July 2000
		if [ $GEOM -eq 8 ]; then
			if [ $YEAR -gt 2000 ]; then
				abvar=-a
			else
				if [ $YEAR -eq 2000 ] && [ $MONTH -ge 7 ]; then
					abvar=-a
				else
					abvar=-b
				fi
			fi
		fi
		echo Running atmospheric correction...
		# Output is reflectance
		# Output the command about to run
		echo "i.atcorr ${abvar} -o ${GISPREF}.rad ialt=${DEM} icnd=params.txt oimg=${GISPREF}.atcor"
		i.atcorr ${abvar} -o ${GISPREF}.rad ialt=${DEM} icnd=params.txt oimg=${GISPREF}.atcor
		rm params.txt
	done
#######################
	# Reset GISPREF to not include the _band prefix
#	GISPREF=ls_46_26
	# Calculate NDVI
	r.mapcalc "NDVI=(float(${IMG_PREFIX}4.atcor)-float(${IMG_PREFIX}3.atcor))/(float(${IMG_PREFIX}4.atcor)+float(${IMG_PREFIX}3.atcor))"
	# Calculate PETI
	r.mapcalc "PETI=(float(${IMG_PREFIX}4.atcor)-float(${IMG_PREFIX}5.atcor))/(float(${IMG_PREFIX}4.atcor)+float(${IMG_PREFIX}5.atcor))"
	# Calculate ISR
	r.mapcalc "ISR=float(${IMG_PREFIX}4.atcor)/float(${IMG_PREFIX}5.atcor)"
	# Calculate RSR
		# Obtain min and max Band 5 values
 		eval `r.univar -e -g map=${IMG_PREFIX}5.atcor percentile=1`
		eval `r.univar -e -g map=${IMG_PREFIX}5.atcor percentile=99`
	r.mapcalc "RSR=(float(${IMG_PREFIX}4.atcor)/float(${IMG_PREFIX}3.atcor))*(${percentile_99}-float(${IMG_PREFIX}5.atcor))/(${percentile_99}-${percentile_1})"
	# Calculate LAI from different models
	# K parameter model
		mindiff=$(echo "(${ndvimax}-${ndvimin})*e(-$K*${laimax})" | bc -l)
		r.mapcalc "LAIK=if((-1/$K)*log(if((${ndvimax}-NDVI)<${mindiff},${mindiff},${ndvimax}-NDVI)/(${ndvimax}-${ndvimin}))<0,0,(-1/$K)*log(if((${ndvimax}-NDVI)<${mindiff},${mindiff},${ndvimax}-NDVI)/(${ndvimax}-${ndvimin})))"
	# White model (White et al 1997 - Glacier NP Montana, conifer forest with patches of deciduous/shrub/grass)
		r.mapcalc "LAIW=0.2273*exp(4.9721*NDVI)"
	# Stenberg model (Stenberg et al 2004 - Finland, spruce/pine forest)
		r.mapcalc "LAIS=0.23*RSR+0.49"
	# McDonnell model (unpublished - Rio Grande Basin, riparian)
		r.mapcalc "LAIM=12.29*PETI+1.33"
	# Fernandes model (Fernandes 2003 - Canada-wide)
		r.mapcalc "LAIF=pow(0.90+0.69*log(ISR),4)"

# Cap each map of LAI to laimax
# hardcoded laimax to 12, since having issues with quotes
	r.mapcalc "LAIK = min(LAIK,${laimax})"
	r.mapcalc "LAIW = min(LAIW,${laimax})"
	r.mapcalc "LAIS = min(LAIS,${laimax})"
	r.mapcalc "LAIM = min(LAIM,${laimax})"
	r.mapcalc "LAIF = min(LAIF,${laimax})"


	# Calculate watershed-wide mean LAI for each model and output to file
	r.mask input=$WSH maskcats=1
	eval `r.univar -g map=LAIK`
	meanK=$mean
	eval `r.univar -g map=LAIW`
	meanW=$mean
	eval `r.univar -g map=LAIS`
	meanS=$mean
	eval `r.univar -g map=LAIM`
	meanM=$mean
	eval `r.univar -g map=LAIF`
	meanF=$mean
	echo $DT $meanK $meanW $meanS $meanM $meanF > tmpmean
	cat mean.txt tmpmean > newmean
	mv newmean mean.txt
	r.mask -r input=$WSH maskcats=1
done
