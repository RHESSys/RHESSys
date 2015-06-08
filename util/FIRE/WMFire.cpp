/*******************WMFire*****************************************/
/* A shared library to spread fire on a raster grid				*/
/* In current implementation, generates random fires on a square	*/
/* window.  Fire spread is stochastic depending on local conditions	*/
/* including fuel load, moisture, slope and wind direction, which 	*/
/* are supplied in grid format by calling program.				*/
/********************************************************************/

/*#define WMFIRE_EXPORTS
#include "../util/WMFireInterface.h"

#if defined(_WIN32) || defined(__WIN32__)
#include <windows.h>
 
// DLL entry function (called on load, unload, ...)
BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved)
{
    return TRUE;
}
#endif


*/
// mk: the above causes compilation errors, otherwise the program compiles fine
#include <ctime>
#if defined(_WIN32) || defined(__WIN32__)
	#include <windows.h>
	#include <time.h>
#else
	#include <sys/time.h>
#endif
//#include <unistd.h> // for the sleep function
#include "WMFire.h"
#include "boost/random.hpp"
#include "boost/shared_ptr.hpp"

#include <fstream>
#include <cstdio>
#include <iostream>
#include <cstdlib>

using std::cout;
using std::stringstream;
using std::fstream;
using std::ifstream;
using std::ofstream;

using boost::shared_ptr;

// WMFire is used by models that pass values defined in the rhessys_fire.h file.
// The calling model passes a 2D grid of fire_objects, of size nrow X ncol 
//					world[0].fire_grid,*(world[0].defaults[0].fire),command_line[0].fire_grid_res,world[0].num_fire_grid_row,world[0].num_fire_grid_col,current_date.month,current_date.year
struct fire_object **WMFire(double cell_res,  int nrow, int ncol, long year, long month, struct fire_object** fire_grid,struct fire_default def)
{
	cout<<"beginning fire spread using WMFire. month, year, cell_res, nrow, ncol: "<<month<<" "<<year<<"  "<<cell_res<<" "<<nrow<<" "<<ncol<<"\n";
	cout<<"Defaults: moisture k1 and k2, load k1"<<def.moisture_k1<<" "<<def.moisture_k2<<" "<<def.load_k1<<"\n";
	timeval t1;
	#if defined(_WIN32) || defined(__WIN32__)
		long seed=1; 
	#else
		gettimeofday(&t1, NULL); 
		long seed=-t1.tv_usec;
	// seed the rng using a high resolution clock#include <sys/time.h>
	#endif
	
//	srand(t1.tv_usec * t1.tv_sec);
            // this is the source for random numbers for the entire application
	boost::mt19937 rngEngine;
	rngEngine.seed(seed);
            
 	boost::uniform_01<> range;
	GenerateRandom randomNG(rngEngine, range);            

	LandScape landscape(cell_res,fire_grid,def,nrow,ncol); // create landscape object
	if(def.fire_verbose==1)
		cout<<"\nafter landscape constructor\n\n";
	landscape.Reset(); 
	if(def.fire_verbose==1)
		cout<<"\nafter landscape reset\n\n";
	landscape.initializeCurrentFire(randomNG);// reset the information for the current fire, if successful this will be added to the analysis
	if(def.fire_verbose==1)
		cout<<"\nafter landscape initialize current fire\n\n";
	landscape.Burn(randomNG); // run the current fire
	if(def.fire_write>0)
		landscape.writeFire(month,year,def);
	if(def.fire_verbose==1)
		cout<<"\nafter burn landscape\n\n";
	return landscape.FireGrids();  // return the updated fire grid
}


