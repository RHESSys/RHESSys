#!/bin/bash

sqlite3 params.sqlite < createParamDB.sql

# Insert params for the veg base classes
insertParams.py -v --type="stratum" --name="conifer"   --parentName="None" --user="Peter Slaughter" defs/stratum/veg_conifer.def   
insertParams.py -v --type="stratum" --name="deciduous" --parentName="None" --user="Peter Slaughter" defs/stratum/veg_deciduous.def 
insertParams.py -v --type="stratum" --name="grass"     --parentName="None" --user="Peter Slaughter" defs/stratum/veg_grass.def     
insertParams.py -v --type="stratum" --name="nonveg"    --parentName="None" --user="Peter Slaughter" defs/stratum/veg_nonveg.def

# Insert params for the soil base classes
insertParams.py -v --type="soil" --name="clay"            --parentName="None" --user="Peter Slaughter" defs/soil/soil_clay.def
insertParams.py -v --type="soil" --name="clay-loam"       --parentName="None" --user="Peter Slaughter" defs/soil/soil_clayloam.def
insertParams.py -v --type="soil" --name="loamy-sand"      --parentName="None" --user="Peter Slaughter" defs/soil/soil_loamysand.def
insertParams.py -v --type="soil" --name="sandy-clay"      --parentName="None" --user="Peter Slaughter" defs/soil/soil_sandyclay.def
insertParams.py -v --type="soil" --name="sandy-clay-loam" --parentName="None" --user="Peter Slaughter" defs/soil/soil_sandyclayloam.def
insertParams.py -v --type="soil" --name="sandy-loam"      --parentName="None" --user="Peter Slaughter" defs/soil/soil_sandyloam.def
insertParams.py -v --type="soil" --name="silty-clay"      --parentName="None" --user="Peter Slaughter" defs/soil/soil_siltyclay.def
insertParams.py -v --type="soil" --name="silty-clay-loam" --parentName="None" --user="Peter Slaughter" defs/soil/soil_siltyclayloam.def
insertParams.py -v --type="soil" --name="silt-loam"       --parentName="None" --user="Peter Slaughter" defs/soil/soil_siltyloam.def

#insertParams.py -v --type="soil" --name=""        --parentName="None" --user="Peter Slaughter" defs/soil/soil_gravellyloam.def
#insertParams.py -v --type="soil" --name="" --parentName="None" --user="Peter Slaughter" defs/soil/soil_water.def
#insertParams.py -v --type="soil" --name="" --parentName="None" --user="Peter Slaughter" defs/soil/soil_patch.def
#insertParams.py -v --type="soil" --name="" --parentName="None" --user="Peter Slaughter" defs/soil/soil_rock.def
