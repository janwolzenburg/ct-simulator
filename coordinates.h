#pragma once
/*********************************************************************
 * @file   Coordinates.h
 * @brief  Classes for Coordinates in cartesian system
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
 * @brief Class for Coordinates with coordinate system with context
*/
class Coordinates : protected PrimitiveVector3{

	protected:
	const CoordinateSystem* coordinate_system_;						/*!< Pointer to coordinate system*/


	public:

	/*!
	 * @brief Constructor
	 * @param vec3_ Values
	*/
	Coordinates( const Tuple3D vec3_, const CoordinateSystem* const coordinate_system ) : PrimitiveVector3{ vec3_ }, coordinate_system_( coordinate_system ) {};

	/*!
	 * @brief Defaulkt constructor
	*/
	Coordinates( void ) : Coordinates{ Tuple3D{}, DummySystem() } {};

	/*!
	 * @brief Convert coordinate's data to string
	 * @return String with coordinate's data
	*/
	string ToString( const unsigned int newline_tabulators = 0 ) const override;

	/*!
	 * @brief Comparison operator
	 * @param coordinates Coordinates to compare with
	 * @return True when all components are the same in global coordinate system
	*/
	bool operator== ( const Coordinates coordinates ) const;

	/*!
	 * @brief Addition operator
	 * @param coordinates Coordinates to add
	 * @return Coordinates with sum of components in coordinate system of first operand
	*/
	Coordinates operator+ ( const Coordinates coordinates ) const;

	/*!
	 * @brief Substraction operator
	 * @param coordinates Coordinates to add
	 * @return Coordinates with difference of components in coordinate system of first operand
	*/
	Coordinates operator- ( const Coordinates coordinates ) const;

	/*!
	 * @brief Negation operator
	 * @return Negated Coordinates
	*/
	Coordinates operator- ( void ) const;

	/*!
	 * @brief Scale Coordinates
	 * @param scalar Factor to scale by
	 * @return Scaled Coordinates
	*/
	Coordinates operator* ( const double scalar ) const;

	/*!
	 * @brief Divide Coordinates
	 * @param divisor Value to divide by
	 * @return Divided Coordinates
	*/
	Coordinates operator/ ( const double divisor ) const;

	/*!
	 * @brief Check if coordinate system is this coordinate's system
	 * @param coordinate_system System to check
	 * @return True when given system is this coordinate's system
	*/
	bool IsSameSystem( const CoordinateSystem* const coordinate_system ) const { return this->coordinate_system_ == coordinate_system; };

	/*!
	 * @brief Check if two Coordinates have the same coordiante system
	 * @param c Second set of Coordinates
	 * @return True when both Coordinates have the same coordiante system
	*/
	bool IsSameSystem( const Coordinates coordinates ) const { return IsSameSystem( coordinates.coordinate_system_ ); };

	/*!
	 * @brief Convert Coordinates to a different coordinate system
	 * @param target_coordinate_system System to convert to
	 * @return Coordinates in target system
	*/
	Coordinates convertTo( const CoordinateSystem* const target_coordinate_system ) const;


	private:

	/*!
		* @brief Convert coordinate values to parent_ coordinate system
		* @return Coordinates in parent_'s system
	*/
	Coordinates ConvertToParentSystem( void ) const;

	/*!
		* @brief Convert coordiante values to child coordinate system
		* @param child_coordinate_system
		* @return Coordinates in child system
	*/
	Coordinates ConvertToChildSystem( const CoordinateSystem* const child_coordinate_system ) const;
};