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
	grid( const idx2CR size_, const v2CR start_, const v2CR resolution_, D defaultValue = D() );

	/*!
	 * @brief Constructor
	 * @param columnRange Value range of column axis 
	 * @param rowRange Value range of row axis
	 * @param resolution_ Resolutions
	 * @param defaultValue Default value
	*/
	grid( const range columnRange, const range rowRange, const v2CR resolution_, D defaultValue = D() );

	/*!
	 * @brief Constructor from serialized data
	 * @param binData Reference to vector with binary data
	 * @param it Iterator to start of data in vector
	*/
	grid( const vector<char>& binData, vector<char>::const_iterator& it );

	/*!
	 * @brief Get size of grid
	 * @return Size of grid
	*/
	idx2CR Size( void ) const{ return size; };

	/*!
	 * @brief Get starts of axis
	 * @return Start of axis
	*/
	v2CR Start( void ) const{ return start; };

	/*!
	 * @brief Get ends of axis
	 * @return Ens of axis
	*/
	v2CR End( void ) const{
		return getCoordinates( idx2CR{ size.col - 1, size.row - 1 } ); }

	/*!
	 * @brief Get resolution of axis
	 * @return Resolution of axis
	*/
	v2CR Resolution( void ) const{ return resolution; };

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
	bool checkIndex( const idx2CR index ) const;

	/*!
	 * @brief Get indices corresponding to coordinates
	 * @param coordinate Coordinates
	 * @return Indices of coordinate
	*/
	idx2CR getIndex( const v2CR coordinate ) const;
	
	/*!
	 * @brief Get coordinates to given indices
	 * @param index Indices
	 * @return Coordinate of indices
	*/
	v2CR getCoordinates( const idx2CR index ) const;

	/*!
	 * @brief Get element value
	 * @param index Index of element
	 * @return Element value
	*/
	D operator()( const idx2CR index ) const;

	/*!
	 * @brief Get element value
	 * @param index Index of element
	 * @return Value of Element
	*/
	D getData( const idx2CR index ) const{ return this->operator()( index ); };

	/*!
	 * @brief Get element value
	 * @param coordinates Coordinates of element
	 * @return Value of Element
	*/
	D getData( const v2CR coordinates ) const{ return this->getData( getIndex( coordinates ) ); };

	/*!
	 * @brief Set Element data
	 * @param index Index of element
	 * @param newValue New value
	 * @return True when index is valid
	 * @details For non arithmetic types additional code is needed in implementation
	*/
	bool setData( const idx2CR index, const D newValue );

	/*!
	 * @brief Set Element data
	 * @param coordinates Coordinates of element
	 * @param newValue New value
	 * @return True when coordinates are valid
	*/
	bool setData( const v2CR coordinates, const D newValue ){
		return this->setData( getIndex( coordinates ), newValue ); }

	/*!
	 * @brief Serialize this object
	 * @param binData Reference to vector where data will be appended
	*/
	size_t serialize( vector<char>& binData ) const;


	private:
	
	/*!
	 * @brief Fill the points and data vectors with value
	 * @param defaultValue Value to fill
	*/
	void fillVectors( const D defaultValue );


	private:

	idx2CR size;					/*!<Size of grid*/
	v2CR start;						/*!<Start of axis*/
	v2CR resolution;				/*!<Resolution of grid*/

	vector<double> columnPoints;	/*!<Vector with values on column axis*/
	vector<double> rowPoints;		/*!<Vector with values on row axis*/
	vector<vector<D>> data;			/*!<2D Vector with data points*/

	D maxValue;						/*!<Maximum value in grid*/
	D minValue;						/*!<Minimum value in grid*/


	/*!
	 * @brief Access operator for grid elemenrt
	 * @param index Index of element
	 * @return Reference to element value
	*/
	D& operator()( const idx2CR index );

	/*!
	* @brief Access operator for grid elemenrt
	* @param point Point in grid
	* @return Reference to element value
	*/
	D& operator()( const v2CR coordinates ){ return this->operator()( getIndex( coordinates ) ); };

	/*!
	 * @brief Initialise minimum and maximum value
	 * @details For own non arithmetic types of D additional code in function implementation is necessary
	*/
	void initialiseMinMaxValue( void );

};

#include "grid.hpp"