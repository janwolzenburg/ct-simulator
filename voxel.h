#pragma once
/*********************************************************************
 * @file   voxel.h
 * @brief  Classes for 3D volume elements
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/


  /*********************************************************************
	Includes
 *********************************************************************/

#include <array> 
using std::array;
#include "generelMath.h"
#include "vector3D.h"
#include "surface.h"



 /*********************************************************************
	Definitions
 *********************************************************************/





/*
	Classes
*/

/*!
 * @brief Physical voxel data_
*/
#pragma pack(push, 1)	// Memory alignment for serializing model data without serializing single voxel data 
class VoxelData{
	
	public:

	typedef unsigned char SpecialPropertyEnumType;		/*!< Type to store up tu 8 special properties_*/

	/*!
	 * @brief Special propterties a voxel can have
	*/
	enum SpecialProperty : SpecialPropertyEnumType{
		None =			0b00000000,
		Metal =			0b00000001,
		Undefined =		0b00000010
	};

	/*!
	 * @brief Constructor
	 * @param attenuation_at_energy Attenuation coefficient at given energy
	 * @param energy_eV Energy in eV
	*/
	VoxelData( const double attenuation_at_energy, const double energy_eV, const SpecialProperty = None );

	/*!
	 * @brief Constructor from serialized data
	 * @param binary_data Vector with serialized data
	 * @param current_byte Start of data for this object
	*/
	VoxelData( const vector<char>& binary_data, vector<char>::const_iterator& current_byte );

	/*!
	 * @brief Default constructor
	*/
	VoxelData( void ) : attenuation_( -1 ), specialProperties_( Undefined ){};

	/*!
	 * @brief Serialize this object
	 * @param binary_data Reference to vector where data will be appended
	*/
	size_t Serialize( vector<char>& binary_data ) const;

	/*!
	 * @brief Comparison
	 * @param d2 Second voxel data
	 * @return True when attenuation of left operand is smaller than right's
	*/
	bool operator<( const VoxelData& d2 ) const{ return this->attenuation_ < d2.attenuation_; };

	/*!
	 * @brief Comparison
	 * @param d2 Second voxel data
	 * @return True when attenuation of left operand is greater than right's
	*/
	bool operator>( const VoxelData& d2 ) const{ return !operator<( d2 ); };

	/*!
	 * @brief Add special property
	 * @param property Property to add
	*/
	void AddSpecialProperty( const SpecialProperty property ){ specialProperties_ |= ToUnderlying( property ); };

	/*!
	 * @brief Remove special property
	 * @param property Property to remove
	*/
	void RemoveSpecialProperty( const SpecialProperty property ){ specialProperties_ &= ~ToUnderlying( property ); };

	/*!
	 * @brief Get the attenuation_ at given energy
	 * @param energy_eV Energy in eV
	 * @return Attenuation at given energy
	*/
	double GetAttenuationAtEnergy( const double energy_eV ) const;

	/*!
	 * @brief Get attenuation_ at reference energy
	 * @return Attenuation at reference energy 
	*/
	double GetAttenuationAtReferenceEnergy( void ) const{ return attenuation_; };

	/*!
	 * @brief Check if there is a special property
	 * @return True when is has at least one property
	*/
	bool HasSpecialProperty( void ) const{ return specialProperties_ != None; };
	
	/*!
	 * @brief Check for specific property
	 * @param property Property to check
	 * @return True when voxel has specific property
	*/
	bool HasSpecificProperty( const SpecialProperty property ) const;


	private:


	double attenuation_	= -1;										/*!< Absorption coefficient at reference Energy*/
	SpecialPropertyEnumType specialProperties_;						/*!< Special properties_ in voxel*/


	/*!
	 * @brief Get the attenuation at reference energy
	 * @param attenuationAtEnergy Attenuation at given energy
	 * @param energy Energy
	 * @return Attenuation coefficient
	*/
	double GetAttenuationAtReferenceEnergy( const double attenuationAtEnergy, const double energy ) const;

};
#pragma pack(pop)



/*!
 * @brief Class for voxels
*/
class Voxel : public MathematicalObject{

	public:
	
	 /*!
	  * @brief Face identifiers of voxel
	 */
	enum class Face : unsigned char{
		Begin,
		YZ_Xp = Begin,		/*!< Y-Z plane at positive x*/
		XZ_Yp,				/*!< X-Z plane at positive y*/
		XY_Zp,				/*!< X-Y plane at positive z*/
		XY_Zm,				/*!< X-Y plane at negative x*/
		XZ_Ym,				/*!< X-Z plane at negative y*/
		YZ_Xm,				/*!< Y-Z plane at negative z*/
		End,
		Invalid = End		/*!< Invalid face x*/
	};

	/*!
	 * @brief Constructor
	 * @param origin Origin point of voxel
	 * @param size	Size of voxel in unit of coordinate system's axes
	 * @param data	Physical data of voxel
	*/
	Voxel( const Point3D origin, const Tuple3D size, const VoxelData data );

	/*!
	 * @brief Default constructor
	*/
	Voxel( void ) : Voxel{ Point3D{ Tuple3D{ 0, 0, 0 }, DummySystem() }, Tuple3D{ 1, 1, 1 }, VoxelData{} }{};

	/*!
	 * @brief Convert result's data to string
	 * @return String with result's data
	*/
	string ToString( unsigned int newline_tabulators = 0 ) const override;

	/*!
	 * @brief Get origin point of voxel
	 * @return Origin point
	*/
	Point3D origin_corner( void ) const{ return origin_corner_; };

	/*!
	 * @brief Get size of voxel
	 * @return Size of voxel
	*/
	Tuple3D size( void ) const{ return size_; };

	/*!
	 * @brief Get data of voxel
	 * @return Voxel data
	*/
	VoxelData data( void ) const{ return data_; };

	/*!
	 * @brief Get face of voxel
	 * @param face Face ID
	 * @return Bounded surface which is the face
	*/
	BoundedSurface GetFace( Face face ) const{ return faces[ ToUnderlying( face ) ]; };

	/*!
	 * @brief Get center of voxel
	 * @return Point at center of voxel
	*/
	Point3D GetCenter( void ) const{ return origin_corner_ + Vector3D{ Tuple3D{ size_.x / 2, size_.y / 2, size_.z / 2 } , origin_corner_.GetCoordinateSystem() }; };

	/*!
	 * @brief Check if voxel contains point
	 * @param point Point to check
	 * @return True when p is inside the voxel
	*/
	bool Contains( const Point3D point ) const;


	private:

	Tuple3D size_;				/*!< Size in local coordinate system*/
	VoxelData data_;			/*!< Physical voxel data_*/
	Point3D origin_corner_;		/*!< Point as origin_ of voxel in coordinate system*/
	array<BoundedSurface, ToUnderlying( Face::End )> faces;	/*!< Faces in global context*/
};