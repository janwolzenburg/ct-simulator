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
	gantry( cartCSys* const cSys_, const double radius_, const double beamAngle_, const size_t numRaysInBeam_, const tubeParameter tubeParameters_, const detectorParameter detectorParameters_ );

	/*!
	 * @brief Get all rays from tube
	 * @return Vector with all rays in beam
	*/
	vector<ray> getBeam( void ) const;

	/*!
	 * @brief Get all pixel from detector 
	 * @return Vector of vectors with pixel
	*/
	vector<vector<pixel>> getPixel( void ) const;

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


	private:
	cartCSys* cSys;			/*!<Coordinate system*/
	double radius;			/*!<Radius of gantry*/

	tube raySource;			/*!<xRay source*/
	size_t numRaysInBeam;	/*!<Amount of rays in beam*/
	double beamAngle;		/*!<Beam angle*/

	detector rayDetector;	/*!<Ray detector*/

};
