/*********************************************************************
 * @file   generel.hpp
 * @brief  Template implementations
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/


 /*********************************************************************
  Includes
*********************************************************************/

#include "generel.h"


/*********************************************************************
  Declerations
*********************************************************************/

template <typename T>
constexpr typename std::underlying_type_t<T> ConvertToUnderlying( T var ){
	return static_cast<typename std::underlying_type_t<T>>( var );
}

template <typename T>
typename std::enable_if_t<std::is_enum_v<T>, T> operator++( T& var ){
	var = T( ConvertToUnderlying( var ) + 1 );
	return var;
}

template <typename T>
typename std::enable_if_t<std::is_enum_v<T>, T> operator-( const T val1, const T val2 ){
	T diff = ConvertToUnderlying( val1 ) - ConvertToUnderlying( val2 );
	return diff;
}

template<typename T>
string ConvertToString<T>( T value, const int precision ){

	std::ostringstream out;

	if( precision < 0 ){

		T factor = static_cast<T>( pow( 10., static_cast<double>( -precision - 1 ) ) );

		value /= factor;

		value = static_cast<T>( round( value ) );
		value *= factor;

		out.precision( 0 );
	}
	else{
		out.precision( precision );
		out << std::fixed;
		out << value;
	}

	return out.str();
}

template<typename T>
T WriteThreadVar( T& var, const T& value, mutex& m ){

	m.lock();

	var = value;

	m.unlock();

	return value;
}