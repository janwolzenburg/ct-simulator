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


	// Create rays on circular arc 


}