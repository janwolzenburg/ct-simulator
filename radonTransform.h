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
 * @brief Class for radon coordinates
*/
class radonCoords{

	public:
	/*!
	 * @brief Constructor
	 * @param theta_ Angle
	 * @param distance_ Distance
	*/
	radonCoords( const double theta_, const double distance_ ) : 
		theta( theta_ ),
		distance( distance_ )
	{};

	/*!
	 * @brief Constructor
	 * @param cSys Reference coordinate system
	 * @param l Line
	*/
	radonCoords( const cartCSys* const cSys, const line l );

	/*!
	 * @brief Default constructor
	*/
	radonCoords( void ) : radonCoords{ 0, 0 } {};


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
		coordinates( coordinates_ ),
		value( value_ )
	{};

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

	radonTransformed( void );

	/*!
	 * @brief Constructor
	 * @param detectorParameter Parameters to get the grid dimension from
	*/
	radonTransformed( const detectorRadonParameter detectorParameter );

	/*!
	 * @brief Constructor from serialized data
	 * @param binData Reference to vector with binary data
	 * @param it Iterator to start of data in vector
	*/
	radonTransformed( const vector<char>& binData, vector<char>::const_iterator& it );

	/*!
	 * @brief Get grid data
	 * @return Grid data
	*/
	grid<> Data( void ) const{ return ( grid<> ) * this; };
	
	/*!
	 * @brief Assign data at index 
	 * @param data Data to assign
	*/
	void assignData( const idx2CR index, const double value ){ this->setData( index, value ); };

	/*!
	 * @brief Assign data to grid
	 * @param data Data point
	*/
	void assignData( const radonPoint data );

	/*!
	 * @brief Serialize this object
	 * @param binData Reference to vector where data will be appended
	*/
	size_t serialize( vector<char>& binData ) const;


	private:

	vector<vector<v2CR>> gridErrors;	/*!<Errors when interpolating*/
};
