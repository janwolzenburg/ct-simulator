#pragma once
/*********************************************************************
 * @file   cartesian.h
 * @brief  Classes for cartesian coordinate systems
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/



 /*********************************************************************
	Includes
 *********************************************************************/
 #include "primitiveCartCSys.h"

#include "cSysTree.fwd.h"
#include "vec3D.fwd.h"
#include "line.fwd.h"
#include "surf.fwd.h"



 /*********************************************************************
	Definitions
 *********************************************************************/


/*!
 * @brief Class for coordinate system with context in another system
*/
class cartCSys : private primitiveCartCSys {
	
	friend class cSysTree;

	public:

	/*!
	 * @brief Convert coordinate system's data to string
	 * @return String with coordinate system's data
	*/
	string toStr( const unsigned int newLineTabulators = 0 ) const override;

	/*!
	 * @brief Get pointer to this system's parent
	 * @return Pointer to parent
	*/
	inline const cartCSys* Parent( void ) const { return parent; };

	/*!
	 * @brief Checks if this system is a global system
	 * @return True when this system is the global system in tree
	*/
	inline bool isGlobal( void ) const{ return parent == nullptr; };

	/*!
	 * @brief Create copy of this system
	 * @param newName Name of new system
	 * @return Pointer to new coordiante system with same baseCartSystem and parent as this
	*/
	cartCSys* createCopy( const string newName ) const;
	
	/*!
	 * @brief Add coordinate system to this system's tree
	 * @param origin_ O of coordinate system
	 * @param ex_ x-axis
	 * @param ey_ y-axis
	 * @param ez_ z-axis
	 * @param name_ Name of the system
	 * @return Pointer to created system with this system as parent
	*/
	cartCSys* addCSys( const primitiveVec3 origin_, const primitiveVec3 ex_, const primitiveVec3 ey_, const primitiveVec3 ez_, const std::string name_ ) const;

	/*!
	 * @brief Get path from global system to this system
	 * @return Vector with pointers to intermediate coordinate systems excluding global and this system
	*/
	vector<const cartCSys*> getPathFromGlobal( void ) const;

	/*!
	 * @brief Get origin of this coordinate system in context of parent
	 * @return Origin componenets
	*/
	inline primitiveVec3 O( void ) const { return origin; };

	/*!
	 * @brief Get x-axis of this coordinate system in context of parent
	 * @return x-axis values
	*/
	inline primitiveVec3 Ex( void ) const{ return ex; };

	/*!
	 * @brief Get y-axis of this coordinate system in context of parent
	 * @return y-axis values
	*/
	inline primitiveVec3 Ey( void ) const{ return ey; };

	/*!
	 * @brief Get z-axis of this coordinate system in context of parent
	 * @return z-axis values
	*/
	inline primitiveVec3 Ez( void ) const{ return ez; };

	/*!
	 * @brief Get origin point in this system's context
	 * @return Point of system's origin
	*/
	pnt3 OPnt( void ) const;

	/*!
	 * @brief Get origin point in this parent's context
	 * @return Point of system's origin
	*/
	pnt3 OPntPrnt( void ) const;

	/*!
	 * @brief Get unit vector of x-axis in this system's context
	 * @return x-axis unit vector
	*/
	uvec3 ExVec( void ) const;

	/*!
	 * @brief Get unit vector of y-axis in this system's context
	 * @return y-axis unit vector
	*/
	uvec3 EyVec( void ) const;

	/*!
	 * @brief Get unit vector of z-axis in this system's context
	 * @return z-axis unit vector
	*/
	uvec3 EzVec( void ) const;

	/*!
	 * @brief Get x-axis in parent coordinate system
	 * @return The x-axis as a line
	*/
	line xAxis( void ) const;

	/*!
	 * @brief Get y-axis in parent coordinate system
	 * @return The y-axis as a line
	*/
	line yAxis( void ) const;

	/*!
	 * @brief Get z-axis in parent coordinate system
	 * @return The z-axis as a line
	*/
	line zAxis( void ) const;

	/*!
	 * @brief Get the xy-plane in parent coordinate system
	 * @return xy-plane as surface
	*/
	surf xyPlane( void ) const;

	/*!
	 * @brief Get the yz-plane in parent coordinate system
	 * @return yz-plane as surface
	*/
	surf yzPlane( void ) const;

	/*!
	 * @brief Get the xz-plane in parent coordinate system
	 * @return xz-plane as surface
	*/
	surf xzPlane( void ) const;

	/*!
	 * @brief Translate coordinate system
	 * @param dV Translation vector
	*/
	MATH_ERR translateM( const vec3 dV );

	/*!
	 * @brief Rotate coordinate system around vector
	 * @param n Rotation axis
	 * @param phi Angle
	 * @return Error code
	*/
	MATH_ERR rotateM( const uvec3 n, const double phi );

	/*!
	 * @brief Rotate coordinate system
	 * @param l Rotation axis
	 * @param phi Angle
	 * @return Error code
	*/
	MATH_ERR rotateM( const line l, const double phi );

	/*!
	 * @brief Get the primitve coordinate system of this system
	 * @return Coordinate system without parent context
	*/
	inline primitiveCartCSys getPrimitive( void ) const{ return (primitiveCartCSys) *this; };

	/*!
	 * @brief Set the primitve coordinate system of this system
	 * @param primitiveCSys primitve system to set
	*/
	void setPrimitive( const primitiveCartCSys primitiveCSys );

	/*!
	 * @brief Serialize this object
	 * @param binData Reference to vector where data will be appended
	*/
	size_t serialize( vector<char>& binData ) const;

	private:

	/*!
		* @brief Constructor
		* @param origin_ O of coordinate system
		* @param ex_ x-axis
		* @param ey_ y-axis
		* @param ez_ z-axis
		* @param parent_ Pointer to parent system
		* @param name_ Name of the system
	*/
	cartCSys( const primitiveVec3 origin_, const primitiveVec3 ex_, const primitiveVec3 ey_, const primitiveVec3 ez_,
				const cartCSys* const parent_, const string name_ );

	/*!
		* @brief Default constructor
	*/
	cartCSys( void );


	private:

	const cartCSys* parent;		/*!<Pointer to parent system*/
	string name;				/*!<Name of system*/
};