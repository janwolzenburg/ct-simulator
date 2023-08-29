/*********************************************************************
 * @file   generelMath.hpp
 * @brief  Template implementations
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/

 #include "generelMath.h"


template <typename T>
typename std::enable_if_t<std::is_arithmetic_v<T>, bool> 
isEven( const T integer ){
	return (long long) integer % 2 == 0;
}

template <typename T>
typename std::enable_if_t<std::is_arithmetic_v<T>, T> 
Min( const T a, const T b ){
	if( a < b ) return a;
	return b;
}

template <typename T>
typename std::enable_if_t<std::is_arithmetic_v<T>, T> 
Max( const T a, const T b ){
	if( a >= b ) return a;
	return b;
}


template <typename T>
typename std::enable_if_t<std::is_arithmetic_v<T>, T>
Fmax( const T value, const T maximum ){
	return Min( value, maximum );
}

template <typename T>
typename std::enable_if_t<std::is_arithmetic_v<T>, T>  
Fmin1( T a ){
	return Max( (T) 1, a );
}

template <typename T>
typename std::enable_if_t<std::is_arithmetic_v<T>, T>
Fmin( T a, T minValue ){
	return Max( minValue, a );
}


template <typename T>
typename std::enable_if_t<std::is_arithmetic_v<T>, T>
Frange( const T value, const T minimum, const T maximum ){

	return Fmin<T>( Fmax<T>( value, maximum ), minimum );

}

template <typename T>
typename std::enable_if_t<std::is_arithmetic_v<T>, T>  Fpos( T a ){
	if( a <= (T) 0 ) return (T) 1;
	return a;
}

template <typename T>
typename std::enable_if_t<std::is_integral_v<T>, T>  FOdd( T a ){
	if( a % 2 == 0 ) return a + 1;
	return a;
}



template <typename T>
double relDeviation( const T a, const T b ){

	return (double) std::abs( a - b ) / (double) std::abs( b );

}