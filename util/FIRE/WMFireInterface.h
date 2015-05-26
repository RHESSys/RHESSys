#pragma once

#include "rhessys_fire.h"

#ifdef __cplusplus
extern "C" {
#endif


#if defined(_WIN32) || defined(__WIN32__)
  #if defined(WMFIRE_EXPORTS)
    #define  WMFIRE_EXPORT __declspec(dllexport)
  #else
    #define  WMFIRE_EXPORT  __declspec(dllimport)
  #endif
 #elif defined(linux) || defined(__linux)
  #define WMFIRE_EXPORT
#endif

//WMFIRE_EXPORT void WMFire(fire_object** &fire_grid,const fire_default &def, double cell_res,int nrow, int ncol);
struct fire_object** WMFire(double cell_res, int nrow, int ncol, long year,long month, struct fire_object** fire_grid,struct fire_default def);

#ifdef __cplusplus
}
#endif
