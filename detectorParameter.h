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
 #include "radonProperties.h"



/*********************************************************************
   Definitions
*********************************************************************/


/*!
 * @brief Class for physical detector or simulation parameters which are not dependent on the radon parameters
*/
class detectorIndipendentParameter{

	public:

	static const string FILE_PREAMBLE;

	/*!
	 * @brief Constructor
	 * @param raysPerPixel_ Amount of rays per pixel to simulate
	 * @param arcRadius_ Radius of the detector arc
	 * @param columnSize_ Pixel size_ in z-Direction
	 * @param structured_ Flag for anti scattering structure
	 * @param maxRayAngleDetectable_ maximum GetAngle between pixel normal and incident Ray allowed by anti-scattering structure
	*/
	detectorIndipendentParameter( const size_t raysPerPixel_, const double arcRadius_, const double columnSize_, const bool structured_ = false, const double maxRayAngleDetectable_ =  5. / 360. * 2. * PI );
	
	/*!
	 * @brief Default constructor
	*/
	detectorIndipendentParameter( void ) :
		detectorIndipendentParameter{ 1, 1000., 5., true, 5. / 360. * 2. * PI }{};

	/*!
	 * @brief Constructor from serialized data_
	 * @param binary_data Reference to vector with binary data_
	 * @param it Iterator to start of data_ in vector
	*/
	detectorIndipendentParameter( const vector<char>& binary_data, vector<char>::const_iterator& it );

	/*!
		* @brief Serialize this object
		* @param binary_data Reference to vector where data_ will be appended
	*/
	size_t Serialize( vector<char>& binary_data ) const;


	public:

	size_t raysPerPixel;			/*!<Amount of rays per pixel*/
	double arcRadius;				/*!<Radius of arc where the pixels lie on*/
	double columnSize;				/*!<Size of one pixel in column direction*/
	bool structured;				/*!<Flag for anti scatter structure*/
	double maxRayAngleDetectable;	/*!<Maximum GetAngle between pixel normal and Ray in radians*/
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
	detectorPhysicalParameter( const radonProperties radonParameter, const detectorIndipendentParameter indipendentParameter );


	public:

	GridIndex number;					/*!<Amount of pixel in each dimension*/
	double angle;					/*!<Angle between outer normals*/
	double detectorFocusDistance;	/*!Distance of focus and detector pixel*/
	bool structured;				/*!<Flag for anti scatter structure*/
	double maxRayAngleDetectable;	/*!<Maximum GetAngle between pixel normal and Ray*/
};