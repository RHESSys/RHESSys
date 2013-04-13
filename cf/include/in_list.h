/** @file in_list.h
 *	@brief Determines if a value is in a list of a given length
 * 	returns the position in the list if found otherwise
 *	returns a zero value
 */
#ifndef IN_LIST_H
#define IN_LIST_H

/** @brief Determine if a value is in an array of integers
 *
 * 	@param value The value to be found in the array
 * 	@param list The array of integers to search
 * 	@param length The length of the array
 *
 *	@return If the value is found, the index in the array of the value is returned,
 *	otherwise 0 is returned
 *
 *	@note Values in list are assumed to be zero indexed.
 */
int in_list(int value, int *list, int length);

#endif
