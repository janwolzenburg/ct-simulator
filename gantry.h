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

constexpr size_t maxRadiationLoops = 16;

// TODO: Clean up construction
/*!
 * @brief Class for a gantry with xRay source and detector
*/
class gantry {

	public:
	//TODO: Put beam angle and amount of rays into struct
	
	/*!
	 * @brief Constructor
	 * @param cSys_ Coordinate system
	 * @param radius_ Radius of gantry
	 * @param beamAngle_ Beam angle
	 * @param numRaysInBeam_ Amount of rays in beam
	 * @param tubeParameters_ Parameter of xRay tube
	 * @param detectorParameters_ Parameter of xRay detector
	*/
	gantry( cartCSys* const cSys_, const size_t numRaysInBeam_, const tubeParameter tubeParameters_, const detectorRadonParameter detectorParameters_, 
			size_t numRows_, const double angle_, const double columnSize, const bool structered );

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

	tube raySource;						/*!<xRay source*/
	detector rayDetector;				/*!<Ray detector*/

	size_t numRaysInBeam;				/*!<Amount of rays in beam*/
	double beamAngle;					/*!<Beam angle*/
	double radius;						/*!<Radius of gantry*/


};
