#pragma once

#include <vector>
#include <list>
//#include "util.h"
#include "RanNums.h"
#include "boost/multi_array.hpp"
//#include "../util/WMFireInterface.h"
#include "../rhessys/include/WMFireInterface.h"
//#include "../util/rhessys_fire.h" // header file shared between RHESSys and WMFire


/********************************************************************/
/* BurnedCells structure											*/
/*																	*/
/* lists the indices of the x and y arrays for each cell that has	*/
/* been burned and is still to test for spread.						*/
/********************************************************************/
struct BurnedCells
{
	int rowId;	//x index for current burned cell
	int colId; // y index for current burned cell
};

struct IgnitionCells // attributes of cells available for ignition, will draw random ignition from this list
{
	double rowId;	//x index for current cell available for ignition
	double colId; // y index for current cell available for ignition
};

struct LocalFireNodes // internal WMFire grid for processing
{
	int iter; // in which iteration did the cell burn?
	int failedIter; // 
	double pSlope; // probability of spread associated with slope
	double pLoad; // probability of spread associated with fuel load
	double pDef; // probability of spread associated with moisture
	double pWind; // probability of spread associated with wind
	double pUnderDef; // probability of ignition associated with understory moisture condition
    LocalFireNodes() : iter(-1)
    {}
};

/****************************************************************/
/* fire_years structure											*/
/* holds the relevant information for a single fire to be		*/
/* propagated													*/
/****************************************************************/
struct fire_years
{
	double update_size;	// what is the size of the fire that is being burned, to be updated each iteration and be tested against size
	double ignRow;	//the x coordinate of the fire ignition
	double ignCol;	//the y coordinate of the fire ignition
	int stop; // record the stopping rule for this fire
	double winddir; // randomly drawn wind direction for current fire
	double windspeed; //randomly drawn windspeed for current fire

//	fire_years() { ZeroMem(this); }  // why not?
};
typedef std::vector<fire_years> Fires;


/********************************************************************/
/* LandScape class												*/
/*																	*/
/* Information for the landscape, including the dimensions and cell	*/
/* resolution.														*/
/********************************************************************/
typedef boost::multi_array<LocalFireNodes, 2> LocalFireGrid; // the 2-D grid
class LandScape
{
 // mk: so, this is the initializer for when a new LandScape object is created?
public:
	LandScape() : rows_(0), cols_(0), buffer_(5), cell_res_(0) 
	{
		for(size_t i = 0; i < sizeof(borders_)/sizeof(borders_[0]); ++i) // mk: so, this takes the length of the borders vector, divided by the length of the 1st element of the borders vector (so, in case it's a 2-D array?).
		{
			borders_[i] = 0;
		}
	}
	LandScape(double cell_res,struct fire_object **fire_grid,struct fire_default def, int nrow, int ncol); // function to initialize the landscape
	int Rows() const { return rows_; } // number of rows
	int Cols() const { return cols_; } // number of columns
	int BufferValue() const { return buffer_; } 
	double CellResolution() const { return cell_res_; } // cell resolution

	void Reset(); // reset the landscape
 	void Burn(GenerateRandom& rng); // test for spread
	void initializeCurrentFire(GenerateRandom& rng); // initialize fire
	void chooseIgnPix(GenerateRandom& rng); // which pixels are chosen for ignition
	void drawNumIgn(double lambda, GenerateRandom& rng); // to test whether the randomly chosen cell should ignite

	void writeFire(long month, long year,struct fire_default def); // write fire results
	struct fire_object** &FireGrids() { return fireGrid_; } // the fire grid
	fire_default& FireDefault() {return def_;} // fire default values
	fire_years& FireYears() {return cur_fire_; } 
	LocalFireGrid& LocalFireGrids() { return localFireGrid_; }


private:
	int rows_;    // to record the number of rows and columns in the landscape
	int cols_; 
	int buffer_;     // how many cells from edge will ignitions not be allowed?
	double cell_res_; // the resolution of each pixel, so the area of a pixel is cell_res_^2
	int n_ign_; // the number of cells available for ignition
	int n_cur_ign_; // a random draw for the number of ignitions to test for this run
	int borders_[4]; // indicates whether all borders have been reached
	std::vector<BurnedCells> firstBurned_; // vector of cells burned the previous iteration available to be sources of spread for the next iteration
	std::vector<IgnitionCells> ignCells_; // locations of all random ignitions
	fire_object **fireGrid_;	// 2-D array of pixels for the current landscape, FireNodes above
	fire_default def_; // fire default values
	fire_years cur_fire_; // the fire currently burning.
    // private methods
	double GetBurnProb(double probability, GenerateRandom& rng); 
	bool IsBurned(GenerateRandom& rng,double cur_pBurn);
	int BurnCells(int iter,GenerateRandom& rng); // runs through the vector of source cells for spread and tests neighbors
	int testIgnition(int cur_row, int cur_col, GenerateRandom& rng); // to test whether the randomly chosen cell should ignite
	double calc_pSpreadTest(int cur_row, int cur_col, int new_row, int new_col,double fire_dir); // calculate p_spread based on conditions of the pixel
	double calc_FireSuppression(double pBurn, int cur_row, int cur_col,double fire_dir);
	void calc_FireEffects(int new_row,int new_col, int iter,double cur_pBurn); // updates the grid to return a measure of fire effects, in this case the p_spread value
	int TestFireStop(int numBurnedThisIter,int test_once,int borders[4]); // test whether conditions are met for stopping the fire
	LocalFireGrid localFireGrid_;	// 2-D array of pixels for the current landscape, FireNodes above
};
/********************************************************************/
/*	end WMFire.h								*/
/********************************************************************/


