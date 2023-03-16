/*********************************************************************
 * @file   generel.hpp
 * @brief  Template implementations
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/

#include <type_traits>
#include "generel.h"


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
size_t serializeBuildIn( const T val, vector<char>& binData ){

	size_t i = 0;

	char* valStartPtr = (char*) &val;

	binData.insert( binData.end(), valStartPtr, valStartPtr + sizeof( T ) );

	return sizeof(T);

}

template< typename T >
size_t deSerializeBuildIn( T& val, T defaultVal, const vector<char>& binData, vector<char>::const_iterator& it ){

	val = 0;

	// Not enough data left in vector
	if( binData.cend() - it < sizeof( T ) ) val = defaultVal;

	val = *( (T*) &(*it) );

	it += sizeof( T );

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