LandScape::LandScape(double cell_res,struct fire_object **fire_grid,struct fire_default def, int nrow, int ncol)
					: rows_(0), cols_(0), buffer_(5), cell_res_(0)
{
	cell_res_=cell_res; // can you write to these private members here?
	fireGrid_=fire_grid; // will need to keep track of pointers, and return this updated grid
	def_=def;
	rows_=nrow;
	cols_=ncol;
	buffer_=0;
	n_ign_=0;
	localFireGrid_.resize(boost::extents[rows_][cols_]); // local fire information
	ignCells_.clear();
	for(int i=0; i<rows_; i++)	//then, for each row, allocate an array with the # of columns.  this is now a 2-D array of fireGrids
	{
		for(int j=0; j<cols_; j++)	// fill in the landscape information for each pixel
		{
			if (fireGrid_[i][j].ign_available==1)
			{
				IgnitionCells ic = {i, j}; // the cell indices give the current row and column for this pixel available for ignition
				ignCells_.push_back(ic);		// 0 indicates that the pixel has not been burned
				n_ign_++;
			}
		}
	}
//	cout<<"def vals:"<<def.slope_k1<<"  "<<def.slope_k2<<"  "<<def.windmax<<"\n";
//	cout<<"default values"<<landscape.def_.veg_fuel_weighting<<" "<<landscape.def_.ndays_average<<" "<<landscape.def_.load_k1<<" "<<landscape.def_.spread_calc_type<<" "<<landscape.def_.mean_log_wind<<" "<<landscape.def_.sd_log_wind<<" "<<landscape.def_.mean1_rvm<<" "<<landscape.def_.mean2_rvm<<" "<<landscape.def_.kappa1_rvm<<" "<<landscape.def_.kappa2_rvm<<" "<<landscape.def_.p_rvm<<"\n";
	if(def.fire_verbose==1)
		cout<<"default values"<<def_.veg_fuel_weighting<<" "<<def_.ndays_average<<" "<<def_.load_k1<<" "<<def_.spread_calc_type<<" "<<def_.mean_log_wind<<" "<<def_.sd_log_wind<<" "<<def_.mean1_rvm<<" "<<def_.mean2_rvm<<" "<<def_.kappa1_rvm<<" "<<def_.kappa2_rvm<<" "<<def_.p_rvm<<"\n";

 }
/*****************************Reset**************************************/
/* resets the landscape to initialize the next fire history				*/
/************************************************************************/
void LandScape::Reset()	// just to fill in the raster fire object.  Called when the Raster grid is pre-processed
{
	for(int i=0; i<rows_; i++)	//then, for each row, allocate an array with the # of columns.  this is now a 2-D array of fireGrids
	{
		for(int j=0; j<cols_; j++)	// fill in the landscape information for each pixel
		{
			fireGrid_[i][j].burn=0;		// 0 indicates that the pixel has not been burned
			localFireGrid_[i][j].iter=-1;
//			cout<<fireGrid_[i][j].burn<<"\t";
			fireGrid_[i][j].wind_direction=fireGrid_[i][j].wind_direction*3.141593/180; // transform wind direction to radians, for RHESSys
			// for debugging:
//			cout<<"moistures: "<<fireGrid_[i][j].fuel_moist<<"  loads: "<<fireGrid_[i][j].fuel_litter<<"  ";
		}
	}
	return ;
}




/************************* burn_landscape *******************************/
/* Takes the details of a single fire and propagates it across the		*/
/* cur_LandScape.  The details include ignition point and fire size		*/
/* This function calls BurnCells, which iterates through the cells that */
/* had been burned the previous year and tests their neighbors for		*/
/* fire spread. The first iteration, this is just the ignition point.	*/
/* For each subsequent iteration, the linked list of burned cells is	*/
/* re-set to be the new burned cells.									*/
/*																		*/
/* called by main()														*/
/************************************************************************/
void LandScape::Burn(GenerateRandom& rng)	// to be called in main, to replace RandomScar mk: ? RandomScar?
{
// the ignition point gives the row,column array index for the landscape
// then, to navigate the array for fire spread, take a rook approach (no diagonals)
// keep track of the number of iterations, and the iteration at which it is burned becomes the value of the array
// if it is tested and not burned, keep it at -1 because it can be tested again if another neighbor gets burned
// But, once the neighbors of a burned pixel are tested, that pixel can no longer spread fires.

		// for debugging: 
	if(def_.fire_verbose==1)
		cout<<"Defaults: moisture k1 and k2, load k1"<<def_.moisture_k1<<" "<<def_.moisture_k2<<" "<<def_.load_k1<<"\n";

	
	for(int i=0;i<rows_;i++)	// this loop re-sets the landscape to completely un-burned
	{
		for(int j=0;j<cols_;j++)
			fireGrid_[i][j].burn=0;
	}
	if(def_.fire_verbose==1)
		cout<<"in burn after setting burn=0--here\n\n";
	// hold all of the information for the burning fire in the cur_fire_ object, and retain it only if the fire reaches the appropriate size
	int cur_row = int (cur_fire_.ignRow);		
	int cur_col = int (cur_fire_.ignCol);

	if(def_.fire_verbose==1)
		cout<<"in burn before testIgnition: \n\n";
	int ign;
	ign=testIgnition(cur_row,cur_col,rng); // test whether the fire successfully ignites based on the fuel moisture and fuel load
	if(def_.fire_verbose==1)
		cout<<"in burn after testIgnition: "<<ign<<"\n\n";
	int stop = 0;
	int iter = 0;	
	if(ign==1)
	{
//		fireGrid_[cur_row][cur_col].burn=1; // 1 indicates that this cell has been burned.
//		calc_FireEffects(cur_row, cur_col,iter);
		firstBurned_.clear(); // reset the firstBurned_ vector
		for(int i = 0; i < 4; ++i)
			borders_[i] = 0;

		BurnedCells bc = {cur_row, cur_col};
		firstBurned_.push_back(bc);
		// continue to propagate the fire until one of the stopping conditions is met.
		//  where an iteration is a set of burned cells, beginning with one burned cell
		//  and its neighbors.  the next iteration will be the set of new burned cells and
		//  these are tested.  the next iteration will be the set of cells burned from the previous iteration

		while(stop==0)	 
		{
			iter++;
			stop = BurnCells(iter, rng);	// this function will navigate the vector of burned cells and test spread for each, as well as update the area burned for the current year. it will also test for stopping
		}
/*		int temp_borders=0;
		for(int i=0; i<4; i++)
			temp_borders = temp_borders + borders_[i];
		if(temp_borders==4)
			stop=5;
allow fire to continue burning even if all borders are reached, so the only way to stop is to have no successful tests of spread*/ 	
		cur_fire_.stop=stop;
	}
	return ;
}

