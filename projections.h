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
#include "tomography.h"



/*********************************************************************
   Definitions
*********************************************************************/


/*!
 * @brief Class for projections data by tomography
*/
class Projections : private DataGrid<> {

	public:

	static const string FILE_PREAMBLE; /*!< String to prepend to file when storing as file*/

	/*!
	 * @brief Default constructor
	*/
	Projections( void );

	/*!
	 * @brief Constructor
	 * @param properties Transformation properties
	 * @param tomography_properties Properties of tomography used to get these projections
	*/
	Projections( const ProjectionsProperties properties, const TomographyProperties tomography_properties );

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
	 * @brief Get start of projections
	 * @return Smallest angle and distance
	*/
	GridCoordinates start( void ) const{ return DataGrid<>::start(); };

	/*!
	 * @brief Get resolution
	 * @return Angle and distance resolution
	*/
	GridCoordinates resolution( void ) const{ return DataGrid<>::resolution(); };

	/*!
	 * @brief Get size
	 * @return Get grid size
	*/
	GridIndex size(void) const { return DataGrid<>::size(); };

	/*!
	 * @brief Get maximum value
	 * @return The maximum value
	*/
	double max_value(void) const { return DataGrid<>::max_value(); };

	/*!
	 * @brief Get projection properties
	 * @return Properties of projections
	*/
	ProjectionsProperties properties( void ) const{ return properties_; };
	
	/*!
	 * @brief Get tomography properties used for these projections
	 * @return Tomography properties
	*/
	TomographyProperties tomography_properties( void ) const;

	/*!
	 * @brief Assign data at index 
	 * @param data Data to assign
	*/
	void AssignData( const GridIndex index, const double value ){ this->SetData( index, value ); };

	/*!
	 * @brief Get data at index
	 * @param index index of data
	 * @return value at index
	*/
	double GetData(const GridIndex index) { return DataGrid<>::GetData(index); };

	/*!
	 * @brief Assign data to grid
	 * @param radon_point Data point
	*/
	void AssignData( const RadonPoint radon_point );


	private:

	ProjectionsProperties properties_;				/*!< proeprties of projection*/
	TomographyProperties tomography_properties_;	/*!< Tomography properties used*/
	vector<vector<GridCoordinates>> grid_errors_;	/*!< Grid errors when interpolating*/
};
