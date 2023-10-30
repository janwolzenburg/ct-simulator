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
#include "radonTransformationProperties.h"
#include "radonCoordinates.h"



/*********************************************************************
   Definitions
*********************************************************************/


/*!
 * @brief Class for a radon transformation
*/
class RadonTransformation : private grid<> {

	public:


	static const string FILE_PREAMBLE;

	/*!
	 * @brief Default constructor
	*/
	RadonTransformation( void );

	/*!
	 * @brief Constructor
	 * @param properties Transformation properties
	*/
	RadonTransformation( const RadonTransformationProperties properties );

	/*!
	 * @brief Constructor from serialized data
	 * @param binary_data Reference to vector with binary data
	 * @param current_byte Iterator to start of data in vector
	*/
	RadonTransformation( const vector<char>& binary_data, vector<char>::const_iterator& current_byte );
	
	/*!
	 * @brief Serialize this object
	 * @param binary_data Reference to vector where data will be appended
	*/
	size_t Serialize( vector<char>& binary_data ) const;

	/*!
	 * @brief Get grid data
	 * @return Grid data
	*/
	grid<> data( void ) const{ return static_cast<grid<>>( *this ); };
	
	/*!
	 * @brief Assign data at index 
	 * @param data Data to assign
	*/
	void AssignData( const GridIndex index, const double value ){ this->setData( index, value ); };

	/*!
	 * @brief Assign data to grid
	 * @param radon_point Data point
	*/
	void assignData( const RadonPoint radon_point );


	private:

	vector<vector<GridCoordinates>> grid_errors_;	/*!<Grid errors when interpolating*/
};
