/*********************************************************************
 * @file   generelMath.hpp
 * @brief  Template implementations
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/

 #include "generelMath.h"


template <typename T>
typename std::enable_if_t<std::is_arithmetic_v<T>, bool> isEven( const T integer ){
	return integer % 2 == 0;
}

template <typename T>
typename std::enable_if_t<std::is_arithmetic_v<T>, T> min( const T a, const T b ){
	if( a < b ) return a;
	return b;
}

template <typename T>
typename std::enable_if_t<std::is_arithmetic_v<T>, T> max( const T a, const T b ){
	if( a >= b ) return a;
	return b;
}


template <typename T>
typename std::enable_if_t<std::is_arithmetic_v<T>, T>
Fmax( const T value, const T maximum ){
	return min( value, maximum )
}

template <typename T>
typename std::enable_if_t<std::is_arithmetic_v<T>, T>  Fmin1( T a ){
	return max( (T) 1, a );
}

template <typename T>
typename std::enable_if_t<std::is_arithmetic_v<T>, T>
Fmin( T a, T minValue ){
	return max( minValue, a );
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