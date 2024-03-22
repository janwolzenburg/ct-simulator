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
IsEven( const T integer ){
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
ForceToMax( const T value, const T maximum ){
	return Min( value, maximum );
}

template <typename T>
typename std::enable_if_t<std::is_arithmetic_v<T>, T>  
ForceToMin1( T a ){
	return Max( (T) 1, a );
}

template <typename T>
typename std::enable_if_t<std::is_arithmetic_v<T>, T>
ForceToMin( T a, T minValue ){
	return Max( minValue, a );
}

template <typename T>
typename std::enable_if_t<std::is_arithmetic_v<T>, T>
ForceRange( const T value, const T minimum, const T maximum ){

	return ForceToMin<T>( ForceToMax<T>( value, maximum ), minimum );

}

template <typename T>
typename std::enable_if_t<std::is_arithmetic_v<T>, T>  ForcePositive( T a ){
	if( a <= (T) 0 ) return (T) 1;
	return a;
}

template <typename T>
typename std::enable_if_t<std::is_integral_v<T>, T>  ForceOdd( T a ){
	if( a % 2 == 0 ) return a + 1;
	return a;
}

template <typename T>
typename std::enable_if_t<std::is_integral_v<T>, T>  ForceEven( T a ){
	if( a % 2 != 0 ) return a + 1;
	return a;
}

template <typename T>
double RelativeDeviation( const T a, const T b ){
	return static_cast<double>( std::abs( a - b ) ) / static_cast<double>( std::abs( b ) );
}