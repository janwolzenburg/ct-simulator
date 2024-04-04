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





/*
	classes
*/

/*!
 * @brief physical voxel data_
*/
#pragma pack(push, 1)	// memory alignment for serializing model data without serializing single voxel data 
class VoxelData{
	
	public:

	typedef unsigned char SpecialPropertyEnumType;		/*!< Type to store up tu 8 special properties_*/

	/*!
	 * @brief special propterties a voxel can have
	*/
	enum SpecialProperty : SpecialPropertyEnumType{
		None =			0b00000000,
		Metal =			0b00000001,
		Undefined =	0b00000010
	};

	static const std::map<SpecialProperty, string> special_property_names;	/*!< Names of enumerated properties*/
	
	/*!
	 * @brief get enumeration from string
	 * @param property_string String to find property for
	 * @return Proeprty enumeration
	*/
	static SpecialProperty GetPropertyEnum( const string property_string );

	/*!
	 * @brief get the absorption at reference energy
	 * @param absorptionAtEnergy Absorption at given energy
	 * @param energy Energy
	 * @return absorption coefficient
	*/
	static double GetAbsorptionAtReferenceEnergy( const double absorptionAtEnergy, const double energy );

	/*!
	 * @brief constructor
	 * @param absorption_at_energy Absorption coefficient at given energy
	 * @param energy_eV Energy in eV
	*/
	VoxelData( const double absorption_at_energy, const double energy_eV, const SpecialProperty = None );

	/*!
	 * @brief constructor from serialized data
	 * @param binary_data vector with serialized data
	 * @param current_byte Start of data for this object
	*/
	VoxelData( const vector<char>& binary_data, vector<char>::const_iterator& current_byte );

	/*!
	 * @brief default constructor
	*/
	VoxelData( void ) : absorption_( -1 ), specialProperties_( Undefined ){};

	/*!
	 * @brief serialize this object
	 * @param binary_data reference to vector where data will be appended
	*/
	size_t Serialize( vector<char>& binary_data ) const;

	/*!
	 * @brief comparison
	 * @param d2 Second voxel data
	 * @return true when absorption of left operand is smaller than right's
	*/
	bool operator<( const VoxelData& d2 ) const{ return this->absorption_ < d2.absorption_; };

	/*!
	 * @brief comparison
	 * @param d2 Second voxel data
	 * @return true when absorption of left operand is greater than right's
	*/
	bool operator>( const VoxelData& d2 ) const{ return !operator<( d2 ); };

	/*!
	 * @brief add special property
	 * @param property Property to add
	*/
	void AddSpecialProperty( const SpecialProperty property ){ specialProperties_ |= ConvertToUnderlying( property ); };

	/*!
	 * @brief remove special property
	 * @param property Property to remove
	*/
	void RemoveSpecialProperty( const SpecialProperty property ){ specialProperties_ &= ~ConvertToUnderlying( property ); };

	/*!
	 * @brief get the absorption_ at given energy
	 * @param energy_eV Energy in eV
	 * @return absorption at given energy
	*/
	double GetAbsorptionAtEnergy( const double energy_eV ) const;

	/*!
	 * @brief get absorption_ at reference energy
	 * @return absorption at reference energy 
	*/
	double GetAbsorptionAtReferenceEnergy( void ) const{ return absorption_; };

	/*!
	 * @brief check if there is a special property
	 * @return true when is has at least one property
	*/
	bool HasSpecialProperty( void ) const{ return specialProperties_ != None; };
	
	/*!
	 * @brief check for specific property
	 * @param property Property to check
	 * @return true when voxel has specific property
	*/
	bool HasSpecificProperty( const SpecialProperty property ) const;

	/*!
	 * @brief set the metal artifact impact factor
	 * @param artefact_impact_factor New factor
	*/
	static void SetArtefactImpactFactor( const double artefact_impact_factor );


	private:

	static double artefact_impact_factor_;			/*!< Factor for altering metal artefact strength*/

	double absorption_	= -1;										/*!< absorption coefficient at reference Energy*/
	SpecialPropertyEnumType specialProperties_;	/*!< special properties in voxel*/

};
#pragma pack(pop)



/*!
 * @brief class for voxels
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
		Invalid = End	/*!< Invalid face x*/
	};

	/*!
	 * @brief constructor
	 * @param origin Origin point of voxel
	 * @param size	Size of voxel in unit of coordinate system's axes
	 * @param data	Physical data of voxel
	*/
	Voxel( const Point3D origin, const Tuple3D size, const VoxelData data );

	/*!
	 * @brief default constructor
	*/
	Voxel( void ) : Voxel{ Point3D{ Tuple3D{ 0, 0, 0 }, DummySystem() }, Tuple3D{ 1, 1, 1 }, VoxelData{} }{};

	/*!
	 * @brief convert result's data to string
	 * @return string with result's data
	*/
	string ConvertToString( unsigned int newline_tabulators = 0 ) const override;

	/*!
	 * @brief get origin point of voxel
	 * @return Origin point
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
	 * @param face Face ID
	 * @return Bounded surface which is the face
	*/
	BoundedSurface GetFace( Face face ) const{ return faces[ ConvertToUnderlying( face ) ]; };

	/*!
	 * @brief get center of voxel
	 * @return Point at center of voxel
	*/
	Point3D GetCenter( void ) const{ return origin_corner_ + vector3D{ Tuple3D{ size_.x / 2, size_.y / 2, size_.z / 2 } , origin_corner_.GetCoordinateSystem() }; };

	/*!
	 * @brief check if voxel contains point
	 * @param point Point to check
	 * @return true when p is inside the voxel
	*/
	bool Contains( const Point3D point ) const;


	private:

	Tuple3D size_;				/*!< size in local coordinate system*/
	VoxelData data_;			/*!< physical voxel data_*/
	Point3D origin_corner_;		/*!< point as origin_ of voxel in coordinate system*/
	array<BoundedSurface, ConvertToUnderlying( Face::End )> faces;	/*!< Faces in global context*/
};