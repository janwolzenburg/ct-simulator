#pragma once
/*********************************************************************
 * @file   radonProperties.h
 * @brief  classes radon transformation
 *
 * @author Jan Wolzenburg
 * @date   October 2023
 *********************************************************************/

/*********************************************************************
   Includes
*********************************************************************/

 #include "coordinateSystem.h"
 #include "line.h"
 

/*********************************************************************
	Definitions
*********************************************************************/

/*!
 * @brief class for radon Coordinates
*/
class RadonCoordinates{

	public:

	/*!
	 * @brief constructor
	 * @param coordinate_system Reference coordinate system
	 * @param line Line
	*/
	RadonCoordinates( const CoordinateSystem* const coordinate_system, const Line line );


	double theta;		/*!< angle*/
	double distance;	/*!< distance*/
	
};


/*!
 * @brief radon point consisting of coordinates and an associated value
*/
class RadonPoint : public RadonCoordinates{

	public:

	/*!
	 * @brief cosntructor
	 * @param radon_coordinates Coordinates 
	 * @param value Value
	*/
	RadonPoint( const RadonCoordinates radon_coordinates, const double value ) :
		RadonCoordinates{ radon_coordinates }, value( value ) {};


	double value;				/*!< Value*/

};