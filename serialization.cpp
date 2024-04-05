/*********************************************************************
 * @file   serialization.cpp
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
 * @brief character to pad string when serializing. only for easier reading of files in hex-editor
*/
constexpr char string_padding = static_cast<char>( 0x9D );


template<>
size_t SerializeBuildIn<string>( const string& string_to_serialize, vector<char>& binary_data ){

	size_t character_index = 0;
	const size_t padding = 24;	// amount of character in each "line" of hex-editor

	// insert padding so string start at "new line" when each line has 24 bytes
	binary_data.insert( binary_data.end(), ( padding - ( binary_data.size() ) % padding ) % padding, string_padding );

	// iterate all characters in string
	for( const char c : string_to_serialize ){
		character_index++;
		binary_data.push_back( c );
	}

	// add padding at the end so data start at a new line
	binary_data.insert( binary_data.end(), ( padding - ( binary_data.size() + 1 ) % padding ) % padding, string_padding );

	// termination
	binary_data.push_back( '\0' );

	return character_index;
}


template<>
size_t SerializeBuildIn<vector<vector<GridCoordinates>>>( const vector<vector<GridCoordinates>>& vector_to_serialize, vector<char>& binary_data ){

	size_t index = 0;

	// amount of sub vectors
	index += SerializeBuildIn<size_t>( vector_to_serialize.size(), binary_data );

	// iterate all sub vectors
	for( const vector<GridCoordinates>& sub_vector : vector_to_serialize ){

		// amount of elements in current sub vector
		index += SerializeBuildIn<size_t>( sub_vector.size(), binary_data );
		
		// serialize each element
		for( const GridCoordinates& value : sub_vector ){
			index += value.Serialize( binary_data );
		}

	}

	return index;

}


template<>
size_t DeSerializeBuildIn<string>( string& string_to_serialize, string default_value, const vector<char>& binary_data, vector<char>::const_iterator& current_byte ){

	size_t character_index = 0;

	// make sure string is empty
	string_to_serialize.clear();

	// loop while temrination character not read and the end has not been reached
	while( *current_byte != '\0' && current_byte < binary_data.end() ){
		
		// the current char
		const char current_character = *( current_byte++ );

		// if character is not string padding add to string
		if( current_character != string_padding ) string_to_serialize.push_back( current_character );
		character_index++;
	}

	// end reached before termination
	if( *current_byte != '\0' ){
		// set to default value
		string_to_serialize = default_value;
		return string_to_serialize.size();
	}

	// skip '\0' so the iterator is pointing to next data_
	if( current_byte < binary_data.end() ) current_byte++;

	return character_index;
}


template<>
vector<vector<GridCoordinates>> DeSerialize<vector<vector<GridCoordinates>>>( const vector<char>& binary_data, vector<char>::const_iterator& current_byte ){

	// amount sub vectors
	size_t amount_sub_vectors = DeSerializeBuildIn<size_t>( 0, binary_data, current_byte );
	
	// instance to return
	vector<vector<GridCoordinates>> deserialsized_vector;

	if( amount_sub_vectors > maximum_vector_size )
		return deserialsized_vector;

	// loop fo amount of sub vectors
	for( size_t i = 0; i < amount_sub_vectors; i++ ){

		// amount of elements in sub vector
		size_t numElements = DeSerializeBuildIn<size_t>( 0, binary_data, current_byte );
		
		if( numElements > maximum_vector_size )
		return deserialsized_vector;

		// initialise sub vector
		vector<GridCoordinates> subVec( numElements, GridCoordinates( 0., 0. ) );

		// assign deserialsized data to sub vector elements 
		for( size_t j = 0; j < numElements; j++ ){
			subVec.at( j ) =  GridCoordinates{ binary_data, current_byte };
		}

		// add sub vector
		deserialsized_vector.push_back( subVec );
	}

	return deserialsized_vector;
}


bool ExportSerialized( const path file_path, const vector<char>& binary_data ){

	if( file_path.empty() ) return false;

	// storage directory when current_byte does not exist
	if( !std::filesystem::exists( file_path.parent_path()  )) std::filesystem::create_directory( file_path.parent_path() );

	return ExportSerialized( file_path.string(), binary_data );

}


bool ExportSerialized( const string file_name, const vector<char>& binary_data ){
	
	// file handle
	std::ofstream output_file;
	
	// overwrite existing and open as binary
	output_file.open( file_name, std::ios::trunc | std::ios::binary );
	if( output_file.fail() ) return false;

	// write data_
	output_file.write( (char*) binary_data.data(), binary_data.size() );
	
	if( output_file.fail() ) return false;
	output_file.close();

	return true;
}


vector<char> ImportSerialized( const path file_path ){

	return ImportSerialized( file_path.string() );

}


vector<char> ImportSerialized( const string file_name ){

	std::ifstream input_file;
	input_file.open( file_name, std::ios::binary );
	if( input_file.fail() ) return vector<char>();

	// get file size_
	size_t file_size = std::filesystem::file_size( file_name );
	
	// allocate memory and read
	char* raw_file_data = new char[ file_size ];
	input_file.read( raw_file_data, file_size );

	if( input_file.eof() ){
		input_file.close();
		return vector<char>();
	}

	// write to binary data_
	vector<char> binary_data{ raw_file_data, raw_file_data + file_size };

	delete[] raw_file_data;

	return binary_data;

}


bool IsValidBinaryData( const string preamble, const vector<char>& binary_data, vector<char>::const_iterator& current_byte ){
	string read_preamble;
	
	// read file preamble
	DeSerializeBuildIn<string>( read_preamble, string{}, binary_data, current_byte );

	// compare
	return preamble == read_preamble;

}