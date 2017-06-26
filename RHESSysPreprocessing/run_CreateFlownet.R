# Run_CreateFlownet
# Will Burke 6/21/17

# This is a script showing how CreateFlownet.R should be run

# RUN world_gen FIRST

# Note -  it's reccomended to run CreateFlownet from the same directory as you ran world_gen,
# as world_gen created two files (typepars, cf_maps), that CreateFlownet depends on.

# Next, open the file "cf_maps" which was generated whereever your current directory was when you ran world_gen
# In cf_maps, specify the name of your streams map, on the line starting with "stream".
# If you do not do this, you will be prompted to do so when you run CreateFlownet.

# Set flow network name. ".flow" will be appended automatically if not included explicitly
flownetname = "example.flow"

# Optional arguments:
#typepars - if you want to use a different GIS environment than was used in world_gen
#cf_maps - if you want to use different maps to generate your flow network than you used to generate your worldfile

# Actually run CreateFlownet and make the flow network file for RHESSys
CreateFlownet(cfname = flownetname)

