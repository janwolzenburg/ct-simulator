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
		detectedRayProperties( 0, rayProperties{} )
	{};

	/*!
	 * @brief Constructor
	 * @param surface Surface as base object
	 * @param properties Detected ray's properties
	*/
	pixel( const BoundedSurface surface, const vector<rayProperties> properties ) :
		BoundedSurface{ surface },
		detectedRayProperties( properties )
	{};

	/*!
	 * @brief Reset detected rays
	*/
	void reset( void ){ detectedRayProperties.clear(); };

	/*!
	 * @brief Get the value of radon point from detected ray properties
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
	pixel convertTo( const CoordinateSystem* const target_CSys ) const;

	/*!
	 * @brief Add ray properties
	 * @param properties Properties to add
	*/
	void  addDetectedProperties( const rayProperties properties ){
		detectedRayProperties.push_back( properties ); };

	/*!
	 * @brief Get the detected ray properties
	 * @return Vector with properties
	*/
	vector<rayProperties> getProperties( void ) const{ return detectedRayProperties; };

	private:
	vector<rayProperties> detectedRayProperties;		/*!<Rays detected with this pixel*/

 };


/*!
 * @brief Class for the intersection result of a pixel and a ray
*/
class rayPix_Intersection_Result : public linSurf_Intersection_Result{

	using linSurf_Intersection_Result::linSurf_Intersection_Result;

	public:
	/*!
	 * @brief Constructor
	 * @param linSurfResult Line surface intersection result
	*/
	rayPix_Intersection_Result( const linSurf_Intersection_Result linSurfResult ) :
		linSurf_Intersection_Result( linSurfResult )
	{};

	public:
	rayProperties rayProps;  /*!< Properties of rays that hit the pixel*/
	
};


/*!
 * @brief Class for the intersection of pixel and ray
*/
class rayPix_Intersection : private linSurfIntersection<ray, pixel>{

	public:

	/*!
	 * @brief Constructor
	 * @param direction_ Ray
	 * @param px Pixel
	*/
	rayPix_Intersection( const ray r, const pixel px );

	/*!
	 * @brief Get the result
	 * @return Result
	*/
	rayPix_Intersection_Result Result( void ) const{ return result; };


	public:
	rayPix_Intersection_Result result;			/*!<Result of intersection*/


 };

