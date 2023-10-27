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
	detectorRadonParameter( const GridIndex numberPoints_, const double distanceRange_ );

	/*!
	 * @brief Default constructor
	*/
	detectorRadonParameter( void ): detectorRadonParameter{ GridIndex{ 32, 16 }, 500. } {};

	/*!
	 * @brief Constructor from serialized data
	 * @param binary_data Reference to vector with binary data
	 * @param it Iterator to start of data in vector
	*/
	detectorRadonParameter( const vector<char>& binary_data, vector<char>::const_iterator& it );

	/*!
		* @brief Serialize this object
		* @param binary_data Reference to vector where data will be appended
	*/
	size_t Serialize( vector<char>& binary_data ) const;


	public:

	double distanceRange;			/*!<Measure field in mm*/
	GridIndex numberPoints;			/*!<Number of points on the axis*/
	GridCoordinates resolution;				/*!GetResolution of the axis*/
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
	 * @param maxRayAngleDetectable_ maximum GetAngle between pixel normal and incident ray allowed by anti-scattering structure
	*/
	detectorIndipendentParameter( const size_t raysPerPixel_, const double arcRadius_, const double columnSize_, const bool structured_ = false, const double maxRayAngleDetectable_ =  5. / 360. * 2. * PI );
	
	/*!
	 * @brief Default constructor
	*/
	detectorIndipendentParameter( void ) :
		detectorIndipendentParameter{ 1, 1000., 5., true, 5. / 360. * 2. * PI }{};

	/*!
	 * @brief Constructor from serialized data
	 * @param binary_data Reference to vector with binary data
	 * @param it Iterator to start of data in vector
	*/
	detectorIndipendentParameter( const vector<char>& binary_data, vector<char>::const_iterator& it );

	/*!
		* @brief Serialize this object
		* @param binary_data Reference to vector where data will be appended
	*/
	size_t Serialize( vector<char>& binary_data ) const;


	public:

	size_t raysPerPixel;			/*!<Amount of rays per pixel*/
	double arcRadius;				/*!<Radius of arc where the pixels lie on*/
	double columnSize;				/*!<Size of one pixel in column direction*/
	bool structured;				/*!<Flag for anti scatter structure*/
	double maxRayAngleDetectable;	/*!<Maximum GetAngle between pixel normal and ray in radians*/
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

	GridIndex number;					/*!<Amount of pixel in each dimension*/
	double angle;					/*!<Angle between outer normals*/
	double detectorFocusDistance;	/*!Distance of focus and detector pixel*/
	bool structured;				/*!<Flag for anti scatter structure*/
	double maxRayAngleDetectable;	/*!<Maximum GetAngle between pixel normal and ray*/
};