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

#include "line.h"
#include "detector.h"
#include <tuple>


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
	inline idx2CR Size( void ) const{ return size; };

	/*!
	 * @brief Get starts of axis
	 * @return Start of axis
	*/
	inline v2CR Start( void ) const{ return start; };

	/*!
	 * @brief Get ends of axis
	 * @return Ens of axis
	*/
	v2CR End( void ) const;

	/*!
	 * @brief Get resolution of axis
	 * @return Resolution of axis
	*/
	inline v2CR Resolution( void ) const{ return resolution; };

	/*!
	 * @brief Check indices for validity
	 * @param index Indices to check
	 * @return True when valid
	*/
	bool checkIndex( const idx2CR index ) const;

	/*!
	 * @brief Access operator for grid elemenrt
	 * @param index Index of element
	 * @return Element value
	*/
	D operator()( const idx2CR index ) const;

	/*!
	 * @brief Access operator for grid elemenrt
	 * @param index Index of element
	 * @return Reference to element value
	*/
	D& operator()( const idx2CR index );

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
	 * @brief Access operator for grid elemenrt
	 * @param point Point in grid
	 * @return Element value
	*/
	D operator()( const v2CR point ) const;

	/*!
	 * @brief Access operator for grid elemenrt
	 * @param point Point in grid
	 * @return Reference to element value
	*/
	D& operator()( const v2CR point );

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
	vector<vector<D>> data;	/*!<2D Vector with data points*/

};

#include "grid.hpp"