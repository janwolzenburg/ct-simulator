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