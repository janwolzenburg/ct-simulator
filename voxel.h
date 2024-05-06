#pragma once
/*********************************************************************
 * @file   voxel.h
 * @brief  classes for 3D volume elements
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/


  /*********************************************************************
	Includes
 *********************************************************************/

#include <array> 
using std::array;
#include <map>
using std::map;
#include "generelMath.h"
#include "vector3D.h"
#include "surface.h"



 /*********************************************************************
	Definitions
 *********************************************************************/

/*!
 * @brief physical voxel data
*/
#pragma pack(push, 1)	// memory alignment for serializing model data without serializing single voxel data 
class VoxelData{
	
	public:

	typedef unsigned char SpecialPropertyEnumType;		/*!< type to store up tu 8 special properties_*/

	/*!
	 * @brief special propterties a voxel can have
	*/
	enum class SpecialProperty : SpecialPropertyEnumType{
		None =			0,
		Metal =			1 << 0,
		Undefined =	1 << 1
	};

	static const std::map<SpecialProperty, string> special_property_names;	/*!< names of enumerated properties*/
	
	/*!
	 * @brief get enumeration from string
	 * @param property_string string to find property for
	 * @return property enumeration
	*/
	static SpecialProperty GetPropertyEnum( const string property_string );

	/*!
	 * @brief get the absorption at reference energy
	 * @param absorption_at_energy absorption at given energy
	 * @param energy energy
	 * @return absorption coefficient
	*/
	static double GetAbsorptionAtReferenceEnergy( const double absorption_at_energy, const double energy );

	/*!
	 * @brief constructor
	 * @param absorption_at_energy absorption coefficient at given energy
	 * @param energy_eV energy in eV
	 * @param special_properties special properties
	*/
	VoxelData( const double absorption_at_energy, const double energy_eV, const SpecialProperty special_properties = SpecialProperty::None );

	/*!
	 * @brief constructor from serialized data
	 * @param binary_data vector with serialized data
	 * @param current_byte Start of data for this object
	*/
	VoxelData( const vector<char>& binary_data, vector<char>::const_iterator& current_byte );

	/*!
	 * @brief default constructor
	*/
	VoxelData( void ) : absorption_( -1 ), specialProperties_( ConvertToUnderlying( SpecialProperty::Undefined ) ){};

	/*!
	 * @brief serialize this object
	 * @param binary_data reference to vector where data will be appended
	 * @return written bytes
	*/
	size_t Serialize( vector<char>& binary_data ) const;

	/*!
	 * @brief comparison
	 * @param voxel_data_to_compare second voxel data
	 * @return true when absorption of left operand is smaller than right's
	*/
	bool operator<( const VoxelData& voxel_data_to_compare ) const{ return this->absorption_ < voxel_data_to_compare.absorption_; };

	/*!
	 * @brief comparison
	 * @param voxel_data_to_compare second voxel data
	 * @return true when absorption of left operand is greater than right's
	*/
	bool operator>( const VoxelData& voxel_data_to_compare ) const{ return !operator<( voxel_data_to_compare ); };

	/*!
	 * @brief add special property
	 * @param property property to add
	*/
	void AddSpecialProperty( const SpecialProperty property ){ specialProperties_ |= ConvertToUnderlying( property ); };

	/*!
	 * @brief remove special property
	 * @param property property to remove
	*/
	void RemoveSpecialProperty( const SpecialProperty property ){ specialProperties_ &= ~ConvertToUnderlying( property ); };

	/*!
	 * @brief get the absorption_ at given energy
	 * @param energy_eV energy in eV
	 * @return absorption at given energy
	*/
	double GetAbsorptionAtEnergy( const double energy_eV ) const;

	/*!
	 * @brief get absorption at reference energy
	 * @return absorption at reference energy 
	*/
	double GetAbsorptionAtReferenceEnergy( void ) const{ return absorption_; };

	/*!
	 * @brief check if there is a special property
	 * @return true when is has at least one property
	*/
	bool HasSpecialProperty( void ) const{ return specialProperties_ != ConvertToUnderlying( SpecialProperty::None ); };
	
	/*!
	 * @brief check for specific property
	 * @param property property to check
	 * @return true when voxel has specific property
	*/
	bool HasSpecificProperty( const SpecialProperty property ) const;

	/*!
	 * @brief set the metal artifact impact factor
	 * @param artefact_impact_factor new factor
	*/
	static void SetArtefactImpactFactor( const double artefact_impact_factor );


	private:

	static double artefact_impact_factor_;			/*!< factor for altering metal artefact strength*/

	double absorption_	= -1;										/*!< absorption coefficient at reference Energy*/
	SpecialPropertyEnumType specialProperties_;	/*!< special properties in voxel*/

};
#pragma pack(pop)



/*!
 * @brief class for voxel
*/
class Voxel : public MathematicalObject{

	public:
	
	 /*!
	  * @brief face identifiers of voxel
	 */
	enum class Face : unsigned char{
		Begin,
		YZ_Xp = Begin,/*!< Y-Z plane at positive x*/
		XZ_Yp,				/*!< X-Z plane at positive y*/
		XY_Zp,				/*!< X-Y plane at positive z*/
		XY_Zm,				/*!< X-Y plane at negative x*/
		XZ_Ym,				/*!< X-Z plane at negative y*/
		YZ_Xm,				/*!< Y-Z plane at negative z*/
		End,
		Invalid = End	/*!< invalid face x*/
	};

	/*!
	 * @brief constructor
	 * @param origin origin point of voxel
	 * @param size	size of voxel in unit of coordinate system's axes
	 * @param data	physical data of voxel
	*/
	Voxel( const Point3D origin, const Tuple3D size, const VoxelData data );

	/*!
	 * @brief default constructor
	*/
	Voxel( void ) : Voxel{ Point3D{ Tuple3D{ 0, 0, 0 }, GetDummySystem() }, Tuple3D{ 1, 1, 1 }, VoxelData{} }{};

	/*!
	 * @brief convert result's data to string
	 * @return string with result's data
	*/
	string ConvertToString( unsigned int newline_tabulators = 0 ) const override;

	/*!
	 * @brief get origin point of voxel
	 * @return origin point
	*/
	Point3D origin_corner( void ) const{ return origin_corner_; };

	/*!
	 * @brief get size of voxel
	 * @return size of voxel
	*/
	Tuple3D size( void ) const{ return size_; };

	/*!
	 * @brief get data of voxel
	 * @return voxel data
	*/
	VoxelData data( void ) const{ return data_; };

	/*!
	 * @brief get face of voxel
	 * @param face face ID
	 * @return bounded surface which is the face
	*/
	BoundedSurface GetFace( Face face ) const{ return faces[ ConvertToUnderlying( face ) ]; };

	/*!
	 * @brief get center of voxel
	 * @return point at center of voxel
	*/
	Point3D GetCenter( void ) const{ return origin_corner_ + Vector3D{ Tuple3D{ size_.x / 2, size_.y / 2, size_.z / 2 } , origin_corner_.GetCoordinateSystem() }; };

	/*!
	 * @brief check if voxel contains point
	 * @param point Point to check
	 * @return true when p is inside the voxel
	*/
	bool Contains( const Point3D point ) const;


	private:

	Tuple3D size_;						/*!< size in local coordinate system*/
	VoxelData data_;					/*!< physical voxel data_*/
	Point3D origin_corner_;		/*!< point as origin_ of voxel in coordinate system*/
	array<BoundedSurface, ConvertToUnderlying( Face::End )> faces;	/*!< faces in global context*/
};