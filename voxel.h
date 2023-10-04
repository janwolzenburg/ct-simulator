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
#include "generelMath.h"
#include "vec3D.h"
#include "surf.h"



 /*********************************************************************
	Definitions
 *********************************************************************/


 /*!
  * @brief Face identifiers of voxel
 */
enum class FACE_ID : size_t{
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
#pragma pack(push, 1)
class voxData{
	
	public:

	typedef unsigned char specialEnumType;

	enum specialProperty : specialEnumType{
		NONE =			0b00000000,
		METAL =			0b00000001,
		UNDEFINED =		0b00000010
	};

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
	voxData( void );

	bool operator<( const voxData& d2 ) const{ return this->attenuation < d2.attenuation; };

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
	 * @brief Serialize this object
	 * @param binData Reference to vector where data will be appended
	*/
	size_t serialize( vector<char>& binData ) const;

	void addSpecialProperty( const specialProperty property );

	void removeSpecialProperty( const specialProperty property );

	bool hasSpecialProperty( void ) const;
	
	bool hasSpecificProperty( const specialProperty property ) const;

	static double ReferencyEnergy( void ) { return referenceEnergy; };

	private:

	static constexpr double referenceEnergy = 120000.;		/*!<Reference Energy for attenuation coefficients in eV*/
	static constexpr double referenceEnergy_3 = 1.728e+15;		/*!<Cube of reference energy*/

	double attenuation	= -1;					/*!<Absorption coefficient at reference Energy*/
	specialEnumType specialProperties;

	private:

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
class vox : virtual public mathObj{

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
	vox();

	/*!
	 * @brief Copy constructor
	 * @param v Voxel to copy from
	*/
	vox( const vox& v );

	/*!
	 * @brief Convert result's data to string
	 * @return String with result's data
	*/
	string toStr( unsigned int newLineTabulators = 0 ) const override;

	/*!
	 * @brief Assignment constructor
	 * @param v Voxel to assign to this
	 * @return This voxel
	*/
	vox& operator=( const vox& v );

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
	surfLim getFace( FACE_ID id_ ) const;

	/*!
	 * @brief Get center of voxel
	 * @return Point at center of voxel
	*/
	pnt3 getCenter( void ) const;

	/*!
	 * @brief Check if voxel contains point
	 * @param p Point to check
	 * @return True when p is inside the voxel
	*/
	bool contains( const pnt3 p ) const;


	private:

	v3 size;										/*!<Size in local coordinate system*/
	voxData data;									/*!<Physical voxel data*/
	pnt3 o;											/*!<Point as origin of voxel in coordinate system*/
	surfLim faces[ toUnderlying( FACE_ID::END ) ];	/*!<Faces in global context*/
};