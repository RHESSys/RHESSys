#pragma once

#ifdef __cplusplus
extern "C" {
#endif
/************************fire.h**************************/
/* shared header file between RHESSys and WMFire	*/
/**********************************************************/
struct fire_default {
	int ID;
	double veg_fuel_weighting ;	
	double ndays_average;
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
	double ignition_col; // allow for ignition x and y coordinates, but not yet implemented. default to -1
	double ignition_row;
	int fire_write; // should the fire results be written to a file?
	int fire_verbose; // should the fire runs be verbose?
	int n_rows;
	int n_cols;
	int fire_in_buffer;
	int spread_calc_type;
	double mean_log_wind;
	double sd_log_wind;
	double mean1_rvm;
	double mean2_rvm;
	double kappa1_rvm;
	double kappa2_rvm;
	double p_rvm;
	double ign_def_mod;
	double veg_k1; // for ignition use veg fuel
	double veg_k2; // for ignition use veg fuel

//	char **patch_file_name;
};

// this will replace the fire_nodes 
struct fire_object 
{
	double burn;			/* 0-1 */
	double fuel_veg;  		/* kgC/m2 */
	double fuel_litter; 		/* kgC/m2 */
	double fuel_moist; 		/* 0-1 */
	double soil_moist; 		/* 0-1 */
	double z; 			/* m */
	double wind; 			/* m/s */
	double wind_direction; 		/*degrees */
	double relative_humidity;	/* 0-1 */
	double temp; /* temperature of the cell */
	double et;
	double pet;
	int ign_available; /* 1 if available for ignition, 0 otherwise*/

};	

#ifdef __cplusplus
}
#endif