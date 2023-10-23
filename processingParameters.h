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
	 * @param binData Reference to vector with binary data
	 * @param it Iterator to start of data in vector
	*/
	processingParameter( const vector<char>& binData, vector<char>::const_iterator& it );

	/*!
	 * @brief Serialize this object
	 * @param binData Reference to vector where data will be appended
	*/
	size_t serialize( vector<char>& binData ) const;


	public:

	range projectionsContrast;			/*!<Contrast for sinogram*/
	discreteFilter::TYPE filterType;	/*!<The filter for filtering the projections*/
	range filteredProjectionsContrast;	/*!<Contrast for filteredsinogram*/
	range reconstrucedImageContrast;	/*!<Contrast for reconstructed image*/

};