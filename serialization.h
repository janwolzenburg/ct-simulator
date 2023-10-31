#pragma once
/*********************************************************************
 * @file   serialization.h
 * @brief  
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
 * @brief Serialize build in datatype
 * @tparam T Type of variable
 * @param val Value
 * @param binary_data Vector to append binary data_
 * @return Amount of bytes appended
*/
template< typename T >
size_t SerializeBuildIn( const T& val, vector<char>& binary_data );

/*!
 * @brief Serialize string
 * @param val String to serialize
 * @param binary_data Reference to vector to append to
 * @return Amount of bytes appended
*/
template<>
size_t SerializeBuildIn<string>( const string& val, vector<char>& binary_data );

/*!
 * @brief Serialize 2D-vector of grid points
 * @param val Object to serialize
 * @param binary_data Reference to vector to append to
 * @return Amount of bytes appended
*/
template<>
size_t SerializeBuildIn<vector<vector<GridCoordinates>>>( const vector<vector<GridCoordinates>>& val, vector<char>& binary_data );

/*!
 * @brief Deserialize build in data type
 * @tparam T Expected type
 * @param val Reference to write value to
 * @param binary_data Vector with binary data_
 * @param it Iterator to start reading from. Will be advanced
 * @return Amount of bytes read
*/
template< typename T >
size_t DeSerializeBuildIn( T& val, T defaultVal, const vector<char>& binary_data, vector<char>::const_iterator& current_byte );

template<>
size_t DeSerializeBuildIn<string>( string& val, string defaultVal, const vector<char>& binary_data, vector<char>::const_iterator& current_byte );

/*!
 * @brief Deserialize build in data type
 * @tparam T Type
 * @param defaultVal Default value 
 * @param binary_data Vector with binary data_
 * @param it Iterator to start reading from. Will be advanced
 * @return Deserialized object
*/
template< typename T>
T DeSerializeBuildIn( T defaultVal, const vector<char>& binary_data, vector<char>::const_iterator& current_byte );

/*!
 * @brief Deserialize and return object
 * @tparam T Type
 * @param binary_data Binary data 
 * @param current_byte Iterator
 * @return Deserialized object
*/
template< typename T>
T DeSerialize( const vector<char>& binary_data, vector<char>::const_iterator& current_byte );

template<>
vector<vector<GridCoordinates>> DeSerialize<vector<vector<GridCoordinates>>>( const vector<char>& binary_data, vector<char>::const_iterator& current_byte );


/*!
 * @brief Export serial data to file
 * @param file_name Filename
 * @param binary_data Vector with binary data
 * @return True when file found
*/
bool ExportSerialized( const string file_name, const vector<char>& binary_data );

bool ExportSerialized( const path file_path, const vector<char>& binary_data );


/*!
 * @brief Import serial data from file
 * @param file_name Filename
 * @return Vector with data
*/
vector<char> ImportSerialized( const string file_name );

vector<char> ImportSerialized( const path file_path );

/*!
	* @brief Check if data in vector is from a valid file
	* @param binary_data Reference to vector with binary data
	* @param it Iterator to start of data in vector
	* @return True when preambles match
*/
bool ValidBinaryData( const string preamble, const vector<char>& binary_data, vector<char>::const_iterator& current_byte );

#include "Serialization.hpp"