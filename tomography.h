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
#include "projections.h"

#include "progress.h"


/*********************************************************************
   Definitions
*********************************************************************/

/*!
 * @brief Class for tomography properties
*/
class TomographyProperties{

	public:

	static const string FILE_PREAMBLE;

	/*!
	 * @brief Default constructor
	*/
	TomographyProperties( void );

	/*!
	 * @brief Constructor
	 * @param exposure_time Exposure time in seconds 
	 * @param scattering_enabled Enable Ray scattering during transmission
	 * @param max_scattering_occurrences Maximum amount of loops, when rays are scattered. How often can a single ray be scattered
	 * @param scatter_propability Approximate propability that a Ray is scattered once when transmitted through whole model
	 * @param ray_step_length Step size used in ray-tracing
	*/
	TomographyProperties( const double exposure_time, const bool scattering_enabled, const size_t max_scattering_occurrences, const double scatter_propability, const double ray_step_length );
	
	/*!
	 * @brief Constructor from serialized data
	 * @param binary_data Reference to vector with binary data
	 * @param current_byte Iterator to start of data in vector
	*/
	TomographyProperties( const vector<char>& binary_data, vector<char>::const_iterator& current_byte );

	/*!
	 * @brief Serialize this object
	 * @param binary_data Reference to vector where data will be appended
	*/
	size_t Serialize( vector<char>& binary_data ) const;


	public:

	double exposure_time;				/*!<Exposure time in seconds*/
	bool scattering_enabled;			/*!<Enable scattering*/
	size_t max_scattering_occurrences;	/*!<Max. amount each rays can be scattered*/
	double scatter_propability;			/*!<Approximate propability that a Ray is scattered once when transmitted through whole model*/
	double ray_step_length;				/*!<Step size used in Ray tracing in mm*/

};


/*!
 * @brief Class for computed tomography
*/
class Tomography{

	public:

	/*!
	 * @brief Constructor
	 * @param properties Properties of computed tomography
	*/
	Tomography( const TomographyProperties properties ) :
		properties_( properties ), radon_coordinate_system_( DummySystem() )
	{};

	/*!
	 * @brief Default constructor
	*/
	Tomography( void ) :
		properties_( TomographyProperties{} ), radon_coordinate_system_(DummySystem())
	{};

	/*!
	 * @brief Record a slice via simulated computed tomography
	 * @param radon_properties Properties of radon transformed
	 * @param gantry Gantry of CT-Device
	 * @param model Model to slice
	 * @param z_position z-positon of slice
	 * @param progress_window Window to show progress
	 * @return The sinogram
	*/
	Projections RecordSlice( const RadonTransformationProperties radon_properties, Gantry gantry, const Model& model, const double z_position, Fl_Progress_Window* progress_window = nullptr );


	private:

	TomographyProperties properties_;			/*!<Properties used for tomography*/
	CoordinateSystem* radon_coordinate_system_;	/*!<Coordinate system to use as reference for radon Coordinates calculation*/
};

