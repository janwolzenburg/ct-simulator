/*********************************************************************
 * @file   generelMath.hpp
 * @brief  Template implementations
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/

 #include "generelMath.h"


template <typename T>
bool isEven( const T integer ){ 
	return integer % 2 == 0;
}


template <typename T>
T max( const T a, const T b ){
	if( a >= b ) return a;
	return b;
}


template <typename T>
T Fmin1( T a ){
	return max( (T) 1, a );
}


template <typename T>
T Fpos( T a ){
	if( a <= (T) 0 ) return (T) 1;
	return a;
}