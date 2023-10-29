#pragma once
/*********************************************************************
 * @file   radonTransform.h
 * @brief  Classes radon transformation
 *
 * @author Jan Wolzenburg
 * @date   January 2023
 *********************************************************************/



 /*********************************************************************
  Includes
*********************************************************************/


#include "grid.h"
#include "radonProperties.h"
#include "radonCoordinates.h"



/*********************************************************************
   Definitions
*********************************************************************/


/*!
 * @brief Class for a radon transformation
*/
class radonTransformed : private grid<> {

	public:


	static const string FILE_PREAMBLE;

	/*!
	 * @brief Default constructor
	*/
	radonTransformed( void );

	/*!
	 * @brief Constructor
	 * @param physical_detector_properties_ Parameters to get the grid dimension from
	*/
	radonTransformed( const radonProperties detectorParameter );

	/*!
	 * @brief Constructor from serialized data_
	 * @param binary_data Reference to vector with binary data_
	 * @param it Iterator to start of data_ in vector
	*/
	radonTransformed( const vector<char>& binary_data, vector<char>::const_iterator& it );

	/*!
	 * @brief Get grid data_
	 * @return Grid data_
	*/
	grid<> Data( void ) const{ return ( grid<> ) * this; };
	
	/*!
	 * @brief Assign data_ at index 
	 * @param data_ Data to assign
	*/
	void assignData( const GridIndex index, const double value ){ this->setData( index, value ); };

	/*!
	 * @brief Assign data_ to grid
	 * @param data_ Data point
	*/
	void assignData( const radonPoint data_ );

	/*!
	 * @brief Serialize this object
	 * @param binary_data Reference to vector where data_ will be appended
	*/
	size_t Serialize( vector<char>& binary_data ) const;


	private:

	vector<vector<GridCoordinates>> gridErrors;	/*!<Errors when interpolating*/
};
