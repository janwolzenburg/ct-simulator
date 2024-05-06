#pragma once
/*********************************************************************
 * @file   coordinates.h
 * @brief  classes for coordinates in cartesian system
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/



/*********************************************************************
	Includes
*********************************************************************/
#include "generelMath.h"
#include "primitiveVector3.h"
#include "coordinateSystem.h"
#include "coordinateSystemTree.h"


 /*********************************************************************
	Definitions
 *********************************************************************/


 /*!
 * @brief class for Coordinates with coordinate system with context
*/
class Coordinates : protected PrimitiveVector3{

	protected:
	const CoordinateSystem* coordinate_system_;						/*!< pointer to coordinate system*/


	public:

	/*!
	 * @brief constructor
	 * @param components coordinate components
	 * @param coordinate_system system in which these coordinates are defined
	*/
	Coordinates( const Tuple3D components, const CoordinateSystem* const coordinate_system ) : PrimitiveVector3{ components }, coordinate_system_( coordinate_system ) {};

	/*!
	 * @brief default constructor
	*/
	Coordinates( void ) : Coordinates{ Tuple3D{}, GetDummySystem() } {};

	/*!
	 * @brief convert coordinate's data to string
	 * @return string with coordinate's data
	*/
	string ConvertToString( const unsigned int newline_tabulators = 0 ) const override;

	/*!
	 * @brief comparison operator
	 * @param coordinates_to_compare coordinates to compare with
	 * @return true when all components are the same in global coordinate system
	*/
	bool operator== ( const Coordinates coordinates_to_compare ) const;

	/*!
	 * @brief addition operator
	 * @param summand coordinates to add
	 * @return coordinates with sum of components in coordinate system of first operand
	*/
	Coordinates operator+ ( const Coordinates summand ) const;

	/*!
	 * @brief substraction operator
	 * @param subtrahend coordinates to add
	 * @return coordinates with difference of components in coordinate system of first operand
	*/
	Coordinates operator- ( const Coordinates subtrahend ) const;

	/*!
	 * @brief negation operator
	 * @return negated Coordinates
	*/
	Coordinates operator- ( void ) const;

	/*!
	 * @brief scale Coordinates
	 * @param scalar factor to scale by
	 * @return scaled Coordinates
	*/
	Coordinates operator* ( const double scalar ) const;

	/*!
	 * @brief divide Coordinates
	 * @param divisor value to divide by
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
	 * @brief check if two coordinates have the same coordiante system
	 * @param coordinates second set of coordinates
	 * @return true when both coordinates have the same coordiante system
	*/
	bool HasSameSystem( const Coordinates coordinates ) const { return HasSameSystem( coordinates.coordinate_system_ ); };

	/*!
	 * @brief convert coordinates to a different coordinate system
	 * @param target_coordinate_system System to convert to
	 * @return coordinates in target system
	*/
	Coordinates ConvertTo( const CoordinateSystem* const target_coordinate_system ) const;


	private:

	/*!
		* @brief convert coordinate values to parent coordinate system
		* @return coordinates in parent's system
	*/
	Coordinates ConvertToParentSystem( void ) const;

	/*!
		* @brief convert coordiante values to child coordinate system
		* @param child_coordinate_system
		* @return coordinates in child system
	*/
	Coordinates ConvertToChildSystem( const CoordinateSystem* const child_coordinate_system ) const;
};