/*********************************************************************
 * @file   processingParameters.cpp
 * @brief  Implementations
 *
 * @author Jan Wolzenburg
 * @date   June 2023
 *********************************************************************/


/*********************************************************************
	Includes
 *********************************************************************/

#include "processingParameters.h"
#include "serialization.h"


/*********************************************************************
	Implementations
*********************************************************************/


const string processingParameter::FILE_PREAMBLE{ "PROCESSINGPARAMETERS_FILE_PREAMBLE" };



processingParameter::processingParameter( void ) :
	projectionsContrast{},
	filterType( discreteFilter::TYPE::constant ),
	filteredProjectionsContrast{},
	reconstrucedImageContrast{}
{}

processingParameter::processingParameter( const vector<char>& binary_data, vector<char>::const_iterator& it ) :
	projectionsContrast{ binary_data, it },
	filterType( discreteFilter::getEnum( DeSerializeBuildIn<string>( string(), binary_data, it ) ) ),
	filteredProjectionsContrast{ binary_data, it },
	reconstrucedImageContrast{ binary_data, it }
{}

size_t processingParameter::Serialize( vector<char>& binary_data ) const{

	size_t num_bytes = 0;

	num_bytes += SerializeBuildIn( FILE_PREAMBLE, binary_data );
	num_bytes += projectionsContrast.Serialize( binary_data );
	num_bytes += SerializeBuildIn( discreteFilter::filterTypes.at( filterType ), binary_data );
	num_bytes += filteredProjectionsContrast.Serialize( binary_data );
	num_bytes += reconstrucedImageContrast.Serialize( binary_data );

	return num_bytes;

}