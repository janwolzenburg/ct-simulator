#pragma once
/*********************************************************************
 * @file   detector.h
 * @brief  Detector class
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
#include "detectorPixel.h"



/*********************************************************************
   Definitions
*********************************************************************/

// TODO: Split implementiation and comment
// TODO: Maybe encapsulate some physical parameters further

/*!
 * @brief Class for detector parameters 
*/
class detectorRadonParameter{

	public:
	/*!
	 * @brief Constructor
	 * @param numberPoints_ Amount of point in radon transformed
	 * @param maxAbsoluteDistance_ Maximum absoulte distance in radon transformed
	*/
	detectorRadonParameter( const idx2CR numberPoints_, const double maxAbsoluteDistance_ ) :
		numberPoints(  numberPoints_ ),
		distanceRange( 2. * Fpos( maxAbsoluteDistance_ ) ),
		resolution{ v2CR{	PI / ( 2. * (double) ( numberPoints.c - 1 ) ), 
							distanceRange / (double) ( numberPoints.r - 1 ) }},
		framesToFillSinogram( 3 * numberPoints.c - 3 )
	{};

	/*!
	 * @brief Get the necessary detector radius based on the angle
	 * @param angle Angle between the outer detector normals
	 * @return Radius of detector
	*/
	double getRadius( const double angle ) const {
		return distanceRange / 2. / ( 2. * sin( angle / 2 ) );
	};

	/*!
	 * @brief Get the necessary pixel size along a row
	 * @param radius The detector radius
	 * @return Row size of pixel
	*/
	double getRowSize( const double radius ) const{
		return 4. * radius * tan( resolution.c / 2. );
	};


	public:

	idx2CR numberPoints;			/*!<Number of points on the axis*/
	double distanceRange;			/*!<Range of distances. Difference Dmax - Dmin*/
	v2CR resolution;				/*!Resolution of the axis*/
	size_t framesToFillSinogram;	/*!<Amount of frames to fill sinogram*/
};


/*!
 * @brief Class for physical detector parameters which are not dependent on the radon parameters
*/
class detectorIndipendentParameter{

	public:

	/*!
	 * @brief Constructor
	 * @param angle_ Detector angle
	 * @param columnSize_ Size of pixel in column direction
	 * @param structured_ Flag for anti scatter structure
	*/
	detectorIndipendentParameter( const double angle_, const double columnSize_, const bool structured_ ) : 
		angle( angle_ ),
		columnSize( columnSize_ ),
		structured( structured_ )
	{};

	double angle;			/*!<Angle between the outer detector normals*/
	double columnSize;		/*!<Size of one pixel in column direction*/
	bool structured;		/*!<Flag for anti scatter structure*/

};


/*!
 * @brief Struct for detector parameters
*/
class detectorPhysicalParameter{

	public:

	/*!
	 * @brief Constructor
	 * @param radonParameter Radon parameters
	 * @param indipendentParameter Other detector parameters
	*/
	detectorPhysicalParameter( const detectorRadonParameter radonParameter, const detectorIndipendentParameter indipendentParameter ) :
		number{ radonParameter.numberPoints.c, 1 },
		angle( indipendentParameter.angle ),
		radius( radonParameter.getRadius( angle )),
		pixelSize{ indipendentParameter.columnSize, radonParameter.getRowSize( radius ) },
		structured( indipendentParameter.structured )
	{};


	public:

	idx2CR number;		/*!<Amount of pixel in each dimension*/

	double angle;		
	double radius;		/*!Radius of detector arc*/
	v2CR pixelSize;		/*!<Size of one pixel*/

	bool structured;	/*!<Flag for anti scatter structure*/

};


/*!
 * @brief Class for ray detector
*/
class detector {

	public:

	/*!
	 * @brief Constructor
	 * @param cSys_ Coordinate system
	 * @param parameter Detector parameter
	*/
	detector(cartCSys* const cSys_, const detectorRadonParameter radonParameter, const detectorIndipendentParameter indipendentParameter );

	/*!
	 * @brief Get all detector pixel
	 * @return Vector of pixels in one row
	*/
	vector<pixel> getPixel( void ) const;

	/*!
	 * @brief Reset all pixel
	*/
	void reset( void );

	/*!
	 * @brief Detect ray
	 * @param r Ray to detect
	*/
	void detectRay( const ray r );

	/*!
	 * @brief Get the radon parameters of detector
	 * @return Radon parameters of detector
	*/
	detectorRadonParameter getSignalParameter( void ) const;

	/*!
	 * @brief Get the physical parameters of detector
	 * @return Physical parameters of detector
	*/
	detectorPhysicalParameter getPhysicalParameters( void ) const;


	private:

	cartCSys* cSys;									/*!<Local coordinate system*/
	vector<pixel> allPixel;							/*!<Pixels of detector*/

	detectorRadonParameter radonParameters;			/*!<Radon parameters*/
	detectorPhysicalParameter physicalParameters;	/*!<Physical parameters*/


};

