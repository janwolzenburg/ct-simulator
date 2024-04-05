/******************************************************************
* @file   modelViewProperties.cpp
*
* @author Jan Wolzenburg
* @date   October 2023
* ********************************************************************/



/*********************************************************************
  Includes
*********************************************************************/
#include "modelViewProperties.h"
#include "serialization.h"


/*********************************************************************
  Implementations
*********************************************************************/

const string ModelViewProperties::FILE_PREAMBLE{ "Ver02MODELVIEWPARAMETER_FILE_PREAMBLE" };

size_t ModelViewProperties::Serialize( vector<char>& binary_data ) const{

	size_t number_of_bytes = 0;

	number_of_bytes += contrast.Serialize( binary_data );
	number_of_bytes += slice_plane.Serialize( binary_data );
	number_of_bytes += SerializeBuildIn<double>( artefact_impact, binary_data );

	return number_of_bytes;

}