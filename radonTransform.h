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

#include "line.h"
#include "detector.h"
#include "grid.h"



/*********************************************************************
   Definitions
*********************************************************************/


/*!
 * @brief Class for radon Coordinates
*/
class radonCoords{

	public:
	/*!
	 * @brief Constructor
	 * @param theta_ Angle
	 * @param distance_ Distance
	*/
	radonCoords( const double theta_, const double distance_ ) : 
		theta( theta_ ), distance( distance_ ) {};

	/*!
	 * @brief Default constructor
	*/
	radonCoords( void ) : radonCoords{ 0, 0 } {};

	/*!
	 * @brief Constructor
	 * @param coordinate_system_ Reference coordinate system
	 * @param l Line
	*/
	radonCoords( const CoordinateSystem* const cSys, const Line l );


	public:

	double theta;		/*!<Angle*/
	double distance;	/*!<Distance*/
	
};


/*!
 * @brief Radon point
*/
class radonPoint{

	public:

	/*!
	 * @brief Cosntructor
	 * @param coordinates_ Coordinates 
	 * @param value_ Value
	*/
	radonPoint( const radonCoords coordinates_, const double value_ ) :
		coordinates( coordinates_ ), value( value_ ) {};

	public:

	radonCoords coordinates;	/*!<Coordiantes*/
	double value;				/*!<Value*/

};


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
	 * @param detectorParameter Parameters to get the grid dimension from
	*/
	radonTransformed( const detectorRadonParameter detectorParameter );

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
