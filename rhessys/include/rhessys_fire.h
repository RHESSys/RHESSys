#pragma once

#ifdef __cplusplus
extern "C" {
#endif
/************************fire.h**************************/
/* shared header file between RHESSys and WMFire		*/
/* defines the fire default file and the fire object that is the */
/* basis for the bi-directional coupling between RHESSys	*/
/* and WMFire								*/
/*******************************************************/
struct fire_default {
	int ID;
	double veg_fuel_weighting;	// A preliminary idea from model development. Never fully implemented, and likely to go away. Intention was to weight vegetation carbon in fuel loading for fire spread

	double ndays_average; // how many days to include in the averaging of patch moisture and load status? Probably not worth tweakings
	double load_k1; // k1 for the fuel load sigmoid function
	double moisture_k1; //k1 for the fuel moisture sigmoid function
	double load_k2; // k2 for the fuel load sigmoid function
	double moisture_k2; // k2 for the fuel moisture sigmoid function
	double winddir_k1; // the modifying constant for wind direction, depending on windspeed
	double winddir_k2; // intercept for wind direction, giving the probability of backing fire
	double windmax; // maximum wind speed to calculate relative windspeed
	double slope_k1; // k1 for the concave up slope function (k1*exp(k2*slope^2)), where sign(k2) is sign(slope)
	double slope_k2; // k2 for the concave up slope function (k1*exp(k2*slope^2)), where sign(k2) is sign(slope)
	double ignition_tmin; // the minimum temperature for a possible ignition
	double ignition_col; // allow for ignition x and y coordinates, but not yet implemented. default to -1 for both col and row, for random ignitions
	double ignition_row;
	int fire_write; // should the fire results be written to a file? Creates large suite of files including rasters of fire spread, and calculated values for each predictor variable
	int fire_verbose; // for debugging, printf statements giving location in code and status of fire spread
	int n_rows; // number of rows in the DEM and patch grids
	int n_cols; // number of columns in the DEM and patch grids
	int fire_in_buffer; // should fire be allowed to spread outside of the watershed boundaries within the square grid? 1=Y, 0=N
	int spread_calc_type; // temporary for model development. Alternative calculations of p_spread and p_ign. set = 9 for current testing mode, = 7 for current published mode
	double mean_log_wind; // These value relate to empirical wind distributions. Windspeed is modeled as a lognormal, with a mean log and a sd log.
	double sd_log_wind;
	double mean1_rvm; // rvm is for a random mixed von mises distribution, bimodal for wind direction. These values are empirically estimated for the wind distribution in the climate record.
	double mean2_rvm;
	double kappa1_rvm;
	double kappa2_rvm;
	double p_rvm;	// this gives the proportion of wind direction entries observed within each mode. See R code for techniques to estimate these values.
	double ign_def_mod; // modifier for ignition moisture_k2, multiply the threshold deficit by this value
	int veg_ign; // use vegetation for ignition? If so, use the parameters below. This is a temporary parameter used in development, not likely to be retained in final model
	double veg_k1; // for ignition use veg fuel, this is a temporary parameter used in development, not likely to be retained in final model. not necessary for spread_calc_type=7 or =9
	double veg_k2; // for ignition use veg fuel, this is a temporary parameter used in development, not likely to be retained in final model. not necessary for spread_calc_type=7 or =9
	double mean_ign; // mean number of ignitions per month, for a monthly poisson draw. mandatory parameter value
	long ran_seed; // 0 for stochastic runs, or integer seed for deterministic runs and debugging
	double moisture_ign_k1; // alternative model for ignition probability under development. for spread_calc_type = 8 or 9 (check! might only be 9). If there is no understory, then overall et and pet is used for ignition moisture. possibly replace with an aet with a fixed rooting depth, bec def in understory with shallow roots might overrepresent deficit (an imaginary understory) to represent moisture condition down there
	double moisture_ign_k2;// alternative model for ignition probability under development. for spread_calc_type = 8 or 9 (check! might only be 9)
	double load_ign_k1;// alternative model for ignition probability under development. Currently not implemented pending evaluation of moisture ignition model
	double load_ign_k2;// alternative model for ignition probability under development. Currently not implemented pending evaluation of moisture ignition model
	int calc_fire_effects; // 0 for no fire effects, 1 for fire effects
	int seed_multiplier; //controlled stochastic NREN 201808
	int include_wui; //0 for no WUI grid, 1 for wui grid--0 by default
	int calc_above_ground_litter; //0 for no need to calculate the differences between above and below ground litter
//	char **patch_file_name;
};

/*
struct node_fire_wui_dist // I think , to be the linked list?
{
	double dist; // the distance to this patch WUI
	struct patch_object **patches;
	struct node_fire_wui_dist *next; 	//ptr to next in list;

}
*/
// The object that is  passed between RHESSys and WMFire
struct fire_object
{
	double burn;			/* 0-1 , recrds the value of p_spread for those pixels that experienced fire*/
	double fuel_veg;  		/* kgC/m2 ; mass of carbon in the vegetation layer, may not be used*/
	double fuel_litter; 		/* kgC/m2 ; mass of carbon in the litter layer*/
	double fuel_moist; 		/* 0-1 ; measure of moisture used in spread calculation. Varies by spread_calc_type*/
	double soil_moist; 		/* 0-1; soil moisture*/
	double z; 			/* m  elevation*/
	double wind; 			/* m/s  randomly drawn windspeed*/
	double wind_direction; 		/*degrees randomly drawn wind direction*/
	double relative_humidity;	/* 0-1 */
	double temp; /* temperature of the cell */
	double et; // total evapotranspiration
	double pet; // total potential evapotranspiration
	double trans;
	int ign_available; /* 1 if available for ignition, 0 otherwise*/
	double understory_et; //evapotranspiration of only the understory
	double understory_pet; //potential evapotranspiration of only the understory
	double fire_size; // I think this would be the easiest way to transfer fire size to rhessys,and allow for an if fire_size>0 then calculate fire effects, otherwise don't bother; keep as 0 in general, and just fill in the first element in the grid as a placeholder
						// returned as the number of pixels, should be converted to ha
	//double *wui_dists;  this has to be a dynamically allocated array with nWUI from the fire default
	//struct node_fire_wui_dist *patch_wui_dist[3] // intended to be an array of 3 patch WUI linked lists
};

#ifdef __cplusplus
}
#endif
