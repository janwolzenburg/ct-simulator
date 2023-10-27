#pragma once
/*********************************************************************
 * @file   grid.h
 * @brief  Class for 2D grid
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
 * @brief Class for a 2D grid
*/
template<class D = double>
class grid{

	public:

	grid( void );

	/*!
	 * @brief Constructor
	 * @param size_ Size of grid
	 * @param start_ Starts of axis
	 * @param resolution_ Axis resolution
	 * @param defaultValue Default value
	*/
	grid( const GridIndex size_, const GridCoordinates start_, const GridCoordinates resolution_, D defaultValue = D() );

	/*!
	 * @brief Constructor
	 * @param columnRange Value range of column axis 
	 * @param rowRange Value range of row axis
	 * @param resolution_ Resolutions
	 * @param defaultValue Default value
	*/
	grid( const NumberRange columnRange, const NumberRange rowRange, const GridCoordinates resolution_, D defaultValue = D() );

	/*!
	 * @brief Constructor from serialized data_
	 * @param binary_data Reference to vector with binary data_
	 * @param it Iterator to start of data_ in vector
	*/
	grid( const vector<char>& binary_data, vector<char>::const_iterator& it );

	/*!
	 * @brief Get size_ of grid
	 * @return Size of grid
	*/
	GridIndex Size( void ) const{ return size_; };

	/*!
	 * @brief Get starts of axis
	 * @return Start of axis
	*/
	GridCoordinates Start( void ) const{ return start; };

	/*!
	 * @brief Get ends of axis
	 * @return Ens of axis
	*/
	GridCoordinates End( void ) const{
		return getCoordinates( GridIndex{ size_.c - 1, size_.r - 1 } ); }

	/*!
	 * @brief Get resolution of axis
	 * @return GetResolution of axis
	*/
	GridCoordinates Resolution( void ) const{ return resolution; };

	/*!
	 * @brief Get the maximum value in grid
	 * @return Maximum value
	*/
	D MaxValue( void ) const{ return maxValue; };

	/*!
	 * @brief Get the minimum value in grid
	 * @return Minimum value
	*/
	D MinValue( void ) const{ return minValue; };

	/*!
	 * @brief Check indices for validity
	 * @param index Indices to check
	 * @return True when valid
	*/
	bool checkIndex( const GridIndex index ) const;

	/*!
	 * @brief Get indices corresponding to Coordinates
	 * @param coordinate Coordinates
	 * @return Indices of coordinate
	*/
	GridIndex getIndex( const GridCoordinates coordinate ) const;
	
	/*!
	 * @brief Get Coordinates to given indices
	 * @param index Indices
	 * @return Coordinate of indices
	*/
	GridCoordinates getCoordinates( const GridIndex index ) const;

	/*!
	 * @brief Get element value
	 * @param index Index of element
	 * @return Element value
	*/
	D operator()( const GridIndex index ) const;

	/*!
	 * @brief Get element value
	 * @param index Index of element
	 * @return Value of Element
	*/
	D getData( const GridIndex index ) const{ return this->operator()( index ); };

	/*!
	 * @brief Get element value
	 * @param Coordinates Coordinates of element
	 * @return Value of Element
	*/
	D getData( const GridCoordinates coordinates ) const{ return this->getData( getIndex( coordinates ) ); };

	/*!
	 * @brief Set Element data_
	 * @param index Index of element
	 * @param newValue New value
	 * @return True when index is valid
	 * @details For non arithmetic types additional code is needed in implementation
	*/
	bool setData( const GridIndex index, const D newValue );

	/*!
	 * @brief Set Element data_
	 * @param Coordinates Coordinates of element
	 * @param newValue New value
	 * @return True when Coordinates are valid
	*/
	bool setData( const GridCoordinates coordinates, const D newValue ){
		return this->setData( getIndex( coordinates ), newValue ); }

	/*!
	 * @brief Serialize this object
	 * @param binary_data Reference to vector where data_ will be appended
	*/
	size_t Serialize( vector<char>& binary_data ) const;


	private:
	
	/*!
	 * @brief Fill the points and data_ vectors with value
	 * @param defaultValue Value to fill
	*/
	void fillVectors( const D defaultValue );


	private:

	GridIndex size_;					/*!<Size of grid*/
	GridCoordinates start;						/*!<Start of axis*/
	GridCoordinates resolution;				/*!<GetResolution of grid*/

	vector<double> columnPoints;	/*!<Vector with values on column axis*/
	vector<double> rowPoints;		/*!<Vector with values on row axis*/
	vector<vector<D>> data_;			/*!<2D Vector with data_ points*/

	D maxValue;						/*!<Maximum value in grid*/
	D minValue;						/*!<Minimum value in grid*/


	/*!
	 * @brief Access operator for grid elemenrt
	 * @param index Index of element
	 * @return Reference to element value
	*/
	D& operator()( const GridIndex index );

	/*!
	* @brief Access operator for grid elemenrt
	* @param point Point in grid
	* @return Reference to element value
	*/
	D& operator()( const GridCoordinates coordinates ){ return this->operator()( getIndex( coordinates ) ); };

	/*!
	 * @brief Initialise minimum and maximum value
	 * @details For own non arithmetic types of D additional code in function implementation is necessary
	*/
	void initialiseMinMaxValue( void );

};

#include "grid.hpp"