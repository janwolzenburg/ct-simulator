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
#include <mutex>
#include "scattering.h"



/*********************************************************************
   Definitions
*********************************************************************/


constexpr size_t maxRadiationLoops = 16;		/*!<How often can a ray be scattered*/

/*!
 * @brief Thread function to speed up transmission of multiple rays through model
 * @param radModel Reference to model
 * @param enableScattering Flag that enables scattering
 * @param rayScatterAngles Reference to object with information about ray scattering
 * @param rays Reference to vector with rays to transmit
 * @param sharedCurrentRayIndex Index of the next ray in vector to transmit. Will be changed at each call
 * @param currentRayIndexMutex Mutex instance for ray index
 * @param raysForNextIteration Reference to vector which hold the rays for the next iteration
 * @param detectorMutex Mutex for the detector Object
 * @param rayDetector Reference to ray detector
 * @param iterationMutex Mutex for vector with rays for next iteration
*/
void threadFunction( const model& radModel, const bool enableScattering, const rayScattering& rayScatterAngles,
					 const vector<ray>& rays, size_t& sharedCurrentRayIndex, std::mutex& currentRayIndexMutex,
					 vector<ray>& raysForNextIteration, std::mutex& detectorMutex,
					 detector& rayDetector, std::mutex& iterationMutex );


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

	/*!
	 * @brief Get reference to scattering member object
	 * @return Reference to member 
	*/
	rayScattering& rayScattering( void ){ return rayScatterAngles; };


	private:
	
	cartCSys* cSys;						/*!<Coordinate system*/
	primitiveCartCSys resetPostition;	/*!<Initial position of coordinate system*/

	detector rayDetector;				/*!<Ray detector*/
	tube raySource;						/*!<xRay source*/
	
	size_t raysPerPixel;				/*!<Amount of rays per pixel*/
	double radius;						/*!<Radius of gantry*/

	rayScattering rayScatterAngles;	/*!<Object with information about scattering and ablge propabilities*/
};
