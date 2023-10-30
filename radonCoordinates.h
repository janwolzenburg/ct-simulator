#pragma once
/*********************************************************************
 * @file   radonProperties.h
 * @brief  Classes radon transformation
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
 * @brief Class for radon Coordinates
*/
class RadonCoordinates{

	public:

	/*!
	 * @brief Constructor
	 * @param coordinate_system Reference coordinate system
	 * @param line Line
	*/
	RadonCoordinates( const CoordinateSystem* const coordinate_system, const Line line );


	double theta;		/*!<Angle*/
	double distance;	/*!<Distance*/
	
};


/*!
 * @brief Radon point consisting of coordinates and an associated value
*/
class RadonPoint : public RadonCoordinates{

	public:

	/*!
	 * @brief Cosntructor
	 * @param radon_coordinates Coordinates 
	 * @param value Value
	*/
	RadonPoint( const RadonCoordinates radon_coordinates, const double value ) :
		RadonCoordinates{ radon_coordinates }, value( value ) {};


	double value;				/*!<Value*/

};