#include "RanNums.h"
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <cstdio>
#include <iostream>

using std::cout;
using std::stringstream;
using std::fstream;
using std::ifstream;
using std::ofstream;


/******************************************** expdev ***********************/
/*	From Numerical Recipes in C, pp 287. Returns a gamma deviate for small */
/*	integer alpha as the sum of alpha exponential deviates 				   */
/*	(with rate parameter lambda).										   */
/*																		   */
/*	Called by:	shoot_ini(); shoot_extension()							   */
/*	Calls:	ran3(); ran2()												   */
/***************************************************************************/
double expdev(double ia, double lambda, GenerateRandom rng)
// Returns gamma deviates for small integer alpha as the sum of exponential   
// deviates.  For alpha=1 simply an exponential deviate
{
	double gam;
	double dum=rng();
	int j;

	for (j=2;j<=ia;j++) dum*=rng(); // uses seed


//		gam=-1/lambda*log(dum*lambda);
	gam=-1/lambda*log(dum);
	return gam;

} // end expdev

/********************************************** poisdev ********************/
/*	From Numerical Reciped in C, pp 294.  Returns a poisson random deviate */
/*	with "xm" as the rate parameter.									   */
/*																		   */
/*																		   */
/*	Calls:	ran3(); ran2()												   */
/***************************************************************************/
double poisdev(double xm, GenerateRandom rng)
// returns as a floating point number an integer value that is a random 
// deviate drawn from a Poisson distribution with 
// lambda=xm, using ran3 as a source of uniform random deviates; 
// uses rejection method as outlined in book
{
	static double sq,alxm,g,oldm=(-1,0);
	double em,t,y;

	if (xm<12.0)	//****MCK: need to double-check why this if statement is here.
	{
		if (xm!=oldm)
		{
			oldm=xm;
			g=exp(-xm);
		}

		em=-1;
		t=1.0;
		do
		{
			++em;
			t*=rng();
		}
		while (t>g);
	}
	else
	{
		if(xm!=oldm)
		{
			oldm=xm;
			sq=sqrt(2.0*xm);
			alxm=log(xm);
			g=xm*alxm-gammln(xm+1);
		}
		do
		{
			do
			{
				y=tan(PI*rng());
				em=sq*y+xm;
			} while(em<0);
			em=floor(em);
			t=0.9*(1.0+y*y)*exp(em*alxm-gammln(em+1)-g);
		} while(rng()>t);
	}
	return em;
} // end poisdev


/**********************************************************************/
/* from Numerical Recipes in C, p. 214								*/
/********************************************************************/
double gammln(double xx)
{
	double x,y,tmp,ser;
	static double cof[6]={76.18009172947146,-86.50532032941677,24.01409824083091,-1.231739572450155,0.1208650973866179e-2,-0.5395239384953e-5};
	int j;
	y=x=xx;
	tmp=x+5.5;
	tmp-=(x+.5)*log(tmp);
	ser=1.000000000190015;
	for(j=0;j<=5;j++) ser+=cof[j]/++y;
	return -tmp+log(2.5066282746310005*ser/x);
}

/********************* gasdev() *********************************/
/* returns single rnorm(0,1)									*/
/* from Numerical Recipes in C, p. 289							*/
/****************************************************************/
double gasdev(GenerateRandom rng)
{
	static int iset=0;
	static double gset;
	double fac,rsq,v1,v2;

	if(rng()<0) iset=0;
	if(iset==0)
	{
		do
		{ 
			v1=2.0*rng()-1.0;
			v2=2.0*rng()-1.0;
			rsq=v1*v1+v2*v2;
		} while (rsq>=1.0||rsq==0.0);
		fac=sqrt(-2.0*log(rsq)/rsq);
		gset=v1*fac;
		iset=1;
		return v2*fac;
	}
	else
	{
		iset=0;
		return gset;
	}
}

/***********************paretodev********************************/
/* uses the property of CDFs that generates a random number		*/
/* by drawing a number from a uniform distribution, then 		*/
/* inserting that into the inverse of the CDF for a given 		*/
/* distribution function, the Pareto in this case.				*/
/****************************************************************/
double paretodev(GenerateRandom rng, double alpha, double xmin)
{
	double pareto_val;
	pareto_val=exp(1/(1-alpha)*log((1-rng())/pow(xmin,(alpha-1))));
	return pareto_val;
}

