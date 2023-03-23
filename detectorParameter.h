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

	detectorRadonParameter( void ) :
		detectorRadonParameter{ idx2CR{ 32, 16 }, 500. }
	{};

	detectorRadonParameter( const vector<char>& binData, vector<char>::const_iterator& it ) :
		numberPoints( deSerialize<idx2CR>( binData, it)),
		resolution( deSerialize<v2CR>( binData, it ) ),
		framesToFillSinogram( deSerializeBuildIn( 1, binData, it ) ){

	}


	/*!
		* @brief Serialize this object
		* @param binData Reference to vector where data will be appended
	*/
	size_t serialize( vector<char>& binData ) const{
		size_t numBytes = 0;

		numBytes += numberPoints.serialize( binData );
		numBytes += resolution.serialize( binData );
		numBytes += serializeBuildIn( framesToFillSinogram, binData );

		return numBytes;
	}

	public:

	idx2CR numberPoints;			/*!<Number of points on the axis*/
	v2CR resolution;				/*!Resolution of the axis*/
	size_t framesToFillSinogram;	/*!<Amount of frames to fill sinogram*/
};



/*!
 * @brief Class for physical detector parameters which are not dependent on the radon parameters
*/
class detectorIndipendentParameter{

	public:

	static const string FILE_PREAMBLE;

	public:

	/*!
	 * @brief Constructor
	 * @param angle_ Detector angle
	 * @param columnSize_ Size of pixel in column direction
	 * @param structured_ Flag for anti scatter structure
	 * @param maxRayAngleDetecable Maximum angle between pixel normal and ray
	*/
	detectorIndipendentParameter( const double arcRadius_, const double columnSize_, const bool structured_ = false, const double maxRayAngleDetectable_ =  0.15 );

	detectorIndipendentParameter( void ) :
		detectorIndipendentParameter{ 1000., 50., true, .15 }
	{};

	detectorIndipendentParameter( const vector<char>& binData, vector<char>::const_iterator& it ) :
		arcRadius( deSerializeBuildIn( 1000., binData, it ) ),
		columnSize( deSerializeBuildIn( 50., binData, it ) ),
		structured( deSerializeBuildIn( true, binData, it ) ),
		maxRayAngleDetectable( deSerializeBuildIn( .15, binData, it ) ){

	}


	/*!
		* @brief Serialize this object
		* @param binData Reference to vector where data will be appended
	*/
	size_t serialize( vector<char>& binData ) const{
		size_t numBytes = 0;

		numBytes += serializeBuildIn( arcRadius, binData );
		numBytes += serializeBuildIn( columnSize, binData );
		numBytes += serializeBuildIn( structured, binData );
		numBytes += serializeBuildIn( maxRayAngleDetectable, binData );

		return numBytes;
	}

	public:

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