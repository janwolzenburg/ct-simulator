/*********************************************************************
 * @file   serialization.cpp
 * @brief  
 *
 * @author Jan Wolzenburg
 * @date   October 2023
 *********************************************************************/


/*********************************************************************
	Includes
 *********************************************************************/

#include "serialization.h"
#include <fstream>


/*********************************************************************
	Implementations
 *********************************************************************/

 

/*!
 * @brief Character to pad string when serializing. Only for easier reading of files in hex-editor
*/
constexpr char string_padding = static_cast<char>( 0x9D );

 
/*!
 * Serialization implementation
*/

template<>
size_t SerializeBuildIn<string>( const string& val, vector<char>& binary_data ){

	size_t i = 0;
	const size_t padding = 24;	// Amount of character in each "line" of hex-editor

	// Insert padding so string start at "new line" when each line has 24 Bytes
	binary_data.insert( binary_data.end(), ( padding - ( binary_data.size() ) % padding ) % padding, string_padding );

	// Iterate all characters in string
	for( const char c : val ){
		i++;
		binary_data.push_back( c );
	}


	// Add padding at the end so data start at a new line
	binary_data.insert( binary_data.end(), ( padding - ( binary_data.size() + 1 ) % padding ) % padding, string_padding );

	// Termination
	binary_data.push_back( '\0' );

	return i;
}

template<>
size_t SerializeBuildIn<vector<vector<GridCoordinates>>>( const vector<vector<GridCoordinates>>& vec, vector<char>& binary_data ){

	size_t i = 0;

	// Amount of sub vectors
	i += SerializeBuildIn<size_t>( vec.size(), binary_data );

	// Iterate all sub vectors
	for( const vector<GridCoordinates>& subVec : vec ){

		// Amount of elements in current sub vector
		i += SerializeBuildIn<size_t>( subVec.size(), binary_data );
		
		// Serialize each element
		for( const GridCoordinates& value : subVec ){
			i +=value.Serialize( binary_data );
		}

	}

	return i;

}


template<>
size_t DeSerializeBuildIn<string>( string& value, string default_value, const vector<char>& binary_data, vector<char>::const_iterator& it ){

	size_t i = 0;

	// Make sure string is empty
	value.clear();

	// Loop while temrination character not read and the end has not been reached
	while( *it != '\0' && it < binary_data.end() ){
		
		// The current char
		char current_character = *( it++ );

		// If character is not string padding add to string
		if( current_character != string_padding ) value.push_back( current_character );
		i++;
	}

	// End reached before termination
	if( *it != '\0' ){
		// Set to default value
		value = default_value;
		return value.size();
	}

	// Skip '\0' so the iterator is pointing to next data
	if( it < binary_data.end() ) it++;

	return i;
}

template<>
vector<vector<GridCoordinates>> DeSerialize<vector<vector<GridCoordinates>>>( const vector<char>& binary_data, vector<char>::const_iterator& it ){

	// Amount sub vectors
	size_t amount_sub_vectors = DeSerializeBuildIn<size_t>( (size_t) 0, binary_data, it );
	
	// Instance to return
	vector<vector<GridCoordinates>> vec;

	// Loop fo amount of sub vectors
	for( size_t i = 0; i < amount_sub_vectors; i++ ){

		// Amount of elements in sub vector
		size_t numElements = DeSerializeBuildIn<size_t>( (size_t) 0, binary_data, it );
		
		// Initialise sub vector
		vector<GridCoordinates> subVec( numElements, GridCoordinates( 0., 0. ) );

		// Assign deserialsized data to sub vector elements 
		for( size_t j = 0; j < numElements; j++ ){
			subVec.at( j ) =  GridCoordinates{ binary_data, it };
		}

		// Add sub vector
		vec.push_back( subVec );
	}

	return vec;
}

bool ExportSerialized( const path file_path, const vector<char>& binary_data ){

	// Storage directory when it does not exist
	if( !std::filesystem::exists( file_path.parent_path()  )) std::filesystem::create_directory( file_path.parent_path() );

	return ExportSerialized( file_path.string(), binary_data );

}

bool ExportSerialized( const string file_name, const vector<char>& binary_data ){
	
	// File handle
	std::ofstream outFile;
	
	// Overwrite existing and open as binary
	outFile.open( file_name, std::ios::trunc | std::ios::binary );
	if( outFile.fail() ) return false;

	// Write data
	outFile.write( (char*) binary_data.data(), binary_data.size() );
	
	if( outFile.fail() ) return false;
	outFile.close();

	return true;
}

vector<char> ImportSerialized( const path file_path ){

	return ImportSerialized( file_path.string() );

}

vector<char> ImportSerialized( const string file_name ){

	std::ifstream inFile;
	inFile.open( file_name, std::ios::binary );
	if( inFile.fail() ) return vector<char>();

	// Get file size
	size_t file_size = std::filesystem::file_size( file_name );
	
	// Allocate memory and read
	char* dArray = new char[ file_size ];
	inFile.read( dArray, file_size );

	if( inFile.eof() ){
		inFile.close();
		return vector<char>();
	}

	// Write to binary data
	vector<char> binary_data{ dArray, dArray + file_size };

	delete[] dArray;

	return std::move( binary_data );

}


bool ValidBinaryData( const string preamble, const vector<char>& binary_data, vector<char>::const_iterator& it ){
	string readPreamble;
	
	// Read file preamble
	DeSerializeBuildIn( readPreamble, string{}, binary_data, it );

	// Compare
	return preamble == readPreamble;

}