
use one of the MODIS images to set the region in grass
output the following grass maps - (use arc ascii format)
	- hill, patch, zone
	- vegid

(use r.mode to make sure vegid is already the mode of the vegid per patch;
otherwise program will just take the last id it finds when making the patch)

create ann allometric table
first row should be number of entries, then
each entry should have the following columns for each vegtype
id sla  leaf:root  leaf:stem  stem:coarseroot livestem:deadstem cn_leaf
cn_froot cn_livewood cn_deadwood 


have an existing worldfile
- delete any spaces at the top of the file or it will not read header
  correctly

cal lairead -pre (prefix of images) -a (if arc format) -old oldworldfile
-redef newworldfile -allom (allometric ratio file)


	
