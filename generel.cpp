/*********************************************************************
 * @file   generel.cpp
 * @brief  Generel function implementation
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/


/*********************************************************************
	Includes
 *********************************************************************/

#include <vector>
using std::vector;
#include <string>
using std::string;
#include <fstream>
#include <filesystem>

#include "generel.h"



/*********************************************************************
	Implementations
 *********************************************************************/

const size_t numThreads = 8;

/*!
 * @brief Character to pad string when serializing. Only for easier reading of files in hex-editor
*/
const char stringPadding = (char) 0x9D;

/*!
 * Indices and vector implementation
*/

idx2::idx2( const size_t x_, const size_t y_ ) : x( x_ ), y( y_ ){};
idx2::idx2( void ) : x( 0 ), y( 0 ){};

v2::v2( const double x_, const double y_ ) : x( x_ ), y( y_ ){};
v2::v2( void ) : v2( 0, 0 ){};


/*!
 * idx3 implementation
*/

idx3::idx3( const size_t x_, const size_t y_, const size_t z_ ) : x( x_ ), y( y_ ), z( z_ ){};
idx3::idx3( void ) : x( 0 ), y( 0 ), z( 0 ){};

idx3::idx3( const vector<char>& binData, vector<char>::const_iterator& it ){

	deSerializeBuildIn( this->x, (size_t) 0, binData, it );
	deSerializeBuildIn( this->y, (size_t) 0, binData, it );
	deSerializeBuildIn( this->z, (size_t) 0, binData, it );

}

bool idx3::operator==( const idx3& second ) const{

	if( x != second.x ) return false;
	if( y != second.y ) return false;
	if( z != second.z ) return false;

	return true;

}

size_t idx3::serialize( vector<char>& binData ) const{

	size_t numBytes = 0;
	numBytes += serializeBuildIn( this->x, binData );
	numBytes += serializeBuildIn( this->y, binData );
	numBytes += serializeBuildIn( this->z, binData );

	return numBytes;
}


/*!
 * v3 implementation
*/

v3::v3( const double x_, const double y_, const double z_ ) : x( x_ ), y( y_ ), z( z_ ){};
v3::v3( void ) : v3( 0, 0, 0 ){};

v3::v3( const vector<char>& binData, vector<char>::const_iterator& it ){

	deSerializeBuildIn( this->x, (double) 0, binData, it );
	deSerializeBuildIn( this->y, (double) 0, binData, it );
	deSerializeBuildIn( this->z, (double) 0, binData, it );

}

size_t v3::serialize( vector<char>& binData ) const{

	size_t numBytes = 0;
	numBytes += serializeBuildIn( this->x, binData );
	numBytes += serializeBuildIn( this->y, binData );
	numBytes += serializeBuildIn( this->z, binData );

	return numBytes;
}


/*!
 * idx2CR implementation
*/

idx2CR::idx2CR( const size_t c_, const size_t r_ ) : col( c_ ), row( r_ ){};
idx2CR::idx2CR( void ) : idx2CR( 0, 0 ){};

idx2CR::idx2CR( const vector<char>& binData, vector<char>::const_iterator& it ){

	deSerializeBuildIn( this->col, (size_t) 0, binData, it );
	deSerializeBuildIn( this->row, (size_t) 0, binData, it );

}

size_t idx2CR::serialize( vector<char>& binData ) const{

	size_t numBytes = 0;
	numBytes += serializeBuildIn( this->col, binData );
	numBytes += serializeBuildIn( this->row, binData );

	return numBytes;
}


/*!
 * v2CR implementation
*/

v2CR::v2CR( const double c_, const double r_ ) : col( c_ ), row( r_ ){};
v2CR::v2CR( void ) : v2CR( 0, 0 ){};

v2CR::v2CR( const vector<char>& binData, vector<char>::const_iterator& it ){

	deSerializeBuildIn( this->col, (double) 0, binData, it );
	deSerializeBuildIn( this->row, (double) 0, binData, it );

}

size_t v2CR::serialize( vector<char>& binData ) const{

	size_t numBytes = 0;
	numBytes += serializeBuildIn( this->col, binData );
	numBytes += serializeBuildIn( this->row, binData );

	return numBytes;
}


/*!
 * range implementation
*/

range::range( const double start_, const double end_ ) : start( start_ ), end( end_ ){
	if( start >= end ){
		cerr << "Start must be less than end!" << endl;
		start = end - 1.;
	}
};


range::range( const Zrange naturalRange ) :
	range( (double) naturalRange.start, (double) naturalRange.end )
{

}


range::range( void ) : 
	range{ 0., 1. }
{

}

range::range( const vector<char>& binData, vector<char>::const_iterator& it ) :
	start( deSerializeBuildIn( 0., binData, it ) ),
	end( deSerializeBuildIn( 0., binData, it ) ){

}

double range::Resolution( const size_t number ) const{
	if( number < 2 ) return 1;
	return Diff() / ( number - 1 );
}

size_t range::serialize( vector<char>& binData ) const{
	size_t numBytes = 0;

	numBytes += serializeBuildIn( start, binData );
	numBytes += serializeBuildIn( end, binData );

	return numBytes;
}

/*!
 * Zrange implementation
*/

