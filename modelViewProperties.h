#pragma once
/*********************************************************************
 * @file   modelViewProperties.h
 *
 * @author Jan Wolzenburg
 * @date   September 2023
 *********************************************************************/

#include "generel.h"
#include "slicePlane.h"
#include "serialization.h"


/*!
 * @brief class to store parameters used in model view
*/
class ModelViewProperties{

	public:

	static const string FILE_PREAMBLE; /*!< string to prepend to file when storing as file*/

	/*!
	 * @brief default constructor
	*/
	ModelViewProperties( void ) : contrast{}, slice_plane{}, artefact_impact( 10 ){};

	/*!
	 * @brief constructor from serialized data
	 * @param binary_data reference to vector with binary data
	 * @param current_byte iterator to start of data in vector
	*/
	ModelViewProperties( const vector<char>& binary_data, vector<char>::const_iterator& current_byte ) : 
		contrast{ binary_data, current_byte }, slice_plane{ binary_data, current_byte }, artefact_impact( DeSerializeBuildIn<double>( 1., binary_data, current_byte ) )
		{};

	/*!
	 * @brief serialize this instance
	 * @param binary_data data to write to
	 * @return bytes written
	 */
	size_t Serialize( vector<char>& binary_data ) const;


	NumberRange contrast;			/*!< contrast of slice image*/
	SlicePlane slice_plane;		/*!< surface to slice model with*/
	double artefact_impact;		/*!< artefact impact factor*/
};