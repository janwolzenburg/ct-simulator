#pragma once
/*********************************************************************
 * @file   detectorParameter.h
 * @brief  Detector parameter classes
 *
 * @author Jan Wolzenburg
 * @date   January 2023
 *********************************************************************/



 /*********************************************************************
	Includes
 *********************************************************************/

 #include "generel.h"
 #include "generelMath.h"



/*********************************************************************
   Definitions
*********************************************************************/

constexpr double MAX_DETECTOR_ANGLE = 2. * PI * ( 60./360. );
constexpr double MIN_DETECTOR_ANGLE = 2. * PI * ( 40. / 360. );


/*!
 * @brief Class for detector parameters
*/
class detectorRadonParameter{

	public:
	/*!
	 * @brief Constructor
	 * @param numberPoints_ Amount of point in radon transformed
	 * @param distanceRange Distance range
	*/
	detectorRadonParameter( const idx2CR numberPoints_, const double distanceRange_ );

	/*!
	 * @brief Get the necessary detector radius based on the angle
	 * @param angle Angle between the outer detector normals
	 * @return Radius of detector
	*/
	//double getDetectorFocusDistance( const double angle ) const;

	/*!
	 * @brief Get the necessary pixel size along a row
	 * @param radius The detector radius
	 * @return Row size of pixel
	*/
	//double getRowSize( const double radius ) const;


	public:

	idx2CR numberPoints;			/*!<Number of points on the axis*/
	//double distanceRange;			/*!<Range of distances. Difference Dmax - Dmin*/
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
	detectorIndipendentParameter( const double arcRadius_, const double columnSize_, const bool structured_ );


	public:

	double arcRadius;		/*!<Radius of arc where the pixels lie on*/
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
	detectorPhysicalParameter( const detectorRadonParameter radonParameter, const detectorIndipendentParameter indipendentParameter );


	public:

	idx2CR number;					/*!<Amount of pixel in each dimension*/
	double angle;					/*!<Angle between outer normals*/
	double detectorFocusDistance;	/*!Distance of focus and detector pixel*/
	bool structured;				/*!<Flag for anti scatter structure*/

};