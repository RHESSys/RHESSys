#pragma once
/****************************************************************/
/* ranNums.h													*/
/*																*/
/****************************************************************/
#include "boost/random.hpp"
typedef boost::variate_generator<boost::mt19937&, boost::uniform_01<> > GenerateRandom;

#define PI 3.141592

// From numerical recipes in C.  Used in random number generation.
double poisdev(double xm, GenerateRandom rng);
//double expdev(double lambda, long *idum);
double expdev(double ia, double lambda, GenerateRandom rng);
double gammln(double xx);
double gasdev(GenerateRandom rng);
double paretodev(GenerateRandom rng,double alpha,double xmin);
double rvmdev(GenerateRandom rng,double mean1, double mean2, double kappa1, double kappa2, double p);
