#pragma once
/*********************************************************************
 * @file   tomography.h
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
#include "projections.fwd.h"

#include "fl_ProgressWindow.h"


/*********************************************************************
   Definitions
*********************************************************************/

/*!
 * @brief class for tomography properties
*/
class TomographyProperties{

	public:

	static const string FILE_PREAMBLE; /*!< string to prepend to file when storing as file*/

	/*!
	 * @brief default constructor
	*/
	TomographyProperties( void );

	/*!
	 * @brief constructor
	 * @param scattering_enabled enable ray scattering during transmission
	 * @param max_scattering_occurrences maximum amount of loops, when rays are scattered. How often can a single ray be scattered
	 * @param scatter_propability_correction correction of scattering propability
	 * @param use_simple_absorption use simple or energy dependant attaenuation
	 * @param scattered_ray_absorption_factor factor to scale the scattered rays energies by
	 * @param name name for identification
	 * @param filter_active flag for active tube filter
	*/
	TomographyProperties( const bool scattering_enabled, const size_t max_scattering_occurrences, const double scatter_propability_correction, 
						  const bool use_simple_absorption, const double scattered_ray_absorption_factor,
						  const string name = "Unnamed", const bool filter_active = false, const size_t simulation_quality = 9 );
	
	/*!
	 * @brief constructor from serialized data
	 * @param binary_data reference to vector with binary data
	 * @param current_byte iterator to start of data in vector
	*/
	TomographyProperties( const vector<char>& binary_data, vector<char>::const_iterator& current_byte );

	/*!
	 * @brief serialize this object
	 * @param binary_data reference to vector where data will be appended
	 * @return written bytes
	 */
	size_t Serialize( vector<char>& binary_data ) const;


	public:
	
	bool scattering_enabled;								/*!< enable scattering*/
	size_t max_scattering_occurrences;			/*!< max. amount each rays can be scattered*/
	double scatter_propability_correction;	/*!< correction of scattering propability. Adjusts propabiltiy to make simulation possible*/
	bool use_simple_absorption;							/*!< flag for using simple absorption*/
	double scattered_ray_absorption_factor;	/*!< factor to scale the scattered rays energies by*/
	double mean_energy_of_tube;							/*!< mean energy of tube when radiated*/
	string name;														/*!< name for identifiaction*/
	bool filter_active;											/*!< flag for filter*/
	size_t simulation_quality;							/*!< stored simulation quality*/
};


/*!
 * @brief class for computed tomography
*/
class Tomography{

	public:

	/*!
	 * @brief constructor
	 * @param properties properties of computed tomography
	*/
	Tomography( const TomographyProperties properties ) :
		properties_( properties ), radon_coordinate_system_( GetDummySystem() )
	{};

	/*!
	 * @brief default constructor
	*/
	Tomography( void ) :
		properties_( TomographyProperties{} ), radon_coordinate_system_(GetDummySystem())
	{};

	/*!
	 * @brief record a slice via simulated computed tomography
	 * @param projections_properties properties of radon transformed
	 * @param gantry gantry of ct-device
	 * @param model model to slice
	 * @param z_position z-positon of slice
	 * @param progress_window window to show progress
	 * @return the projections when process was not terminated
	*/
	optional<Projections> RecordSlice( const ProjectionsProperties projections_properties, Gantry gantry, const Model& model, const double z_position, Fl_Progress_Window* progress_window = nullptr );

	
	private:

	TomographyProperties properties_;						/*!< properties used for tomography*/
	CoordinateSystem* radon_coordinate_system_;	/*!< coordinate system to use as reference for radon coordinates calculation*/
};

