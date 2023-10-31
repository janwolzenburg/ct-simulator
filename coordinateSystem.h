#pragma once
/*********************************************************************
 * @file   cartesian.h
 * @brief  Classes for cartesian coordinate systems_
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/



 /*********************************************************************
	Includes
 *********************************************************************/
 #include "primitiveCoordinateSystem.h"

#include "coordinateSystemTree.fwd.h"
#include "vector3D.fwd.h"
#include "line.fwd.h"
#include "surf.fwd.h"



 /*********************************************************************
	Definitions
 *********************************************************************/


/*!
 * @brief Class for coordinate system with context in another system
*/
class CoordinateSystem : private PrimitiveCoordinateSystem {
	
	friend class CoordinateSystemTree;

	public:

	/*!
	 * @brief Convert coordinate system's data to string
	 * @return String with coordinate system's data_
	*/
	string ToString( const unsigned int newline_tabulators = 0 ) const override;

	/*!
	 * @brief Serialize this object
	 * @param binary_data Reference to vector where data will be appended
	*/
	size_t Serialize( vector<char>& binary_data ) const;

	/*!
	 * @brief Get pointer to this system's parent_
	 * @return Pointer to parent_
	*/
	const CoordinateSystem* parent( void ) const{ return parent_; };

	/*!
	 * @brief Get the primitve coordinate system of this system
	 * @return Coordinate system without parent_ context
	*/
	PrimitiveCoordinateSystem GetPrimitive( void ) const{ return static_cast<PrimitiveCoordinateSystem>( *this ); };

	/*!
	 * @brief Set the primitve coordinate system of this system
	 * @param primitiveCSys primitve system to set
	*/
	void SetPrimitive( const PrimitiveCoordinateSystem new_primitive );

	/*!
	 * @brief Checks if this system is a global system
	 * @return True when this system is the global system in tree
	*/
	bool IsGlobal( void ) const{ return parent_ == nullptr; };

	/*!
	 * @brief Create copy of this system
	 * @param newName Name of new system
	 * @return Pointer to new coordiante system with same baseCartSystem and parent_ as this
	*/
	CoordinateSystem* CreateCopy( const string new_name ) const;
	
	/*!
	 * @brief Copy data from source to this
	 * @param sourceCSys Source system
	*/
	void CopyPrimitiveFrom( const CoordinateSystem* const source_system );

	/*!
	 * @brief Add coordinate system to this system's tree
	 * @param origin Origin of coordinate system
	 * @param ex x-axis
	 * @param ey y-axis
	 * @param ez z-axis
	 * @param name_ Name of the system
	 * @return Pointer to created system with this system as parent_
	*/
	CoordinateSystem* AddCoordinateSystem( const PrimitiveVector3 origin, const PrimitiveVector3 ex, const PrimitiveVector3 ey, const PrimitiveVector3 ez, const string name ) const;

	/*!
	 * @brief Get path from global system to this system
	 * @return Vector with pointers to intermediate coordinate systems_ excluding global and this system
	*/
	vector<const CoordinateSystem *> GetPathFromGlobal( void ) const;

	/*!
	 * @brief Get origin point in this system's context
	 * @return Point of system's origin_
	*/
	Point3D GetOriginPoint( void ) const;

	/*!
	 * @brief Get origin point in this parent_'s context
	 * @return Point of system's origin_
	*/
	Point3D GetOriginInParentSystem( void ) const;

	/*!
	 * @brief Get unit vector of x-axis in this system's context
	 * @return x-axis unit vector
	*/
	UnitVector3D GetEx( void ) const;

	/*!
	 * @brief Get unit vector of y-axis in this system's context
	 * @return y-axis unit vector
	*/
	UnitVector3D GetEy( void ) const;

	/*!
	 * @brief Get unit vector of z-axis in this system's context
	 * @return z-axis unit vector
	*/
	UnitVector3D GetEz( void ) const;

	/*!
	 * @brief Get x-axis in parent_ coordinate system
	 * @return The x-axis as a Line
	*/
	Line GetXAxis( void ) const;

	/*!
	 * @brief Get y-axis in parent_ coordinate system
	 * @return The y-axis as a Line
	*/
	Line GetYAxis( void ) const;

	/*!
	 * @brief Get z-axis in parent_ coordinate system
	 * @return The z-axis as a Line
	*/
	Line GetZAxis( void ) const;

	/*!
	 * @brief Get the xy-plane in parent_ coordinate system
	 * @return xy-plane as surface
	*/
	Surface GetXYPlane( void ) const;

	/*!
	 * @brief Get the yz-plane in parent_ coordinate system
	 * @return yz-plane as surface
	*/
	Surface GetYZPlane( void ) const;

	/*!
	 * @brief Get the xz-plane in parent_ coordinate system
	 * @return xz-plane as surface
	*/
	Surface GetXZPlane( void ) const;

	/*!
	 * @brief Translate coordinate system
	 * @param dV Translation vector
	*/
	MathError Translate( const Vector3D direction );

	/*!
	 * @brief Rotate coordinate system around vector
	 * @param n Rotation axis
	 * @param phi Angle
	 * @return Error code
	*/
	MathError Rotate( const UnitVector3D axis, const double arc_angle );

	/*!
	 * @brief Rotate coordinate system
	 * @param l Rotation axis
	 * @param phi Angle
	 * @return Error code
	*/
	MathError Rotate( const Line axis, const double arc_angle );




	private:

	/*!
		* @brief Constructor
		* @param origin Origin of coordinate system
		* @param ex x-axis
		* @param ey y-axis
		* @param ez z-axis
		* @param parent Pointer to parent_ system
		* @param name Name of the system
	*/
	CoordinateSystem( const PrimitiveVector3 origin, const PrimitiveVector3 ex, const PrimitiveVector3 ey, const PrimitiveVector3 ez,
				const CoordinateSystem* const parent, const string name );

	/*!
		* @brief Default constructor
	*/
	CoordinateSystem( void );


	private:

	const CoordinateSystem* parent_;	/*!<Pointer to parent system*/
	string name_;						/*!<Name of system*/
};