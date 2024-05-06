/*********************************************************************
 * @file   serialization.hpp
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
size_t SerializeBuildIn( const T& value, vector<char>& binary_data ){

	char* value_address = (char*) &value;

	binary_data.insert( binary_data.end(), value_address, value_address + sizeof( T ) );

	return sizeof(T);

}

template< typename T >
size_t DeSerializeBuildIn( T& value, T default_value, const vector<char>& binary_data, vector<char>::const_iterator& current_byte ){

	value = 0;

	if( binary_data.cend() < current_byte ){
		value = default_value;
		return sizeof( T );
	}
		

	// not enough data left in vector
	if( static_cast<size_t>( binary_data.cend() - current_byte ) < sizeof( T ) ) value = default_value;
	else{
		value = *( (T*) &( *current_byte ) );
		current_byte += sizeof( T );
	}

	return sizeof(T);
}

template< typename T >
T DeSerializeBuildIn( T default_value, const vector<char>& binary_data, vector<char>::const_iterator& current_byte ){

	T tempory_value{};
	
	DeSerializeBuildIn<T>( tempory_value, default_value, binary_data, current_byte );

	return tempory_value;

}


template< typename T >
T DeSerialize( const vector<char>& binary_data, vector<char>::const_iterator& current_byte ){

	return T{ binary_data, current_byte };

}
