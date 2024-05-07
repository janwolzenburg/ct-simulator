
/*********************************************************************
 * @file   tomography.cpp
 *
 * @author Jan Wolzenburg
 * @date   January 2023
 *********************************************************************/



 /*********************************************************************
  Includes
*********************************************************************/

#include <FL/Fl.H>

#include "tomography.h"
#include "coordinateSystemTree.h"
#include "simulation.h"
#include "serialization.h"
#include "projections.h"


/*********************************************************************
   Implemnetations
*********************************************************************/


const string TomographyProperties::FILE_PREAMBLE{ "TOMO_PARAMETER_FILE_PREAMBLE_Ver06" };

TomographyProperties::TomographyProperties( void ) :
	scattering_enabled( true ),
	max_scattering_occurrences( default_max_radiation_loops ),
	scatter_propability_correction( default_scatter_propability_correction ),
	use_simple_absorption( true ),
	scattered_ray_absorption_factor( 1. ),
	mean_energy_of_tube( reference_energy_for_mu_eV ),
	name( "Unnamed" ),
	filter_active( false ),
	simulation_quality( 9 )

{}

TomographyProperties::TomographyProperties( const bool scattering_enabled, const size_t max_scattering_occurrences, const double scatter_propability_correction, 
											const bool use_simple_absorption, const double scattered_ray_absorption_factor,
											const string name_, const bool filter_active_, const size_t simulation_quality_ ) :
	scattering_enabled( scattering_enabled ),
	max_scattering_occurrences( max_scattering_occurrences ),
	scatter_propability_correction( scatter_propability_correction ),
	use_simple_absorption( use_simple_absorption ),
	scattered_ray_absorption_factor( scattered_ray_absorption_factor ),
	mean_energy_of_tube( reference_energy_for_mu_eV ),
	name( name_ ),
	filter_active( filter_active_ ),
	simulation_quality( simulation_quality_ )
{}

TomographyProperties::TomographyProperties( const vector<char>& binary_data, vector<char>::const_iterator& current_byte ) :
	scattering_enabled( DeSerializeBuildIn<bool>(true, binary_data, current_byte) ),
	max_scattering_occurrences( DeSerializeBuildIn<size_t>( default_max_radiation_loops, binary_data, current_byte ) ),
	scatter_propability_correction( DeSerializeBuildIn<double>( default_scatter_propability_correction, binary_data, current_byte ) ),
	use_simple_absorption( DeSerializeBuildIn<bool>(true, binary_data, current_byte) ),
	scattered_ray_absorption_factor( DeSerializeBuildIn<double>( default_scatter_propability_correction, binary_data, current_byte ) ),
	mean_energy_of_tube( DeSerializeBuildIn<double>( reference_energy_for_mu_eV, binary_data, current_byte ) ),
	name( DeSerializeBuildIn<string>( "Unnamed", binary_data, current_byte ) ),
	filter_active( DeSerializeBuildIn<bool>(false, binary_data, current_byte) ),
	simulation_quality( DeSerializeBuildIn<size_t>(9, binary_data, current_byte) )

{
}

size_t TomographyProperties::Serialize( vector<char>& binary_data ) const{

	size_t number_of_bytes = 0;
	number_of_bytes += SerializeBuildIn<bool>( scattering_enabled, binary_data );
	number_of_bytes += SerializeBuildIn<size_t>( max_scattering_occurrences, binary_data );
	number_of_bytes += SerializeBuildIn<double>( scatter_propability_correction, binary_data );
	number_of_bytes += SerializeBuildIn<bool>( use_simple_absorption, binary_data );
	number_of_bytes += SerializeBuildIn<double>( scattered_ray_absorption_factor, binary_data );
	number_of_bytes += SerializeBuildIn<double>( mean_energy_of_tube, binary_data );
	number_of_bytes += SerializeBuildIn<string>( name, binary_data );
	number_of_bytes += SerializeBuildIn<bool>( filter_active, binary_data );
	number_of_bytes += SerializeBuildIn<size_t>( simulation_quality, binary_data );


	return number_of_bytes;

}


optional<Projections> Tomography::RecordSlice( 
																		const ProjectionsProperties projection_properties, 
																		Gantry gantry, const Model& model, 
																		const double z_position,  
																		Fl_Progress_Window* progress_window ){

	// update simulation properties
	simulation_properties = SimulationProperties{ properties_.simulation_quality };

	// reset gantry to its initial position
	gantry.ResetGantry();

	// translate gantry
	if( z_position != 0. )
		gantry.TranslateInZDirection( z_position );

	// assign gantry coordinate-system's unit-vectors to radon coordinate system
	this->radon_coordinate_system_->CopyPrimitiveFrom( gantry.coordinate_system() );

	// create projections 
	Projections projections{ projection_properties, properties_ };


	RayScattering scattering_information{
		simulation_properties.number_of_scatter_angles,
		gantry.tube().GetEmittedEnergyRange(),
		simulation_properties.number_of_energies_for_scattering,
		gantry.coordinate_system()->GetEz(),
		atan(gantry.detector().properties().row_width /
				 gantry.detector().properties().detector_focus_distance / 2) };

	// radiate the model for each frame
	for( size_t frame_index = 0; 
							frame_index < projection_properties.number_of_frames_to_fill();			 
							frame_index++ ){
		
		if( progress_window != nullptr ) 
			progress_window->ChangeLineText( 0, "Radiating frame " + 
				ConvertToString( frame_index ) + " of " + 
				ConvertToString( projection_properties.number_of_frames_to_fill() ) );



		// radiate
		gantry.RadiateModel( model, properties_, scattering_information );

		// get the detection result
		const vector<DetectorPixel> pixel_array = gantry.pixel_array();

		// iterate all pixel
		for( const DetectorPixel& pixel : pixel_array ){

			// get coordinates for pixel
			const RadonCoordinates radon_coordinates{ this->radon_coordinate_system_, 
																								pixel.NormalLine() };

			optional<double> line_integral = 
				pixel.GetProjectionValue( properties_.use_simple_absorption, 
					gantry.tube().number_of_rays_per_pixel(), gantry.tube().GetEmittedBeamPower() /
					( static_cast<double>( pixel_array.size() ) * 
						static_cast<double>( gantry.tube().number_of_rays_per_pixel() )
					) );
			
			// if no value no ray was detected by pixel, the line integral would be infinite.
			// set current_byte to a high value instead
			if( !line_integral.has_value() ){
				line_integral = 25.; // is like ray's energy is 1 / 10^11 of its start energy
			}

			// get the radon point
			const RadonPoint radon_point{ radon_coordinates, line_integral.value() };

			// assign the data to sinogram
			projections.AssignData( radon_point );
		}
		
		// rotate gantry
		gantry.RotateCounterClockwise( projection_properties.angles_resolution() );

		Fl::check();

		if( progress_window != nullptr ){
			if( !progress_window->visible() )
				return {};
		}
	}

	return projections;
}