g.region -p
r.what --v -f -n input=patch_5m@taehee east_north=349325.089515,4350341.816966
ls
ls
cd Dropbox/
ls
cd BES-DeadRun/
ls
mkdir RHESSys
open .
cd RHESSys
ls
mkdir templates
mkdir flow
mkdir worldfiles
mkdir output
mkdir tecfiles
mkdir defs
mkdir clim
ls
mkdir src
ls
ls
cd src
ls
cd ..
ls
ls -l `which g2w`
sudo rm /usr/local/bin/g2w
ls -l /usr/local/bin
sudo rm /usr/local/bin/rat
ls
cd src
ls
cd g2w/
ls
make clean clobber
make 
sudo make install
ls
cd ..
cd ..
ls
open worldfiles/
diff ../RHESSys_Mar2013_taehee/templates/template514_dr5_5m ../RHESSys_Mar2013_taehee/templates/template514_dr5_5m_roof
ls
g2w -t templates/template514_dr5_5m -w worldfiles/world5m_dr5
ls
ls -l worldfiles/
rm worldfiles/world5m_dr5 
g2w -t templates/template514_dr5_5m -w worldfiles/world5m_dr5
g.list rast
cf9.1 output=flow/world5m_dr5.flow template=templates/template514_dr5_5m stream=streams roads=roads dem=dem slope=slope cellsize=5
cf9.1 output=flow/world5m_dr5.flow template=templates/template514_dr5_5m stream=streams road=roads dem=dem slope=slope cellsize=5
cf9.1 output=flow/world5m_dr5_fillDEM.flow template=templates/template514_dr5_5m stream=streams road=roads dem=demf slope=slope cellsize=5
ls
rm flow/world5m_dr5_fillDEM.flow.flow 
ls
ls
g.list rast
g.remove dem_org
cd /Users/miles/Dropbox/RHESSys/eclipse/rhessys-git/cf/test/data/cfRHESSys 
ls
cf9.1 output=flow/DR5_5m_cf91 template=templates/dr5_514_5m stream=streams road=roads dem=dem slope=slope cellsize=5
cf10.0a1 output=flow/DR5_5m_cf10a1 template=templates/dr5_514_5m stream=streams road=roads dem=dem slope=slope cellsize=5
ls
diff flow/DR5_5m_cf91.flow flow/DR5_5m_cf10a1.flow 
g.list rast
g.list rast
exit
