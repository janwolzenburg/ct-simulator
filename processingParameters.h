#pragma once
/*********************************************************************
 * @file   processingParameters.h
 * @brief  A custom file chooser based on FLTK native file chooser class
 *
 * @author Jan Wolzenburg
 * @date   June 2023
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/


#include "generel.h"
#include "filter.h"

/*!
 * @brief Class for storing image contrasts and filter used in processing
*/
class processingParameter{

	public:

	static const string FILE_PREAMBLE;

	/*!
	 * @brief Default constructor 
	*/
	processingParameter( void );

	/*!
	 * @brief Constructor from serialized data
	 * @param binary_data Reference to vector with binary data
	 * @param it Iterator to start of data in vector
	*/
	processingParameter( const vector<char>& binary_data, vector<char>::const_iterator& it );

	/*!
	 * @brief Serialize this object
	 * @param binary_data Reference to vector where data will be appended
	*/
	size_t Serialize( vector<char>& binary_data ) const;


	public:

	NumberRange projectionsContrast;			/*!<Contrast for sinogram*/
	discreteFilter::TYPE filterType;	/*!<The filter for filtering the projections*/
	NumberRange filteredProjectionsContrast;	/*!<Contrast for filteredsinogram*/
	NumberRange reconstrucedImageContrast;	/*!<Contrast for reconstructed image*/

};