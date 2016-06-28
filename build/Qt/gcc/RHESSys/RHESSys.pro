#-------------------------------------------------
#
# Project created by QtCreator 2016-04-19T12:48:39
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = RHESSys
CONFIG   += console
CONFIG   -= app_bundle
QMAKE_CC = gcc -std=c99
QMAKE_CXXFLAGS += -fpermissive
#-std=c++0x
QMAKE_CXXFLAGS += -fopenmp
QMAKE_CFLAGS += -fopenmp
QMAKE_LFLAGS += -fopenmp
#QMAKE_CFLAGS += -STD=C99

TEMPLATE = app
INCLUDEPATH += ../../../../rhessys/include \
               /usr/include

DEFINES += NO_UPDATE_160419 \
           LIU_NETCDF_READER \
           CHECK_NCCLIM_DATA \
           FIND_STATION_BASED_ON_ID \
           RUN_WITH_SPINUP_PERIOD_USING_RANDOM_CLIMATE_YEAR_DATA

SOURCES += \
    ../../../../rhessys/clim/compute_toc_wind.c \
    ../../../../rhessys/clim/compute_atm_pres.c \
    ../../../../rhessys/cn/zero_stratum_daily_flux.c \
    ../../../../rhessys/cn/zero_stratum_annual_flux.c \
    ../../../../rhessys/cn/update_shadow_strata.c \
    ../../../../rhessys/cn/update_septic.c \
    ../../../../rhessys/cn/update_rooting_depth.c \
    ../../../../rhessys/cn/update_phenology.c \
    ../../../../rhessys/cn/update_nitrif.c \
    ../../../../rhessys/cn/update_N_stratum_daily.c \
    ../../../../rhessys/cn/update_mortality.c \
    ../../../../rhessys/cn/update_dissolved_organic_losses.c \
    ../../../../rhessys/cn/update_denitrif.c \
    ../../../../rhessys/cn/update_decomp.c \
    ../../../../rhessys/cn/update_C_stratum_daily.c \
    ../../../../rhessys/cn/update_branch_mortality.c \
    ../../../../rhessys/cn/resolve_sminn_competition.c \
    ../../../../rhessys/cn/leaf_conductance_vpd_curve.c \
    ../../../../rhessys/cn/leaf_conductance_tmin_curve.c \
    ../../../../rhessys/cn/leaf_conductance_tavg_curve.c \
    ../../../../rhessys/cn/leaf_conductance_LWP_curve.c \
    ../../../../rhessys/cn/leaf_conductance_CO2_curve.c \
    ../../../../rhessys/cn/leaf_conductance_APAR_curve.c \
    ../../../../rhessys/cn/Ksat_z_curve.c \
    ../../../../rhessys/cn/compute_prop_alloc_daily.c \
    ../../../../rhessys/cn/compute_potential_N_uptake.c \
    ../../../../rhessys/cn/compute_potential_N_uptake_Waring.c \
    ../../../../rhessys/cn/compute_potential_N_uptake_Dickenson.c \
    ../../../../rhessys/cn/compute_potential_N_uptake_combined.c \
    ../../../../rhessys/cn/compute_potential_decomp.c \
    ../../../../rhessys/cn/compute_N_leached.c \
    ../../../../rhessys/cn/compute_N_absorbed.c \
    ../../../../rhessys/cn/compute_maint_resp.c \
    ../../../../rhessys/cn/compute_leaf_litfall.c \
    ../../../../rhessys/cn/compute_growth_resp.c \
    ../../../../rhessys/cn/compute_growingseason_index.c \
    ../../../../rhessys/cn/compute_froot_litfall.c \
    ../../../../rhessys/cn/compute_farq_psn.c \
    ../../../../rhessys/cn/compute_deadleaf_turnover.c \
    ../../../../rhessys/cn/compute_cwd_decay.c \
    ../../../../rhessys/cn/compute_annual_turnover.c \
    ../../../../rhessys/cn/compute_annual_litfall.c \
    ../../../../rhessys/cn/allocate_daily_growth.c \
    ../../../../rhessys/cn/allocate_annual_growth.c \
    ../../../../rhessys/cycle/zone_hourly.c \
    ../../../../rhessys/cycle/zone_daily_I.c \
    ../../../../rhessys/cycle/zone_daily_F.c \
    ../../../../rhessys/cycle/world_hourly.c \
    ../../../../rhessys/cycle/world_daily_I.c \
    ../../../../rhessys/cycle/world_daily_F.c \
    ../../../../rhessys/cycle/surface_hourly.c \
    ../../../../rhessys/cycle/surface_daily_F.c \
    ../../../../rhessys/cycle/patch_hourly.c \
    ../../../../rhessys/cycle/patch_daily_I.c \
    ../../../../rhessys/cycle/patch_daily_F.c \
    ../../../../rhessys/cycle/hillslope_hourly.c \
    ../../../../rhessys/cycle/hillslope_daily_I.c \
    ../../../../rhessys/cycle/hillslope_daily_F.c \
    ../../../../rhessys/cycle/canopy_stratum_hourly.c \
    ../../../../rhessys/cycle/canopy_stratum_growth.c \
    ../../../../rhessys/cycle/canopy_stratum_daily_I.c \
    ../../../../rhessys/cycle/canopy_stratum_daily_F.c \
    ../../../../rhessys/cycle/basin_hourly.c \
    ../../../../rhessys/cycle/basin_daily_I.c \
    ../../../../rhessys/cycle/basin_daily_F.c \
    ../../../../rhessys/hydro/zero_patch_daily_flux.c \
    ../../../../rhessys/hydro/update_soil_moisture.c \
    ../../../../rhessys/hydro/update_litter_interception_capacity.c \
    ../../../../rhessys/hydro/update_hillslope_accumulator.c \
    ../../../../rhessys/hydro/update_gw_drainage.c \
    ../../../../rhessys/hydro/update_drainage_stream.c \
    ../../../../rhessys/hydro/update_drainage_road.c \
    ../../../../rhessys/hydro/update_drainage_land.c \
    ../../../../rhessys/hydro/update_basin_patch_accumulator.c \
    ../../../../rhessys/hydro/top_model.c \
    ../../../../rhessys/hydro/snowpack_daily_F.c \
    ../../../../rhessys/hydro/recompute_gamma.c \
    ../../../../rhessys/hydro/penman_monteith.c \
    ../../../../rhessys/hydro/compute_z_final.c \
    ../../../../rhessys/hydro/compute_vascular_stratum_conductance.c \
    ../../../../rhessys/hydro/compute_varbased_returnflow.c \
    ../../../../rhessys/hydro/compute_varbased_flow.c \
    ../../../../rhessys/hydro/compute_vapour_conductance.c \
    ../../../../rhessys/hydro/compute_unsat_zone_drainage.c \
    ../../../../rhessys/hydro/compute_transmissivity_curve.c \
    ../../../../rhessys/hydro/compute_subsurface_routing.c \
    ../../../../rhessys/hydro/compute_subsurface_routing_hourly.c \
    ../../../../rhessys/hydro/compute_stream_routing.c \
    ../../../../rhessys/hydro/compute_stability_correction.c \
    ../../../../rhessys/hydro/compute_soil_water_potential.c \
    ../../../../rhessys/hydro/compute_snow_sublimation.c \
    ../../../../rhessys/hydro/compute_snow_stored.c \
    ../../../../rhessys/hydro/compute_return_flow.c \
    ../../../../rhessys/hydro/compute_rain_stored.c \
    ../../../../rhessys/hydro/compute_ra_understory.c \
    ../../../../rhessys/hydro/compute_ra_surface.c \
    ../../../../rhessys/hydro/compute_ra_overstory.c \
    ../../../../rhessys/hydro/compute_potential_snow_interception.c \
    ../../../../rhessys/hydro/compute_potential_rain_interception.c \
    ../../../../rhessys/hydro/compute_potential_exfiltration.c \
    ../../../../rhessys/hydro/compute_nonvascular_stratum_conductance.c \
    ../../../../rhessys/hydro/compute_lwp_predawn.c \
    ../../../../rhessys/hydro/compute_litter_rain_stored.c \
    ../../../../rhessys/hydro/compute_layer_field_capacity.c \
    ../../../../rhessys/hydro/compute_infiltration.c \
    ../../../../rhessys/hydro/compute_hourly_rain_stored.c \
    ../../../../rhessys/hydro/compute_hourly_litter_rain_stored.c \
    ../../../../rhessys/hydro/compute_field_capacity.c \
    ../../../../rhessys/hydro/compute_delta_water.c \
    ../../../../rhessys/hydro/compute_capillary_rise.c \
    ../../../../rhessys/init/reconstruct_hourly_clim.c \
    ../../../../rhessys/init/read_netcdf.c \
    ../../../../rhessys/init/parse_veg_type.c \
    ../../../../rhessys/init/parse_phenology_type.c \
    ../../../../rhessys/init/parse_dyn_flag.c \
    ../../../../rhessys/init/parse_alloc_flag.c \
    ../../../../rhessys/init/parse_albedo_flag.c \
    ../../../../rhessys/init/destroy_zone.c \
    ../../../../rhessys/init/destroy_zone_defaults.c \
    ../../../../rhessys/init/destroy_world.c \
    ../../../../rhessys/init/destroy_tec.c \
    ../../../../rhessys/init/destroy_surface_energy_defaults.c \
    ../../../../rhessys/init/destroy_stratum_defaults.c \
    ../../../../rhessys/init/destroy_soil_defaults.c \
    ../../../../rhessys/init/destroy_patch.c \
    ../../../../rhessys/init/destroy_output_fileset.c \
    ../../../../rhessys/init/destroy_output_files.c \
    ../../../../rhessys/init/destroy_landuse_defaults.c \
    ../../../../rhessys/init/destroy_hillslope.c \
    ../../../../rhessys/init/destroy_hillslope_defaults.c \
    ../../../../rhessys/init/destroy_fire_defaults.c \
    ../../../../rhessys/init/destroy_command_line.c \
    ../../../../rhessys/init/destroy_canopy_stratum.c \
    ../../../../rhessys/init/destroy_basin.c \
    ../../../../rhessys/init/destroy_basin_defaults.c \
    ../../../../rhessys/init/destroy_base_station.c \
    ../../../../rhessys/init/construct_zone.c \
    ../../../../rhessys/init/construct_zone_defaults.c \
    ../../../../rhessys/init/construct_yearly_clim.c \
    ../../../../rhessys/init/construct_world.c \
    ../../../../rhessys/init/construct_topmodel_patchlist.c \
    ../../../../rhessys/init/construct_tec.c \
    ../../../../rhessys/init/construct_tec_entry.c \
    ../../../../rhessys/init/construct_surface_energy_defaults.c \
    ../../../../rhessys/init/construct_stream_routing_topology.c \
    ../../../../rhessys/init/construct_stratum_defaults.c \
    ../../../../rhessys/init/construct_spinup_thresholds.c \
    ../../../../rhessys/init/construct_spinup_defaults.c \
    ../../../../rhessys/init/construct_soil_defaults.c \
    ../../../../rhessys/init/construct_routing_topology.c \
    ../../../../rhessys/init/construct_patch.c \
    ../../../../rhessys/init/construct_output_fileset.c \
    ../../../../rhessys/init/construct_output_files.c \
    ../../../../rhessys/init/construct_netcdf_header.c \
    ../../../../rhessys/init/construct_netcdf_grid.c \
    ../../../../rhessys/init/construct_monthly_clim.c \
    ../../../../rhessys/init/construct_landuse_defaults.c \
    ../../../../rhessys/init/construct_hourly_clim.c \
    ../../../../rhessys/init/construct_hillslope.c \
    ../../../../rhessys/init/construct_hillslope_defaults.c \
    ../../../../rhessys/init/construct_fire_grid.c \
    ../../../../rhessys/init/construct_fire_defaults.c \
    ../../../../rhessys/init/construct_filename_list.c \
    ../../../../rhessys/init/construct_empty_shadow_strata.c \
    ../../../../rhessys/init/construct_ddn_routing_topology.c \
    ../../../../rhessys/init/construct_dated_input.c \
    ../../../../rhessys/init/construct_dated_clim_sequence.c \
    ../../../../rhessys/init/construct_daily_clim.c \
    ../../../../rhessys/init/construct_command_line.c \
    ../../../../rhessys/init/construct_clim_sequence.c \
    ../../../../rhessys/init/construct_canopy_strata.c \
    ../../../../rhessys/init/construct_basin.c \
    ../../../../rhessys/init/construct_basin_defaults.c \
    ../../../../rhessys/init/construct_base_station.c \
    ../../../../rhessys/init/construct_ascii_grid.c \
    ../../../../rhessys/init/compute_mean_hillslope_parameters.c \
    ../../../../rhessys/init/assign_neighbours.c \
    ../../../../rhessys/init/assign_base_station.c \
    ../../../../rhessys/init/assign_base_station_xy.c \
    ../../../../rhessys/output/output_zone.c \
    ../../../../rhessys/output/output_zone_state.c \
    ../../../../rhessys/output/output_yearly_zone.c \
    ../../../../rhessys/output/output_yearly_patch.c \
    ../../../../rhessys/output/output_yearly_hillslope.c \
    ../../../../rhessys/output/output_yearly_growth_zone.c \
    ../../../../rhessys/output/output_yearly_growth_patch.c \
    ../../../../rhessys/output/output_yearly_growth_hillslope.c \
    ../../../../rhessys/output/output_yearly_growth_canopy_stratum.c \
    ../../../../rhessys/output/output_yearly_growth_basin.c \
    ../../../../rhessys/output/output_yearly_canopy_stratum.c \
    ../../../../rhessys/output/output_yearly_basin.c \
    ../../../../rhessys/output/output_template_structure.c \
    ../../../../rhessys/output/output_stream_routing.c \
    ../../../../rhessys/output/output_patch.c \
    ../../../../rhessys/output/output_patch_state.c \
    ../../../../rhessys/output/output_monthly_zone.c \
    ../../../../rhessys/output/output_monthly_patch.c \
    ../../../../rhessys/output/output_monthly_hillslope.c \
    ../../../../rhessys/output/output_monthly_canopy_stratum.c \
    ../../../../rhessys/output/output_monthly_basin.c \
    ../../../../rhessys/output/output_hourly_zone.c \
    ../../../../rhessys/output/output_hourly_growth_basin.c \
    ../../../../rhessys/output/output_hourly_basin.c \
    ../../../../rhessys/output/output_hillslope.c \
    ../../../../rhessys/output/output_hillslope_state.c \
    ../../../../rhessys/output/output_growth_zone.c \
    ../../../../rhessys/output/output_growth_patch.c \
    ../../../../rhessys/output/output_growth_hillslope.c \
    ../../../../rhessys/output/output_growth_canopy_stratum.c \
    ../../../../rhessys/output/output_growth_basin.c \
    ../../../../rhessys/output/output_csv_zone.c \
    ../../../../rhessys/output/output_csv_yearly_zone.c \
    ../../../../rhessys/output/output_csv_yearly_patch.c \
    ../../../../rhessys/output/output_csv_yearly_hillslope.c \
    ../../../../rhessys/output/output_csv_yearly_growth_patch.c \
    ../../../../rhessys/output/output_csv_yearly_growth_basin.c \
    ../../../../rhessys/output/output_csv_yearly_canopy_stratum.c \
    ../../../../rhessys/output/output_csv_yearly_basin.c \
    ../../../../rhessys/output/output_csv_patch.c \
    ../../../../rhessys/output/output_csv_monthly_zone.c \
    ../../../../rhessys/output/output_csv_monthly_patch.c \
    ../../../../rhessys/output/output_csv_monthly_hillslope.c \
    ../../../../rhessys/output/output_csv_monthly_canopy_stratum.c \
    ../../../../rhessys/output/output_csv_monthly_basin.c \
    ../../../../rhessys/output/output_csv_hillslope.c \
    ../../../../rhessys/output/output_csv_growth_zone.c \
    ../../../../rhessys/output/output_csv_growth_patch.c \
    ../../../../rhessys/output/output_csv_growth_hillslope.c \
    ../../../../rhessys/output/output_csv_growth_canopy_stratum.c \
    ../../../../rhessys/output/output_csv_growth_basin.c \
    ../../../../rhessys/output/output_csv_canopy_stratum.c \
    ../../../../rhessys/output/output_csv_basin.c \
    ../../../../rhessys/output/output_canopy_stratum.c \
    ../../../../rhessys/output/output_canopy_strata_state.c \
    ../../../../rhessys/output/output_basin_state.c \
    ../../../../rhessys/output/add_headers.c \
    ../../../../rhessys/output/add_growth_headers.c \
    ../../../../rhessys/output/add_csv_headers.c \
    ../../../../rhessys/output/add_csv_growth_headers.c \
    ../../../../rhessys/rad/compute_surface_heat_flux.c \
    ../../../../rhessys/rad/compute_subsurface_temperature_profile.c \
    ../../../../rhessys/rad/compute_radiative_fluxes.c \
    ../../../../rhessys/rad/compute_Lstar.c \
    ../../../../rhessys/rad/compute_Lstar_canopy.c \
    ../../../../rhessys/rad/compute_direct_radiative_PAR_fluxes.c \
    ../../../../rhessys/rad/compute_direct_radiative_fluxes.c \
    ../../../../rhessys/rad/compute_diffuse_radiative_PAR_fluxes.c \
    ../../../../rhessys/rad/compute_diffuse_radiative_fluxes.c \
    ../../../../rhessys/tec/valid_option.c \
    ../../../../rhessys/tec/skip_zone.c \
    ../../../../rhessys/tec/skip_strata.c \
    ../../../../rhessys/tec/skip_patch.c \
    ../../../../rhessys/tec/skip_hillslope.c \
    ../../../../rhessys/tec/skip_basin.c \
    ../../../../rhessys/tec/input_new_zone.c \
    ../../../../rhessys/tec/input_new_zone_mult.c \
    ../../../../rhessys/tec/input_new_strata.c \
    ../../../../rhessys/tec/input_new_strata_thin.c \
    ../../../../rhessys/tec/input_new_strata_mult.c \
    ../../../../rhessys/tec/input_new_patch.c \
    ../../../../rhessys/tec/input_new_patch_mult.c \
    ../../../../rhessys/tec/input_new_hillslope.c \
    ../../../../rhessys/tec/input_new_hillslope_mult.c \
    ../../../../rhessys/tec/input_new_basin.c \
    ../../../../rhessys/tec/input_new_basin_mult.c \
    ../../../../rhessys/tec/handle_event.c \
    ../../../../rhessys/tec/execute_yearly_output_event.c \
    ../../../../rhessys/tec/execute_yearly_growth_output_event.c \
    ../../../../rhessys/tec/execute_tec.c \
    ../../../../rhessys/tec/execute_state_output_event.c \
    ../../../../rhessys/tec/execute_road_construction_event.c \
    ../../../../rhessys/tec/execute_redefine_world_thin_event.c \
    ../../../../rhessys/tec/execute_redefine_world_mult_event.c \
    ../../../../rhessys/tec/execute_redefine_world_event.c \
    ../../../../rhessys/tec/execute_redefine_strata_event.c \
    ../../../../rhessys/tec/execute_monthly_output_event.c \
    ../../../../rhessys/tec/execute_hourly_output_event.c \
    ../../../../rhessys/tec/execute_hourly_growth_output_event.c \
    ../../../../rhessys/tec/execute_daily_output_event.c \
    ../../../../rhessys/tec/execute_daily_growth_output_event.c \
    ../../../../rhessys/tec/execute_csv_yearly_output_event.c \
    ../../../../rhessys/tec/execute_csv_yearly_growth_output_event.c \
    ../../../../rhessys/tec/execute_csv_monthly_output_event.c \
    ../../../../rhessys/tec/execute_csv_daily_output_event.c \
    ../../../../rhessys/tec/execute_csv_daily_growth_output_event.c \
    ../../../../rhessys/util/yearday.c \
    ../../../../rhessys/util/union_date_init.c \
    ../../../../rhessys/util/union_date_combine.c \
    ../../../../rhessys/util/sort_patch_layers.c \
    ../../../../rhessys/util/sort_by_elevation.c \
    ../../../../rhessys/util/resemble_hourly_date.c \
    ../../../../rhessys/util/readtag_worldfile.c \
    ../../../../rhessys/util/read_record.c \
    ../../../../rhessys/util/read_basin.c \
    ../../../../rhessys/util/params.c \
    ../../../../rhessys/util/key_compare.c \
    ../../../../rhessys/util/julday.c \
    ../../../../rhessys/util/get_num_daywhourly.c \
    ../../../../rhessys/util/find_zone_in_hillslope.c \
    ../../../../rhessys/util/find_stratum.c \
    ../../../../rhessys/util/find_stratum_in_patch.c \
    ../../../../rhessys/util/find_patch.c \
    ../../../../rhessys/util/find_patch_in_zone.c \
    ../../../../rhessys/util/find_hillslope_in_basin.c \
    ../../../../rhessys/util/find_basin.c \
    ../../../../rhessys/util/date_oper_union.c \
    ../../../../rhessys/util/create_random_distrb.c \
    ../../../../rhessys/util/compute_year_day.c \
    ../../../../rhessys/util/check_zero_stores.c \
    ../../../../rhessys/util/caldat.c \
    ../../../../rhessys/util/cal_date_lt.c \
    ../../../../rhessys/util/alloc.c \
    ../../../../rhessys/main.c \
    ../../../../rhessys/tec/execute_firespread_event.c \
    ../../../../util/FIRE/WMFire.cpp \
    ../../../../util/FIRE/RanNums.cpp \
    ../../../../rhessys/hydro/compute_saturation_vapor_pressure.c \
    ../../../../rhessys/hydro/compute_vapor_pressure_deficit.c \
    ../../../../rhessys/output/output_basin.c

OTHER_FILES += \
    ../../../../rhessys/cycle/patch_daily_F.c,v

HEADERS += \
    ../../../../rhessys/cycle/canopy_stratum_daily_F.new \
    ../../../../rhessys/hydro/compute_varbased_returnflow.normal \
    ../../../../rhessys/include/rhessys.h \
    ../../../../rhessys/include/phys_constants.h \
    ../../../../rhessys/include/params.h \
    ../../../../rhessys/include/netcdf.h \
    ../../../../rhessys/include/functions.h \
    ../../../../rhessys/init/construct_routing_topology.alt

LIBS += -L/usr/lib -lnetcdf -lgomp -lpthread -fopenmp
