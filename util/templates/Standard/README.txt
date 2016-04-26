
EDITS

FULL WORLDFILE TEMPLATE = template.example

Header:
user must edit the defintion file names
soil_SOILTYPE.def must be changed to the name of the soil definition file to be used in project, i.e. soil_sandyloam.def
veg_VEGTYPE.def must be changed to the name of the vegetation definition file to be used in project, i.e. veg_douglasfir.def
CLIMSTATION_base must be changed to the name of the climate base station file to be used in project, i.e. cs2met_base

Levels:
user must edit names for each hierarchal level to coincide with raster map names appropriate to their project, i.e.
at each hierarchal level:
_basin
_hillslope
_zone
_patch
_stratum

the names BASIN_MAP, HILLSLOPE_MAP, HILL_OR_PATCH_MAP, PATCH_MAP  must be changed to the name of the appropriate raster map used in project

State Variables:
edit latitude value of XX.XX to the appropriate value for project, 37.45
edit DEM_MAP to the name of project dem map, i.e. dem30m
this template is set up to use the map names slope and aspect - if project slope and aspect map names differ, edit appropriately
if using maps to define east and west horizons, edit from value 0.17, i.e.    eqn 0.01 0.0 ehr.100
edit soil_default_ID of value 3.0 appropriate to default_ID used in soil definition file (or map name if multiple soil types)
edit veg_default_ID of value 7 appropriate to default_ID used in vegetation definition file (or map name if multiple veg types)


REDEFINE TEMPLATE = template.redef.example
as above, edit hierarchal level map names appropriately
this redefinition template is not set up to make any changes at all
user must change -9999 for the state variable(s) where change is to occur, i.e.
if user wants to zero out all vegetation carbon and nitrogen stores, value -9999 should be changed to value 0.0 for state variables cs_cpool through ns_retransn under the _stratum level.




