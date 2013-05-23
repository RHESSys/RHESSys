#!/bin/bash

sqlite3 params.sqlite < createParamDB.sql

# Insert params for the veg base classes
./insertParams.py -v --type="stratum" --name="evergreen"   --parentName="None" --user="RHESSys" defs/stratum/veg_evergreen.def   
./insertParams.py -v --type="stratum" --name="deciduous" --parentName="None" --user="RHESSys" defs/stratum/veg_deciduous.def 
./insertParams.py -v --type="stratum" --name="grass"     --parentName="None" --user="RHESSys" defs/stratum/veg_grass.def     
./insertParams.py -v --type="stratum" --name="nonveg"    --parentName="None" --user="RHESSys" defs/stratum/veg_nonveg.def

# Insert params for the soil base classes
./insertParams.py -v --type="soil" --name="clay"            --parentName="None" --user="RHESSys" defs/soil/soil_clay.def
./insertParams.py -v --type="soil" --name="clayloam"       --parentName="None" --user="RHESSys" defs/soil/soil_clayloam.def
./insertParams.py -v --type="soil" --name="loam"       --parentName="None" --user="RHESSys" defs/soil/soil_loam.def
./insertParams.py -v --type="soil" --name="loamysand"      --parentName="None" --user="RHESSys" defs/soil/soil_loamysand.def
./insertParams.py -v --type="soil" --name="rock"      --parentName="None" --user="RHESSys" defs/soil/soil_rock.def
./insertParams.py -v --type="soil" --name="sand"      --parentName="None" --user="RHESSys" defs/soil/soil_sand.def
./insertParams.py -v --type="soil" --name="sandyclay"      --parentName="None" --user="RHESSys" defs/soil/soil_sandyclay.def
./insertParams.py -v --type="soil" --name="sandyclayloam" --parentName="None" --user="RHESSys" defs/soil/soil_sandyclayloam.def
./insertParams.py -v --type="soil" --name="sandyloam"      --parentName="None" --user="RHESSys" defs/soil/soil_sandyloam.def
./insertParams.py -v --type="soil" --name="silt"      --parentName="None" --user="RHESSys" defs/soil/soil_silt.def
./insertParams.py -v --type="soil" --name="siltclay"      --parentName="None" --user="RHESSys" defs/soil/soil_siltyclay.def
./insertParams.py -v --type="soil" --name="siltclayloam" --parentName="None" --user="RHESSys" defs/soil/soil_siltyclayloam.def
./insertParams.py -v --type="soil" --name="siltloam"       --parentName="None" --user="RHESSys" defs/soil/soil_siltyloam.def
./insertParams.py -v --type="soil" --name="water"      --parentName="None" --user="RHESSys" defs/soil/soil_water.def

