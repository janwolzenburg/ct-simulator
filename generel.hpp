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
	for( ; i < sizeof( T ); i++ ) binData.push_back( *( ( (char*) &val ) + ( sizeof( T ) - 1 - i ) ) );

	return i;
}

template< typename T >
size_t deSerializeBuildIn( T& val, T defaultVal, const vector<char>& binData, vector<char>::const_iterator& it ){

	val = 0;

	size_t i = 0;
	for( ; i < sizeof( T ) && it != binData.cend(); i++ ){
		char* byte = (char*) &val + ( sizeof( T ) - 1 - i );
		*byte = *( it++ );
	}

	// Set to default val
	if( i != sizeof( T ) ) val = defaultVal;

	return i;
}