/************************* burnCells ****************************************/
/* This function navigates the linked list of cells that were burned the	*/
/* previous iteration.  For each, the neighbor cells are identified and 	*/
/* tested for fire spread.  A new linked-list is initialized and updated	*/
/* with the new burned cells, to be spread from the next iteration.			*/
/*																			*/
/* called by burn_landscape()												*/
/****************************************************************************/
int LandScape::BurnCells(int iter, GenerateRandom& rng)
{
//	cout<<"In BurnCells\n";
	int stop;
	int new_row,new_col; // to track the indices of the x and y arrays neighboring the current cell, to be updated for each new cell

	int add_row[4]={1,-1,0,0};	// to calculate the neighbor indices in the x-direction, orthogonal only
	int add_col[4]={0,0,1,-1};	// to calculate the neighbor indices in the y-direction, orthogonal only
	double fire_dir[4]={0,3.1416,4.712,1.5708};  // the orientation of the neighbor pixels, as defined by add_row 
											// and add_col, in rad.  So, the pixel above (row -1) means the fire is moving
											// from the south, in line with a southerly wind (pi)
	int i;
	int test_burn,burned;
	int currentBorders[4]={0};	// this will keep track of which borders are reached by the fire in the current iteration

	int numBurnedThisIter=0;
	int test_once=0;
	double cur_pBurn;

	std::vector<BurnedCells> burnedThisTime;

// mk: now this loop will start at zero, and run the length of the firstBurned vector
	for(size_t x = 0; x < firstBurned_.size(); ++x)
	{
		for(i=0;i<4;i++)	// for each cell that was burned the previous iteration, navigate the 4 neighbors
		{
			test_burn=1;		// this will be set to 0 if the current cell is on any of the borders, or if the neighbor has already been burned
								// some conditions include whether you are at the border of the landscape, whether the neighboring cell is already burned

			new_row=firstBurned_[x].rowId+add_row[i];  // calculate the array indices of the neighbor cell					
			new_col=firstBurned_[x].colId+add_col[i];

			if(new_row<0)						// if any of the indices are outside of the raster area, then clearly don't burn
			{
				test_burn=0;
				currentBorders[0]=1;
				borders_[0]=1;
			}
			if(new_col<0) 
			{
				test_burn=0;
				currentBorders[2]=1;
				borders_[2]=1;
			}

			if(new_row>=rows_)
			{
				test_burn=0;
				currentBorders[1]=1;
				borders_[1]=1;
			}
			if(new_col>=cols_)
			{
				test_burn=0;
				currentBorders[3]=1;
				borders_[3]=1;
			}

			if(test_burn==1&&fireGrid_[new_row][new_col].burn==0) // only test if it is not already burned, and not beyond the border
			{
				test_once=test_once+1;

				cur_pBurn=calc_pSpreadTest(firstBurned_[x].rowId, firstBurned_[x].colId, new_row, new_col, fire_dir[i]); // mk: calculate the spread probability for this combination of idx/idy and new_idx/new_idy
				burned = IsBurned(rng, cur_pBurn); // mk: need to merge IsBurned with BurnTest
				if(burned==1)	// if 1 is returned, then burn the cell and update the new linked list of burned cells
				{
					calc_FireEffects(new_row, new_col,iter,cur_pBurn);

					numBurnedThisIter=numBurnedThisIter+1; // update the number burned
					BurnedCells bc = {new_row, new_col};
					burnedThisTime.push_back(bc);
				}
			}
	
		}
	}
	firstBurned_= burnedThisTime;
	stop=TestFireStop(numBurnedThisIter,test_once,currentBorders);

	return stop;
}


