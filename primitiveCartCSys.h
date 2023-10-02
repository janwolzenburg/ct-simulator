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
class primitiveCartCSys : public mathObj{
	
	public:
	
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
	 * @brief Constructor from serialized data
	 * @param binData Reference to vector with binary data
	 * @param it Iterator to start of data in vector
	*/
	primitiveCartCSys( const vector<char>& binData, vector<char>::const_iterator& it );

	/*!
	 * @brief Convert coordinate system's data to string
	 * @return String with coordinate system's data
	*/
	string toStr( const unsigned int newLineTabulators = 0 ) const override;

	/*!
	 * @brief Get primitive origin
	 * @return Primitive represantation of the origin
	*/
	primitiveVec3 O( void ) const{ return origin; };

	/*!
	 * @brief Get primitive x Axis
	 * @return Primitive represantation of the x Axis
	*/
	primitiveVec3 Ex( void ) const{ return ex; };

	/*!
	 * @brief Get primitive y Axis
	 * @return Primitive represantation of the y Axis
	*/
	primitiveVec3 Ey( void ) const{ return ey; };

	/*!
	 * @brief Get primitive z Axis
	 * @return Primitive represantation of the z Axis
	*/
	primitiveVec3 Ez( void ) const{ return ez; };

	/*!
	 * @brief Serialize this object
	 * @param binData Reference to vector where data will be appended
	*/
	size_t serialize( vector<char>& binData ) const;


	protected:

	/*!
	 * @brief Set origin
	 * @param newOrigin New origin
	 * @return New origin
	*/
	primitiveVec3 O( const primitiveVec3 newOrigin ){ return origin = newOrigin; };

	/*!
	 * @brief Translate coordinate system
	 * @param c Translation in x, y and z direction
	*/
	void translateM( const primitiveVec3 coords ){ origin = origin + coords; };

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
