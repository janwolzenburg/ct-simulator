#pragma once
/*********************************************************************
 * @file   tomography.h
 * @brief  Classes 
 *
 * @author Jan Wolzenburg
 * @date   January 2023
 *********************************************************************/



 /*********************************************************************
  Includes
*********************************************************************/


#include "generel.h"
#include "gantry.h"
#include "model.h"
#include "radonTransform.h"

#include "progress.h"


/*********************************************************************
   Definitions
*********************************************************************/

/*!
 * @brief Class for tomography parameter
*/
class tomographyParameter{

	public:

	static const string FILE_PREAMBLE;

	/*!
	 * @brief Default constructor
	*/
	tomographyParameter( void );

	/*!
	 * @brief Constructor
	 * @param exposureTime_ Exposure time in seconds 
	 * @param scattering_ Enable ray scattering during transmission
	 * @param maxRadiationLoops_ Maximum amount of loops, when rays are scattered
	 * @param scatterPropability_ Approximate propability that a ray is scattered once when transmitted through whole model
	 * @param rayStepSize_ Step size used in ray tracing
	*/
	tomographyParameter( const double exposureTime_, const bool scattering_, const size_t maxRadiationLoops_, const double scatterPropability_, const double rayStepSize_ );
	
	/*!
	 * @brief Constructor from serialized data
	 * @param binary_data Reference to vector with binary data
	 * @param it Iterator to start of data in vector
	*/
	tomographyParameter( const vector<char>& binary_data, vector<char>::const_iterator& it );

	/*!
	 * @brief Serialize this object
	 * @param binary_data Reference to vector where data will be appended
	*/
	size_t Serialize( vector<char>& binary_data ) const;


	public:

	double exposureTime;			/*!<Exposure time in seconds*/
	bool scattering;				/*!<Enable scattering*/
	size_t maxRadiationLoops;		/*!<Max. amount rays can be scattered*/
	double scatterPropability;		/*!<Approximate propability that a ray is scattered once when transmitted through whole model*/
	double rayStepSize;				/*!<Step size used in ray tracing in mm*/

};


/*!
 * @brief Class for computed tomography
 * 
*/
class tomography{

	public:
	/*!
	 * @brief Constructor
	 * @param gantry_ 
	 * @param model_ 
	*/
	tomography( const tomographyParameter parameter_ ) :
		parameter( parameter_ ), radonCSys( DummySystem() )
	{};

	/*!
	 * @brief Default constructor
	*/
	tomography( void ) :
		parameter( ), radonCSys( DummySystem() )
	{};

	/*!
	 * @brief Record a slice
	 * @return Radon transformed of model slice
	*/
	radonTransformed recordSlice( gantry gantry_, const model& model_, const double zPosition, Fl_Progress_Window* progressWindow = nullptr );


	private:

	tomographyParameter parameter;	/*!<Parameter used for tomography*/
	CoordinateSystem* radonCSys;			/*!<Coordinate system to use as reference for radon Coordinates calculation*/
};