/******************** IsBurned **********************************************/
/* This function tests a runif (0,1) against the burn probability (bp) of	*/
/* the current cell.  The cell is burned if the random value is < the bp.	*/
/*																			*/
/* called from burnCells(), and returns 1 if the cell should be burned, 0		*/
/* otherwise.																*/
/****************************************************************************/
bool LandScape::IsBurned(GenerateRandom& rng,double cur_pBurn)	// test whether the current pixel gets burned
{
	double test=rng();
	return test <= cur_pBurn ? true : false;
}


/************************************************************************************/
/* tests whether fire spread should cease											*/
/************************************************************************************/
int LandScape::TestFireStop(int numBurnedThisIter,int test_once,int borders[4])
{
	int stop=0; // stop is 0 unless one of these conditions is met
	if(numBurnedThisIter==0)	// stop fire spread if there are no new cells burned
	{
		if(test_once>0)
			stop=2;
		else stop=4;
	}

/*	int num_borders=0;
	for(int i=0;i<4;i++)
		num_borders=num_borders+borders[i];
	if(num_borders==4)	// stop fire spread if the fire has touched all four borders in one iteration
		stop=3;*/
	return stop;
}

/***************************initializeCurrentFire********************************/
/* reset the values of the cur_fire_ object to be updated with a new fire		*/
/********************************************************************************/
void LandScape::initializeCurrentFire(GenerateRandom& rng)
{
	fire_years fire;
// allow for expanding the code to allow for set ignitions, but requires modifying the default file.
	if(def_.ignition_col>=0&&def_.ignition_col<cols_&&def_.ignition_row>=0&&def_.ignition_row<rows_)
	{
		fire.ignRow=def_.ignition_row;
		fire.ignCol=def_.ignition_col;
	}
	else
	{
		int vecID=0;
		vecID=int(floor(rng()*(n_ign_+1)));
		fire.ignRow=ignCells_[vecID].rowId;
		fire.ignCol=ignCells_[vecID].colId;
	//	fire.ignRow=buffer_+(rows_-2*buffer_)*rng();
	//	fire.ignCol=buffer_+(cols_-2*buffer_)*rng();
	}
	
	if(def_.fire_verbose==1)
		cout<<"ignition row and column: "<<fire.ignRow<<"\t"<<fire.ignCol<<"\n";
//	cur_fire_.year=0;	//which year is this
	fire.update_size=0;	// what is the size of the fire that is being burned, to be updated each iteration and be tested against size
	fire.stop=0; // record the stopping rule for this fire
	
	if(def_.mean_log_wind>=0)
	{
		fire.windspeed=exp(def_.mean_log_wind+gasdev(rng)*def_.sd_log_wind);
		fire.winddir=rvmdev(rng,def_.mean1_rvm,def_.mean2_rvm,def_.kappa1_rvm,def_.kappa2_rvm,def_.p_rvm);
	}
	else 
	{
		fire.windspeed=-1; // then fill in the wind speed and direction in calc_pSpreadTest based on the grid-level values
		fire.winddir=-1;
	}
	if(def_.fire_verbose==1)
		cout<<"pvm here?: "<<def_.p_rvm<<"\n";
	
	if(def_.fire_verbose==1)
		cout<<"wind speed: "<<fire.windspeed<<"\nwinddir: "<<fire.winddir<<"\n";
	
	cur_fire_=fire;
//	cur_fire_.numUnburnedPatches=0;
//	cur_fire_.patches.clear();// rest the patches vector to be empty

	return ;
}
 
