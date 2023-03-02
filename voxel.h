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

 constexpr double coefficientReferenceEnergy = 120000.;
 constexpr double fAtRefE = coefficientReferenceEnergy / h_Js;
 const double fAtRefE_3 = pow( fAtRefE, 3 );

/*!
 * @brief Physical voxel data
*/
class voxData{
	
	public:
	voxData( const double kAtFrequency = -1., const double frequency = fAtRefE) : 
	k( kAtRefE( kAtFrequency, frequency ) )
	{};

	voxData( const vector<char>& binData, vector<char>::const_iterator& it ){

		deSerializeBuildIn( k, (double) -1, binData, it );

	}

	double k_At( const double f ) const{

		const double constant =  k * fAtRefE_3 ;

		return constant * pow( f, 3 );
	};

	double kAtRefE( void ) const{ return k; };


	size_t serialize( vector<char>& binData ) const{
		serializeBuildIn( k, binData );
	};


	private:
	double k = -1;			/*!<Absorption coefficient at 120keV*/



	private:
	double kAtRefE( const double kAtF, const double f ) const{

		return ( kAtF * pow( f, 3 ) )/ fAtRefE_3;

	};



};


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
	inline pnt3 O( void ) const{ return o; };

	/*!
	 * @brief Get size of voxel
	 * @return Size of voxel
	*/
	inline v3 Size( void ) const{ return size; };

	/*!
	 * @brief Get data of voxel
	 * @return Voxel data
	*/
	inline voxData Data( void ) const{ return data; };

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
	 * @brief Find entrance of ray into voxel
	 * @param r Ray
	 * @return Intersection of the entrance face with ray
	*/
	//rayVox_Intersection_Result findEntrance( const ray r ) const;

	/*!
	 * @brief Find exit of ray into voxel
	 * @param r Ray
	 * @return Intersection of the exit face with ray
	*/
	//rayVox_Intersection_Result findExit( const ray r ) const;

	bool contains( const pnt3 p ) const;

	private:
	v3 size;										/*!<Size in local coordinate system*/
	voxData data;									/*!<Physical voxel data*/
	pnt3 o;											/*!<Point as origin of voxel in coordinate system*/
	surfLim faces[ toUnderlying( FACE_ID::END ) ];	/*!<Faces in global context*/
};