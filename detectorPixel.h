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

 };


class rayPix_Intersection_Result : public linSurf_Intersection_Result{

	using linSurf_Intersection_Result::linSurf_Intersection_Result;

	public:
	rayProperties rayProps;			
	
};


class rayPix_Intersection : private linSurfIntersection<ray, pixel>{

	rayPix_Intersection( const ray r, const pixel px );

	inline rayPix_Intersection_Result Result( void ) const{ return result; };

	public:
	rayPix_Intersection_Result result;


 };

