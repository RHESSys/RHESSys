# Edit the rooftop raster to create edge cases
d.mon x1
d.rast.edit input=rooftop_conn output=rooftop_conn_ed2
d.rast.edit input=rooftop_conn_ed2 output=rooftop_conn_ed3
d.rast rooftop_conn_ed3

# Multiply by 100 to make roof map integer
r.mapcalc 'rooftop_conn_ed3_int = int(rooftop_conn_ed3 * 100)'
r.stats rooftop_conn_ed3_int

#Check and set map area to roof edge cases extent
r.stats rooftop_conn_ed3_int
g.region rast=rooftop_conn_ed3_int
r.mask rast=rooftop_conn_ed3_int

# Set 9900 to Null 
r.reclass rooftop_conn_ed3_int output=rooftop_conn_ed3_int.2 --o
-25=-25
0=0
25=25
50=50
190=190
9900=NULL
end

#Overlay on land use
d.rast lulc_5m_roof
r.colors rooftop_conn_ed3_int.2 color=grey
d.rast rooftop_conn_ed3_int.2 -o


# Create new lulc map that uses updated roofs.
# Have to reset old roof tiles. Do one map with grass, one with imperv.
# isnull condition must come first due to order of opps #issues

# Do Impervious one
r.reclass lulc_5m_roof output=lulc_5m_roof.i  --o
1 = 1
2 = 2
5 = 7
6 = 6
7 = 7
end

r.mapcalc 'new_lulc_5m_roof.i = if(isnull(rooftop_conn_ed3_int.2),  lulc_5m_roof.i, 5)'

# Compare
r.colors new_lulc_5m_roof.i color=rainbow
d.rast new_lulc_5m_roof.i
d.rast rooftop_conn_ed3_int -o

# Do Pervious one
r.reclass lulc_5m_roof output=lulc_5m_roof.p  --o
1 = 1
2 = 2
5 = 2
6 = 6
7 = 7
end

r.mapcalc 'new_lulc_5m_roof.p = if(isnull(rooftop_conn_ed3_int.2),  lulc_5m_roof.p, 5)'

# Compare
r.colors new_lulc_5m_roof.p color=rainbow
d.rast new_lulc_5m_roof.p
d.rast rooftop_conn_ed3_int -o


# Change roof map back to decimal
r.mapcalc 'rooftop_conn_ed4 = float(rooftop_conn_ed3_int.2) / 100'


# Impervious surface maps
r.reclass input=new_lulc_5m_roof.i output=impervious.i 
5 6 7 = 1 impervious
1 2 3 4 = 0 pervious
end

r.reclass input=new_lulc_5m_roof.p output=impervious.p
5 6 7 = 1 impervious
1 2 3 4 = 0 pervious
end

d.rast impervious.i
d.rast impervious.p


# Export necessary rasters to run model
r.out.gdal input=rooftop_conn_ed4 output=roofs.tif format=GTiff
r.out.gdal input=new_lulc_5m_roof.p output=landcover.tif format=GTiff
r.out.gdal input=new_lulc_5m_roof.i output=landcover_max_i.tif format=GTiff
r.out.gdal input=impervious.p output=impervious.tif format=GTiff
r.out.gdal input=impervious.i output=impervious_max_i.tif format=GTiff
r.out.gdal input=dem_5m output=dem.tif format=GTiff
r.out.gdal input=streams output=streams.tif format=GTiff
r.out.gdal input=roads_5m_bin output=roads.tif format=GTiff
r.out.gdal input=zone output=zone.tif format=GTiff
r.out.gdal input=patch_5m output=patch.tif format=GTiff
r.out.gdal input=hillslope output=hillslope.tif format=GTiff

#------
#In between, used GRASS GUI to create new workspace and set region based on #roofs
#------
r.in.gdal input=roofs.tif out=roofs
r.in.gdal input=landcover.tif out=landcover
r.in.gdal input=landcover_max_i.tif out=landcover_max_i
r.in.gdal input=impervious.tif out=impervious
r.in.gdal input=impervious_max_i.tif out=impervious_max_i
r.in.gdal input=dem.tif out=dem
r.in.gdal input=streams.tif out=streams
r.in.gdal input=roads.tif out=roads
r.in.gdal input=zone.tif out=zone
r.in.gdal input=patch.tif out=patch
r.in.gdal input=hillslope.tif out=hillslope


# Exported out to png and asc for Jason and Harry to test cf9
# lines did not get saved. used r.out.png and r.out.ascii


