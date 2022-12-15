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
 //#include "operations.h"



  /*********************************************************************
	Definitions
 *********************************************************************/

/*!
 * @brief Class for ray properties
*/
class rayProperties{
	public:
	double intensity = 0;		/*!< Intensity of ray */
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
	 * @brief Calculate intersection between this ray and surface
	 * @param sur Surface to intersect with
	 * @return Instersection result
	*/
	//linSurf_Intersection_Result isectSurf( const surfLim sur ) const;

	/*!
	 * @brief Project ray on XY plane of coordinate system
	 * @param cSys System to project on
	 * @return Projected ray
	*/
	ray projectOnXYPlane( const cartCSys* const cSys ) const;

	inline bool paraInBounds( const double para ) const{ return para >= 0; };


	private:
	rayProperties properties;			/*!< Property of ray*/
};