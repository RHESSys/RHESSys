// as taken from https://stackoverflow.com/questions/1300180/ignore-openmp-on-machine-that-does-not-have-it

#if defined(_OPENMP)
#include <omp.h>
#else
typedef int omp_int_t;
inline omp_int_t omp_get_thread_num(void) { return 0;}
inline omp_int_t omp_get_max_threads(void) { return 1;}
#endif
