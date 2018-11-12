/*--------------------------------------------------------------*/
/*								 								*/
/*		alloc.c													*/
/*																*/
/*	alloc.c - attempts to alloacte a portion of ram				*/
/*																*/
/*	NAME														*/
/*	alloc.c - attempts to alloacte a portion of ram				*/
/*																*/
/*	SYNOPSIS													*/
/*	void	*alloc( size_t, char*, char* )						*/
/*																*/
/*	OPTIONS														*/
/*	size_t	size	- size of array in bytes					*/
/*	char	*array_name - calling function name					*/
/*	char	*calling_function	- message for stderr 			*/
/*																*/
/*	DESCRIPTION													*/
/*	This routine attempts to use malloc to allocate and			*/
/*	array of "size" bytes in RAM.  If the requested size		*/
/*	is zero the routine returns a null pointer instead of 		*/
/*	using malloc. If the requested size is non-zero malloc		*/
/*	is used.  If malloc returns a NULL pointer a fatal			*/
/* 	error results.												*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*	The routine performs as follows:							*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void	*alloc(size_t size, char *array_name, char *calling_function)
{
	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	void	*array;
	
	/*-------------------------------------------------*/
	/*	Check if the requested array is of size 0 		*/
	/*-------------------------------------------------*/
	if ( size == 0 ){
		/*----------------------------------------------*/
		/*		We return NULL if array size is zero.		*/
		/*----------------------------------------------*/
		return( NULL );
	}
	else{
		/*--------------------------------------------------------------*/
		/*		The array size is not zero so call malloc.	*/
		/*--------------------------------------------------------------*/
		array = malloc(size);
		/*--------------------------------------------------------------*/
		/*		Check if malloc was unable to allocate array.	*/
		/*--------------------------------------------------------------*/
		if ( array == NULL ){
			/*--------------------------------------------------------------*/
			/*			Report fatal error message if needed.	*/
			/*--------------------------------------------------------------*/
			fprintf(stderr,
				"FATAL ERROR: in malloc, unable to allocate %s for %s\n",
				array_name, calling_function );
			exit(EXIT_FAILURE);
		}
		else{
			/*--------------------------------------------------------------*/
			/*		Initialize array to zero								*/
			/*--------------------------------------------------------------*/
			memset(array, 0, size);
			/*--------------------------------------------------------------*/
			/*			Return pointer to allocated array.	*/
			/*--------------------------------------------------------------*/
			return(array);
		}
	}
} /*end alloc.c*/
#ifdef LIU_NETCDF_READER
int is_approximately(const double value,const double target,const double tolerance)
{ return ((value) < ((target) +(tolerance))) && ((value) > ((target) - (tolerance)));}
#endif