/**************************calc_pSpreadTest*******************************/
/* implements the fire spread strategy by updating pSpread each year,	*/
/* depending on spread strategy indicated in the configuration file		*/
/* There will be two functions.  This one is called each "year", or the	*/
/* time steps between fires.  The next will be called for each cell that */
/* is tested for fire spread											*/
/************************************************************************/
double LandScape::calc_pSpreadTest(int cur_row, int cur_col,int new_row,int new_col,double fire_dir)
{
	double temp_pBurn=0;
	double ind=1;
	double slope;
	double p_slope,p_winddir,p_moisture,p_load,winddir,windspeed,k1wind;
	double cur_load,cur_moist;
	slope=(fireGrid_[new_row][new_col].z-fireGrid_[cur_row][cur_col].z)/cell_res_; // for now, just the orthogonal
								//neighbors, so the slope is just the difference in elevation divided by the distance 
	//	cout<<"winddir: "<<winddir<<"\t";
	if(slope<=0)
		ind=-1;
	p_slope=def_.slope_k1*exp(ind*def_.slope_k2*pow(slope,2)); // pSpread due to the slope
	if(p_slope>1) // ensure that the slope function stays between 0,1
		p_slope=1;
	if(p_slope<0)
		p_slope=0;
	
	if(cur_fire_.winddir>=0)
	{
		winddir=cur_fire_.winddir;//fireGrid_[cur_row][cur_col].wind_direction;							//between the cells (the resolution)
		if(cur_fire_.windspeed<=def_.windmax)
			windspeed=cur_fire_.windspeed/def_.windmax;
		else
			windspeed=1;
	}
	else
	{
		winddir=fireGrid_[cur_row][cur_col].wind_direction;//*3.141593/180;	
		cur_fire_.windspeed=fireGrid_[cur_row][cur_col].wind;		//between the cells (the resolution)
		if(cur_fire_.windspeed<=def_.windmax)
			windspeed=cur_fire_.windspeed/def_.windmax;
		else
			windspeed=1;
	}
//	cout<<"Windspeed and winddirection: "<<windspeed<<"   "<<winddir<<"\t";
	k1wind=def_.winddir_k1*windspeed;
//	p_winddir=def_.winddir_k2+def_.winddir_k1*(1+cos(fire_dir-winddir)); // pSpread due to the orientation of the cells relative to the wind direction
	p_winddir=def_.winddir_k2+k1wind *(1+cos(fire_dir-winddir)); // pSpread due to the orientation of the cells relative to the wind direction
		// still need to figure out wind speed...
	if(p_winddir>1) // ensure that the slope function stays between 0,1
		p_winddir=1;
	if(p_winddir<0)
		p_winddir=0;
	if(def_.spread_calc_type<4)
		p_moisture=1-1/(1+exp(-(def_.moisture_k1*(fireGrid_[new_row][new_col].fuel_moist-def_.moisture_k2))));
	else // use deficit
	{
		if(def_.spread_calc_type<7) // absolute difference
			cur_moist=fireGrid_[new_row][new_col].pet-fireGrid_[new_row][new_col].et;
		else // et relative to pt
		{
			if(fireGrid_[new_row][new_col].pet>0)
				cur_moist=1-fireGrid_[new_row][new_col].et/(fireGrid_[new_row][new_col].pet); // for now see if fixes
			else
				cur_moist=0;
		}
	//	cout<<"deficit calculated, et, pet: "<<cur_moist<<"   "<<fireGrid_[new_row][new_col].et<<"   "<<fireGrid_[new_row][new_col].pet<<"\t";
		p_moisture=1/(1+exp(-(def_.moisture_k1*(cur_moist-def_.moisture_k2)))); //use deficit for moisture status
	}	
	cur_load=(1-def_.veg_fuel_weighting)*fireGrid_[new_row][new_col].fuel_litter+(def_.veg_fuel_weighting)*fireGrid_[new_row][new_col].fuel_veg; // modify this to always include all of the litter fuels and some proportion up to 1 of the veg fuels
	p_load=1/(1+exp(-(def_.load_k1*(cur_load-def_.load_k2))));

	switch(def_.spread_calc_type)
	{
	case 1: // multiplicative
		temp_pBurn=p_slope*p_winddir*p_moisture*p_load; // if including wind direction, the overall pSpread is the product of the individual pSpreads.
		break;
	case 2: // minimum
		temp_pBurn=1;
		if(p_slope<temp_pBurn)
			temp_pBurn=p_slope;
		if(p_winddir<temp_pBurn)
			temp_pBurn=p_winddir;
		if(p_moisture<temp_pBurn)
			temp_pBurn=p_moisture;
		if(p_load<temp_pBurn)
			temp_pBurn=p_load;		
		break;
	case 3: // mean
		temp_pBurn=(p_slope+p_winddir+p_moisture+p_load)/4; 
		break;
	case 4: // multiplicative with def
		temp_pBurn=p_slope*p_winddir*p_moisture*p_load; 
		break;
	case 5: // minimum with def
		temp_pBurn=1;
		if(p_slope<temp_pBurn)
			temp_pBurn=p_slope;
		if(p_winddir<temp_pBurn)
			temp_pBurn=p_winddir;
		if(p_moisture<temp_pBurn)
			temp_pBurn=p_moisture;
		if(p_load<temp_pBurn)
			temp_pBurn=p_load;		
		break;
	case 6: // mean with def
		temp_pBurn=(p_slope+p_winddir+p_moisture+p_load)/4; // if including wind direction, the overall pSpread is the product of the individual pSpreads.
		break;
	case 7: // multiplicative with relative def
		temp_pBurn=p_slope*p_winddir*p_moisture*p_load; 
		break;
	case 8: // minimum with relative def
		temp_pBurn=1;
		if(p_slope<temp_pBurn)
			temp_pBurn=p_slope;
		if(p_winddir<temp_pBurn)
			temp_pBurn=p_winddir;
		if(p_moisture<temp_pBurn)
			temp_pBurn=p_moisture;
		if(p_load<temp_pBurn)
			temp_pBurn=p_load;		
		break;
	case 9: // mean with realtive def
		temp_pBurn=(p_slope+p_winddir+p_moisture+p_load)/4; // 
		break;
	default: // for now default to multiplicative
		temp_pBurn=p_slope*p_winddir*p_moisture*p_load; // if including wind direction, the overall pSpread is the product of the individual pSpreads.
	}		
	
	// Here we would put other pSpread calculations, depending on the parent node and the node being tested for spread
//	cout<<"burn test, slope: "<<p_slope<<" wind: "<<p_winddir<<" moisture: "<<p_moisture<<" load: "<<p_load<<"\n";
	return temp_pBurn;
}

