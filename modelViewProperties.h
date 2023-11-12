#pragma once
/*********************************************************************
 * @file   fl_ModelView.h
 * @brief
 *
 * @author Jan Wolzenburg
 * @date   September 2023
 *********************************************************************/

#include "generel.h"
#include "slicePlane.h"


/*!
 * @brief Class to store parameters used in model view
*/
class ModelViewProperties{

	public:

	static const string FILE_PREAMBLE; /*!< String to prepend to file when storing as file*/

	/*!
	 * @brief Default constructor
	*/
	ModelViewProperties( void ) : contrast{}, slice_plane{}{};

	/*!
	 * @brief Constructor from serialized data
	 * @param binary_data Reference to vector with binary data
	 * @param current_byte Iterator to start of data in vector
	*/
	ModelViewProperties( const vector<char>& binary_data, vector<char>::const_iterator& current_byte ) : 
		contrast{ binary_data, current_byte }, slice_plane{ binary_data, current_byte }{};

	/*!
	 * @brief Serialize this object
	 * @param binary_data Reference to vector where data will be appended
	*/
	size_t Serialize( vector<char>& binary_data ) const;


	NumberRange contrast;			/*!< Contrast of slice image*/
	SlicePlane slice_plane;			/*!< Surface to slice model with*/
};