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
 #include "simulation.h"



/*********************************************************************
   Definitions
*********************************************************************/



/*!
 * @brief Class for detector parameters
*/
class detectorRadonParameter{

	public:

	static const string FILE_PREAMBLE;


	public:
	/*!
	 * @brief Constructor
	 * @param numberPoints_ Amount of point in radon transformed
	 * @param distanceRange Distance range
	*/
	detectorRadonParameter( const idx2CR numberPoints_, const double distanceRange_ );

	detectorRadonParameter( void );

	detectorRadonParameter( const vector<char>& binData, vector<char>::const_iterator& it );


	/*!
		* @brief Serialize this object
		* @param binData Reference to vector where data will be appended
	*/
	size_t serialize( vector<char>& binData ) const;


	public:

	double distanceRange;			/*!<Measure field in mm*/
	idx2CR numberPoints;			/*!<Number of points on the axis*/
	v2CR resolution;				/*!Resolution of the axis*/
	size_t framesToFillSinogram;	/*!<Amount of frames to fill sinogram*/
};



/*!
 * @brief Class for physical detector or simulation parameters which are not dependent on the radon parameters
*/
class detectorIndipendentParameter{

	public:

	static const string FILE_PREAMBLE;


	public:

	/*!
	 * @brief Constructor
	 * @param raysPerPixel_ Amount of rays per pixel to simulate
	 * @param arcRadius_ Radius of the detector arc
	 * @param columnSize_ Pixel size in z-Direction
	 * @param structured_ Flag for anti scattering structure
	 * @param maxRayAngleDetectable_ maximum angle between pixel normal and incident ray allowed by anti-scattering structure
	*/
	detectorIndipendentParameter( const size_t raysPerPixel_, const double arcRadius_, const double columnSize_, const bool structured_ = false, const double maxRayAngleDetectable_ =  0.15 );

	detectorIndipendentParameter( void );

	detectorIndipendentParameter( const vector<char>& binData, vector<char>::const_iterator& it );


	/*!
		* @brief Serialize this object
		* @param binData Reference to vector where data will be appended
	*/
	size_t serialize( vector<char>& binData ) const;


	public:

	size_t raysPerPixel;			/*!<Amount of rays per pixel*/
	double arcRadius;				/*!<Radius of arc where the pixels lie on*/
	double columnSize;				/*!<Size of one pixel in column direction*/
	bool structured;				/*!<Flag for anti scatter structure*/
	double maxRayAngleDetectable;	/*!<Maximum angle between pixel normal and ray*/
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
	double maxRayAngleDetectable;	/*!<Maximum angle between pixel normal and ray*/
};