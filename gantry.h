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
#include "model.h"


/*********************************************************************
   Definitions
*********************************************************************/

void threadFunction( const ray currentRay, const model& radModel, const bool enableScattering, vector<ray>& raysToDetect, vector<ray>& raysForNextIteration );


constexpr size_t maxRadiationLoops = 16;

/*!
 * @brief Class for a gantry with xRay source and detector
*/
class gantry {

	public:
	
	/*!
	 * @brief Constructor
	 * @param cSys_ Coordinate system
	 * @param raysPerPixel_ Amount of rays per pixel in beam
	 * @param tubeParameters Parameter of xRay tube
	 * @param radonParameters Radon parameter of xRay detector
	 * @param indipendentParameter Other parameter
	*/
	gantry( cartCSys* const cSys_, const size_t raysPerPixel_, const tubeParameter tubeParameter_, const detectorRadonParameter radonParameter,
			const detectorIndipendentParameter indipendentParameter );

	/*!
	 * @brief Get all rays from tube
	 * @return Vector with all rays in beam
	*/
	vector<ray> getBeam( void ) const;

	/*!
	 * @brief Get all pixel from detector 
	 * @return Vector with pixel
	*/
	vector<pixel> getPixel( void ) const;

	/*!
	 * @brief Get radius of gantry
	 * @return Radius
	*/
	double Radius( void ) const;

	/*!
	 * @brief Get center of gantry
	 * @return Center point
	*/
	pnt3 Center( void ) const;

	/*!
	 * @brief Rotate gantry counter clockwise around zAxis
	 * @param angle Rotation angle
	*/
	void rotateCounterClockwise( const double angle );

	/*!
	 * @brief Radiate model with beam
	 * @param radModel Model to radiate
	*/
	void radiate( const model& radModel ) ;
	
	/*!
	 * @brief Reset ganrtry to its initial position
	*/
	void reset( void );

	/*!
	 * @brief Get the coordinate system of gantry
	 * @return Coordinate system of this gantry
	*/
	const cartCSys* CSys( void ) const;

	/*!
	 * @brief Get the detector radon parameters
	 * @param cSys Reference coordinate system for radon transform
	 * @return 
	*/
	detectorRadonParameter getDetectorParameter(void) const;


	private:
	
	cartCSys* cSys;						/*!<Coordinate system*/
	primitiveCartCSys resetPostition;	/*!<Initial position of coordinate system*/

	detector rayDetector;				/*!<Ray detector*/
	tube raySource;						/*!<xRay source*/
	
	size_t raysPerPixel;				/*!<Amount of rays per pixel*/
	double radius;						/*!<Radius of gantry*/


};
