/*********************************************************************
 * @file   serialization.hpp
 * @brief  
 *
 * @author Jan Wolzenburg
 * @date   October 2023
 *********************************************************************/


 /*********************************************************************
  Includes
*********************************************************************/

#include "serialization.h"


/*********************************************************************
  Declerations
*********************************************************************/


template< typename T >
size_t SerializeBuildIn( const T& val, vector<char>& binary_data ){

	char* valStartPtr = (char*) &val;

	binary_data.insert( binary_data.end(), valStartPtr, valStartPtr + sizeof( T ) );

	return sizeof(T);

}

template< typename T >
size_t DeSerializeBuildIn( T& val, T defaultVal, const vector<char>& binary_data, vector<char>::const_iterator& it ){

	val = 0;

	// Not enough data_ left in vector
	if( binary_data.cend() - it < sizeof( T ) ) val = defaultVal;
	else{
		val = *( (T*) &(*it) );
		it += sizeof( T );
	}

	return sizeof(T);
}

template< typename T >
T DeSerializeBuildIn( T defaultVal, const vector<char>& binary_data, vector<char>::const_iterator& it ){

	T temp{};
	
	DeSerializeBuildIn( temp, defaultVal, binary_data, it );

	return temp;

}


template< typename T >
T DeSerialize( const vector<char>& binary_data, vector<char>::const_iterator& it ){

	return T{ binary_data, it };

}
