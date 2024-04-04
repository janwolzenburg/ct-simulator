#pragma once
/*********************************************************************
 * @file   Coordinates.h
 * @brief  classes for Coordinates in cartesian system
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/



/*********************************************************************
	Includes
*********************************************************************/
#include "generelMath.h"
#include "primitivevector3.h"
#include "coordinateSystem.h"
#include "coordinateSystemTree.h"


 /*********************************************************************
	Definitions
 *********************************************************************/


 /*!
 * @brief class for Coordinates with coordinate system with context
*/
class Coordinates : protected Primitivevector3{

	protected:
	const CoordinateSystem* coordinate_system_;						/*!< pointer to coordinate system*/


	public:

	/*!
	 * @brief constructor
	 * @param vec3_ Values
	*/
	Coordinates( const Tuple3D components, const CoordinateSystem* const coordinate_system ) : Primitivevector3{ components }, coordinate_system_( coordinate_system ) {};

	/*!
	 * @brief defaulkt constructor
	*/
	Coordinates( void ) : Coordinates{ Tuple3D{}, GetDummySystem() } {};

	/*!
	 * @brief convert coordinate's data to string
	 * @return string with coordinate's data
	*/
	string ConvertToString( const unsigned int newline_tabulators = 0 ) const override;

	/*!
	 * @brief comparison operator
	 * @param operand Coordinates to compare with
	 * @return true when all components are the same in global coordinate system
	*/
	bool operator== ( const Coordinates operand ) const;

	/*!
	 * @brief addition operator
	 * @param summand Coordinates to add
	 * @return Coordinates with sum of components in coordinate system of first operand
	*/
	Coordinates operator+ ( const Coordinates summand ) const;

	/*!
	 * @brief substraction operator
	 * @param subtrahend Coordinates to add
	 * @return Coordinates with difference of components in coordinate system of first operand
	*/
	Coordinates operator- ( const Coordinates subtrahend ) const;

	/*!
	 * @brief negation operator
	 * @return negated Coordinates
	*/
	Coordinates operator- ( void ) const;

	/*!
	 * @brief scale Coordinates
	 * @param scalar Factor to scale by
	 * @return scaled Coordinates
	*/
	Coordinates operator* ( const double scalar ) const;

	/*!
	 * @brief divide Coordinates
	 * @param divisor Value to divide by
	 * @return divided Coordinates
	*/
	Coordinates operator/ ( const double divisor ) const;

	/*!
	 * @brief check if coordinate system is this coordinate's system
	 * @param coordinate_system System to check
	 * @return true when given system is this coordinate's system
	*/
	bool HasSameSystem( const CoordinateSystem* const coordinate_system ) const { return this->coordinate_system_ == coordinate_system; };

	/*!
	 * @brief check if two Coordinates have the same coordiante system
	 * @param c Second set of Coordinates
	 * @return true when both Coordinates have the same coordiante system
	*/
	bool HasSameSystem( const Coordinates coordinates ) const { return HasSameSystem( coordinates.coordinate_system_ ); };

	/*!
	 * @brief convert Coordinates to a different coordinate system
	 * @param target_coordinate_system System to convert to
	 * @return Coordinates in target system
	*/
	Coordinates ConvertTo( const CoordinateSystem* const target_coordinate_system ) const;


	private:

	/*!
		* @brief convert coordinate values to parent_ coordinate system
		* @return Coordinates in parent_'s system
	*/
	Coordinates ConvertToParentSystem( void ) const;

	/*!
		* @brief convert coordiante values to child coordinate system
		* @param child_coordinate_system
		* @return Coordinates in child system
	*/
	Coordinates ConvertToChildSystem( const CoordinateSystem* const child_coordinate_system ) const;
};