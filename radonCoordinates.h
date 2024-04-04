#pragma once
/*********************************************************************
 * @file   radonCoordinates.h
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
	 * @param coordinate_system reference coordinate system
	 * @param line line to calculate the radon coordinates for
	*/
	RadonCoordinates( const CoordinateSystem* const coordinate_system, const Line line );


	double theta;			/*!< angle*/
	double distance;	/*!< distance*/
	
};


/*!
 * @brief radon point consisting of coordinates and an associated value
*/
class RadonPoint : public RadonCoordinates{

	public:

	/*!
	 * @brief cosntructor
	 * @param radon_coordinates coordinates 
	 * @param value value
	*/
	RadonPoint( const RadonCoordinates radon_coordinates, const double value ) :
		RadonCoordinates{ radon_coordinates }, value( value ) {};

	double value;				/*!< value*/

};