Zrange::Zrange( const signed long long start_, const signed long long end_ ) : start( start_ ), end( end_ ){
	if( start >= end ){
		cerr << "Start must be less than end!" << endl;
		start = end - 1;
	}
};


/*!
 * Serialization implementation
*/

template<>
size_t serializeBuildIn<string>( const string& val, vector<char>& binData ){

	size_t i = 0;
	const size_t padding = 24;	// Amount of character in each "line" of hex-editor

	// Insert padding so string start at "new line" when each line has 24 Bytes
	binData.insert( binData.end(), ( padding - ( binData.size() ) % padding ) % padding, stringPadding );

	// Iterate all characters in string
	for( const char c : val ){
		i++;
		binData.push_back( c );
	}


	// Add padding at the end so data start at a new line
	binData.insert( binData.end(), ( padding - ( binData.size() + 1 ) % padding ) % padding, stringPadding );

	// Termination
	binData.push_back( '\0' );

	return i;
}

template<>
size_t serializeBuildIn<vector<vector<v2CR>>>( const vector<vector<v2CR>>& vec, vector<char>& binData ){

	size_t i = 0;

	// Amount of sub vectors
	i += serializeBuildIn<size_t>( vec.size(), binData );

	// Iterate all sub vectors
	for( const vector<v2CR>& subVec : vec ){

		// Amount of elements in current sub vector
		i += serializeBuildIn<size_t>( subVec.size(), binData );
		
		// Serialize each element
		for( const v2CR& value : subVec ){
			i +=value.serialize( binData );
		}

	}

	return i;

}


template<>
size_t deSerializeBuildIn<string>( string& val, string defaultVal, const vector<char>& binData, vector<char>::const_iterator& it ){

	size_t i = 0;

	// Make sure string is empty
	val.clear();

	// Loop while temrination character not read and the end has not been reached
	while( *it != '\0' && it < binData.end() ){
		
		// The current char
		char curChar = *( it++ );

		// If character is not string padding add to string
		if( curChar != stringPadding ) val.push_back( curChar );
		i++;
	}

	// End reached before termination
	if( *it != '\0' ){
		// Set to default value
		val = defaultVal;
		return val.size();
	}

	// Skip '\0' so the iterator is pointing to next data
	if( it < binData.end() ) it++;

	return i;
}

template<>
vector<vector<v2CR>> deSerialize<vector<vector<v2CR>>>( const vector<char>& binData, vector<char>::const_iterator& it ){

	// Amount sub vectors
	size_t numSubVecs = deSerializeBuildIn<size_t>( (size_t) 0, binData, it );
	
	// Instance to return
	vector<vector<v2CR>> vec;

	// Loop fo amount of sub vectors
	for( size_t i = 0; i < numSubVecs; i++ ){

		// Amount of elements in sub vector
		size_t numElements = deSerializeBuildIn<size_t>( (size_t) 0, binData, it );
		
		// Initialize sub vector
		vector<v2CR> subVec( numElements, v2CR( 0., 0. ) );

		// Assign deserialsized data to sub vector elements 
		for( size_t j = 0; j < numElements; j++ ){
			subVec.at( j ) =  v2CR( binData, it );
		}

		// Add sub vector
		vec.push_back( subVec );
	}

	return vec;
}

bool exportSerialized( const path filePath, const vector<char> binData ){

	// Storage directory when it does not exist
	if( !std::filesystem::exists( filePath.parent_path()  )) std::filesystem::create_directory( filePath.parent_path() );

	return exportSerialized( filePath.string(), binData );

}

bool exportSerialized( const string fileName, const vector<char> binData ){
	
	// File handle
	std::ofstream outFile;
	
	// Overwrite existing and open as binary
	outFile.open( fileName, std::ios::trunc | std::ios::binary );
	if( outFile.fail() ) return false;

	// Write data
	outFile.write( (char*) binData.data(), binData.size() );
	
	if( outFile.fail() ) return false;
	outFile.close();

	return true;
};

vector<char> importSerialized( const path filePath ){

	return importSerialized( filePath.string() );

}

vector<char> importSerialized( const string fileName ){

	std::ifstream inFile;
	inFile.open( fileName, std::ios::binary );
	if( inFile.fail() ) return vector<char>();

	// Get file size
	size_t file_size = std::filesystem::file_size( fileName );
	
	// Allocate memory and read
	char* dArray = new char[ file_size ];
	inFile.read( dArray, file_size );

	if( inFile.eof() ){
		inFile.close();
		return vector<char>();
	}

	// Write to binary data
	vector<char> binData{ dArray, dArray + file_size };

	delete[] dArray;

	return binData;

}


bool validBinaryData( const string preamble, const vector<char>& binData, vector<char>::const_iterator& it ){

	
	string readPreamble;
	
	// Read file preamble
	deSerializeBuildIn( readPreamble, string{}, binData, it );

	// Compare
	return preamble == readPreamble;

}


template<>
double toNum<double>( const string str ){

	return std::stod( str );

}

template<>
int toNum<int>( const string str ){

	return std::stoi( str );

}

template<>
long long toNum<long long>( const string str ){

	return std::stoll( str );

}

template<>
size_t toNum<size_t>( const string str ){

	return std::stoll( str );

}