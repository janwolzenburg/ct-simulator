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
#include "vec3D.h"
#include "surf.h"



 /*********************************************************************
	Definitions
 *********************************************************************/


 /*!
  * @brief Face identifiers of voxel
 */
enum class FACE_ID : unsigned char{
	BEGIN,
	YZ_Xp = BEGIN,		/*!<Y-Z plane at positive x*/
	XZ_Yp,				/*!<X-Z plane at positive y*/
	XY_Zp,				/*!<X-Y plane at positive z*/
	XY_Zm,				/*!<X-Y plane at negative x*/
	XZ_Ym,				/*!<X-Z plane at negative y*/
	YZ_Xm,				/*!<Y-Z plane at negative z*/
	END,
	INVALID = END		/*!<Invalid face x*/
};



/*
	Classes
*/

/*!
 * @brief Physical voxel data
*/
#pragma pack(push, 1)	// Memory alignment for serializing model data without serializing single voxel data 
class voxData{
	
	public:

	typedef unsigned char specialEnumType;		/*!<Type to store up tu 8 special properties*/

	/*!
	 * @brief Special propterties a voxel can have
	*/
	enum specialProperty : specialEnumType{
		NONE =			0b00000000,
		METAL =			0b00000001,
		UNDEFINED =		0b00000010
	};

	/*!
	 * @brief Return the reference energy globally used
	 * @return Reference energy in eV
	*/
	static double ReferencyEnergy( void ) { return referenceEnergy; };

	/*!
	 * @brief Constructor
	 * @param attenuationAtFrequency Attenuation coefficient at given frequency
	 * @param frequency Frequency
	*/
	voxData( const double attenuationAtEnergy, const double energy, const specialProperty = UNDEFINED );

	/*!
	 * @brief Constructor from serialized data
	 * @param binData Vector with serialized data
	 * @param it Start of data for this object
	*/
	voxData( const vector<char>& binData, vector<char>::const_iterator& it );

	/*!
	 * @brief Default constructor
	*/
	voxData( void ) : attenuation( -1 ), specialProperties( UNDEFINED ){};

	/*!
	 * @brief Comparison
	 * @param d2 Second voxel data
	 * @return True when attenuation of left operand is smaller than right's
	*/
	bool operator<( const voxData& d2 ) const{ return this->attenuation < d2.attenuation; };

	/*!
	 * @brief Comparison
	 * @param d2 Second voxel data
	 * @return True when attenuation of left operand is greater than right's
	*/
	bool operator>( const voxData& d2 ) const{ return !operator<( d2 ); };

	/*!
	 * @brief Get the attenuation at given energy
	 * @param frequency Frequency
	 * @return Attenuation 
	*/
	double attenuationAt( const double energy ) const;

	/*!
	 * @brief Get attenuation at reference energy
	 * @return Attenuation at reference energy 
	*/
	double attenuationAtRefE( void ) const{ return attenuation; };

	/*!
	 * @brief Add special property
	 * @param property Property to add
	*/
	void addSpecialProperty( const specialProperty property ){ specialProperties |= toUnderlying( property ); };

	/*!
	 * @brief Remove special property
	 * @param property Property to remove
	*/
	void removeSpecialProperty( const specialProperty property ){ specialProperties &= ~toUnderlying( property ); };

	/*!
	 * @brief Check if there is a special property
	 * @return True when is has at least one property
	*/
	bool hasSpecialProperty( void ) const{ return specialProperties != NONE; };
	
	/*!
	 * @brief Check for specific property
	 * @param property Property to check
	 * @return True when voxel has specific property
	*/
	bool hasSpecificProperty( const specialProperty property ) const;

	/*!
	 * @brief Serialize this object
	 * @param binData Reference to vector where data will be appended
	*/
	size_t serialize( vector<char>& binData ) const;

		
	private:

	static constexpr double referenceEnergy = 120000.;			/*!<Reference Energy for attenuation coefficients in eV*/
	static constexpr double referenceEnergy_3 = 1.728e+15;		/*!<Cube of reference energy*/

	double attenuation	= -1;									/*!<Absorption coefficient at reference Energy*/
	specialEnumType specialProperties;							/*!<Special properties in voxel*/


	/*!
	 * @brief Return the 
	 * @param attenuationAtEnergy
	 * @param frequency 
	 * @return 
	*/
	double attenuationAtRefE( const double attenuationAtEnergy, const double energy ) const;

};
#pragma pack(pop)



/*!
 * @brief Class for voxels
*/
class vox : public mathObj{

	public:

	/*!
	 * @brief Constructor
	 * @param o_ Origin point of voxel
	 * @param size_	Size of voxel in unit of coordinate system's axes
	 * @param data_	Physical data of voxel
	*/
	vox( const pnt3 o_, const v3 size_, const voxData data_ );

	/*!
	 * @brief Default constructor
	*/
	vox( void ) : vox{ pnt3{ v3{ 0, 0, 0 }, DUMMY_CSYS() }, v3{1, 1, 1}, voxData{} }{};

	/*!
	 * @brief Convert result's data to string
	 * @return String with result's data
	*/
	string toStr( unsigned int newLineTabulators = 0 ) const override;

	/*!
	 * @brief Get origin point of voxel
	 * @return Origin point
	*/
	pnt3 O( void ) const{ return o; };

	/*!
	 * @brief Get size of voxel
	 * @return Size of voxel
	*/
	v3 Size( void ) const{ return size; };

	/*!
	 * @brief Get data of voxel
	 * @return Voxel data
	*/
	voxData Data( void ) const{ return data; };

	/*!
	 * @brief Get face of voxel
	 * @param id_ Face ID
	 * @return Bounded surface which is the face
	*/
	surfLim getFace( FACE_ID id_ ) const{ return faces[ toUnderlying( id_ ) ]; };

	/*!
	 * @brief Get center of voxel
	 * @return Point at center of voxel
	*/
	pnt3 getCenter( void ) const{ return o + vec3{ v3{ size.x / 2, size.y / 2, size.z / 2 } , o.CSys() }; };

	/*!
	 * @brief Check if voxel contains point
	 * @param p Point to check
	 * @return True when p is inside the voxel
	*/
	bool contains( const pnt3 p ) const;


	private:

	v3 size;											/*!<Size in local coordinate system*/
	voxData data;										/*!<Physical voxel data*/
	pnt3 o;												/*!<Point as origin of voxel in coordinate system*/
	array<surfLim, toUnderlying( FACE_ID::END )> faces;	/*!<Faces in global context*/
};