/*************************testIgnition*********************/
/**********************************************************/
int LandScape::testIgnition(int cur_row, int cur_col, GenerateRandom& rng) // need the moisture and load values for the pixel, as well as the rng
{
//	cout<<"in test ignition row: "<<cur_row<<"col: "<<cur_col<<"  temp: "<<fireGrid_[cur_row][cur_col].temp<<"  minTemp"<<def_.ignition_tmin<<"\n\n";
	int ign=0;
	double pIgn=0;
	double p_moisture,p_load,cur_load,cur_moist,p_veg;

	double ign_moisture_k2=def_.moisture_k2*def_.ign_def_mod;
	if(ign_moisture_k2>1)
		ign_moisture_k2=1;
	// for WMFire, do we make this based on load and moisture (probably)
	if(fireGrid_[cur_row][cur_col].temp>=def_.ignition_tmin)
	{
//		cout<<"In if\n";
		if(def_.spread_calc_type<4)
		{
			p_moisture=1-1/(1+exp(-(def_.moisture_k1*(fireGrid_[cur_row][cur_col].fuel_moist-def_.moisture_k2))));
			cur_moist=fireGrid_[cur_row][cur_col].pet-fireGrid_[cur_row][cur_col].fuel_moist;
//			cout<<"wrong spread calc type: "<<def_.spread_calc_type<<"\n";
		}
		else
		{
			if(def_.spread_calc_type<7)
				cur_moist=fireGrid_[cur_row][cur_col].pet-fireGrid_[cur_row][cur_col].et; // use absolute deficit for fm
			else
			{
				if(fireGrid_[cur_row][cur_col].pet>0)
					cur_moist=1-fireGrid_[cur_row][cur_col].et/(fireGrid_[cur_row][cur_col].pet); // for now, see if it solves the problem
				else
					cur_moist=0;
			}
			p_moisture=1/(1+exp(-(def_.moisture_k1*(cur_moist-def_.moisture_k2)))); //use relative deficit for moisture status
			if(def_.fire_verbose==1)
				cout<<"using deficit for moisture: cur_moist, et, pet: "<<cur_moist<<"  "<<fireGrid_[cur_row][cur_col].et<<"   "<<fireGrid_[cur_row][cur_col].pet<<"\n";
		}	

		if(def_.fire_verbose==1)
			cout<<"in test ignition p_moisture: "<<p_moisture<<"moisture: "<<cur_moist<<"\n\n";
		cur_load=(1-def_.veg_fuel_weighting)*fireGrid_[cur_row][cur_col].fuel_litter+(def_.veg_fuel_weighting)*fireGrid_[cur_row][cur_col].fuel_veg;
		p_load=1/(1+exp(-(def_.load_k1*(cur_load-def_.load_k2))));
		if(def_.fire_verbose==1)
			cout<<"in test ignition p_load "<<p_load<<"load: "<<cur_load<<"\n\n";
		if(def_.veg_ign==1)
		{
			p_veg=1-1/(1+exp(-(def_.veg_k1*(fireGrid_[cur_row][cur_col].fuel_veg-def_.veg_k2))));
			pIgn=p_moisture*p_load*p_veg;
		}
		else
			pIgn=p_moisture*p_load;


		double test=rng();
		if(test<=pIgn)
		{
			ign=1;
			fireGrid_[cur_row][cur_col].burn=pIgn;
		}
	}
	if(def_.fire_verbose==1)
		cout<<"in test ignition pIgn: "<<pIgn<<"ign: "<<ign<<"temperature: "<<fireGrid_[cur_row][cur_col].temp<<"\n\n";
	return ign;
}


