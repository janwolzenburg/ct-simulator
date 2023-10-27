#pragma once
/*********************************************************************
 * @file   detectorPixel.h
 * @brief  Class for a detector pixel
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/


  /*********************************************************************
	Includes
 *********************************************************************/
 #include "surface.h"
 #include "rays.h"
 #include "intersections.h"



 /*********************************************************************
   Definitions
 *********************************************************************/

/*!
 * @brief Class for detector pixel
*/
class pixel : public BoundedSurface{

	using BoundedSurface::BoundedSurface;

	public:

	/*!
	 * @brief Constructor
	 * @param surface Surface as base object
	*/
	pixel( const BoundedSurface surface ) :
		BoundedSurface{ surface },
		detectedRayProperties( 0, RayProperties{} )
	{};

	/*!
	 * @brief Constructor
	 * @param surface Surface as base object
	 * @param properties_ Detected Ray's properties_
	*/
	pixel( const BoundedSurface surface, const vector<RayProperties> properties_ ) :
		BoundedSurface{ surface },
		detectedRayProperties( properties_ )
	{};

	/*!
	 * @brief Reset detected rays
	*/
	void reset( void ){ detectedRayProperties.clear(); };

	/*!
	 * @brief Get the value of radon point from detected Ray properties_
	 * @return Value of radon point
	*/
	double getRadonValue( void ) const;

	/*!
	 * @brief Get the normal of the pixel
	 * @return Line with pixel's center as origin_ an direction of surface normal
	*/
	Line NormalLine( void ) const;

	/*!
	 * @brief Convert this pixel ot given coordinate system
	 * @param target_CSys Target system 
	 * @return This pixel in given coordiante system
	*/
	pixel ConvertTo( const CoordinateSystem* const target_CSys ) const;

	/*!
	 * @brief Add Ray properties_
	 * @param properties_ properties to add
	*/
	void  addDetectedProperties( const RayProperties properties_ ){
		detectedRayProperties.push_back( properties_ ); };

	/*!
	 * @brief Get the detected Ray properties_
	 * @return Vector with properties_
	*/
	vector<RayProperties> getProperties( void ) const{ return detectedRayProperties; };

	private:
	vector<RayProperties> detectedRayProperties;		/*!<Rays detected with this pixel*/

 };


/*!
 * @brief Class for the intersection of pixel and Ray
*/
class RayPixelIntersection : public LineSurfaceIntersection<Ray, pixel>{

	public:

	/*!
	 * @brief Constructor
	 * @param direction_ Ray
	 * @param px Pixel
	*/
	RayPixelIntersection( const Ray r, const pixel px );


	RayProperties rayProps; 

 };

