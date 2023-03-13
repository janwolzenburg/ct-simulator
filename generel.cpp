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


double range::Resolution( const size_t number ) const{
	if( number < 2 ) return 1;
	return Diff() / ( number - 1 );
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
size_t serializeBuildIn<string>( const string val, vector<char>& binData ){

	size_t i = 0;

	for( const char c : val ){
		i++;
		binData.push_back( c );
	}

	binData.push_back( '\0' );

	return i;
}

template<>
size_t deSerializeBuildIn<string>( string& val, string defaultVal, const vector<char>& binData, vector<char>::const_iterator& it ){

	size_t i = 0;

	val.clear();

	while( *it != '\0' && it < binData.end() ){
		val.push_back( *( it++ ) );
		i++;
	}

	if( *it != '\0' ){
		val = defaultVal;
		return val.size();
	}

	// Skip '\0'
	if( it < binData.end() ) it++;

	return i;
}


bool exportSerialized( const path filePath, const vector<char> binData ){

	return exportSerialized( filePath.string(), binData );

}

bool exportSerialized( const string fileName, const vector<char> binData ){
	// File handle
	std::ofstream outFile;
	outFile.open( fileName, std::ios::trunc | std::ios::binary );
	if( outFile.fail() ) return false;

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

	size_t file_size = std::filesystem::file_size( fileName );
	char* dArray = new char[ file_size ];
	inFile.read( dArray, file_size );

	if( inFile.eof() ){
		inFile.close();
		return vector<char>();
	}

	vector<char> binData{ dArray, dArray + file_size };

	return binData;

}


bool validBinaryData( const string preamble, const vector<char>& binData, vector<char>::const_iterator& it ){

	string readPreamble;
	deSerializeBuildIn( readPreamble, string{}, binData, it );

	return preamble == readPreamble;

}