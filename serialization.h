#pragma once
/*********************************************************************
 * @file   serialization.h
 *
 * @author Jan Wolzenburg
 * @date   October 2023
 *********************************************************************/



 /*********************************************************************
	Includes
 *********************************************************************/
 #include "generel.h"


 /*********************************************************************
	Definitions
 *********************************************************************/


/*!
 * @brief serialize build in datatype
 * @tparam T type of variable
 * @param value value
 * @param binary_data vector to append binary data
 * @return amount of bytes appended
*/
template< typename T >
size_t SerializeBuildIn( const T& value, vector<char>& binary_data );

/*!
 * @brief serialize string
 * @param value String to serialize
 * @param binary_data reference to vector to append to
 * @return amount of bytes appended
*/
template<>
size_t SerializeBuildIn<string>( const string& string_to_serialize, vector<char>& binary_data );

/*!
 * @brief serialize 2D-vector of grid points
 * @param vector_to_serialize object to serialize
 * @param binary_data reference to vector to append to
 * @return amount of bytes appended
*/
template<>
size_t SerializeBuildIn<vector<vector<GridCoordinates>>>( const vector<vector<GridCoordinates>>& vector_to_serialize, vector<char>& binary_data );

/*!
 * @brief deserialize build in data type
 * @tparam T expected type
 * @param value reference to write value to
 * @param default_value default value when reading fails
 * @param binary_data vector with binary data
 * @param current_byte iterator to start reading from. will be advanced
 * @return amount of bytes read
*/
template< typename T >
size_t DeSerializeBuildIn( T& value, T default_value, const vector<char>& binary_data, vector<char>::const_iterator& current_byte );

/*!
 * @brief deserialize string
 * @param string_to_serialize reference to write value to
 * @param default_string default value when reading fails
 * @param binary_data vector with binary data
 * @param current_byte iterator to start reading from. will be advanced
 * @return amount of bytes read
 */
template<>
size_t DeSerializeBuildIn<string>( string& string_to_serialize, string default_string, const vector<char>& binary_data, vector<char>::const_iterator& current_byte );

/*!
 * @brief deserialize build in data type
 * @tparam T type
 * @param default_value default value when reading fails
 * @param binary_data vector with binary data
 * @param current_byte iterator to start reading from. will be advanced
 * @return deserialized object
*/
template< typename T>
T DeSerializeBuildIn( T default_value, const vector<char>& binary_data, vector<char>::const_iterator& current_byte );

/*!
 * @brief deserialize and return object
 * @tparam T type
 * @param binary_data vector with binary data
 * @param current_byte iterator to start reading from. will be advanced
 * @return deserialized object
*/
template< typename T>
T DeSerialize( const vector<char>& binary_data, vector<char>::const_iterator& current_byte );

/*!
 * @brief deserialize 2D-vector of grid coordinates
 * @param binary_data vector with binary data
 * @param current_byte iterator to start reading from. will be advanced
 * @return deserialized object
 */
template<>
vector<vector<GridCoordinates>> DeSerialize<vector<vector<GridCoordinates>>>( const vector<char>& binary_data, vector<char>::const_iterator& current_byte );


/*!
 * @brief export serial data to file
 * @param file_name filename
 * @param binary_data vector with binary data
 * @return true when file found
*/
bool ExportSerialized( const string file_name, const vector<char>& binary_data );

/*!
 * @brief export serial data to file
 * @param file_path path to file
 * @param binary_data vector with binary data
 * @return true when file found
*/
bool ExportSerialized( const path file_path, const vector<char>& binary_data );


/*!
 * @brief import serial data from file
 * @param file_name filename
 * @return vector with data
*/
vector<char> ImportSerialized( const string file_name );

/*!
 * @brief import serial data from file
 * @param file_path path to file
 * @return vector with data
*/
vector<char> ImportSerialized( const path file_path );

/*!
	* @brief check if data in vector is from a valid file
	* @param preamble preamble to check for
	* @param binary_data reference to vector with binary data
	* @param current_byte iterator to start of data in vector
	* @return true when preambles match
*/
bool IsValidBinaryData( const string preamble, const vector<char>& binary_data, vector<char>::const_iterator& current_byte );

#include "serialization.hpp"