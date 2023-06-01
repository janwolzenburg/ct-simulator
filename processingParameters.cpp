#include "processingParameters.h"



const string processingParameter::FILE_PREAMBLE{ "PROCESSINGPARAMETERS_FILE_PREAMBLE" };



processingParameter::processingParameter( void ) :
	projectionsContrast(),
	filterType( discreteFilter::TYPE::constant ){

}

processingParameter::processingParameter( const vector<char>& binData, vector<char>::const_iterator& it ) :
	projectionsContrast( binData, it ),
	filterType( discreteFilter::getEnum( deSerializeBuildIn<string>( string(), binData, it ) ) ){

}

size_t processingParameter::serialize( vector<char>& binData ) const{

	size_t numBytes = 0;

	numBytes += serializeBuildIn( FILE_PREAMBLE, binData );
	numBytes += projectionsContrast.serialize( binData );
	numBytes += serializeBuildIn( discreteFilter::filterTypes.at( filterType ), binData );

	return numBytes;

}