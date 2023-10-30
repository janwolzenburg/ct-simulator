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


#include "dataGrid.h"
#include "projectionsProperties.h"
#include "radonCoordinates.h"



/*********************************************************************
   Definitions
*********************************************************************/


/*!
 * @brief Class for projections data by tomography
*/
class Projections : private DataGrid<> {

	public:

	static const string FILE_PREAMBLE;

	/*!
	 * @brief Default constructor
	*/
	Projections( void );

	/*!
	 * @brief Constructor
	 * @param properties Transformation properties
	*/
	Projections( const ProjectionsProperties properties );

	/*!
	 * @brief Constructor from serialized data
	 * @param binary_data Reference to vector with binary data
	 * @param current_byte Iterator to start of data in vector
	*/
	Projections( const vector<char>& binary_data, vector<char>::const_iterator& current_byte );
	
	/*!
	 * @brief Serialize this object
	 * @param binary_data Reference to vector where data will be appended
	*/
	size_t Serialize( vector<char>& binary_data ) const;

	/*!
	 * @brief Get grid data
	 * @return Grid data
	*/
	DataGrid<> data( void ) const{ return static_cast<DataGrid<>>( *this ); };
	
	/*!
	 * @brief Assign data at index 
	 * @param data Data to assign
	*/
	void AssignData( const GridIndex index, const double value ){ this->SetData( index, value ); };

	/*!
	 * @brief Assign data to grid
	 * @param radon_point Data point
	*/
	void AssignData( const RadonPoint radon_point );


	private:

	vector<vector<GridCoordinates>> grid_errors_;	/*!<Grid errors when interpolating*/
};
