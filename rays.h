#pragma once
/*********************************************************************
 * @file   rays.h
 * @brief  Classes for rays
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/
 


 /*********************************************************************
	Includes
 *********************************************************************/
 #include "line.h"
 #include "voxel.h"
 #include "spectrum.h"
 //#include "operations.h"



  /*********************************************************************
	Definitions
 *********************************************************************/

/*!
 * @brief Class for ray properties
*/
class rayProperties{
	
	friend class ray;
	friend class pixel;

	public:
	rayProperties( const spectrum spectrum_ );

	rayProperties( void );

	spectrum PowerSpectrum( void ) const;


	private:
	spectrum powerSpectrum;
};


/*!
 * @brief Class for rays
*/
class ray : public line{
	public:

	/*!
	 * @brief Constructor
	 * @param v_ Trajectory
	 * @param p_ O
	 * @param intensity_ Intensity
	*/
	explicit ray( const vec3 v_, const pnt3 p_, const rayProperties properties_ );

	/*!
	 * @brief Constructor
	 * @param line_ Line
	 * @param intensity_ Intensity
	*/
	explicit ray( const line line_, const rayProperties  properties_ );

	/*!
	 * @brief Default constructor
	*/
	ray( void );

	/*!
	 * @brief Get intensity
	 * @return Intensity
	*/
	inline rayProperties Properties( void ) const{ return properties; };

	/*!
	 * @brief Convert ray components to different coordinate system
	 * @param target Target system
	 * @return Ray in target system
	*/
	ray convertTo( const cartCSys* const target ) const;

	/*!
	 * @brief Get ray parameter corresponding to point
	 * @param p Point on ray
	 * @param success Is set to true when the given point lies on the ray. False if not
	 * @return Ray parameter
	*/
	double getPara( const pnt3 p, bool* const success ) const;

	/*!
	 * @brief Update ray properties passing through voxel for specific distance
	 * @param data Voxel properties
	 * @param distance Distance the ray is inside voxel
	*/
	void updateProperties( const voxData data, const double distance );

	/*!
	 * @brief Project ray on XY plane of coordinate system
	 * @param cSys System to project on
	 * @return Projected ray
	*/
	ray projectOnXYPlane( const cartCSys* const cSys ) const;

	/*!
	 * @brief Checks if parameter is greater than one
	 * @param para Parameter
	 * @return True when parameter is valid
	*/
	bool paraInBounds( const double para ) const;

	/*!
	 * @brief Get the faces, which are aligned with the coordinate system of the ray, through which the ray could exit
	 * @return Vector with possible face IDs
	*/
	vector<FACE_ID> getPossibleVoxelExits( void ) const;

	void scaleSpectrum( const double factor );

	double getMeanFrequency( void ) const;

	private:
	rayProperties properties;			/*!< Property of ray*/
};