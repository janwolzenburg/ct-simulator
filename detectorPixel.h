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
 #include "ray.h"
 #include "intersections.h"



 /*********************************************************************
   Definitions
 *********************************************************************/

/*!
 * @brief Class for detector pixel
*/
class DetectorPixel : public BoundedSurface{

	public:

	/*!
	 * @brief Constructor
	 * @param surface Surface as base object
	*/
	DetectorPixel( const BoundedSurface surface ) :
		BoundedSurface( surface ), detected_ray_properties_( 0, RayProperties{} )
	{};

	/*!
	 * @brief Constructor
	 * @param surface Surface as base object
	 * @param properties Detected Ray's properties
	*/
	DetectorPixel( const BoundedSurface surface, const vector<RayProperties> properties ) :
		BoundedSurface{ surface }, detected_ray_properties_( properties )
	{};

	/*!
	 * @brief Get the detected Ray properties
	 * @return Vector with properties
	*/
	vector<RayProperties> detected_ray_properties( void ) const{ return detected_ray_properties_; };

	/*!
	 * @brief Reset detected rays
	*/
	void ResetDetectedRayProperties( void ){ detected_ray_properties_.clear(); };

	/*!
	 * @brief Get the normal of the pixel
	 * @return Line with pixel's center as origin and direction of surface normal
	*/
	Line NormalLine( void ) const{ return Line{ Surface::GetNormal(), this->origin_ }; };

	/*!
	 * @brief Get the value of radon point for the detected Ray properties
	 * @param use_simple_attenuation If set use ideal model attenuation which is not energy dependent
	 * @return Value of radon point
	*/
	double GetDetectedLineIntegral( const bool use_simple_attenuation ) const;

	/*!
	 * @brief Convert this pixel ot given coordinate system
	 * @param target_coordinate_system Target system 
	 * @return This pixel in given coordiante system
	*/
	DetectorPixel ConvertTo( const CoordinateSystem* const target_coordinate_system ) const;

	/*!
	 * @brief Add Ray properties
	 * @param properties_ properties to add
	*/
	void  AddDetectedRayProperties( const RayProperties properties ){
		detected_ray_properties_.push_back( properties ); };


	private:

	vector<RayProperties> detected_ray_properties_;		/*!< Ray properties of rays detected with this pixel*/

 };


/*!
 * @brief Class for the intersection of pixel and ray
*/
class RayPixelIntersection : public LineSurfaceIntersection<Ray, DetectorPixel>{

	public:

	/*!
	 * @brief Constructor
	 * @param ray Ray
	 * @param pixel Pixel
	*/
	RayPixelIntersection( const Ray ray, const DetectorPixel pixel );


	RayProperties ray_properties;	/*!< Properties of detected ray*/

 };

