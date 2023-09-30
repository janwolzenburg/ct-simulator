#pragma once
/*********************************************************************
 * @file   coordinates.h
 * @brief  Classes for coordinates in cartesian system
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/



 /*********************************************************************
	Includes
 *********************************************************************/
 #include "primitiveVec3.h"
 #include "cartesian.h"
#include "cSysTree.h"


 /*********************************************************************
	Definitions
 *********************************************************************/


 /*!
 * @brief Class for coordinates with coordinate system with context
*/
class coordinates : protected primitiveVec3{

	protected:
	const cartCSys* cSys;						/*!< Pointer to coordinate system*/


	public:

	/*!
	 * @brief Constructor
	 * @param vec3_ Values
	*/
	coordinates( const v3 vec3_, const cartCSys* const cSys_ ) : primitiveVec3{ vec3_ }, cSys( cSys ) {};

	/*!
	 * @brief Defaulkt constructor
	*/
	coordinates( void ) : coordinates{ v3{}, DUMMY_CSYS() } {};

	/*!
	 * @brief Convert coordinate's data to string
	 * @return String with coordinate's data
	*/
	string toStr( const unsigned int newLineTabulators = 0 ) const override;

	/*!
	 * @brief Comparison operator
	 * @param coords Coordinates to compare with
	 * @return True when all components are the same in global coordinate system
	*/
	bool operator== ( const coordinates coords ) const;

	/*!
	 * @brief Addition operator
	 * @param coords Coordinates to add
	 * @return Coordinates with sum of components in coordinate system of first operand
	*/
	coordinates operator+ ( const coordinates coords ) const;

	/*!
	 * @brief Substraction operator
	 * @param coords Coordinates to add
	 * @return Coordinates with difference of components in coordinate system of first operand
	*/
	coordinates operator- ( const coordinates coords ) const;

	/*!
	 * @brief Negation operator
	 * @return Negated coordinates
	*/
	coordinates operator- ( void ) const;

	/*!
	 * @brief Scale coordinates
	 * @param scalar Factor to scale by
	 * @return Scaled coordinates
	*/
	coordinates operator* ( const double scalar ) const;

	/*!
	 * @brief Divide coordinates
	 * @param divisor Value to divide by
	 * @return Divided coordinates
	*/
	coordinates operator/ ( const double divisor ) const;

	/*!
	 * @brief Check if coordinate system is this coordinate's system
	 * @param cSys_ System to check
	 * @return True when given system is this coordinate's system
	*/
	bool sameSystem( const cartCSys* const cSys_ ) const { return this->cSys == cSys_; };

	/*!
	 * @brief Check if two coordinates have the same coordiante system
	 * @param c Second set of coordinates
	 * @return True when both coordinates have the same coordiante system
	*/
	bool sameSystem( const coordinates c ) const { return sameSystem( c.cSys ); };

	/*!
	 * @brief Convert coordinates to a different coordinate system
	 * @param target_cSys System to convert to
	 * @return Coordinates in target system
	*/
	coordinates convertTo( const cartCSys* const target_cSys ) const;


	private:

	/*!
		* @brief Convert coordinate values to parent coordinate system
		* @return Coordinates in parent's system
	*/
	coordinates toParentcSys( void ) const;

	/*!
		* @brief Convert coordiante values to child coordinate system
		* @param child_cSys
		* @return Coordinates in child system
	*/
	coordinates toChildcSys( const cartCSys* const child_cSys ) const;
};