/**********calc_FireEffects*************************************************/
/* for each pixel that a fire reaches, calculate the fire effects			*/
/* for now the effects are simply 0=unburned, 1=burned			*/
/***************************************************************************/
void LandScape::calc_FireEffects(int new_row,int new_col, int iter, double cur_pBurn)
{
//	fireGrid_[new_row][new_col].burn=1;	// update the land array to indicate this cell is burned during this iteration
	fireGrid_[new_row][new_col].burn=cur_pBurn;	// update the land array to indicate this cell is burned during this iteration, and the associated probability
	localFireGrid_[new_row][new_col].iter=iter;
	cur_fire_.update_size++;	// add a pixel to the current fire size
	return ;
}

/***************write the fire grid******************************************/
/* with the date written to the file								*/
/***************************************************************************/
void LandScape::writeFire(long month, long year,struct fire_default def)
{
//	char const* outFile;
	std::string curFile;
	std::string curPropFile;
	
	std::string curMonth;
	std::stringstream out;
	out<<month;
	curMonth=out.str();
	
	std::string curYear;
	std::stringstream outYear;
	outYear<<year;
	curYear=outYear.str();
	
	curFile.assign("FireSpreadIterGridYear");
	curFile.append(curYear);
	curFile.append("Month");
	curFile.append(curMonth);
	curFile.append(".txt");

	curPropFile.assign("FireSpreadPropGridYear");
	curPropFile.append(curYear);
	curPropFile.append("Month");
	curPropFile.append(curMonth);
	curPropFile.append(".txt");
	
	if(def_.fire_write>1)
	{
		ofstream fireOut;
		fireOut.open(curFile.c_str());
		
		ofstream firePropOut;
		firePropOut.open(curPropFile.c_str());
		
	//	fireOut.open("FireSpreadGrid.txt");
		for(int i=0; i<rows_; i++)	//then, for each row, allocate an array with the # of columns.  this is now a 2-D array of fireGrids
		{
			for(int j=0; j<cols_; j++)	// fill in the landscape information for each pixel
			{
				fireOut<<localFireGrid_[i][j].iter<<"\t";
				firePropOut<<fireGrid_[i][j].burn<<"\t";
			}
			fireOut<<"\n";
			firePropOut<<"\n";
		}
		fireOut.close();
		firePropOut.close();
	}

	cout<<"Year: "<<year<<" Month: "<<month<<"\n\n";

	if(def_.fire_write>2)
	{
		curFile.assign("LoadGridYear");
		curFile.append(curYear);
		curFile.append("Month");
		curFile.append(curMonth);
		curFile.append(".txt");

		ofstream fireOut;
		fireOut.open(curFile.c_str());
		
		
//		fireOut.open("FireSpreadGrid.txt");
		for(int i=0; i<rows_; i++)	//then, for each row, allocate an array with the # of columns.  this is now a 2-D array of fireGrids
		{
			for(int j=0; j<cols_; j++)	// fill in the landscape information for each pixel
			{
				fireOut<<fireGrid_[i][j].fuel_litter<<"\t";
			}
			fireOut<<"\n";
		}
		fireOut.close();

	curFile.assign("SoilMoistGridYear");
		curFile.append(curYear);
		curFile.append("Month");
		curFile.append(curMonth);
		curFile.append(".txt");

		ofstream soilMoistOut;
		soilMoistOut.open(curFile.c_str());
		
		
//		fireOut.open("FireSpreadGrid.txt");
		for(int i=0; i<rows_; i++)	//then, for each row, allocate an array with the # of columns.  this is now a 2-D array of fireGrids
		{
			for(int j=0; j<cols_; j++)	// fill in the landscape information for each pixel
			{
				soilMoistOut<<fireGrid_[i][j].soil_moist<<"\t";
			}
			soilMoistOut<<"\n";
		}
		soilMoistOut.close();


		curFile.assign("VegLoadGridYear");
		curFile.append(curYear);
		curFile.append("Month");
		curFile.append(curMonth);
		curFile.append(".txt");

	//	ofstream fireOut;
		fireOut.open(curFile.c_str());
		for(int i=0; i<rows_; i++)	//then, for each row, allocate an array with the # of columns.  this is now a 2-D array of fireGrids
		{
			for(int j=0; j<cols_; j++)	// fill in the landscape information for each pixel
			{
				fireOut<<fireGrid_[i][j].fuel_veg<<"\t";
			}
			fireOut<<"\n";
		}
		fireOut.close();

		
		curFile.assign("RelDefGridYear");
		curFile.append(curYear);
		curFile.append("Month");
		curFile.append(curMonth);
		curFile.append(".txt");

	//	ofstream fireOut;
		fireOut.open(curFile.c_str());
		for(int i=0; i<rows_; i++)	//then, for each row, allocate an array with the # of columns.  this is now a 2-D array of fireGrids
		{
			for(int j=0; j<cols_; j++)	// fill in the landscape information for each pixel
			{
				if(def_.spread_calc_type<4)
					fireOut<<fireGrid_[i][j].fuel_moist<<"\t";
				else
				{
					if(def_.spread_calc_type<7)
						fireOut<<fireGrid_[i][j].pet-fireGrid_[i][j].et<<"\t";
					else
					{
					if(fireGrid_[i][j].pet>0)
						fireOut<<1-fireGrid_[i][j].et/(fireGrid_[i][j].pet)<<"\t";
					else
						fireOut<<0<<"\t";
					}
				}
	
			}
			fireOut<<"\n";
		}
		fireOut.close();
		
		curFile.assign("PETGridYear");
		curFile.append(curYear);
		curFile.append("Month");
		curFile.append(curMonth);
		curFile.append(".txt");

	//	ofstream fireOut;
		fireOut.open(curFile.c_str());
		for(int i=0; i<rows_; i++)	//then, for each row, allocate an array with the # of columns.  this is now a 2-D array of fireGrids
		{
			for(int j=0; j<cols_; j++)	// fill in the landscape information for each pixel
			{
				fireOut<<fireGrid_[i][j].pet<<"\t";
				}
			fireOut<<"\n";
		}
		fireOut.close();

		curFile.assign("ETGridYear");
		curFile.append(curYear);
		curFile.append("Month");
		curFile.append(curMonth);
		curFile.append(".txt");

	//	ofstream fireOut;
		fireOut.open(curFile.c_str());
		for(int i=0; i<rows_; i++)	//then, for each row, allocate an array with the # of columns.  this is now a 2-D array of fireGrids
		{
			for(int j=0; j<cols_; j++)	// fill in the landscape information for each pixel
			{
				fireOut<<fireGrid_[i][j].et<<"\t";
				}
			fireOut<<"\n";
		}
		fireOut.close();	
	}

	ofstream demOut;
	demOut.open("RhessysDemInWMFire.txt");
	for(int i=0; i<rows_; i++)	//then, for each row, allocate an array with the # of columns.  this is now a 2-D array of fireGrids
	{
		for(int j=0; j<cols_; j++)	// fill in the landscape information for each pixel
		{
			demOut<<fireGrid_[i][j].z<<"\t";
		}
		demOut<<"\n";
	}
	demOut.close();
	if(def_.fire_write>0)
	{
		ofstream sizeOut;
		sizeOut.open("FireSizes.txt", ofstream::app);
		sizeOut<<cur_fire_.update_size<<"\t"<<year<<"\t"<<month<<"\n";
		sizeOut.close();
	}	
	
	return ;
}

/************************************************************************/
/* end WMFire.cpp													*/
/************************************************************************/

