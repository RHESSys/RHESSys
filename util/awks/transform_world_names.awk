BEGIN {h=0;}
{a = 0; }
($2 == "basin_ID") { h=1; }
($2 == "default_ID") && (h==1) {printf("%f	%s\n",$1,$2="basin_parm_ID"); a=1;}
($2 == "hillslope_ID") { h=2; }
($2 == "default_ID") && (h==2) {printf("%f	%s\n",$1,$2="hill_parm_ID"); a=1;}
($2 == "zone_ID") { h=3; }
($2 == "default_ID") && (h==3) {printf("%f	%s\n",$1,$2="zone_parm_ID"); a=1;}
($2 == "stratum_ID") { h=4; }
($2 == "default_ID") && (h==4) {printf("%f	%s\n",$1,$2="veg_parm_ID"); a=1; h=0;}

($2 == "world_ID") {printf("%f %s\n",$1,$2="world_id"); a=1;}
($2 == "gw_storage") {printf("%f %s\n",$1,$2="gw.storage"); a=1;}
($2 == "gw_NO3") {printf("%f %s\n",$1,$2="gw.NO3"); a=1;}
($2 == "a_area") {printf("%f %s\n",$1,$2="area"); a=1;}
($2 == "isohyet") {printf("%f %s\n",$1,$2="precip_lapse_rate"); a=1;}
($2 == "east_horizon") {printf("%f %s\n",$1,$2="e_horizon"); a=1;}
($2 == "west_horizon") {printf("%f %s\n",$1,$2="w_horizon"); a=1;}
($2 == "base_station_ID") {printf("%f %s\n",$1,$2="zone_base_station_ID"); a=1;}
($2 == "p_base_station_ID") {printf("%f %s\n",$1,$2="zone_base_station_ID"); a=1;}
($2 == "num_patchs") {printf("%f %s\n",$1,$2="num_patches"); a=1;}
($2 == "soil_default") {printf("%f %s\n",$1,$2="soil_parm_ID"); a=1;}
($2 == "soil_default_ID") {printf("%f %s\n",$1,$2="soil_parm_ID"); a=1;}
($2 == "landuse_default") {printf("%f %s\n",$1,$2="landuse_parm_ID"); a=1;}
($2 == "landuse_default_ID") {printf("%f %s\n",$1,$2="landuse_parm_ID"); a=1;}
($2 == "a_area") {printf("%f %s\n",$1,$2="area"); a=1;}
($2 == "wetness_index") {printf("%f %s\n",$1,$2="lna"); a=1;}
($2 == "m") {printf("%f %s\n",$1,$2="mpar"); a=1;}
($2 == "m_par") {printf("%f %s\n",$1,$2="mpar"); a=1;}
($2 == "snowpack_water_equivalent_depth") {printf("%f %s\n",$1,$2="snowpack.water_equivalent_depth"); a=1;}
($2 == "snowpack_water_depth") {printf("%f %s\n",$1,$2="snowpack.water_depth"); a=1;}
($2 == "snowpack_T") {printf("%f %s\n",$1,$2="snowpack.T"); a=1;}
($2 == "snowpack_surface_age") {printf("%f %s\n",$1,$2="snowpack.surface_age"); a=1;}
($2 == "snowpack_energy_deficit") {printf("%f %s\n",$1,$2="snowpack.energy_deficit"); a=1;}
($2 == "num_stratums") {printf("%f %s\n",$1,$2="num_canopy_strata"); a=1;}
($2 == "stratum_ID") {printf("%f %s\n",$1,$2="canopy_strata_ID"); a=1;}
($2 == "root_depth") {printf("%f %s\n",$1,$2="rootzone.depth"); a=1;}
($2 == "cs_pool") {printf("%f %s\n",$1,$2="cs.cpool"); a=1;}
($2 == "cs_leafc") {printf("%f %s\n",$1,$2="cs.leafc"); a=1;}
($2 == "cs_dead_leafc") {printf("%f %s\n",$1,$2="cs.dead_leafc"); a=1;}
($2 == "cs_leafc_store") {printf("%f %s\n",$1,$2="cs.leafc_store"); a=1;}
($2 == "cs_leafc_transfer") {printf("%f %s\n",$1,$2="cs.leafc_transfer"); a=1;}
($2 == "cs_live_stemc") {printf("%f %s\n",$1,$2="cs.live_stemc"); a=1;}
($2 == "cs_live_stemc_store") {printf("%f %s\n",$1,$2="cs.livestemc_store"); a=1;}
($2 == "cs_live_stemc_transfer") {printf("%f %s\n",$1,$2="cs.livestemc_transfer"); a=1;}
($2 == "cs_dead_stem") {printf("%f %s\n",$1,$2="cs.dead_stemc"); a=1;}
($2 == "cs_dead_stemc_store") {printf("%f %s\n",$1,$2="cs.deadstemc_store"); a=1;}
($2 == "cs_dead_stemc_transfer") {printf("%f %s\n",$1,$2="cs.deadstemc_transfer"); a=1;}
($2 == "cs_live_crootc") {printf("%f %s\n",$1,$2="cs.live_crootc"); a=1;}
($2 == "cs_live_crootc_store") {printf("%f %s\n",$1,$2="cs.livecrootc_store"); a=1;}
($2 == "cs_live_crootc_transfer") {printf("%f %s\n",$1,$2="cs.livecrootc_transfer"); a=1;}
($2 == "cs_dead_crootc") {printf("%f %s\n",$1,$2="cs.dead_crootc"); a=1;}
($2 == "cs_dead_crootc_store") {printf("%f %s\n",$1,$2="cs.deadcrootc_store"); a=1;}
($2 == "cs_dead_crootc_transfer") {printf("%f %s\n",$1,$2="cs.deadcrootc_transfer"); a=1;}
($2 == "cs_frootc") {printf("%f %s\n",$1,$2="cs.frootc"); a=1;}
($2 == "cs_frootc_store") {printf("%f %s\n",$1,$2="cs.frootc_store"); a=1;}
($2 == "cs_frootc_transfer") {printf("%f %s\n",$1,$2="cs.frootc_transfer"); a=1;}
($2 == "cs_cwdc") {printf("%f %s\n",$1,$2="cs.cwdc"); a=1;}
($2 == "ns_pool") {printf("%f %s\n",$1,$2="ns.npool"); a=1;}
($2 == "ns_leafn") {printf("%f %s\n",$1,$2="ns.leafn"); a=1;}
($2 == "ns_dead_leafn") {printf("%f %s\n",$1,$2="ns.dead_leafn"); a=1;}
($2 == "ns_leafn_store") {printf("%f %s\n",$1,$2="ns.leafn_store"); a=1;}
($2 == "ns_leafn_transfer") {printf("%f %s\n",$1,$2="ns.leafn_transfer"); a=1;}
($2 == "ns_live_stemn") {printf("%f %s\n",$1,$2="ns.live_stemn"); a=1;}
($2 == "ns_live_stemn_store") {printf("%f %s\n",$1,$2="ns.livestemn_store"); a=1;}
($2 == "ns_live_stemn_transfer") {printf("%f %s\n",$1,$2="ns.livestemn_transfer"); a=1;}
($2 == "ns_dead_stem") {printf("%f %s\n",$1,$2="ns.dead_stemn"); a=1;}
($2 == "ns_dead_stemn_store") {printf("%f %s\n",$1,$2="ns.deadstemn_store"); a=1;}
($2 == "ns_dead_stemn_transfer") {printf("%f %s\n",$1,$2="ns.deadstemn_transfer"); a=1;}
($2 == "ns_live_crootn") {printf("%f %s\n",$1,$2="ns.live_crootn"); a=1;}
($2 == "ns_live_crootn_store") {printf("%f %s\n",$1,$2="ns.livecrootn_store"); a=1;}
($2 == "ns_live_crootn_transfer") {printf("%f %s\n",$1,$2="ns.livecrootn_transfer"); a=1;}
($2 == "ns_dead_crootn") {printf("%f %s\n",$1,$2="ns.dead_crootn"); a=1;}
($2 == "ns_dead_crootn_store") {printf("%f %s\n",$1,$2="ns.deadcrootn_store"); a=1;}
($2 == "ns_dead_crootn_transfer") {printf("%f %s\n",$1,$2="ns.deadcrootn_transfer"); a=1;}
($2 == "ns_frootn") {printf("%f %s\n",$1,$2="ns.frootn"); a=1;}
($2 == "ns_frootn_store") {printf("%f %s\n",$1,$2="ns.frootn_store"); a=1;}
($2 == "ns_frootn_transfer") {printf("%f %s\n",$1,$2="ns.frootn_transfer"); a=1;}
($2 == "ns_cwdn") {printf("%f %s\n",$1,$2="ns.cwdn"); a=1;}
($2 == "ns_retransn") {printf("%f %s\n",$1,$2="ns.retransn"); a=1;}
($2 == "epv_wstress") {printf("%f %s\n",$1,$2="epv.wstress_days"); a=1;}
($2 == "epv_min_fparabs") {printf("%f %s\n",$1,$2="epv.max_fparabs"); a=1;}
($2 == "epv_min_vwc") {printf("%f %s\n",$1,$2="epv.min_vwc"); a=1;}

(a == 0) {printf("%s	%s\n",$1,$2);}
