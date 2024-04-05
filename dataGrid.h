#pragma once
/*********************************************************************
 * @file   grid.h
 * @brief  class for 2D grid
 *
 * @author Jan Wolzenburg
 * @date   January 2023
 *********************************************************************/



 /*********************************************************************
 Includes
*********************************************************************/

#include "generel.h"


/*********************************************************************
   Definitions
*********************************************************************/


/*!
 * @brief class for gridded data
 * @tparam D type of data
*/
template<class D = double>
class DataGrid{

	public:

	/*!
	 * @brief default constructor
	*/
	DataGrid( void );

	/*!
	 * @brief constructor
	 * @param size size of grid
	 * @param start starts of axis
	 * @param resolution axis resolution
	 * @param default_value default value
	*/
	DataGrid( const GridIndex size, const GridCoordinates start, const GridCoordinates resolution, D default_value = D{});

	/*!
	 * @brief constructor
	 * @param column_range value range of column axis 
	 * @param row_range value range of row axis
	 * @param resolution resolutions
	 * @param default_value default value
	*/
	DataGrid( const NumberRange column_range, const NumberRange row_range, const GridCoordinates resolution, D default_value = D{});

	/*!
	 * @brief constructor from serialized data
	 * @param binary_data reference to vector with binary data
	 * @param current_byte iterator to start of data in vector
	*/
	DataGrid( const vector<char>& binary_data, vector<char>::const_iterator& current_byte );

	/*!
	 * @brief serialize this object
	 * @param binary_data reference to vector where data will be appended
	 * @return written bytes
	*/
	size_t Serialize( vector<char>& binary_data ) const;

	/*!
	 * @brief get size of grid
	 * @return size of grid
	*/
	GridIndex size( void ) const{ return size_; };

	/*!
	 * @brief get starts of axis
	 * @return start of axis
	*/
	GridCoordinates start( void ) const{ return start_; };

	/*!
	 * @brief get resolution of axis
	 * @return resolution of axis
	*/
	GridCoordinates resolution( void ) const{ return resolution_; };

	/*!
	 * @brief get the maximum value in grid
	 * @return maximum value
	*/
	D max_value( void ) const{ return max_value_; };

	/*!
	 * @brief get the minimum value in grid
	 * @return minimum value
	*/
	D min_value( void ) const{ return min_value_; };

	/*!
	 * @brief get ends of axis
	 * @return end of axis
	*/
	GridCoordinates GetEnd( void ) const{
		return GetCoordinates( GridIndex{ size_.c - 1, size_.r - 1 } ); }

	/*!
	 * @brief check indices for validity
	 * @param index Indices to check
	 * @return true when valid
	*/
	bool IsIndexValid( const GridIndex index ) const;

	/*!
	 * @brief get indices corresponding to Coordinates
	 * @param coordinate Coordinates
	 * @return indices of coordinate
	*/
	GridIndex GetIndex( const GridCoordinates coordinate ) const;
	
	/*!
	 * @brief get Coordinates to given indices
	 * @param index Indices
	 * @return coordinate of indices
	*/
	GridCoordinates GetCoordinates( const GridIndex index ) const;

	/*!
	 * @brief get the row coordinate
	 * @param row_index row index of coordinate
	 * @return row coordinate
	 */
	double GetRowCoordinate( const size_t row_index ) const{ return row_points_.at( row_index ); };

	/*!
	 * @brief get the columns coordinate
	 * @param column_index column index of coordinate
	 * @return columns coordinate
	 */
	double GetColumnCoordinate( const size_t column_index ) const{ return column_points_.at( column_index ); };

	/*!
	 * @brief get element value
	 * @param index Index of element
	 * @return element value
	*/
	D operator()( const GridIndex index ) const;

	/*!
	 * @brief get element value
	 * @param index Index of element
	 * @return value of Element
	*/
	D GetData( const GridIndex index ) const{ return this->operator()( index ); };

	/*!
	 * @brief get element value
	 * @param coordinates coordinates of element
	 * @return value of Element
	*/
	D GetData( const GridCoordinates coordinates ) const{ return this->GetData( GetIndex( coordinates ) ); };

	/*!
	 * @brief get the grid as a grid with double values
	 * @return double grid
	 */
	DataGrid<double> GetDoubleGrid( void ) const;

	/*!
	 * @brief set element data
	 * @param index index of element
	 * @param new_value new value
	 * @return true when index is valid
	 * @details for non arithmetic types additional code is needed in implementation
	*/
	bool SetData( const GridIndex index, const D new_value );

	/*!
	 * @brief set Element data
	 * @param coordinates coordinates of element
	 * @param new_value new value
	 * @return true when coordinates are valid
	 * @details for non arithmetic types additional code is needed in implementation
	*/
	bool SetData( const GridCoordinates coordinates, const D new_value ){
		return this->SetData( GetIndex( coordinates ), new_value ); }


	private:

	GridIndex size_;								/*!< size of grid*/
	GridCoordinates start_;					/*!< start of axis*/
	GridCoordinates resolution_;		/*!< resolution of grid*/

	vector<double> column_points_;	/*!< vector with values on column axis*/
	vector<double> row_points_;			/*!< vector with values on row axis*/
	vector<vector<D>> data_;				/*!< 2D vector with data points*/

	D min_value_;						/*!< minimum value in grid*/
	D max_value_;						/*!< maximum value in grid*/


	/*!
	 * @brief access operator for grid elemenrt
	 * @param index Index of element
	 * @return reference to element value
	*/
	D& operator()( const GridIndex index );

	/*!
	* @brief access operator for grid element
	* @param coordinates coordinates in the grid
	* @return reference to element value
	*/
	D& operator()( const GridCoordinates coordinates ){ return this->operator()( GetIndex( coordinates ) ); };

	/*!
	 * @brief initialise minimum and maximum value
	 * @details for own non arithmetic types of D additional code in function implementation is necessary
	*/
	void InitialiseMinAndMaxValue( void );

	/*!
	 * @brief fill the points and data vectors with value
	 * @param value value to fill
	*/
	void Fillvectors( const D value );

};

#include "dataGrid.hpp"