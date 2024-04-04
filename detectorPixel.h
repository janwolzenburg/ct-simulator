#pragma once
/*********************************************************************
 * @file   detectorPixel.h
 * @brief  class for a detector pixel
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/


  /*********************************************************************
	Includes
 *********************************************************************/
 
 #include <optional>
 using std::optional;
 #include "surface.h"
 #include "ray.h"
 #include "intersections.h"



 /*********************************************************************
   Definitions
 *********************************************************************/

/*!
 * @brief class for detector pixel
*/
class DetectorPixel : public BoundedSurface{

	public:

	/*!
	 * @brief constructor
	 * @param surface Surface as base object
	*/
	DetectorPixel( const BoundedSurface surface ) :
		BoundedSurface( surface ), detected_ray_properties_( 0, RayProperties{} )
	{};

	/*!
	 * @brief constructor
	 * @param surface Surface as base object
	 * @param properties Detected Ray's properties
	*/
	DetectorPixel( const BoundedSurface surface, const vector<RayProperties> properties ) :
		BoundedSurface{ surface }, detected_ray_properties_( properties )
	{};

	/*!
	 * @brief get the detected Ray properties
	 * @return vector with properties
	*/
	vector<RayProperties> detected_ray_properties( void ) const{ return detected_ray_properties_; };

	/*!
	 * @brief reset detected rays
	*/
	void ResetDetectedRayProperties( void ){ detected_ray_properties_.clear(); };

	/*!
	 * @brief get the normal of the pixel
	 * @return line with pixel's center as origin and direction of surface normal
	*/
	Line NormalLine( void ) const{ return Line{ Surface::GetNormal(), this->origin_ }; };

	/*!
	 * @brief get the value of radon point for the detected Ray properties
	 * @param use_simple_absorption If set use ideal model absorption which is not energy dependent
	 * @param expected_ray_hits The expected amount of rays to hit the pixel
	 * @param start_intensity Start intensities of rays
	 * @return value of radon point
	*/
	optional<double> GetProjectionValue( const bool use_simple_absorption, const size_t expected_ray_hits, const double start_intensity ) const;

	/*!
	 * @brief convert this pixel ot given coordinate system
	 * @param target_coordinate_system Target system 
	 * @return this pixel in given coordiante system
	*/
	DetectorPixel ConvertTo( const CoordinateSystem* const target_coordinate_system ) const;

	/*!
	 * @brief add Ray properties
	 * @param properties_ properties to add
	*/
	void  AddDetectedRayProperties( const RayProperties properties ){
		detected_ray_properties_.push_back( properties ); };


	private:

	vector<RayProperties> detected_ray_properties_;		/*!< ray properties of rays detected with this pixel*/

 };


/*!
 * @brief class for the intersection of pixel and ray
*/
class RayPixelIntersection : public LineSurfaceIntersection<Ray, DetectorPixel>{

	public:

	/*!
	 * @brief constructor
	 * @param ray Ray
	 * @param pixel Pixel
	*/
	RayPixelIntersection( const Ray& ray, const DetectorPixel& pixel );


	RayProperties ray_properties;	/*!< properties of detected ray*/

 };

