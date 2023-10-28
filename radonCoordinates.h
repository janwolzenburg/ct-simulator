#pragma once
/*********************************************************************
 * @file   radonProperties.h
 * @brief  Classes radon transformation
 *
 * @author Jan Wolzenburg
 * @date   October 2023
 *********************************************************************/

 #include "coordinateSystem.h"
 #include "line.h"
 

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

	radonCoords coordinates;	/*!<Coordiantes*/
	double value;				/*!<Value*/

};