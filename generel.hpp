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

#include <type_traits>
#include "generel.h"



/*********************************************************************
  Declerations
*********************************************************************/

template <typename T>
constexpr typename std::underlying_type_t<T> toUnderlying( T var ){
	return static_cast<typename std::underlying_type_t<T>>( var );
}

template <typename T>
typename std::enable_if_t<std::is_enum_v<T>, T> operator++( T& var ){
	var = T( toUnderlying( var ) + 1 );
	return var;
}

template <typename T>
typename std::enable_if_t<std::is_enum_v<T>, T> operator-( const T val1, const T val2 ){
	T diff = toUnderlying( val1 ) - toUnderlying( val2 );
	return diff;
}

template< typename T >
size_t serializeBuildIn( const T& val, vector<char>& binData ){

	char* valStartPtr = (char*) &val;

	binData.insert( binData.end(), valStartPtr, valStartPtr + sizeof( T ) );

	return sizeof(T);

}

template< typename T >
size_t deSerializeBuildIn( T& val, T defaultVal, const vector<char>& binData, vector<char>::const_iterator& it ){

	val = 0;

	// Not enough data left in vector
	if( binData.cend() - it < sizeof( T ) ) val = defaultVal;
	else{
		val = *( (T*) &(*it) );
		it += sizeof( T );
	}

	return sizeof(T);
}

template< typename T >
T deSerializeBuildIn( T defaultVal, const vector<char>& binData, vector<char>::const_iterator& it ){

	T temp{};
	
	deSerializeBuildIn( temp, defaultVal, binData, it );

	return temp;

}


template< typename T >
T deSerialize( const vector<char>& binData, vector<char>::const_iterator& it ){

	return T{ binData, it };

}


template<typename T>
string toString<T>( T value, const int precision ){

	std::ostringstream out;

	if( precision < 0 ){

		T factor = (T) pow( 10., (double) ( -precision - 1 ) );

		value /= factor;

		value = (T) round( value );
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
T writeThreadVar( T& var, const T& value, mutex& m ){

	m.lock();

	var = value;

	m.unlock();

	return value;
}