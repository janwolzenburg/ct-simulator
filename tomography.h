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

	tomographyParameter( void );

	tomographyParameter( const double exposureTime_, const bool scattering_, const size_t maxRadiationLoops_, const double scatterPropability_, const double rayStepSize_ );

	tomographyParameter( const vector<char>& binData, vector<char>::const_iterator& it );

	//double ExposureTime( void ) const { return exposureTime; };

	//bool Scattering( void ) const{ return scattering; };

	//size_t MaxLoops( void ) const{ return maxRadiationLoops; };

	//double ScatterPropability( void ) const{ return scatterPropability; };

	//double RayStepSize( void ) const{ return rayStepSize; };

	size_t serialize( vector<char>& binData ) const;


	public:

	double exposureTime;
	bool scattering;
	size_t maxRadiationLoops;
	double scatterPropability;
	double rayStepSize;

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
		parameter( parameter_ ),
		radonCSys( DUMMY_CSYS() )
	{};

	tomography( void ) :
		parameter( ),
		radonCSys( DUMMY_CSYS() )
	{};

	/*!
	 * @brief Record a slice
	 * @return Radon transformed of model slice
	*/
	radonTransformed recordSlice( gantry gantry_, const model& model_, const double zPosition, Fl_Progress_Window* progressWindow = nullptr );


	private:

	tomographyParameter parameter;	/*!<Parameter used for tomography*/
	cartCSys* radonCSys;			/*!<Coordinate system to use as reference for radon coordinates calculation*/
};

