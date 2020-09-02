#ifndef INCLUDE_TYPES_H_
#define INCLUDE_TYPES_H_

/*----------------------------------------------------------*/
/*      Define types                                        */
/*----------------------------------------------------------*/
typedef short bool;
static const short true = 1;
static const short false = 0;

typedef enum {
	DATA_TYPE_UNDEFINED,
	DATA_TYPE_BOOL,
	DATA_TYPE_CHAR,
	DATA_TYPE_CHAR_ARRAY,
	DATA_TYPE_INT,
	DATA_TYPE_LONG,
	DATA_TYPE_LONG_ARRAY,
	DATA_TYPE_FLOAT,
	DATA_TYPE_DOUBLE,
	DATA_TYPE_DOUBLE_ARRAY,
	DATA_TYPE_FILE_PTR,
	DATA_TYPE_STRUCT,
	DATA_TYPE_STRUCT_ARRAY,
	DATA_TYPE_STRUCT_PTR_ARRAY
} DataType;

#endif /* INCLUDE_TYPES_H_ */
