# Run_CreateFlownet

# This is a script showing how CreateFlownet.R should be run

# RUN world_gen FIRST ------ 

# Set flow network name. ".flow" will be appended automatically if not included explicitly
flownetname = "czotest.flow"

# Optional arguments:
#typepars - if you want to use a different GIS environment than was used in world_gen
#cf_maps - if you want to use different maps to generate your flow network than you used to generate your worldfile

# Actually run CreateFlownet and make the flow network file for RHESSys
CreateFlownet(cfname = flownetname)

