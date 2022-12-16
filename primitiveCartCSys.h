#pragma once
/*********************************************************************
 * @file   primitiveCartCSys.h
 * @brief  Classes for cartesian coordinate systems
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/



 /*********************************************************************
	Includes
 *********************************************************************/
#include "generelMath.h"
#include "primitiveVec3.h"


 /*********************************************************************
	Definitions
 *********************************************************************/


/*!
 * @brief Class describing a right-handed cartesian coordinate system
*/
class primitiveCartCSys : virtual public mathObj{

	protected:

	/*!
	 * @brief Constructor
	 * @param origin_ O of coordinate system
	 * @param ex_ x-axis
	 * @param ey_ y-axis
	 * @param ez_ z-axis
	 * @details Axes must be orthogonal to each other
	*/
	primitiveCartCSys( const primitiveVec3 origin_, const primitiveVec3 ex_, const primitiveVec3 ey_, const primitiveVec3 ez_ );

	/*!
	 * @brief Convert coordinate system's data to string
	 * @return String with coordinate system's data
	*/
	string toStr( const unsigned int newLineTabulators = 0 ) const override;

	/*!
	 * @brief Set origin
	 * @param newOrigin New origin
	 * @return New origin
	*/
	inline primitiveVec3 O( const primitiveVec3 newOrigin ){ return origin = newOrigin; };

	/*!
	 * @brief Translate coordinate system
	 * @param c Translation in x, y and z direction
	*/
	inline void translateM( const primitiveVec3 coords ){ origin = origin + coords; };

	/*!
	 * @brief Rotate coordinate system
	 * @param n Rotation axis
	 * @param phi Rotation angle
	*/
	MATH_ERR rotateM( const primitiveVec3 n, const double phi );


	protected:
	primitiveVec3 origin;		/*!< Origin point */
	primitiveVec3 ex;			/*!< x-axis unit vector */
	primitiveVec3 ey;			/*!< y-axis unit vector */
	primitiveVec3 ez;			/*!< z-axis unit vector */
};
