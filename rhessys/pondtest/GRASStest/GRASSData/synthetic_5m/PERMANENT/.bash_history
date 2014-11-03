g.proj -p
cd /Users/miles/Dropbox/RHESSys/eclipse/rhessys-git/cf/test/data/GRASSData 
ls
mkdir dump
cd dump
ls
r.in.gdal intput=dem.tif output=dem
r.in.gdal input=dem.tif output=dem
r.info dem
g.remove dem
r.in.gdal input=dem.tif output=dem_in
r.resamp.interp input=dem_in output=dem method=bilinear res=5
r.resamp.interp input=dem_in output=dem method=bilinear 
r.info dem
r.info dem_in
g.list --help
g.list type=rast
ls
ls -l
ls -l
ls -l
rm streams_old.tif*
ls -l
g.list rast
g.remove rast=dem,dem_in
chmod +x import.py
./import.py 
ls
ls
./import.py 
./import.py 
./import.py 
./import.py 
g.list rast
./import.py 
g.list rast
g.list rast
ls
cd /Users/miles/Dropbox/RHESSys/eclipse/rhessys-git/cf/test/data/GRASSData/dump
ls
./import.py 
g.list rast
open .
r.info acc
cd ..
ls
rm -rf dump/
ls
g.list rast
r.info dem
cd ..
ls
unzip RHESSys.zip 
ls
cd RHESSys
ls
less README.txt
rm README.txt~ 
ls
g.list rast
../../../cf10.0a1 output=flow/synthetic template=templates/synthetic_514 stream=streams road=roads dem=dem slope=slope impervious=impervious roof=roofs cellsize=5
ls
open .
g.list rast
r.info -t roofs
r.info -t patch
r.info -t dem
r.what --v -f -n input=roofs@PERMANENT east_north=348890.027473,4351048.406593
r.what --v -f -n input=roofs@PERMANENT east_north=348851.346154,4351030.384615
r.what --v -f -n input=roofs@PERMANENT east_north=348838.598901,4351034.340659
r.what --v -f -n input=dem@PERMANENT east_north=348751.126374,4351073.901099
r.what --v -f -n input=cfmask@PERMANENT east_north=348818.709617,4350997.440717
r.what --v -f -n input=cfmask@PERMANENT east_north=348813.727382,4351048.923821
ls
cd /Users/miles/Dropbox/RHESSys/eclipse/rhessys-git/cf/test/data/GRASSData
ls
pwd
ls
pwd
ls
cd dump/
ls
r.info -t patch
r.in.gdal input=dem.tif output=dem_in
r.in.gdal input=patch.tif output=patch_in
r.info -t patch_in
r.info patch_in
g.list rast
g.mremove "streams_old*"
g.mremove -f "streams_old*"
g.list rast
r.info -t patch
r.info -t patch_in
g.list ras
g.list rast
r.info -t patch
r.info -t dem
r.info patch_in
g.mremove "patch*"
g.mremove -f "patch*"
g.region -p
r.mapcalc patch='(row() - 1) * 48 + col()'
r.info -t patch
f.into patch
f.info patch
r.info patch
r.clump input=landcover output=patch_clump
ls
g.list rast
g.mremove roofs
g.mremove -f roofs
r.resample input=roofs_in output=roofs
r.info -t soil
r.info -t hillslope
g.list rast
g.mremove "*_in"
g.mremove -f "*_in"
g.list rast
g.remove streams_old
g.list rast
r.mask
r.region
cd ..
ls
rm -rf dump/
ls
rm -rf sythentic/
ls
ls
cd ..
ls
cd RHESSysls
cd RHESSys
ls
../../../cf10.0a1 output=flow/synthetic template=templates/synthetic_514 stream=streams road=roads dem=dem slope=slope impervious=impervious roof=roofs cellsize=5
ls
open .
ls
g.list rast
r.mapcalc cfmask=0
g.rename cfpatch cfpatch_in
g.rename rast=cfpatch,cfpatch_in
g.rename rast=cfmask,cfmask_in
r.edit.rast input=cfmask_in output=cfmask
d.edit.rast input=cfmask_in output=cfmask
d.rast.edit input=cfmask_in output=cfmask
r.mask cfmask maskcats=1
../../../cf10.0a1 output=flow/synthetic template=templates/synthetic_514 stream=streams road=roads dem=dem slope=slope impervious=impervious roof=roofs cellsize=5
g.list rast
r.mask -r
g.list rast
g.remove cfmask
d.rast.edit input=cfmask_in output=cfmask
g.list rast
g.remove cfmask
v.info map=vmask@PERMANENT
g.list rast
d.rast.edit input=cfmask_in output=cfmask
g.list rast
g.remove cfmask
d.rast.edit input=cfmask_in output=cfmask rows=5000 cols=5000
fg
g.remove cfmask
d.rast.edit input=cfmask_in output=cfmask rows=5000 cols=5000 width=1000 height=1000
g.remove cfmask
d.rast.edit input=cfmask_in output=cfmask rows=10000 cols=10000 
g.list vect
g.remove vect=mask
g.list rast
g.remove cfmask
g.remove vect=vectmask
g.remove vect=vectmask
g.list vect
g.remove vect=mask_vect
g.remove cfmask
v.to.rast input=vmask output=cfmask
v.to.rast input=vmask output=cfmask use=attr
v.to.rast input=vmask output=cfmask col=id
v.info vmask
v.info -c vmask
v.to.rast input=vmask output=cfmask col=cat
g.list vect
g.remove vect=vmask2,vmask
g.list rast
g.remove cfmask
g.remove cfmask_in
v.to.rast input=vmask output=cfmask col=cat
r.mask cfmask maskcats=1
cd /Users/miles/Dropbox/RHESSys/eclipse/rhessys-git/cf/test/data
../../../cf10.0a1 output=flow/synthetic template=templates/synthetic_514 stream=streams road=roads dem=dem slope=slope impervious=impervious roof=roofs cellsize=5
cd RHESSys
../../../cf10.0a1 output=flow/synthetic template=templates/synthetic_514 stream=streams road=roads dem=dem slope=slope impervious=impervious roof=roofs cellsize=5
r.what --v -f -n input=landcover@PERMANENT east_north=348867.073942,4351058.022741
r.what --v -f -n input=landcover@PERMANENT east_north=348866.947296,4351058.529327
r.what --v -f -n input=patch@PERMANENT east_north=348866.947296,4351058.529327
r.what --v -f -n input=patch@PERMANENT east_north=348913.173206,4351006.477685
r.what --v -f -n input=patch@PERMANENT east_north=348893.036440,4351052.830242
r.what --v -f -n input=patch@PERMANENT east_north=348892.909793,4351046.497925
r.what --v -f -n input=patch@PERMANENT east_north=348887.843940,4351046.877864
r.what --v -f -n input=patch@PERMANENT east_north=348882.144855,4351047.004511
r.what --v -f -n input=patch@PERMANENT east_north=348877.458941,4351047.131157
r.what --v -f -n input=roofs@PERMANENT east_north=348877.458941,4351047.131157
r.what --v -f -n input=patch@PERMANENT east_north=348877.458941,4351047.131157
r.what --v -f -n input=patch@PERMANENT east_north=348897.215769,4351047.257803
r.what --v -f -n input=patch@PERMANENT east_north=348876.825709,4351042.318596
r.what --v -f -n input=patch@PERMANENT east_north=348876.825709,4351042.318596
r.what --v -f -n input=patch@PERMANENT east_north=348872.899673,4351046.877864
r.what --v -f -n input=patch@PERMANENT east_north=348877.585587,4351053.336827
r.what --v -f -n input=patch@PERMANENT east_north=348896.582537,4351052.830242
r.what --v -f -n input=patch@PERMANENT east_north=348876.952356,4351056.376339
r.what --v -f -n input=patch@PERMANENT east_north=348877.838880,4351053.336827
r.what --v -f -n input=patch@PERMANENT east_north=348877.332295,4351047.131157
r.what --v -f -n input=patch@PERMANENT east_north=348886.957416,4351046.751218
r.what --v -f -n input=patch@PERMANENT east_north=348883.158026,4351046.751218
r.what --v -f -n input=patch@PERMANENT east_north=348883.158026,4351043.078474
r.what --v -f -n input=patch@PERMANENT east_north=348892.529854,4351053.336827
r.what --v -f -n input=patch@PERMANENT east_north=348891.769976,4351057.136217
r.what --v -f -n input=patch@PERMANENT east_north=348876.572417,4351057.389510
r.what --v -f -n input=patch@PERMANENT east_north=348872.266442,4351056.756278
r.what --v -f -n input=patch@PERMANENT east_north=348883.031380,4351056.882924
r.what --v -f -n input=patch@PERMANENT east_north=348872.899673,4351052.956888
r.what --v -f -n input=patch@PERMANENT east_north=348887.970587,4351056.756278
r.what --v -f -n input=patch@PERMANENT east_north=348888.350526,4351041.305426
r.what --v -f -n input=patch@PERMANENT east_north=348883.537965,4351042.191950
r.what --v -f -n input=patch@PERMANENT east_north=348882.651441,4351046.497925
r.what --v -f -n input=patch@PERMANENT east_north=348877.079002,4351043.964999
r.what --v -f -n input=patch@PERMANENT east_north=348882.778087,4351037.379390
r.what --v -f -n input=patch@PERMANENT east_north=348888.730465,4351036.619512
r.what --v -f -n input=patch@PERMANENT east_north=348877.712234,4351042.065304
r.what --v -f -n input=patch@PERMANENT east_north=348873.532905,4351046.497925
r.what --v -f -n input=patch@PERMANENT east_north=348872.646381,4351042.318596
r.what --v -f -n input=patch@PERMANENT east_north=348882.904733,4351041.812011
r.what --v -f -n input=patch@PERMANENT east_north=348882.651441,4351038.265914
r.what --v -f -n input=patch@PERMANENT east_north=348878.092173,4351038.139268
r.what --v -f -n input=patch@PERMANENT east_north=348873.026320,4351037.759329
r.what --v -f -n input=patch@PERMANENT east_north=348891.896623,4351042.951828
r.what --v -f -n input=patch@PERMANENT east_north=348891.643330,4351046.371279
r.what --v -f -n input=patch@PERMANENT east_north=348886.577477,4351046.877864
r.what --v -f -n input=patch@PERMANENT east_north=348887.337355,4351041.558719
r.what --v -f -n input=patch@PERMANENT east_north=348882.651441,4351041.685365
r.what --v -f -n input=patch@PERMANENT east_north=348882.524794,4351038.265914
r.what --v -f -n input=patch@PERMANENT east_north=348888.350526,4351038.392560
r.what --v -f -n input=patch@PERMANENT east_north=348892.656501,4351056.502985
r.what --v -f -n input=patch@PERMANENT east_north=348888.097233,4351057.009571
r.what --v -f -n input=patch@PERMANENT east_north=348882.398148,4351057.516156
r.what --v -f -n input=patch@PERMANENT east_north=348877.965526,4351052.323657
r.what --v -f -n input=patch@PERMANENT east_north=348881.891563,4351052.323657
r.what --v -f -n input=patch@PERMANENT east_north=348897.595708,4351052.830242
r.what --v -f -n input=patch@PERMANENT east_north=348881.258331,4351056.376339
r.what --v -f -n input=patch@PERMANENT east_north=348887.464001,4351053.590120
r.what --v -f -n input=patch@PERMANENT east_north=348897.215769,4351053.083535
r.what --v -f -n input=patch@PERMANENT east_north=348887.590648,4351056.629632
r.what --v -f -n input=patch@PERMANENT east_north=348888.097233,4351052.956888
r.what --v -f -n input=patch@PERMANENT east_north=348892.656501,4351028.007561
r.what --v -f -n input=patch@PERMANENT east_north=348887.843940,4351027.247683
r.what --v -f -n input=patch@PERMANENT east_north=348888.097233,4351022.435123
r.what --v -f -n input=patch@PERMANENT east_north=348887.843940,4351031.806951
r.what --v -f -n input=patch@PERMANENT east_north=348871.506564,4351057.516156
r.what --v -f -n input=patch@PERMANENT east_north=348867.833820,4351057.516156
r.what --v -f -n input=patch@PERMANENT east_north=348872.393088,4351057.136217
r.what --v -f -n input=patch@PERMANENT east_north=348877.205648,4351057.262863
r.what --v -f -n input=patch@PERMANENT east_north=348873.659551,4351053.210181
r.what --v -f -n input=patch@PERMANENT east_north=348867.580527,4351053.463474
r.what --v -f -n input=patch@PERMANENT east_north=348892.529854,4351053.083535
r.what --v -f -n input=patch@PERMANENT east_north=348888.477172,4351057.516156
r.what --v -f -n input=patch@PERMANENT east_north=348888.097233,4351057.262863
r.what --v -f -n input=patch@PERMANENT east_north=348891.896623,4351057.896095
r.what --v -f -n input=patch@PERMANENT east_north=348882.144855,4351057.642802
r.what --v -f -n input=patch@PERMANENT east_north=348888.350526,4351031.553658
r.what --v -f -n input=patch@PERMANENT east_north=348887.717294,4351035.986280
r.what --v -f -n input=patch@PERMANENT east_north=348883.411319,4351035.479695
r.what --v -f -n input=patch@PERMANENT east_north=348883.411319,4351031.553658
r.what --v -f -n input=patch@PERMANENT east_north=348897.089122,4351031.933597
r.what --v -f -n input=patch@PERMANENT east_north=348896.709183,4351028.007561
r.what --v -f -n input=patch@PERMANENT east_north=348891.010098,4351027.374330
r.what --v -f -n input=patch@PERMANENT east_north=348886.830770,4351026.994391
r.what --v -f -n input=patch@PERMANENT east_north=348882.904733,4351027.374330
r.what --v -f -n input=patch@PERMANENT east_north=348877.585587,4351031.933597
r.what --v -f -n input=patch@PERMANENT east_north=348883.284672,4351031.806951
r.what --v -f -n input=patch@PERMANENT east_north=348882.144855,4351022.181830
r.what --v -f -n input=patch@PERMANENT east_north=348887.717294,4351035.732987
r.what --v -f -n input=patch@PERMANENT east_north=348883.537965,4351031.933597
r.what --v -f -n input=patch@PERMANENT east_north=348883.537965,4351035.732987
r.what --v -f -n input=patch@PERMANENT east_north=348892.656501,4351057.389510
r.what --v -f -n input=patch@PERMANENT east_north=348866.694003,4351058.022741
r.what --v -f -n input=patch@PERMANENT east_north=348867.580527,4351053.716766
r.what --v -f -n input=patch@PERMANENT east_north=348871.633210,4351052.956888
r.what --v -f -n input=patch@PERMANENT east_north=348881.764916,4351032.060244
r.what --v -f -n input=patch@PERMANENT east_north=348877.965526,4351031.300366
r.what --v -f -n input=patch@PERMANENT east_north=348882.778087,4351035.859634
r.what --v -f -n input=patch@PERMANENT east_north=348876.952356,4351035.732987
r.what --v -f -n input=patch@PERMANENT east_north=348877.332295,4351031.300366
r.what --v -f -n input=patch@PERMANENT east_north=348882.524794,4351031.680305
r.what --v -f -n input=patch@PERMANENT east_north=348882.524794,4351036.239573
r.what --v -f -n input=patch@PERMANENT east_north=348883.158026,4351028.640793
r.what --v -f -n input=patch@PERMANENT east_north=348883.031380,4351032.440183
r.what --v -f -n input=patch@PERMANENT east_north=348877.458941,4351036.366219
r.what --v -f -n input=patch@PERMANENT east_north=348878.472112,4351038.012621
r.what --v -f -n input=patch@PERMANENT east_north=348881.511624,4351036.112926
r.what --v -f -n input=patch@PERMANENT east_north=348885.817599,4351036.619512
r.what --v -f -n input=patch@PERMANENT east_north=348888.350526,4351035.479695
r.what --v -f -n input=patch@PERMANENT east_north=348887.717294,4351032.313536
r.what --v -f -n input=patch@PERMANENT east_north=348885.311014,4351032.313536
r.what --v -f -n input=patch@PERMANENT east_north=348882.524794,4351031.806951
r.what --v -f -n input=patch@PERMANENT east_north=348883.031380,4351035.099756
r.what --v -f -n input=patch@PERMANENT east_north=348887.210709,4351035.606341
r.what --v -f -n input=patch@PERMANENT east_north=348878.218819,4351035.479695
r.what --v -f -n input=patch@PERMANENT east_north=348877.585587,4351031.300366
r.what --v -f -n input=patch@PERMANENT east_north=348876.825709,4351052.830242
r.what --v -f -n input=patch@PERMANENT east_north=348872.139795,4351053.083535
r.what --v -f -n input=patch@PERMANENT east_north=348873.026320,4351050.044023
r.what --v -f -n input=patch@PERMANENT east_north=348873.532905,4351052.450303
r.what --v -f -n input=patch@PERMANENT east_north=348873.532905,4351048.524267
r.what --v -f -n input=patch@PERMANENT east_north=348868.593698,4351048.270974
r.what --v -f -n input=patch@PERMANENT east_north=348877.205648,4351036.746158
r.what --v -f -n input=patch@PERMANENT east_north=348882.524794,4351037.252743
r.what --v -f -n input=patch@PERMANENT east_north=348877.965526,4351037.759329
r.what --v -f -n input=patch@PERMANENT east_north=348877.712234,4351034.213231
r.what --v -f -n input=patch@PERMANENT east_north=348877.585587,4351032.693475
r.what --v -f -n input=patch@PERMANENT east_north=348873.279612,4351030.667134
r.what --v -f -n input=patch@PERMANENT east_north=348873.532905,4351028.134208
r.what --v -f -n input=patch@PERMANENT east_north=348893.036440,4351027.374330
r.what --v -f -n input=patch@PERMANENT east_north=348893.036440,4351027.374330
r.what --v -f -n input=patch@PERMANENT east_north=348892.276562,4351027.500976
r.what --v -f -n input=patch@PERMANENT east_north=348891.896623,4351022.688415
r.what --v -f -n input=patch@PERMANENT east_north=348887.590648,4351031.806951
r.what --v -f -n input=patch_clump@PERMANENT east_north=348887.590648,4351031.806951
r.what --v -f -n input=patch_clump@PERMANENT east_north=348867.960466,4351057.516156
r.what --v -f -n input=patch_clump@PERMANENT east_north=348887.590648,4351057.009571
r.what --v -f -n input=patch_clump@PERMANENT east_north=348892.783147,4351057.136217
r.what --v -f -n input=hillslope@PERMANENT east_north=348892.783147,4351057.136217
r.what --v -f -n input=hillslope@PERMANENT east_north=348879.485282,4351057.389510
r.what --v -f -n input=hillslope@PERMANENT east_north=348882.778087,4351057.389510
r.what --v -f -n input=hillslope@PERMANENT east_north=348886.197538,4351057.642802
r.what --v -f -n input=hillslope@PERMANENT east_north=348888.983757,4351057.769449
r.what --v -f -n input=hillslope@PERMANENT east_north=348873.532905,4351053.210181
r.what --v -f -n input=hillslope@PERMANENT east_north=348868.087113,4351053.590120
r.what --v -f -n input=hillslope@PERMANENT east_north=348893.163086,4351058.529327
r.what --v -f -n input=hillslope@PERMANENT east_north=348869.606869,4351057.516156
r.what --v -f -n input=hillslope@PERMANENT east_north=348868.213759,4351057.516156
r.what --v -f -n input=hillslope@PERMANENT east_north=348892.403208,4351056.882924
r.what --v -f -n input=hillslope@PERMANENT east_north=348892.403208,4351056.882924
r.what --v -f -n input=patch_clump@PERMANENT east_north=348892.403208,4351056.882924
r.what --v -f -n input=patch_clump@PERMANENT east_north=348892.656501,4351027.627622
r.what --v -f -n input=hillslope@PERMANENT east_north=348892.656501,4351027.627622
r.what --v -f -n input=hillslope@PERMANENT east_north=348912.919914,4351051.690425
r.what --v -f -n input=hillslope@PERMANENT east_north=348912.286682,4351047.384450
r.what --v -f -n input=hillslope@PERMANENT east_north=348911.780097,4351053.210181
r.what --v -f -n input=hillslope@PERMANENT east_north=348913.173206,4351057.642802
r.what --v -f -n input=hillslope@PERMANENT east_north=348908.233999,4351057.642802
r.what --v -f -n input=hillslope@PERMANENT east_north=348908.360646,4351057.642802
r.what --v -f -n input=hillslope@PERMANENT east_north=348913.173206,4351052.323657
r.what --v -f -n input=patch_clump@PERMANENT east_north=348913.173206,4351052.323657
r.what --v -f -n input=patch_clump@PERMANENT east_north=348893.669671,4351027.121037
r.what --v -f -n input=hillslope@PERMANENT east_north=348893.669671,4351027.121037
r.what --v -f -n input=hillslope@PERMANENT east_north=348892.783147,4351026.994391
r.what --v -f -n input=hillslope@PERMANENT east_north=348897.722354,4351027.247683
r.what --v -f -n input=hillslope@PERMANENT east_north=348898.102293,4351032.186890
r.what --v -f -n input=hillslope@PERMANENT east_north=348889.616989,4351032.566829
r.what --v -f -n input=patch_clump@PERMANENT east_north=348893.289732,4351026.994391
r.what --v -f -n input=patch_clump@PERMANENT east_north=348893.669671,4351023.574940
r.what --v -f -n input=patch_clump@PERMANENT east_north=348899.748695,4351024.081525
r.what --v -f -n input=patch_clump@PERMANENT east_north=348888.603818,4351031.680305
r.what --v -f -n input=patch_clump@PERMANENT east_north=348912.919914,4351057.642802
r.what --v -f -n input=patch_clump@PERMANENT east_north=348896.962476,4351042.698535
r.what --v -f -n input=patch_clump@PERMANENT east_north=348887.337355,4351041.178780
r.what --v -f -n input=patch_clump@PERMANENT east_north=348881.638270,4351041.938657
r.what --v -f -n input=patch_clump@PERMANENT east_north=348877.712234,4351041.432072
r.what --v -f -n input=patch_clump@PERMANENT east_north=348897.089122,4351037.632682
r.what --v -f -n input=patch_clump@PERMANENT east_north=348897.215769,4351041.178780
r.what --v -f -n input=patch_clump@PERMANENT east_north=348897.469061,4351033.453353
r.what --v -f -n input=patch_clump@PERMANENT east_north=348892.529854,4351041.178780
r.what --v -f -n input=patch_clump@PERMANENT east_north=348902.534914,4351042.318596
r.what --v -f -n input=patch_clump@PERMANENT east_north=348897.595708,4351047.891035
r.what --v -f -n input=patch_clump@PERMANENT east_north=348897.469061,4351052.070364
r.what --v -f -n input=patch_clump@PERMANENT east_north=348912.666621,4351056.629632
r.what --v -f -n input=patch_clump@PERMANENT east_north=348911.653450,4351052.070364
r.what --v -f -n input=patch_clump@PERMANENT east_north=348913.046560,4351046.497925
r.what --v -f -n input=patch_clump@PERMANENT east_north=348883.664611,4351022.308476
r.what --v -f -n input=patch_clump@PERMANENT east_north=348896.835830,4351032.693475
r.what --v -f -n input=patch_clump@PERMANENT east_north=348883.031380,4351022.688415
r.what --v -f -n input=patch_clump@PERMANENT east_north=348882.524794,4351032.440183
r.what --v -f -n input=patch_clump@PERMANENT east_north=348876.445771,4351041.812011
r.what --v -f -n input=patch_clump@PERMANENT east_north=348883.791258,4351021.421952
r.what --v -f -n input=roofs@PERMANENT east_north=348883.791258,4351021.421952
r.what --v -f -n input=roofs@PERMANENT east_north=348891.136745,4351048.904206
r.what --v -f -n input=patch_clump@PERMANENT east_north=348891.136745,4351048.904206
r.what --v -f -n input=patch_clump@PERMANENT east_north=348877.458941,4351042.698535
r.what --v -f -n input=patch_clump@PERMANENT east_north=348878.472112,4351032.313536
r.what --v -f -n input=patch@PERMANENT east_north=348877.332295,4351052.830242
r.what --v -f -n input=patch@PERMANENT east_north=348877.332295,4351052.830242
r.what --v -f -n input=patch@PERMANENT east_north=348877.712234,4351047.384450
r.what --v -f -n input=patch@PERMANENT east_north=348897.342415,4351047.257803
r.what --v -f -n input=patch@PERMANENT east_north=348897.595708,4351052.323657
cd /Users/miles/Dropbox/RHESSys/eclipse/rhessys-git/cf/test/data
ls
cd RHESSys
ls
../../../cf10.0a1 output=flow/synthetic_clump template=templates/synthetic_514_clump stream=streams road=roads dem=dem slope=slope impervious=impervious roof=roofs cellsize=5
ls
open .
../../../cf10.0a1 output=flow/synthetic_single template=templates/synthetic_514 stream=streams road=roads dem=dem slope=slope cellsize=5
ls
diff flow/synthetic_single.flow flow/synthetic_subsurface.flow 
d.rast.edit --help
d.rast.edit input=impervious output=impervious
d.rast.edit input=impervious output=impervious2
g.remove impervious
g.rename rast=impervious2,impervious
d.rast.edit input=landcover output=landcover2
g.remove landcover
g.rename rast=landcover2,landcover
../../../cf10.0a1 output=flow/synthetic_single template=templates/synthetic_514 stream=streams road=roads dem=dem slope=slope cellsize=5
../../../cf10.0a1 output=flow/synthetic_clump template=templates/synthetic_514_clump stream=streams road=roads dem=dem slope=slope impervious=impervious roof=roofs cellsize=5
../../../cf10.0a1 output=flow/synthetic template=templates/synthetic_514 stream=streams road=roads dem=dem slope=slope impervious=impervious roof=roofs cellsize=5
diff flow/synthetic_clump_subsurface.flow flow/synthetic_single.flow 
../../../cf10.0a1 output=flow/synthetic_clump_single template=templates/synthetic_514_clump stream=streams road=roads dem=dem slope=slope cellsize=5
diff flow/synthetic_clump_single.flow flow/synthetic_clump_subsurface.flow 
diff flow/synthetic_single.flow flow/synthetic_subsurface.flow 
../../../cf10.0a1 output=flow/synthetic template=templates/synthetic_514 stream=streams road=roads dem=dem slope=slope impervious=impervious roof=roofs cellsize=5
gdb ../../../cf10.0a1 
ls
../../../cf10.0a1 output=flow/synthetic template=templates/synthetic_514 stream=streams road=roads dem=dem slope=slope impervious=impervious roof=roofs cellsize=5
ls
cd flow/
ls
for $i in synthetic*; do     mv $i keep-$i; done
for i in synthetic*; do     mv $i keep-$i; done
ls
../../../cf10.0a1 output=flow/synthetic template=templates/synthetic_514 stream=streams road=roads dem=dem slope=slope impervious=impervious roof=roofs cellsize=5
cd ..
../../../cf10.0a1 output=flow/synthetic template=templates/synthetic_514 stream=streams road=roads dem=dem slope=slope impervious=impervious roof=roofs cellsize=5
less flow/synthetic_surface.flow 
../../../cf10.0a1 output=flow/synthetic template=templates/synthetic_514 stream=streams road=roads dem=dem slope=slope impervious=impervious roof=roofs cellsize=5
ls
cd flow
ls
rm keep-synthetic_*
ls
rm synthetic_su*
ls
cd ..
../../../cf10.0a1 output=flow/mask-synthetic template=templates/synthetic_514 stream=streams road=roads dem=dem slope=slope impervious=impervious roof=roofs cellsize=5
../../../cf10.0a1 output=flow/mask-synthetic_clump template=templates/synthetic_514_clump stream=streams road=roads dem=dem slope=slope impervious=impervious roof=roofs cellsize=5
../../../cf10.0a1 output=flow/mask-synthetic_single template=templates/synthetic_514 stream=streams road=roads dem=dem slope=slope cellsize=5
../../../cf10.0a1 output=flow/mask-synthetic_clump_single template=templates/synthetic_514_clump stream=streams road=roads dem=dem slope=slope cellsize=5
ls flow
diff flow/mask-synthetic_clump_single.flow flow/mask-synthetic_clump_subsurface.flow
diff flow/mask-synthetic_single.flow flow/mask-synthetic_subsurface.flow
cd flow
ls
for i in mask-*; do cp $i keep-$i; done
ls
ls
rm mask-synthetic_*
ls
cd ..
../../../cf10.0a1 output=flow/mask-synthetic_clump template=templates/synthetic_514_clump stream=streams road=roads dem=dem slope=slope impervious=impervious roof=roofs cellsize=5
../../../cf10.0a1 output=flow/mask-synthetic template=templates/synthetic_514 stream=streams road=roads dem=dem slope=slope impervious=impervious roof=roofs cellsize=5
ls flow/
diff flow/keep-mask-synthetic_clump_surface.flow flow/mask-synthetic_clump_surface.flow 
diff flow/keep-mask-synthetic_surface.flow flow/mask-synthetic_surface.flow 
gdb ../../../cf10.0a1 
ls flow/
rm flow/mask-synthetic_*
../../../cf10.0a1 output=flow/mask-synthetic template=templates/synthetic_514 stream=streams road=roads dem=dem slope=slope impervious=impervious roof=roofs cellsize=5
../../../cf10.0a1 output=flow/mask-synthetic_clump template=templates/synthetic_514_clump stream=streams road=roads dem=dem slope=slope impervious=impervious roof=roofs cellsize=5
diff flow/keep-mask-synthetic_surface.flow flow/mask-synthetic_surface.flow 
diff flow/keep-mask-synthetic_clump_surface.flow flow/mask-synthetic_clump_surface.flow 
ls
cd flow/
ls
for i in mask-*; do mv $i keep-$i; done
ls
ls
cd ..
ls
less README.txt 
ls
cd ..
ls
rm RHESSys.zip 
ls
zip -x .DS_Store -r RHESSys.zip RHESSys
rm RHESSys.zip 
man zip
ls
zip -r RHESSys.zip RHESSys -x .DS_Store 
rm RHESSys.zip 
zip -r RHESSys.zip RHESSys 
rm RHESSys.zip 
zip -r RHESSys.zip RHESSys -x RHESSys/*/.DS_Store
rm RHESSys.zip 
zip -r RHESSys.zip RHESSys -x RHESSys/.DS_Store RHESSys/*/.DS_Store
ls
r.what --v -f -n input=patch@PERMANENT east_north=348878.454001,4351042.270626
r.what --v -f -n input=hillslope@PERMANENT east_north=348878.454001,4351042.270626
r.what --v -f -n input=patch@PERMANENT east_north=348872.456173,4351047.163590
r.what --v -f -n input=patch@PERMANENT east_north=348872.456173,4351047.794941
r.what --v -f -n input=patch@PERMANENT east_north=348871.824823,4351042.112788
r.what --v -f -n input=patch@PERMANENT east_north=348867.089695,4351038.482524
r.what --v -f -n input=patch@PERMANENT east_north=348867.878883,4351033.273884
r.what --v -f -n input=patch@PERMANENT east_north=348877.349138,4351036.588473
r.what --v -f -n input=patch@PERMANENT east_north=348883.031290,4351037.061986
r.what --v -f -n input=patch@PERMANENT east_north=348883.031290,4351037.061986
r.what --v -f -n input=patch@PERMANENT east_north=348878.769676,4351031.853346
cd /Users/miles/Dropbox/RHESSys/eclipse/rhessys-git/cf/test/data/RHESSys 
../../../cf10.0b1 output=flow/synthetic template=templates/synthetic_514 stream=streams road=roads dem=dem slope=slope impervious=impervious roof=roofs cellsize=5
r.mask -r
../../../cf10.0b1 output=flow/synthetic template=templates/synthetic_514 stream=streams road=roads dem=dem slope=slope impervious=impervious roof=roofs cellsize=5
r.info landcover
r.info roods
r.info roofss
r.info roofs
r.info landcover
d.rast.edit input=roofs output=roofs2
g.rename rast=roofs,roofs_old
g.rename rast=roofs2,roofs
ls
g.list rast
r.in.gdal input=impervious.tif output=impervious_in
r.in.gdal input=landcover.tif output=landcover_in
g.remove impervious
g.remove landcover
r.resample input=impervious_in output=impervious
r.resample input=landcover_in output=landcover
g.remove impervious
g.copy rast=roads,impervious
g.remove impervious
r.resample input=impervious_in output=impervious
g.remove impervious
r.resample input=impervious_in output=impervious_1
r.mapcalc impervious='if(isnull(roads), impervious_1, roads)'
d.rast.edit input=landcover output=landcover2
g.remove landcover
g.rename rast=landcover2,landcover
d.rast.edit input=roofs output=roofs2
g.remove roofs2
d.rast.edit input=roofs output=roofs2
g.remove roofs2
r.info roods
r.info roofs
d.rast.edit input=roofs output=roofs_edit
g.remove roofs_edit
d.rast.edit input=roofs output=roofs_edit
d.rast.edit input=roofs output=roofs_edit
d.rast.edit input=roofs output=roofs_edit --overwrite
d.rast.edit input=roofs_edit output=roofs_edit2 --overwrite
d.rast.edit input=roofs output=roofs_edit --overwrite
d.rast.edit input=impervious output=impervious_edit
g.list rast
g.remove rast=roofs,impervious
g.rename rast=roofs_edit,roofs
g.rename rast=impervious_edit,impervious
ls
rm impervious.tif 
rm landcover.tif 
ls
cd ..
cd -
g.list rast
g.remove impervious_1
g.remove impervious_in
g.remove roofs_edit2
g.remove roofs_old
g.list rast
../../../cf10.0b1 output=flow/synthetic template=templates/synthetic_514 stream=streams road=roads dem=dem slope=slope impervious=impervious roof=roofs cellsize=5
d.rast.edit input=landcover output=landcover2
g.remove landcover
g.rename rast=landcover2,landcover
g.list rast
g.remove landcover_in
open 
open .
g.list rast
cd ..
cd -
ls
cd ..
ls
open .
ls
ls -F
r.what --v -f -n input=roofs@PERMANENT east_north=348881.675824,4351051.043956
r.what --v -f -n input=roofs@PERMANENT east_north=348832.445055,4351036.098901
r.what --v -f -n input=roofs@PERMANENT east_north=348832.005495,4351050.164835
r.what --v -f -n input=roofs@PERMANENT east_north=348831.565934,4351055.000000
r.what --v -f -n input=roofs@PERMANENT east_north=348832.884615,4351012.362637
r.what --v -f -n input=roofs@PERMANENT east_north=348832.884615,4351012.362637
r.what --v -f -n input=roofs@PERMANENT east_north=348928.269231,4350893.681319
cd /Users/miles/Dropbox/RHESSys/eclipse/rhessys-git/cf/test/data/RHESSys-
ls
../../../cf10.0b2 --help
g.list rast
ls
la
ls
r.what --v -f -n input=roofs@PERMANENT east_north=348884.127737,4351051.729927
r.what --v -f -n input=roofs@PERMANENT east_north=348884.723881,4351050.940299
cd /Users/miles/Dropbox/RHESSys/eclipse/rhessys-git/cf/test/data
ls
cd RHESSys-/
ls
rm RoofGeometries.txt 
ls
../../../cf10.0b2 
../../../cf10.0b2 --help
../../../cf10.0b2 output=flow/test_synthetic template=templates/synthetic_514 stream=streams road=roads dem=dem slope=slope impervious=impervious roof=roofs cellsize=5
../../../cf10.0b2 output=flow/test_synthetic_single template=templates/synthetic_514 stream=streams road=roads dem=dem slope=slope cellsize=5
ls
../../../cf10.0b2 output=flow/test_synthetic_double template=templates/synthetic_514 stream=streams road=roads dem=dem slope=slope impervious=impervious roof=roofs cellsize=5
diff flow/test_synthetic_single.flow flow/test_synthetic_double_subsurface.flow 
ls
diff flow/test_synthetic_single.flow flow/test_synthetic_double_surface.flow 
less flow/test_synthetic_double_surface.flow 
ls
cd flow/
ls
rm test_synthetic_*
ls
cd ..
../../../cf10.0b2 output=flow/test_synthetic_single template=templates/synthetic_514 stream=streams road=roads dem=dem slope=slope cellsize=5
../../../cf10.0b2 output=flow/test_synthetic_single template=templates/synthetic_514 stream=streams road=roads dem=dem slope=slope cellsize=5
../../../cf10.0b2 output=flow/test_synthetic_double template=templates/synthetic_514 stream=streams road=roads dem=dem slope=slope impervious=impervious roof=roofs cellsize=5
../../../cf10.0b2 output=flow/test_synthetic_double template=templates/synthetic_514 stream=streams road=roads dem=dem slope=slope impervious=impervious roof=roofs cellsize=5
ls
less flow/test_synthetic_double_surface.flow 
r.what --v -f -n input=patch@PERMANENT east_north=348991.565934,4350858.076923
r.what --v -f -n input=patch@PERMANENT east_north=348721.236264,4350857.637363
r.what --v -f -n input=patch@PERMANENT east_north=348731.785714,4350856.318681
r.what --v -f -n input=patch@PERMANENT east_north=348737.060440,4350855.439560
r.what --v -f -n input=patch@PERMANENT east_north=348743.653846,4350855.439560
r.what --v -f -n input=patch@PERMANENT east_north=348748.489011,4350855.439560
r.what --v -f -n input=dem2@PERMANENT east_north=348741.895604,4350862.472527
r.what --v -f -n input=dem2@PERMANENT east_north=348747.170330,4350861.593407
r.what --v -f -n input=dem2@PERMANENT east_north=348748.489011,4350861.593407
r.what --v -f -n input=dem2@PERMANENT east_north=348753.324176,4350861.593407
r.what --v -f -n input=dem2@PERMANENT east_north=348756.840659,4350861.593407
r.what --v -f -n input=dem2@PERMANENT east_north=348752.005495,4350862.032967
cd /Users/miles/Dropbox/BES-DeadRun/RHESSys/flow
cd ..
ls
less runs.txt 
ls
cd flow/
ls
cd ~/Dropbox/RHESSys/eclipse/rhessys-git/cf/test/data/RHESSys-/
ls
pwd
ls
../../../cf10.0b2 output=flow/test_synthetic_double template=templates/synthetic_514 stream=streams road=roads dem=dem slope=slope impervious=impervious roof=roofs cellsize=5
g.list rast
r.mapcalc --help
r.stats -c roofs
r.stats -cr roofs
r.stats -cir roofs
r.stats -cp roofs
r.report roofs
r.report -e roofs
r.report basin
r.report units=acres map=basin
r.report map=basin units=acres
r.report map=basin units=a
r.report -h map=basin units=a
r.report -h map=basin units=a
r.stats -cp roofs
r.stats -1cp roofs
r.stats -Acp roofs
r.stats -cap roofs
r.mapcalc roofs2a='if(roofs<0, 0, roofs)'
r.mapcalc roofs2='if(roofs2a>1, 1, roofs2a)'
r.stats -cp roofs2
../../../cf10.0b2 output=flow/test_synthetic_double2 template=templates/synthetic_514 stream=streams road=roads dem=dem slope=slope impervious=impervious roof=roofs2 cellsize=5
pwd
g.region rast=dem
r.rast.edit dem
d.rast.edit dem
d.rast.edit input=dem output=dem2
../../../cf10.0b2 output=flow/test_synthetic_double3 template=templates/synthetic_514 stream=streams road=roads dem=dem2 slope=slope impervious=impervious roof=roofs2 cellsize=5
less flow/test_synthetic_double3_surface.flow 
d.rast.edit input=dem2 output=dem3
r.mapcalc dem3='if(isnull(dem2), median(dem2), dem2)
'
g.list rast
r.info roofs
r.out.gdal roofs output=roofs.tif
r.what --v -f -n input=landcover@PERMANENT east_north=348722.994505,4351048.846154
r.what --v -f -n input=landcover@PERMANENT east_north=348729.148352,4351048.846154
r.colors map=raingardens@PERMANENT rules=/Users/miles/Dropbox/RHESSys/eclipse/rhessys-git/cf/test/data/GRASSData/synthetic_5m/PERMANENT/.tmp/kalisti.westell.com/73839.0
r.what --v -f -n input=raingardens@PERMANENT east_north=348729.148352,4351048.846154
r.what --v -f -n input=patch@PERMANENT east_north=348729.148352,4351048.846154
r.what --v -f -n input=patch@PERMANENT east_north=348840.357143,4350991.703297
r.what --v -f -n input=patch@PERMANENT east_north=348927.390110,4350982.912088
r.what --v -f -n input=patch@PERMANENT east_north=348927.829670,4350942.032967
r.what --v -f -n input=patch@PERMANENT east_north=348927.829670,4350947.307692
r.what --v -f -n input=patch@PERMANENT east_north=348929.587912,4350944.670330
r.what --v -f -n input=patch@PERMANENT east_north=348933.104396,4350948.186813
r.what --v -f -n input=patch@PERMANENT east_north=348795.521978,4350938.516484
r.what --v -f -n input=patch@PERMANENT east_north=348795.961538,4350942.912088
r.what --v -f -n input=patch@PERMANENT east_north=348792.884615,4350941.593407
r.what --v -f -n input=patch@PERMANENT east_north=348792.884615,4350932.362637
r.what --v -f -n input=patch@PERMANENT east_north=348797.280220,4350932.802198
r.colors map=raingardens@PERMANENT rules=/Users/miles/Dropbox/RHESSys/eclipse/rhessys-git/cf/test/data/GRASSData/synthetic_5m/PERMANENT/.tmp/maincampusmid-v882-03612.1xwireless.unc.edu/73839.0
r.what --v -f -n input=raingardens_tie@PERMANENT east_north=348928.269231,4350932.362637
r.what --v -f -n input=raingardens_tie@PERMANENT east_north=348927.390110,4350927.527473
r.what --v -f -n input=patch@PERMANENT east_north=348927.390110,4350942.912088
r.what --v -f -n input=patch@PERMANENT east_north=348931.785714,4350942.472527
r.what --v -f -n input=patch@PERMANENT east_north=348933.104396,4350941.593407
r.what --v -f -n input=patch@PERMANENT east_north=348927.390110,4350942.472527
r.what --v -f -n input=patch@PERMANENT east_north=348927.390110,4350937.197802
r.what --v -f -n input=patch@PERMANENT east_north=348927.829670,4350932.802198
r.what --v -f -n input=patch@PERMANENT east_north=348927.829670,4350937.197802
r.what --v -f -n input=patch@PERMANENT east_north=348926.950549,4350932.802198
r.what --v -f -n input=patch@PERMANENT east_north=348927.829670,4350927.527473
r.what --v -f -n input=raingardens@PERMANENT east_north=348927.829670,4350942.032967
r.what --v -f -n input=raingardens@PERMANENT east_north=348928.708791,4350942.472527
r.what --v -f -n input=raingardens@PERMANENT east_north=348927.390110,4350942.032967
r.what --v -f -n input=raingardens@PERMANENT east_north=348927.829670,4350942.032967
r.what --v -f -n input=raingardens@PERMANENT east_north=348927.829670,4350938.076923
r.what --v -f -n input=raingardens@PERMANENT east_north=348928.708791,4350936.758242
r.what --v -f -n input=raingardens@PERMANENT east_north=348928.708791,4350942.472527
r.what --v -f -n input=raingardens_tie@PERMANENT east_north=348928.708791,4350942.472527
r.what --v -f -n input=raingardens_tie@PERMANENT east_north=348929.587912,4350934.560440
r.what --v -f -n input=raingardens_tie@PERMANENT east_north=348926.950549,4350926.648352
r.what --v -f -n input=raingardens_tie@PERMANENT east_north=348926.950549,4350930.604396
r.what --v -f -n input=raingardens_tie@PERMANENT east_north=348927.390110,4350934.560440
r.what --v -f -n input=raingardens_tie@PERMANENT east_north=348928.269231,4350927.527473
r.what --v -f -n input=patch@PERMANENT east_north=348926.510989,4350942.472527
r.what --v -f -n input=patch@PERMANENT east_north=348926.950549,4350938.076923
r.what --v -f -n input=dem@PERMANENT east_north=348927.390110,4350942.912088
r.what --v -f -n input=dem@PERMANENT east_north=348926.950549,4350937.637363
r.what --v -f -n input=dem@PERMANENT east_north=348927.390110,4350932.802198
r.what --v -f -n input=dem@PERMANENT east_north=348927.390110,4350927.967033
r.what --v -f -n input=dem@PERMANENT east_north=348926.950549,4350933.241758
r.what --v -f -n input=dem@PERMANENT east_north=348926.510989,4350927.087912
r.what --v -f -n input=patch@PERMANENT east_north=348927.829670,4350927.527473
r.what --v -f -n input=patch@PERMANENT east_north=348927.778884,4350931.972112
r.what --v -f -n input=patch@PERMANENT east_north=348927.778884,4350931.972112
r.what --v -f -n input=patch@PERMANENT east_north=348928.256972,4350926.713147
r.what --v -f -n input=patch@PERMANENT east_north=348845.547809,4351056.274900
r.what --v -f -n input=patch@PERMANENT east_north=348844.591633,4351038.107570
r.what --v -f -n input=patch@PERMANENT east_north=348848.416335,4351021.852590
r.what --v -f -n input=patch@PERMANENT east_north=348848.416335,4351011.812749
r.what --v -f -n input=patch@PERMANENT east_north=348882.838645,4351011.812749
r.what --v -f -n input=patch@PERMANENT east_north=348889.053785,4351012.768924
r.what --v -f -n input=patch@PERMANENT east_north=348888.575697,4351007.988048
r.what --v -f -n input=patch@PERMANENT east_north=348887.619522,4351012.768924
r.what --v -f -n input=patch@PERMANENT east_north=348883.316733,4351012.290837
r.what --v -f -n input=patch@PERMANENT east_north=348886.663347,4351012.290837
r.what --v -f -n input=patch@PERMANENT east_north=348887.141434,4351007.509960
r.what --v -f -n input=patch@PERMANENT east_north=348887.619522,4351002.250996
r.what --v -f -n input=patch@PERMANENT east_north=348723.635458,4350950.617530
r.what --v -f -n input=patch@PERMANENT east_north=348721.723108,4350919.541833
r.what --v -f -n input=patch@PERMANENT east_north=348723.157371,4350922.888446
r.what --v -f -n input=patch@PERMANENT east_north=348723.157371,4350916.673307
r.what --v -f -n input=patch@PERMANENT east_north=348753.754980,4350916.673307
r.what --v -f -n input=dem@PERMANENT east_north=348753.276892,4350916.195219
r.what --v -f -n input=dem@PERMANENT east_north=348747.539841,4350916.195219
r.what --v -f -n input=dem@PERMANENT east_north=348761.882470,4350916.195219
r.what --v -f -n input=dem@PERMANENT east_north=348766.663347,4350916.195219
r.what --v -f -n input=dem@PERMANENT east_north=348988.974104,4350917.151394
r.what --v -f -n input=dem@PERMANENT east_north=348731.284861,4350912.848606
ls
cd /Users/miles/Dropbox/RHESSys/eclipse/rhessys-git/cf/test/data
ls
g.list rast
g.remove rast=dem,dem2,dem3
g.rename rast=roofs,roofs_outofrange
d.rast.edit input=roofs_outofrange output=roofs
g.list rast
r.in.gdal input=~/Dropbox/BES/imagery/DEM/DR5_DEM_5m_2m-burn-filled.tif output=dem
r.region --help
r.region map=dem raster=landcover
g.remove dem
r.in.gdal input=~/Dropbox/BES/imagery/DEM/DR5_DEM_5m_2m-burn-filled.tif output=dem
g.region --help
g.region rast=landcover
r.mask --help
r.mapcalc mask='!isnull(landcover)'
g.remove mask
ls
cd GRASSData
ls
v.in.ogr --help
v.in.ogr dsn=raingardens.shp output=raingardens
v.to.rast input=raingardens output=raingardens use=attr column=DN
ls
ls -l
cd ..
ls
ls -l
cd RHESSys-
ls
cd flow/
ls
cd ..
ls
../../../cf10.0b3 --help
../../../cf10.0b3 out=flow/synthetic_10.0b3_norg template=templates/synthetic_514 stream=streams road=roads dem=dem slope=slope cellsize=5 roof=roofs impervious=impervious
open flow/
../../../cf10.0b3 out=flow/synthetic_10.0b3 template=templates/synthetic_514 stream=streams road=roads dem=dem slope=slope cellsize=5
diff flow/synthetic_10.0b3.flow flow/synthetic_10.0b3_norg_subsurface.flow 
../../../cf10.0b3 out=flow/synthetic_10.0b3_rg template=templates/synthetic_514 stream=streams road=roads dem=dem slope=slope cellsize=5 roof=roofs impervious=impervious priority=raingardens
diff flow/synthetic_10.0b3_norg_surface.flow flow/synthetic_10.0b3_rg_surface.flow 
../../../cf10.0b3 out=flow/synthetic_10.0b3_rg_tiebreak template=templates/synthetic_514 stream=streams road=roads dem=dem slope=slope cellsize=5 roof=roofs impervious=impervious priority=raingardens
ls
cd flow
ls
diff synthetic_10.0b3_rg_surface.flow synthetic_10.0b3_rg_tiebreak_surface.flow 
cd ..
../../../cf10.0b3 out=flow/synthetic_10.0b3_rg_tiebreak template=templates/synthetic_514 stream=streams road=roads dem=dem slope=slope cellsize=5 roof=roofs impervious=impervious priority=raingardens
diff flow/synthetic_10.0b3_rg_surface.flow flow/synthetic_10.0b3_rg_tiebreak_surface.flow 
../../../cf10.0b3 out=flow/synthetic_10.0b3_rg_tiebreak-B template=templates/synthetic_514 stream=streams road=roads dem=dem slope=slope cellsize=5 roof=roofs impervious=impervious priority=raingardens
diff flow/synthetic_10.0b3_rg_surface.flow flow/synthetic_10.0b3_rg_tiebreak-B_surface.flow 
../../../cf10.0b3 out=flow/synthetic_10.0b3_rg_tiebreak-A template=templates/synthetic_514 stream=streams road=roads dem=dem slope=slope cellsize=5 roof=roofs impervious=impervious priority=raingardens
diff flow/synthetic_10.0b3_rg_surface.flow flow/synthetic_10.0b3_rg_tiebreak-A_surface.flow 
g.list rast
d.rast.edit input=raingardens output=raingardens_tie
g.list rast
g.remove raingardens_tie
d.rast.edit input=raingardens output=raingardens_tie
g.remove raingardens_tie
d.rast.edit input=raingardens output=raingardens_tie
d.rast.edit input=raingardens_tie output=raingardens_tie2
pushd .
cd ..
ls
cd GRASSData
ls
ogr2ogr raingardens.shp raingardens_tie.shp
ogr2ogr raingardens_tie.shp raingardens.shp
ls
g.list rast
g.remove rast=raingardens_tie,raingardens_tie2
v.in.ogr dsn=raingardens_tie.shp output=raingardens_tie
v.to.rast input=raingardens_tie output=raingardens_tie use=attr column=DN
popd
ls
../../../cf10.0b3 out=flow/synthetic_10.0b3_rg_tie template=templates/synthetic_514 stream=streams road=roads dem=dem slope=slope cellsize=5 roof=roofs impervious=impervious priority=raingardens_tie
../../../cf10.0b3 out=flow/synthetic_10.0b3_rg_tie_test template=templates/synthetic_514 stream=streams road=roads dem=dem slope=slope cellsize=5 roof=roofs impervious=impervious priority=raingardens_tie
../../../cf10.0b3 out=flow/synthetic_10.0b3_rg_tie_test template=templates/synthetic_514 stream=streams road=roads dem=dem slope=slope cellsize=5 roof=roofs impervious=impervious priority=raingardens_tie
../../../cf10.0b3 out=flow/synthetic_10.0b3_rg_tie_test template=templates/synthetic_514 stream=streams road=roads dem=dem slope=slope cellsize=5 roof=roofs impervious=impervious priority=raingardens_tie
../../../cf10.0b3 out=flow/synthetic_10.0b3_rg_tie_test template=templates/synthetic_514 stream=streams road=roads dem=dem slope=slope cellsize=5 roof=roofs impervious=impervious priority=raingardens_tie
ls
open flow
../../../cf10.0b3 out=flow/synthetic_10.0b3_rg_tiebreak template=templates/synthetic_514 stream=streams road=roads dem=dem slope=slope cellsize=5 roof=roofs impervious=impervious priority=raingardens_tie
../../../cf10.0b3 out=flow/synthetic_10.0b3_rg_tiebreak2 template=templates/synthetic_514 stream=streams road=roads dem=dem slope=slope cellsize=5 roof=roofs impervious=impervious priority=raingardens_tie
../../../cf10.0b3 out=flow/synthetic_10.0b3_rg_tiebreak2 template=templates/synthetic_514 stream=streams road=roads dem=dem slope=slope cellsize=5 roof=roofs impervious=impervious priority=raingardens_tie
../../../cf10.0b3 out=flow/synthetic_10.0b3_rg_tiebreak2 template=templates/synthetic_514 stream=streams road=roads dem=dem slope=slope cellsize=5 roof=roofs impervious=impervious priority=raingardens_tie
diff flow/synthetic_10.0b3_rg_tiebreak_surface.flow flow/synthetic_10.0b3_rg_tiebreak2_surface.flow
rm flow/synthetic_10.0b3_rg_tiebreak2_surface.flow
rm flow/synthetic_10.0b3_rg_tiebreak2_subsurface.flow 
../../../cf10.0b3 out=flow/synthetic_10.0b3_single template=templates/synthetic_514 stream=streams road=roads dem=dem slope=slope cellsize=5 
g.list rast
r.mapcalc mymask='landcover>0'
r.mask rast=mymask maskcats=1
r.mask input=mymask maskcats=1
ls
../../../cf10.0b3 out=flow/synthetic_10.0b3_single_mask template=templates/synthetic_514 stream=streams road=roads dem=dem slope=slope cellsize=5 
ls
cd ..
r.what --v -f -n input=dem@PERMANENT east_north=348920.796703,4350923.571429
r.what --v -f -n input=dem@PERMANENT east_north=348920.796703,4350930.164835
r.what --v -f -n input=dem@PERMANENT east_north=348920.796703,4350938.956044
r.what --v -f -n input=dem@PERMANENT east_north=348921.675824,4350958.296703
r.what --v -f -n input=dem@PERMANENT east_north=348922.554945,4350990.824176
r.what --v -f -n input=dem@PERMANENT east_north=348922.115385,4351017.637363
r.what --v -f -n input=dem@PERMANENT east_north=348922.115385,4351028.186813
r.what --v -f -n input=dem@PERMANENT east_north=348922.115385,4351046.648352
r.what --v -f -n input=dem@PERMANENT east_north=348920.357143,4351058.956044
r.what --v -f -n input=dem@PERMANENT east_north=348908.489011,4350865.549451
r.what --v -f -n input=streams@PERMANENT east_north=348827.170330,4350928.406593
r.what --v -f -n input=streams@PERMANENT east_north=348832.884615,4350920.934066
r.what --v -f -n input=streams@PERMANENT east_north=348841.236264,4350918.296703
r.what --v -f -n input=deten_roads@PERMANENT east_north=348902.774725,4351064.670330
r.what --v -f -n input=deten_roads@PERMANENT east_north=348810.906593,4351053.241758
r.what --v -f -n input=deten_imp@PERMANENT east_north=348857.060440,4351001.373626
r.what --v -f -n input=deten_imp@PERMANENT east_north=348906.730769,4351007.967033
r.what --v -f -n input=deten_imp@PERMANENT east_north=348858.818681,4351009.725275
r.what --v -f -n input=deten_imp@PERMANENT east_north=348801.236264,4351007.967033
r.what --v -f -n input=deten_imp@PERMANENT east_north=348767.829670,4351019.395604
r.what --v -f -n input=deten_imp@PERMANENT east_north=348755.082418,4351043.131868
r.what --v -f -n input=deten_imp@PERMANENT east_north=348880.796703,4351036.538462
r.what --v -f -n input=deten_imp@PERMANENT east_north=348929.587912,4351031.703297
r.what --v -f -n input=deten_imp@PERMANENT east_north=348958.598901,4351018.516484
r.what --v -f -n input=deten_lc@PERMANENT east_north=348770.027473,4351066.428571
r.what --v -f -n input=deten_lc@PERMANENT east_north=348814.423077,4351060.274725
r.what --v -f -n input=deten_lc@PERMANENT east_north=348834.642857,4351051.043956
r.what --v -f -n input=deten_lc@PERMANENT east_north=348906.291209,4350950.824176
cd ~/Dropbox/RetentionBasins/
ls
v.in.ogr dsn=ponds.shp output=ponds
v.in.ogr dsn=roofs.shp output=roofs
r.null streams_fixed setnull=0
g.list rast
v.to.rast --help
v.to.rast input=roads output=deten_roads use=attr column=dn
v.to.rast input=roofs output=deten_roofs use=attr column=dn
v.to.rast input=ponds output=deten_ponds use=attr column=dn
r.mapcalc 'deten_imp=if(!isnull(deten_roofs) && !isnull(deten_roads), 1, 0)'
r.mapcalc 'deten_imp=if(!isnull(deten_roofs) || !isnull(deten_roads), 1, 0)'
r.mapcalc 'deten_lc=if(isnull(deten_roofs) || isnull(deten_roads) || isnull(deten_ponds), 1, 0)'
r.mapcalc 'deten_lc=if(!isnull(deten_roofs), 3, if(!isnull(deten_roads), 2, if(!isnull(deten_ponds), 4, 1)))'
exit
rxit
exit
r.colors map=deten_ponds_2@PERMANENT color=random
r.what --v -f input=deten_ponds_2@PERMANENT east_north=348900.081578,4350952.257248
r.what --v -f input=deten_ponds_2@PERMANENT east_north=348900.081578,4350951.172592
r.what --v -f input=deten_ponds_2@PERMANENT east_north=348939.129197,4350931.648783
r.what --v -f input=deten_ponds_2@PERMANENT east_north=348926.113324,4351008.659365
r.what --v -f input=deten_ponds_2@PERMANENT east_north=348901.166234,4351006.490052
r.what --v -f input=deten_ponds_2@PERMANENT east_north=348897.912266,4351006.490052
r.what --v -f input=deten_lc@PERMANENT east_north=348897.912266,4351006.490052
r.what --v -f input=deten_lc@PERMANENT east_north=348900.081578,4350949.003280
r.what --v -f input=deten_lc@PERMANENT east_north=348900.081578,4350994.558836
r.what --v -f input=deten_lc@PERMANENT east_north=348918.520731,4351007.574709
r.what --v -f input=deten_lc@PERMANENT east_north=348939.129197,4350953.341904
r.what --v -f input=deten_lc@PERMANENT east_north=348874.049832,4350908.871005
r.what --v -f input=deten_lc@PERMANENT east_north=348903.335546,4350945.749312
r.what --v -f input=deten_lc@PERMANENT east_north=348902.250890,4350951.172592
r.what --v -f input=deten_lc_2@PERMANENT east_north=348902.250890,4350951.172592
r.what --v -f input=deten_lc_2@PERMANENT east_north=348947.806445,4351026.013862
r.what --v -f input=deten_lc_2@PERMANENT east_north=348945.637133,4351029.267830
r.what --v -f input=deten_lc_2@PERMANENT east_north=348925.028668,4350957.680529
r.what --v -f input=deten_lc_2@PERMANENT east_north=348901.166234,4350951.172592
r.what --v -f input=deten_lc_2@PERMANENT east_north=348919.605387,4351004.320740
r.what --v -f input=deten_lc_2@PERMANENT east_north=348897.912266,4351010.828677
r.what --v -f input=deten_lc_2@PERMANENT east_north=348965.160943,4350964.188465
r.what --v -f input=deten_lc_2@PERMANENT east_north=348944.552477,4351032.521798
r.what --v -f input=deten_lc_2@PERMANENT east_north=348921.774699,4351006.490052
r.what --v -f input=deten_lc_2@PERMANENT east_north=348921.774699,4351006.490052
r.what --v -f input=deten_lc_2@PERMANENT east_north=348898.996922,4351017.336613
r.what --v -f input=deten_lc_2@PERMANENT east_north=348898.996922,4351017.336613
r.what --v -f input=deten_lc_2@PERMANENT east_north=348900.081578,4350952.257248
r.what --v -f input=deten_lc_2@PERMANENT east_north=348944.552477,4351031.437142
r.what --v -f input=veg@PERMANENT east_north=348944.552477,4351031.437142
r.what --v -f input=veg@PERMANENT east_north=348964.076287,4351028.183174
r.what --v -f input=veg@PERMANENT east_north=348882.727081,4350962.019153
r.what --v -f input=veg@PERMANENT east_north=348893.573641,4350942.495344
r.what --v -f input=veg@PERMANENT east_north=348893.573641,4350942.495344
r.what --v -f input=veg@PERMANENT east_north=348902.250890,4350949.003280
r.what --v -f input=deten_lc_2@PERMANENT east_north=348902.250890,4350949.003280
r.what --v -f input=deten_lc_2@PERMANENT east_north=348945.637133,4350976.119682
r.what --v -f input=deten_lc_2@PERMANENT east_north=348945.637133,4350976.119682
r.what --v -f input=deten_lc_2@PERMANENT east_north=348867.541895,4350985.881587
r.what --v -f input=deten_lc_2@PERMANENT east_north=348906.589514,4350949.003280
r.what --v -f input=deten_lc_2@PERMANENT east_north=348898.996922,4350984.796931
r.what --v -f input=deten_lc_2@PERMANENT east_north=348808.970467,4350983.712275
r.what --v -f input=deten_lc_2@PERMANENT east_north=348887.065705,4351048.791640
r.what --v -f input=deten_lc_2@PERMANENT east_north=348947.806445,4351026.013862
r.what --v -f input=deten_lc_2@PERMANENT east_north=348896.827610,4350949.003280
r.what --v -f input=deten_veg@PERMANENT east_north=348898.996922,4351043.368359
r.what --v -f input=deten_veg@PERMANENT east_north=348974.922847,4350980.458306
r.what --v -f input=deten_veg@PERMANENT east_north=348945.637133,4351024.929206
r.what --v -f input=deten_veg@PERMANENT east_north=348945.637133,4351024.929206
r.water.outlet drainage=drain@PERMANENT basin=deten_basin easting=348992 northing=4350924
r.watershed elevation=dem@PERMANENT flow=deten_acc drainage=deten_drain basin=deten_basin.t600 stream=deten_str.t600 half.basin=deten_hill.t600 threshold=600
r.watershed elevation=dem@PERMANENT drainage=deten_drain basin=deten_basin.t600 stream=deten_str.t600 half.basin=deten_hill.t600 threshold=600
r.slope.aspect elevation=dem@PERMANENT slope=deten_slope aspect=deten_aspect
r.what --v -f input=deten_imp@PERMANENT east_north=348901.824324,4351027.432432
r.what --v -f input=deten_imp@PERMANENT east_north=348923.445946,4350966.351351
r.what --v -f input=deten_ponds_2_zero@PERMANENT east_north=348872.635135,4351023.648649
r.what --v -f input=deten_ponds_2_zero@PERMANENT east_north=348903.986486,4350950.675676
r.what --v -f input=deten_ponds_2_zero@PERMANENT east_north=348920.743243,4350976.081081
r.what --v -f input=deten_ponds_2_zero@PERMANENT east_north=348900.202703,4350950.135135
r.what --v -f input=deten_ponds_2_zero@PERMANENT east_north=348875.878378,4351020.405405
r.what --v -f input=deten_ponds_2@PERMANENT east_north=348873.716216,4351020.945946
r.what --v -f input=deten_ponds_2@PERMANENT east_north=348904.527027,4350954.459459
r.what --v -f input=deten_patch@PERMANENT east_north=348872.635135,4351022.027027
r.what --v -f input=deten_patch@PERMANENT east_north=348905.608108,4350950.135135
r.what --v -f input=deten_patch@PERMANENT east_north=348831.554054,4350956.621622
r.what --v -f input=deten_patch@PERMANENT east_north=348918.040541,4351057.702703
r.what --v -f input=deten_patch@PERMANENT east_north=348822.905405,4351036.081081
r.colors map=deten_patch@PERMANENT color=random
cf10.0b2 cellsize=5 output=pondtest stream=deten_str.t600 road=deten_imp impervious=deten_imp dem=dem slope=deten_slope template=template.pondtest roof=deten_roofs
r.what --v -f input=deten_roads_2@PERMANENT east_north=348853.453785,4351055.593050
r.slope.aspect --overwrite elevation=dem slope=deten_slope aspect=deten_aspect
r.slope.aspect --overwrite elevation=dem@PERMANENT slope=deten_slope aspect=deten_aspect
cf10.0b2 cellsize=5 output=pondtest stream=deten_str.t600 road=deten_imp impervious=deten_imp dem=deten_dem slope=deten_slope template=template.pondtest roof=deten_roofs
r.what --v -f input=deten_str.t600@PERMANENT east_north=348817.251574,4350937.750788
r.what --v -f input=deten_str.t600@PERMANENT east_north=348817.251574,4350937.750788
r.what --v -f input=deten_str.t600@PERMANENT east_north=348817.251574,4350937.750788
r.what --v -f input=deten_str.t600@PERMANENT east_north=348817.963200,4350937.157767
r.what --v -f input=deten_str.t600@PERMANENT east_north=348817.192272,4350938.729274
r.what --v -f input=deten_str.t600@PERMANENT east_north=348817.251574,4350938.729274
r.what --v -f input=deten_str.t600@PERMANENT east_north=348817.933549,4350938.047299
r.what --v -f input=deten_str.t600@PERMANENT east_north=348817.874247,4350938.225205
r.what --v -f input=deten_str.t600@PERMANENT east_north=348817.874247,4350938.225205
r.what --v -f input=deten_str.t600@PERMANENT east_north=348817.488783,4350937.157767
r.what --v -f input=deten_str.t600@PERMANENT east_north=348817.577736,4350937.780439
r.what --v -f input=deten_patch@PERMANENT east_north=348817.577736,4350937.780439
r.what --v -f input=deten_patch@PERMANENT east_north=348817.696340,4350937.424626
r.what --v -f input=deten_roads_2@PERMANENT east_north=348902.008357,4350993.748172
r.what --v -f input=deten_roads_2@PERMANENT east_north=348900.582283,4350997.075678
r.what --v -f input=deten_roads_2@PERMANENT east_north=348897.492457,4351040.808606
r.what --v -f input=deten_roads_2@PERMANENT east_north=348874.912956,4351019.655179
r.what --v -f input=deten_patch@PERMANENT east_north=348874.912956,4351019.655179
r.what --v -f input=deten_patch@PERMANENT east_north=348877.289745,4351020.843574
r.what --v -f input=deten_patch@PERMANENT east_north=348901.057641,4350950.252922
r.what --v -f input=deten_patch@PERMANENT east_north=348903.196752,4350949.302207
r.what --v -f input=deten_patch@PERMANENT east_north=348900.582283,4350949.302207
r.what --v -f input=deten_patch@PERMANENT east_north=348888.460656,4350953.580428
r.what --v -f input=deten_patch@PERMANENT east_north=348816.959459,4350937.162162
r.what --v -f input=deten_patch@PERMANENT east_north=348822.364865,4350937.702703
r.what --v -f input=deten_patch@PERMANENT east_north=348826.148649,4350934.459459
r.what --v -f input=deten_patch@PERMANENT east_north=348826.689189,4350930.675676
r.what --v -f input=deten_patch@PERMANENT east_north=348826.689189,4350926.891892
r.what --v -f input=deten_patch@PERMANENT east_north=348826.689189,4350922.027027
r.what --v -f input=deten_patch@PERMANENT east_north=348832.094595,4350917.702703
r.what --v -f input=deten_patch@PERMANENT east_north=348827.229730,4350920.945946
r.what --v -f input=deten_patch@PERMANENT east_north=348891.013514,4350922.567568
r.what --v -f input=deten_patch@PERMANENT east_north=348897.500000,4350922.567568
r.what --v -f input=deten_patch@PERMANENT east_north=348906.148649,4350922.567568
r.what --v -f input=deten_patch@PERMANENT east_north=348931.554054,4350922.567568
r.what --v -f input=deten_patch@PERMANENT east_north=348963.986486,4350922.027027
r.what --v -f input=deten_patch@PERMANENT east_north=348992.635135,4350922.027027
r.what --v -f input=deten_roofs@PERMANENT east_north=348992.635135,4350922.027027
r.what --v -f input=deten_roofs@PERMANENT east_north=348887.770270,4351040.945946
r.what --v -f input=deten_patch@PERMANENT east_north=348887.770270,4351040.945946
r.what --v -f input=deten_patch@PERMANENT east_north=348827.229730,4351047.432432
r.what --v -f input=deten_patch@PERMANENT east_north=348827.229730,4351047.432432
r.what --v -f input=deten_patch@PERMANENT east_north=348827.229730,4351053.378378
r.what --v -f input=deten_patch@PERMANENT east_north=348827.229730,4351050.675676
r.what --v -f input=deten_patch@PERMANENT east_north=348827.229730,4351047.432432
r.what --v -f input=deten_patch@PERMANENT east_north=348918.040541,4351002.567568
r.what --v -f input=deten_patch@PERMANENT east_north=348916.959459,4351003.108108
r.what --v -f input=deten_patch@PERMANENT east_north=348916.959459,4351003.108108
r.what --v -f input=deten_patch@PERMANENT east_north=348902.364865,4351012.837838
r.what --v -f input=deten_patch@PERMANENT east_north=348902.364865,4351010.675676
r.what --v -f input=deten_patch@PERMANENT east_north=348902.364865,4351006.351351
r.what --v -f input=deten_patch@PERMANENT east_north=348901.824324,4350997.162162
r.what --v -f input=deten_patch@PERMANENT east_north=348901.283784,4350992.297297
r.what --v -f input=deten_patch@PERMANENT east_north=348901.824324,4350996.081081
r.what --v -f input=deten_patch@PERMANENT east_north=348903.445946,4350998.243243
r.what --v -f input=deten_patch@PERMANENT east_north=348906.689189,4350998.243243
r.what --v -f input=deten_patch@PERMANENT east_north=348910.472973,4350998.243243
r.what --v -f input=deten_patch@PERMANENT east_north=348911.013514,4350998.243243
r.what --v -f input=deten_patch@PERMANENT east_north=348911.013514,4350998.243243
r.what --v -f input=deten_patch@PERMANENT east_north=348911.013514,4350998.243243
cf10.0b2 cellsize=5 output=ponttest stream=deten_str.t600 road=deten_roads_1 impervious=deten_roads_1 dem=deten_dem slope=deten_slope template=template.pondtest roof=deten_roofs
cf10.0b2 cellsize=5 output=pondtest2 stream=deten_str.t600 road=deten_raods_1 impervious=deten_imp dem=deten_dem slope=deten_slope template=template.pondtest roof=deten_roofs
cf10.0b2 cellsize=5 output=pondtest2 stream=deten_str.t600 road=deten_roads_1 impervious=deten_imp dem=deten_dem slope=deten_slope template=template.pondtest roof=deten_roofs
r.what --v -f input=deten_patch@PERMANENT east_north=348911.013514,4350998.243243
r.what --v -f input=deten_patch@PERMANENT east_north=348911.013514,4350998.243243
r.what --v -f input=deten_patch@PERMANENT east_north=348917.500000,4351002.567568
r.what --v -f input=deten_patch@PERMANENT east_north=348917.500000,4351002.027027
r.what --v -f input=deten_patch@PERMANENT east_north=348902.905405,4351003.108108
r.what --v -f input=deten_patch@PERMANENT east_north=348902.905405,4350999.324324
r.what --v -f input=deten_patch@PERMANENT east_north=348899.121622,4350987.972973
r.what --v -f input=deten_patch@PERMANENT east_north=348909.391892,4350995.540541
r.what --v -f input=deten_patch@PERMANENT east_north=348912.094595,4350995.000000
r.what --v -f input=deten_patch@PERMANENT east_north=348911.013514,4350995.000000
r.what --v -f input=deten_patch@PERMANENT east_north=348909.932432,4350995.540541
r.what --v -f input=deten_patch@PERMANENT east_north=348909.932432,4350993.378378
r.what --v -f input=deten_patch@PERMANENT east_north=348909.932432,4350996.081081
r.what --v -f input=deten_patch@PERMANENT east_north=348909.932432,4350999.864865
r.what --v -f input=deten_patch@PERMANENT east_north=348916.418919,4351002.027027
r.what --v -f input=deten_patch@PERMANENT east_north=348918.581081,4351001.486486
r.what --v -f input=deten_patch@PERMANENT east_north=348921.824324,4351001.486486
r.what --v -f input=deten_patch@PERMANENT east_north=348925.067568,4351001.486486
r.what --v -f input=deten_patch@PERMANENT east_north=348927.770270,4351001.486486
r.what --v -f input=deten_patch@PERMANENT east_north=348927.770270,4350998.243243
r.what --v -f input=deten_patch@PERMANENT east_north=348924.527027,4350998.243243
r.what --v -f input=deten_patch@PERMANENT east_north=348920.743243,4350997.702703
r.what --v -f input=deten_patch@PERMANENT east_north=348917.500000,4350997.702703
r.what --v -f input=deten_patch@PERMANENT east_north=348769.391892,4351038.783784
r.what --v -f input=deten_patch@PERMANENT east_north=348851.554054,4351022.567568
r.what --v -f input=deten_patch@PERMANENT east_north=348833.716216,4351056.621622
r.what --v -f input=deten_patch@PERMANENT east_north=348810.472973,4351078.783784
r.what --v -f input=deten_patch@PERMANENT east_north=348821.283784,4351078.243243
r.what --v -f input=deten_patch@PERMANENT east_north=348821.283784,4351073.378378
r.what --v -f input=deten_patch@PERMANENT east_north=348845.608108,4351073.378378
r.what --v -f input=deten_patch@PERMANENT east_north=348869.391892,4351072.297297
r.what --v -f input=deten_patch@PERMANENT east_north=348795.337838,4351065.270270
r.what --v -f input=deten_patch@PERMANENT east_north=348814.797297,4351071.756757
r.what --v -f input=deten_patch@PERMANENT east_north=348806.689189,4351072.297297
r.what --v -f input=deten_patch@PERMANENT east_north=348815.878378,4351060.405405
r.what --v -f input=deten_patch@PERMANENT east_north=348808.851351,4351061.486486
r.what --v -f input=deten_patch@PERMANENT east_north=348871.013514,4351065.810811
r.what --v -f input=deten_patch@PERMANENT east_north=348863.445946,4351065.270270
r.what --v -f input=deten_patch@PERMANENT east_north=348855.878378,4351066.891892
r.what --v -f input=deten_patch@PERMANENT east_north=348853.175676,4351067.972973
r.what --v -f input=deten_patch@PERMANENT east_north=348847.229730,4351067.972973
r.what --v -f input=deten_patch@PERMANENT east_north=348842.905405,4351067.972973
r.what --v -f input=deten_patch@PERMANENT east_north=348838.581081,4351067.972973
r.what --v -f input=deten_patch@PERMANENT east_north=348834.256757,4351067.972973
r.what --v -f input=deten_patch@PERMANENT east_north=348828.310811,4351067.972973
r.what --v -f input=deten_patch@PERMANENT east_north=348822.905405,4351067.432432
r.what --v -f input=deten_patch@PERMANENT east_north=348819.121622,4351067.432432
r.what --v -f input=deten_patch@PERMANENT east_north=348808.851351,4351067.432432
r.what --v -f input=deten_patch@PERMANENT east_north=348803.986486,4351067.432432
r.what --v -f input=deten_patch@PERMANENT east_north=348800.202703,4351067.432432
r.what --v -f input=deten_patch@PERMANENT east_north=348793.716216,4351067.432432
r.what --v -f input=deten_patch@PERMANENT east_north=348791.013514,4351067.972973
r.what --v -f input=deten_patch@PERMANENT east_north=348791.013514,4351071.756757
r.what --v -f input=deten_patch@PERMANENT east_north=348793.175676,4351066.351351
r.what --v -f input=deten_patch@PERMANENT east_north=348789.391892,4351065.810811
r.what --v -f input=deten_patch@PERMANENT east_north=348788.851351,4351061.486486
r.what --v -f input=deten_patch@PERMANENT east_north=348788.851351,4351050.675676
r.what --v -f input=deten_patch@PERMANENT east_north=348795.878378,4351065.270270
r.what --v -f input=deten_patch@PERMANENT east_north=348791.554054,4351066.891892
r.what --v -f input=deten_patch@PERMANENT east_north=348806.689189,4351077.702703
r.what --v -f input=deten_patch@PERMANENT east_north=348800.202703,4351078.783784
r.what --v -f input=deten_patch@PERMANENT east_north=348801.283784,4351077.702703
r.what --v -f input=deten_patch@PERMANENT east_north=348802.905405,4351074.459459
r.what --v -f input=deten_patch@PERMANENT east_north=348805.067568,4351074.459459
r.what --v -f input=deten_patch@PERMANENT east_north=348806.148649,4351074.459459
r.what --v -f input=deten_patch@PERMANENT east_north=348809.932432,4351073.918919
r.what --v -f input=deten_patch@PERMANENT east_north=348815.337838,4351073.378378
r.what --v -f input=deten_patch@PERMANENT east_north=348814.256757,4351078.783784
r.what --v -f input=deten_patch@PERMANENT east_north=348814.797297,4351065.810811
r.what --v -f input=deten_patch@PERMANENT east_north=348807.770270,4351050.675676
r.what --v -f input=deten_patch@PERMANENT east_north=348806.148649,4351050.135135
r.what --v -f input=deten_patch@PERMANENT east_north=348805.608108,4351071.756757
r.what --v -f input=deten_patch@PERMANENT east_north=348799.121622,4351079.324324
r.what --v -f input=deten_patch@PERMANENT east_north=348804.527027,4351079.864865
r.what --v -f input=deten_patch@PERMANENT east_north=348819.662162,4351072.837838
r.what --v -f input=deten_patch@PERMANENT east_north=348822.364865,4351072.837838
r.what --v -f input=deten_patch@PERMANENT east_north=348822.364865,4351071.216216
r.what --v -f input=deten_patch@PERMANENT east_north=348822.364865,4351065.810811
r.what --v -f input=deten_patch@PERMANENT east_north=348822.364865,4351064.729730
r.what --v -f input=deten_patch@PERMANENT east_north=348816.418919,4351065.810811
r.what --v -f input=deten_patch@PERMANENT east_north=348797.500000,4351069.054054
r.what --v -f input=deten_patch@PERMANENT east_north=348802.364865,4351073.918919
r.what --v -f input=deten_patch@PERMANENT east_north=348802.364865,4351071.216216
r.what --v -f input=deten_patch@PERMANENT east_north=348802.364865,4351068.513514
r.what --v -f input=deten_patch@PERMANENT east_north=348802.364865,4351069.054054
r.what --v -f input=deten_patch@PERMANENT east_north=348806.148649,4351069.594595
r.what --v -f input=deten_patch@PERMANENT east_north=348806.689189,4351075.540541
r.what --v -f input=deten_patch@PERMANENT east_north=348807.229730,4351076.081081
r.what --v -f input=deten_patch@PERMANENT east_north=348802.364865,4351071.216216
r.what --v -f input=deten_patch@PERMANENT east_north=348801.283784,4351069.054054
r.what --v -f input=deten_patch@PERMANENT east_north=348801.283784,4351069.054054
r.what --v -f input=deten_patch@PERMANENT east_north=348806.148649,4351077.702703
r.what --v -f input=deten_lc_2@PERMANENT east_north=348816.959459,4350937.702703
r.what --v -f input=deten_lc_2@PERMANENT east_north=348816.959459,4350937.702703
r.what --v -f input=deten_hill.t600@PERMANENT east_north=348816.959459,4350937.702703
r.what --v -f input=deten_hill.t600@PERMANENT east_north=348816.959459,4350937.702703
r.what --v -f input=deten_hill.t600@PERMANENT east_north=348817.500000,4350937.702703
r.what --v -f input=deten_hill.t600@PERMANENT east_north=348892.635135,4350923.108108
r.what --v -f input=deten_hill.t600@PERMANENT east_north=348878.581081,4350917.702703
r.what --v -f input=deten_hill.t600@PERMANENT east_north=348892.635135,4350923.648649
r.what --v -f input=deten_hill.t600@PERMANENT east_north=348828.310811,4350921.486486
r.what --v -f input=deten_hill.t600@PERMANENT east_north=348827.229730,4350926.891892
r.what --v -f input=deten_hill.t600@PERMANENT east_north=348827.229730,4350932.837838
r.what --v -f input=deten_hill.t600@PERMANENT east_north=348827.770270,4350933.378378
r.what --v -f input=deten_hill.t600@PERMANENT east_north=348821.824324,4350937.702703
r.what --v -f input=deten_hill.t600@PERMANENT east_north=348818.040541,4350938.243243
r.what --v -f input=deten_str.t600@PERMANENT east_north=348818.040541,4350938.243243
r.what --v -f input=deten_str.t600@PERMANENT east_north=348907.770270,4350922.027027
r.what --v -f input=deten_patch@PERMANENT east_north=348907.770270,4350922.027027
r.what --v -f input=deten_patch@PERMANENT east_north=348907.229730,4350922.027027
r.what --v -f input=deten_patch@PERMANENT east_north=348906.148649,4350922.027027
r.what --v -f input=deten_patch@PERMANENT east_north=348904.527027,4350922.027027
r.what --v -f input=deten_patch@PERMANENT east_north=348903.445946,4350922.027027
r.what --v -f input=deten_patch@PERMANENT east_north=348903.445946,4350922.027027
r.what --v -f input=deten_patch@PERMANENT east_north=348903.445946,4350922.027027
r.what --v -f input=deten_hill.t600@PERMANENT east_north=348903.445946,4350922.027027
r.what --v -f input=deten_hill.t600@PERMANENT east_north=348914.256757,4350922.027027
r.what --v -f input=deten_hill.t600@PERMANENT east_north=348919.662162,4350922.567568
r.what --v -f input=deten_hill.t600@PERMANENT east_north=348904.527027,4350923.648649
r.what --v -f input=deten_hill.t600@PERMANENT east_north=348901.283784,4350923.108108
r.what --v -f input=deten_hill.t600@PERMANENT east_north=348905.608108,4350923.108108
r.what --v -f input=deten_hill.t600@PERMANENT east_north=348905.608108,4350923.108108
g.list rast
r.info patch
r.mapcalc "deten_patch_1 = patch + 2"
r.mapcalc "deten_lc_2 = if(deten_ponds_2==2,4,deten_lc)"
r.mapcalc "deten_lc_2 = if(isnull(deten_lc),if(deten_ponds_2==2,4),deten_lc)"
r.mapcalc "deten_lc_2 = deten_lc + deten_ponds_2 - deten_ponds"
r.mapcalc "deten_lc_2 = deten_lc + deten_ponds_2"
r.info(deten_lc_2)
r.info deten_lc_@
r.info deten_lc_2
r.inof detens_ponds_2
r.info detens_ponds_2
r.info deten_ponds_2
r.mapcalc "deten_lc_2 = basin - basin + deten_lc + deten_ponds_2 - deten_ponds"
r.info deten_lc_2
r.mapcalc "deten_lc_2 = if(isnull(deten_lc_2),0,deten_lc_2)"
r.mapcalc "deten_lc_2 = deten_lc + deten_lc_2"
r.mapcalc "deten_veg = if(deten_lc == 1,  1, if(deten_lc==4,2,3),3)"
r.mapcalc "deten_veg = if(deten_lc_2 == 1,  1, if(deten_lc_2==4,2,3),3)"
cd pondtest/
cd worldfiles/
vi template.pontest 
r.info dem
r.info deten_lc
g.remove deten_basin
r.info deten_basin.t600
r.mapcalc "deten_basin.t600 = deten_basin.t600 -1"
r.info deten_basin.t600
g.copy deten_basin.t600,MASK
g.remove MASK
g.copy deten_basin.t600,MASK
vi template.pontest 
g.remove MASK
r.horizon -d elevin=dem direction=0 horizon=east
r.horizon -d elevin=dem direction=0 horizon=west
r.mapcalc 'deten_east_horizon = sin(east_0)"
'
r.mapcalc "deten_east_horizon = sin(east_0)"
r.mapcalc "deten_west_horizon = sin(west_0)"
r.mapcalc "deten.ehr.100 = deten_east_horizon*100"
r.mapcalc "deten.whr.100 = deten_west_horizon*100"
r.mapcalc "deten_imperv = if(deten_imp==1,0,1)"
r.mapalc "deten_soil =1"
r.mapcalc "deten_soil =1"
r.info deten_patch
r.info detens_patch
r.mapcalc "detne_patch = patch + deten_ponds_2"
r.mapcalc "deten_ponds_2_zero = if(isnull(deten_ponds_2),0,deten_ponds_2)"
r.mapcalc "deten_ponds_2 = if(deten_ponds_2_zero==0,null(),deten_ponds_2_zero)"
g.remove dtene_patch
g.remove detne_patch
r.info detne_patch_1
r.info deten_patch_1
r.mapcalc "deten_patch = if(deten_ponds_2_zero==0,deten_patch_1,deten_ponds_2_zero)"
g.copy deten_basin.t600,MASK
cd ../../
ls
cd pondtest/
ls
cd GRASStest/
ls
r.statistics -h
r.statistics base=deten_roads cover=deten_patch method=distribution > road.pnos
ls
more road.pnos
g.remove MASK
g.copy deten_roads,deten_roads_2
r.statistics base=deten_roads_2 cover=deten_patch method=distribution > road.pnos
cd ../worldfiles/
ls
cf10.0b2
mv template.pontest template.pondtest
cf10.0b2
r.region -p
g.region -p
cf10.0b2
r.copy deten_basin.t600,MASK
g.copy deten_basin.t600,MASK
cf10.0b2
ls
mv pondtest_su* ../flowtables/
ls
cd ../flowtables/
mkdir unedited
cp pondtest_su* unedited/
ls
cd ../
cd worldfiles/
ls
g2w -t template.pondtest -w w.pondtest
vi template.pondtest 
g2w -t template.pondtest -w w.pondtest
g2w -t template.pondtest -w w.pondtest
ls
g2w -t template.pondtest -w w.pondtest
vi template.pondtest 
g.remove MASK
r.slope.aspect
r.horizon -d elevin=dem direction=0 horizon=east
r.horizon -d elevin=dem direction=0 horizon=west
r.mapcalc "deten_east_horizon = sin(east_0)"
r.mapcalc "deten_west_horizon = sin(west_0)"
r.mapcalc "deten.ehr.100 = deten_east_horizon*100"
r.mapcalc "deten.whr.100 = deten_west_horizon*100"
r.mapcalc "tb=if(detention_basin.t600>0,1,null())"
r.mapcalc "tb=if(deten_basin.t600>0,1,null())"
g.region -p
g.region -s region=smallpond
g.region -s
g.region rast=dem
r.slope.aspect
 r.horizon -d elevin=dem direction=0 horizon=east
