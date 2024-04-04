#pragma once
/*********************************************************************
 * @file   radonTransform.h
 * @brief  classes radon transformation
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
#include "tomography.h"



/*********************************************************************
   Definitions
*********************************************************************/


/*!
 * @brief class for projections data by tomography
*/
class Projections : private DataGrid<> {

	public:

	static const string FILE_PREAMBLE; /*!< string to prepend to file when storing as file*/

	/*!
	 * @brief default constructor
	*/
	Projections( void );

	/*!
	 * @brief constructor
	 * @param properties Transformation properties
	 * @param tomography_properties Properties of tomography used to get these projections
	*/
	Projections( const ProjectionsProperties properties, const TomographyProperties tomography_properties );

	/*!
	 * @brief constructor from serialized data
	 * @param binary_data reference to vector with binary data
	 * @param current_byte iterator to start of data in vector
	*/
	Projections( const vector<char>& binary_data, vector<char>::const_iterator& current_byte );
	
	/*!
	 * @brief serialize this object
	 * @param binary_data reference to vector where data will be appended
	*/
	size_t Serialize( vector<char>& binary_data ) const;

	/*!
	 * @brief get grid data
	 * @return Grid data
	*/
	DataGrid<> data( void ) const{ return static_cast<DataGrid<>>( *this ); };
	
	/*!
	 * @brief get start of projections
	 * @return smallest angle and distance
	*/
	GridCoordinates start( void ) const{ return DataGrid<>::start(); };

	/*!
	 * @brief get resolution
	 * @return angle and distance resolution
	*/
	GridCoordinates resolution( void ) const{ return DataGrid<>::resolution(); };

	/*!
	 * @brief get size
	 * @return Get grid size
	*/
	GridIndex size(void) const { return DataGrid<>::size(); };

	/*!
	 * @brief get maximum value
	 * @return the maximum value
	*/
	double max_value(void) const { return DataGrid<>::max_value(); };

	/*!
	 * @brief get projection properties
	 * @return properties of projections
	*/
	ProjectionsProperties properties( void ) const{ return properties_; };
	
	/*!
	 * @brief get tomography properties used for these projections
	 * @return tomography properties
	*/
	TomographyProperties tomography_properties( void ) const;

	/*!
	 * @brief assign data at index 
	 * @param data Data to assign
	*/
	void AssignData( const GridIndex index, const double value ){ this->SetData( index, value ); };

	/*!
	 * @brief get data at index
	 * @param index index of data
	 * @return value at index
	*/
	double GetData(const GridIndex index) { return DataGrid<>::GetData(index); };

	/*!
	 * @brief assign data to grid
	 * @param radon_point Data point
	*/
	void AssignData( const RadonPoint radon_point );


	private:

	ProjectionsProperties properties_;				/*!< proeprties of projection*/
	TomographyProperties tomography_properties_;	/*!< tomography properties used*/
	vector<vector<GridCoordinates>> grid_errors_;	/*!< Grid errors when interpolating*/
};
