/******************************************************************
* @file   modelViewProperties.cpp
* @brief  Implementations
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

const string ModelViewProperties::FILE_PREAMBLE{ "Ver01MODELVIEWPARAMETER_FILE_PREAMBLE" };

size_t ModelViewProperties::Serialize( vector<char>& binary_data ) const{

	size_t num_bytes = 0;

	num_bytes += contrast.Serialize( binary_data );
	num_bytes += slice_plane.Serialize( binary_data );

	return num_bytes;

}