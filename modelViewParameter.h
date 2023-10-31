#pragma once
/*********************************************************************
 * @file   modelView.h
 * @brief
 *
 * @author Jan Wolzenburg
 * @date   September 2023
 *********************************************************************/

#include <string>
using std::string;

#include <vector>
using std::vector;

#include "generel.h"
#include "slicePlane.h"


/*!
 * @brief Class to store parameters used in model view
*/
class modelViewParameter{

	public:

	static const string FILE_PREAMBLE;

	/*!
	 * @brief Default constructor
	*/
	modelViewParameter( void ) : viewContrast{}, plane{}{};

	/*!
	 * @brief Constructor from serialized data_
	 * @param binary_data Reference to vector with binary data_
	 * @param it Iterator to start of data in vector
	*/
	modelViewParameter( const vector<char>& binary_data, vector<char>::const_iterator& it ) : 
		viewContrast{ binary_data, it }, plane{ binary_data, it }{};

	/*!
	 * @brief Serialize this object
	 * @param binary_data Reference to vector where data will be appended
	*/
	size_t Serialize( vector<char>& binary_data ) const;


	public:

	NumberRange viewContrast;			/*!<Contrast of slice image*/
	slicePlane plane;			/*!<Surface to slice model with*/
};