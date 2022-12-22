/*********************************************************************
 * @file   fanBeam.cpp
 * @brief  Implementations of fan beam class
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/



 /*********************************************************************
   Includes
*********************************************************************/

#include "fanBeam.h"



/*********************************************************************
   Implementations
*********************************************************************/



fanBeam::fanBeam( cartCSys* const cSys_, const size_t numRays_, const spectrum spectrum_, const double angle_) :
	cSys( cSys_ )
{

	// Split spectrum into the ray spectra
	spectrum raySpectrum = spectrum_.getScaled( 1. / (double) numRays_ );


	// Properties of created rays
	rayProperties beamProperties{ raySpectrum };


	// Create rays on circular arc 
	const double dAngle = angle_ / ((double)( numRays_ - 1 ) );				// Angle between two rays

	// Iterate all rays to create
	for ( size_t i = 0; i < numRays_; i++ ) {
		const double yAngle = -( angle_ / 2 ) + (double) i * dAngle;		// Angle between current ray and y-axis

		vec3 direction{ cSys->EyVec() };									// Start with y-axis as direction
		direction.rotZM(yAngle);											// Rotate vector around z-axis

		const ray r{ direction, cSys->OPnt(), beamProperties };				// Create ray

		rays.push_back(r);													// Store ray in ray collection
	}



}