r.horizon -d elevin=dem direction=0 horizon=west
r.mapcalc "deten_east_horizon = sin(east_0)"
r.mapcalc "deten_west_horizon = sin(west_0)"
r.mapcalc "deten.ehr.100 = deten_east_horizon*100"
r.mapcalc "deten.whr.100 = deten_west_horizon*100"
g.region -d
g.region -p
g.copy dentention_basin.t600,MASK
g.copy denten_basin.t600,MASK
g.copy deten_basin.t600,MASK
g2w -t template.pondtest -w w.pondtest 
r.mapcalc "deten_slope = deten_slope"
r.mapcalc "deten_aspect = deten_aspect"
r.mapcalc "deten_e.hr.100 = deten_e.hr.100"
r.mapcalc "deten_ehr.100 = deten_ehr.100"
r.mapcalc "deten.ehr.100 = deten.ehr.100"
r.mapcalc "deten.whr.100 = deten.whr.100"
g2w -t template.pondtest -w w.pondtest
r.mapcalc "deten_dem = dem"
vi template.pondtest 
g2w -t template.pondtest -w w.pondtest
vi template.pondtest 
g2w -t template.pondtest -w w.pondtest
vi template.pondtest 
vi template.pondtest 
g2w -t template.pondtest -w w.pondtest
vi template.pondtest 
cp tb,tb.1
vi template.pondtest 
g2w -t template.pondtest -w w.pondtest
vi template.pondtest 
g2w -t template.pondtest -w w.pondtest
vi template.pondtest 
g2w -t template.pondtest -w w.pondtest
g.remove MASK
v template.pondtest 
vi template.pondtest 
g2w -t template.pondtest -w w.pondtest
g.copy tb,MASK
g2w -t template.pondtest -w w.pondtest
r.info deten_patch
r.info deten_dem
r.info deten_hill.t600
vi template.pondtest 
r.info tb
g.remove tb.1
g.list rast
r.info roofs
r.info deten_roofs
g2w -t template.pondtest -w w.pondtest
cf10.0b2
cd ../GRASStest/
r.statistics -h
r.statistics base=deten_roads_2 cover=deten_patch method=distribution > patchnosforroads
cd ../worldfiles/
cf10.0b2 
r.mapcalc "deten_roads_1 = if(isnull(deten_roads),0,1)"
g.remove mask
r.mapcalc "deten_roads_1 = if(isnull(deten_roads),0,1)"
g.copy tb,MASK
cf10.0b2
cf10.0b2
exit
