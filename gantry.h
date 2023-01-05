#pragma once
/*********************************************************************
 * @file   gantry.h
 * @brief  Gantry class
 *
 * @author Jan Wolzenburg
 * @date   January 2023
 *********************************************************************/



 /*********************************************************************
	Includes
 *********************************************************************/
#include <vector>
using std::vector;

#include "generelMath.h"
#include "tube.h"
#include "detector.h"


/*********************************************************************
   Definitions
*********************************************************************/



//TODO: Add comments and descriptions
class gantry {

	public:
	//TODO: Put beam angle and amount of rays into struct
	gantry( cartCSys* const cSys_, const double radius_, const double beamAngle_, const size_t numRaysInBeam_, const tubeParameter tubeParameters_, const detectorParameter detectorParameters_ );


	vector<ray> getBeam( void ) const;
	vector<vector<pixel>> getPixel( void ) const;
	double Radius( void ) const;
	pnt3 Center( void ) const;
	void rotateCounterClockwise( const double angle );

	private:
	cartCSys* cSys;
	double radius;

	tube raySource;
	size_t numRaysInBeam;
	double beamAngle;

	detector rayDetector;

};