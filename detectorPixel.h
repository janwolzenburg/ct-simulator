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
 #include "surf.h"
 #include "intersections.h"



 /*********************************************************************
   Definitions
 *********************************************************************/

/*!
 * @brief Class for detector pixel
*/
class pixel : public surfLim{

	using surfLim::surfLim;

	public:

	/*!
	 * @brief Reset detected rays
	*/
	void reset( void );


	public:
	vector<rayProperties> detectedRayProperties;		/*!<Rays detected with this pixel*/

 };


/*!
 * @brief Class for the intersection result of a pixel and a ray
*/
class rayPix_Intersection_Result : public linSurf_Intersection_Result{

	using linSurf_Intersection_Result::linSurf_Intersection_Result;

	public:
	rayPix_Intersection_Result( linSurf_Intersection_Result linSurfResult ) : linSurf_Intersection_Result( linSurfResult ){};

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
	 * @param r Ray
	 * @param px Pixel
	*/
	rayPix_Intersection( const ray r, const pixel px );

	/*!
	 * @brief Get the result
	 * @return Result
	*/
	inline rayPix_Intersection_Result Result( void ) const{ return result; };

	public:
	rayPix_Intersection_Result result;			/*!<Result of intersection*/


 };

