#pragma once
/*********************************************************************
 * @file   cartesian.h
 * @brief  classes for cartesian coordinate systems_
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
 * @brief class for coordinate system with context in another system
*/
class CoordinateSystem : private PrimitiveCoordinateSystem {
	
	friend class CoordinateSystemTree;

	public:

	/*!
	 * @brief convert coordinate system's data to string
	 * @return string with coordinate system's data_
	*/
	string ConvertToString( const unsigned int newline_tabulators = 0 ) const override;

	/*!
	 * @brief serialize this object
	 * @param binary_data reference to vector where data will be appended
	*/
	size_t Serialize( vector<char>& binary_data ) const;

	/*!
	 * @brief get pointer to this system's parent_
	 * @return Pointer to parent_
	*/
	const CoordinateSystem* parent( void ) const{ return parent_; };

	/*!
	 * @brief get the primitve coordinate system of this system
	 * @return Coordinate system without parent_ context
	*/
	PrimitiveCoordinateSystem GetPrimitive( void ) const{ return static_cast<PrimitiveCoordinateSystem>( *this ); };

	/*!
	 * @brief set the primitve coordinate system of this system
	 * @param primitiveCSys primitve system to set
	*/
	void SetPrimitive( const PrimitiveCoordinateSystem new_primitive );

	/*!
	 * @brief checks if this system is a global system
	 * @return true when this system is the global system in tree
	*/
	bool IsGlobal( void ) const{ return parent_ == nullptr; };

	/*!
	 * @brief create copy of this system
	 * @param newName Name of new system
	 * @return Pointer to new coordiante system with same baseCartSystem and parent_ as this
	*/
	CoordinateSystem* CreateCopy( const string new_name ) const;
	
	/*!
	 * @brief copy data from source to this
	 * @param sourceCSys Source system
	*/
	void CopyPrimitiveFrom( const CoordinateSystem* const source_system );

	/*!
	 * @brief add coordinate system to this system's tree
	 * @param origin Origin of coordinate system
	 * @param ex x-axis
	 * @param ey y-axis
	 * @param ez z-axis
	 * @param name_ Name of the system
	 * @return Pointer to created system with this system as parent_
	*/
	CoordinateSystem* AddCoordinateSystem( const Primitivevector3 origin, const Primitivevector3 ex, const Primitivevector3 ey, const Primitivevector3 ez, const string name ) const;

	/*!
	 * @brief get path from global system to this system
	 * @return vector with pointers to intermediate coordinate systems_ excluding global and this system
	*/
	vector<const CoordinateSystem *> GetPathFromGlobal( void ) const;

	/*!
	 * @brief get origin point in this system's context
	 * @return Point of system's origin_
	*/
	Point3D GetOriginPoint( void ) const;

	/*!
	 * @brief get origin point in this parent_'s context
	 * @return Point of system's origin_
	*/
	Point3D GetOriginInParentSystem( void ) const;

	/*!
	 * @brief get unit vector of x-axis in this system's context
	 * @return x-axis unit vector
	*/
	Unitvector3D GetEx( void ) const;

	/*!
	 * @brief get unit vector of y-axis in this system's context
	 * @return y-axis unit vector
	*/
	Unitvector3D GetEy( void ) const;

	/*!
	 * @brief get unit vector of z-axis in this system's context
	 * @return z-axis unit vector
	*/
	Unitvector3D GetEz( void ) const;

	/*!
	 * @brief get x-axis in parent_ coordinate system
	 * @return the x-axis as a Line
	*/
	Line GetXAxis( void ) const;

	/*!
	 * @brief get y-axis in parent_ coordinate system
	 * @return the y-axis as a Line
	*/
	Line GetYAxis( void ) const;

	/*!
	 * @brief get z-axis in parent_ coordinate system
	 * @return the z-axis as a Line
	*/
	Line GetZAxis( void ) const;

	/*!
	 * @brief get the xy-plane in parent_ coordinate system
	 * @return xy-plane as surface
	*/
	Surface GetXYPlane( void ) const;

	/*!
	 * @brief get the yz-plane in parent_ coordinate system
	 * @return yz-plane as surface
	*/
	Surface GetYZPlane( void ) const;

	/*!
	 * @brief get the xz-plane in parent_ coordinate system
	 * @return xz-plane as surface
	*/
	Surface GetXZPlane( void ) const;

	/*!
	 * @brief Translate coordinate system
	 * @param dV Translation vector
	*/
	MathError Translate( const vector3D direction );

	/*!
	 * @brief rotate coordinate system around vector
	 * @param n Rotation axis
	 * @param phi Angle
	 * @return error code
	*/
	MathError Rotate( const Unitvector3D axis, const double arc_angle );

	/*!
	 * @brief rotate coordinate system
	 * @param l Rotation axis
	 * @param phi Angle
	 * @return error code
	*/
	MathError Rotate( const Line axis, const double arc_angle );




	private:

	/*!
		* @brief constructor
		* @param origin Origin of coordinate system
		* @param ex x-axis
		* @param ey y-axis
		* @param ez z-axis
		* @param parent Pointer to parent_ system
		* @param name Name of the system
	*/
	CoordinateSystem( const Primitivevector3 origin, const Primitivevector3 ex, const Primitivevector3 ey, const Primitivevector3 ez,
				const CoordinateSystem* const parent, const string name );

	/*!
		* @brief default constructor
	*/
	CoordinateSystem( void );


	private:

	const CoordinateSystem* parent_;	/*!< pointer to parent system*/
	string name_;						/*!< Name of system*/
};