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
	const size_t padding = 24;	// amount of character in each "Line" of hex-editor

	// insert padding so string start at "new Line" when each Line has 24 Bytes
	binary_data.insert( binary_data.end(), ( padding - ( binary_data.size() ) % padding ) % padding, string_padding );

	// iterate all characters in string
	for( const char c : val ){
		i++;
		binary_data.push_back( c );
	}


	// add padding at the end so data start at a new Line
	binary_data.insert( binary_data.end(), ( padding - ( binary_data.size() + 1 ) % padding ) % padding, string_padding );

	// termination
	binary_data.push_back( '\0' );

	return i;
}

template<>
size_t SerializeBuildIn<vector<vector<GridCoordinates>>>( const vector<vector<GridCoordinates>>& vec, vector<char>& binary_data ){

	size_t i = 0;

	// amount of sub vectors
	i += SerializeBuildIn<size_t>( vec.size(), binary_data );

	// iterate all sub vectors
	for( const vector<GridCoordinates>& subVec : vec ){

		// amount of elements in current sub vector
		i += SerializeBuildIn<size_t>( subVec.size(), binary_data );
		
		// serialize each element
		for( const GridCoordinates& value : subVec ){
			i +=value.Serialize( binary_data );
		}

	}

	return i;

}


template<>
size_t DeSerializeBuildIn<string>( string& value, string default_value, const vector<char>& binary_data, vector<char>::const_iterator& it ){

	size_t i = 0;

	// make sure string is empty
	value.clear();

	// loop while temrination character not read and the end has not been reached
	while( *it != '\0' && it < binary_data.end() ){
		
		// the current char
		char current_character = *( it++ );

		// if character is not string padding add to string
		if( current_character != string_padding ) value.push_back( current_character );
		i++;
	}

	// end reached before termination
	if( *it != '\0' ){
		// set to default value
		value = default_value;
		return value.size();
	}

	// skip '\0' so the iterator is pointing to next data_
	if( it < binary_data.end() ) it++;

	return i;
}

template<>
vector<vector<GridCoordinates>> DeSerialize<vector<vector<GridCoordinates>>>( const vector<char>& binary_data, vector<char>::const_iterator& it ){

	// amount sub vectors
	size_t amount_sub_vectors = DeSerializeBuildIn<size_t>( 0, binary_data, it );
	
	// instance to return
	vector<vector<GridCoordinates>> vec;

	if( amount_sub_vectors > maximum_vector_size )
		return vec;

	// loop fo amount of sub vectors
	for( size_t i = 0; i < amount_sub_vectors; i++ ){

		// amount of elements in sub vector
		size_t numElements = DeSerializeBuildIn<size_t>( 0, binary_data, it );
		
		if( numElements > maximum_vector_size )
		return vec;

		// initialise sub vector
		vector<GridCoordinates> subVec( numElements, GridCoordinates( 0., 0. ) );

		// assign deserialsized data to sub vector elements 
		for( size_t j = 0; j < numElements; j++ ){
			subVec.at( j ) =  GridCoordinates{ binary_data, it };
		}

		// add sub vector
		vec.push_back( subVec );
	}

	return vec;
}

bool ExportSerialized( const path file_path, const vector<char>& binary_data ){

	if( file_path.empty() ) return false;

	// storage directory when it does not exist
	if( !std::filesystem::exists( file_path.parent_path()  )) std::filesystem::create_directory( file_path.parent_path() );

	return ExportSerialized( file_path.string(), binary_data );

}

bool ExportSerialized( const string file_name, const vector<char>& binary_data ){
	
	// file handle
	std::ofstream outFile;
	
	// overwrite existing and open as binary
	outFile.open( file_name, std::ios::trunc | std::ios::binary );
	if( outFile.fail() ) return false;

	// write data_
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

	// get file size_
	size_t file_size = std::filesystem::file_size( file_name );
	
	// allocate memory and read
	char* dArray = new char[ file_size ];
	inFile.read( dArray, file_size );

	if( inFile.eof() ){
		inFile.close();
		return vector<char>();
	}

	// write to binary data_
	vector<char> binary_data{ dArray, dArray + file_size };

	delete[] dArray;

	return binary_data;

}


bool ValidBinaryData( const string preamble, const vector<char>& binary_data, vector<char>::const_iterator& it ){
	string readPreamble;
	
	// read file preamble
	DeSerializeBuildIn<string>( readPreamble, string{}, binary_data, it );

	// compare
	return preamble == readPreamble;

}