/********************rvmdev***********************************/
/* generates random mixed vonMises distribution for circular data. */
/* cribbed from the rvm function in CircStats package in R		*/
/* p is the proportion of draws expected from mean1			*/
/**********************************************************************/
double rvmdev(GenerateRandom rng,double mean1, double mean2, double kappa1, double kappa2, double p)
{
	double test0, test1,test2,test3,curMean=mean2,curKappa=kappa2;
	test0=rng();
	if(test0<=p)
	{
		curMean=mean1;
		curKappa=kappa1;
	}
	
	cout<<"p: "<<p<<" mean: "<<curMean<<" kappa: "<<curKappa<<"\n";
	double rvm_val=1;
	
	int vm=1,flag=1;
	double a=1+ pow(1+4*pow(curKappa,2),0.5);
	double b=(a-pow(2*a,0.5))/(2*curKappa);
	double r=(1+pow(b,2))/(2*b);
	
	double tmpVal=0,tmpMod;
	while(flag==1)
	{
		test1=rng();
		double z=cos(3.141593*test1);
		double f=(1+r*z)/(r+z);
		double c=curKappa*(r-f);
		test2=rng();
		if((c*(2-c)-test2)>0)
		{
			test3=rng();
			if((test3-0.5)<0)
				tmpVal=-1;
			else
				tmpVal=1;
			cout<<"rvm_val1: "<<rvm_val<<" acosf "<<acos(f)<<"\n";
			rvm_val=tmpVal*acos(f)+curMean; // sign!
			cout<<"rvm_val1: "<<rvm_val<<" acosf "<<acos(f)<<"\n";
			if(rvm_val<0)
			{
				rvm_val=ceil(-rvm_val/(2*3.141593))*2*3.141593+rvm_val;
			}
			else
			{
				rvm_val=rvm_val-floor(rvm_val/(2*3.141593))*2*3.141593;
			}
		//	rvm_val=fmod((rvm_val),(2*3.141593)); // %%!
			flag=0;
			cout<<"testing rvmdev1: test0:"<<test0<<" test1 "<<test1<<" a "<<a<<" b "<<b<<" r "<<r<<" z "<<z<<" f  "<<f<<" test2 "<<test2<<" c "<<c<<" test3 "<<test3<<" tmpVal "<<tmpVal<<" rvm_val "<<rvm_val<<" mean "<<curMean<<" kappa "<<curKappa<<" \n";
		}
		else
		{
			if((log(c/test2)+1-c)>=0)
			{
				test3=rng();
				if((test3-0.5)<0)
					tmpVal=-1;
				else
					tmpVal=1;
				cout<<"rvm_val2: "<<" acosf "<<acos(f)<<rvm_val<<"\n";
				rvm_val=tmpVal*acos(f)+curMean; // sign!
				cout<<"rvm_val2: "<<" acosf "<<acos(f)<<rvm_val<<"\n";
				if(rvm_val<0)
				{
					rvm_val=ceil(-rvm_val/(2*3.141593))*2*3.141593+rvm_val;
				}
				else
				{
					rvm_val=rvm_val-floor(rvm_val/(2*3.141593))*2*3.141593;
				}
		//	rvm_val=fmod((rvm_val),(2*3.141593)); // %%!
			//rvm_val=fmod(rvm_val,(2*3.141593)); // %%! = modulus I think
				flag=0;
				cout<<"testing rvmdev2: test0:"<<test0<<" test1 "<<test1<<" a "<<a<<" b "<<b<<" r "<<r<<" z "<<z<<" f  "<<f<<" test2 "<<test2<<" c "<<c<<" test3 "<<test3<<" tmpVal "<<tmpVal<<" rvm_val "<<rvm_val<<" mean "<<curMean<<" kappa "<<curKappa<<" \n";
			}
		}
	}
	
	
	return rvm_val;

} // end rvmdev
