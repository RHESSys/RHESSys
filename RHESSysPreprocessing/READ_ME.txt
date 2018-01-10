RHESSys PreProcessing
Will Burke 11/18/17

Developed using R Version 3.4.0. May be incompatible with older R versions

Compatible with GRASS GIS - versions 6.4.x and 7.x

The preprocessing package has two main components: world_gen and CreateFlownet, 
which create the worldfile and flow network respectively. These replace the previous C functions g2w and cf.

First use world_gen, open the setup/example script "run_world_gen.R".  There, follow the commented directions to use the function.

Next use CreateFlownet use the "run_CreateFlownet.R" and follow the commented instructions.

Additional information on each function can be found in the R documentation by using help(world_gen) or help(CreateFlownet).
Additional information can also be found (soon) on the RHESSys Github wiki page https://github.com/RHESSys/RHESSys/wiki

If you encounter bugs feel free to let me know wburke@ucsb.edu