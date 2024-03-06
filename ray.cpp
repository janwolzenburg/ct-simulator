/*********************************************************************
 * @file   rays.cpp
 * @brief  Implementations
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/


/*********************************************************************
	Includes
 *********************************************************************/
#include "ray.h"
#include "simulation.h"
#include "tomography.h"

/*********************************************************************
   Implementations
*********************************************************************/


/*
	RayProperties implementation
*/

void RayProperties::AttenuateSpectrum( const VoxelData& voxel_data, const double distance ){
	energy_spectrum_.GetAbsorped( voxel_data, distance );
	#ifdef TRANSMISSION_TRACKING
	only_absorption_spectrum.GetAbsorped( voxel_data, distance );
	#endif
	simple_intensity_ *= exp( -voxel_data.GetAbsorptionAtEnergy( reference_energy_for_mu_eV ) * distance );
}


/*
	Ray implementation
*/

Ray Ray::ConvertTo( const CoordinateSystem* const target ) const{
	return Ray{ this->Line::ConvertTo( target ), properties_ };
};

double Ray::GetLineParameter( const Point3D p, bool* const solution_found_ ) const{
	double t = Line::GetLineParameter( p, solution_found_ );
	*solution_found_ = *solution_found_ && ( t >= 0 );
	return t;
}

Ray Ray::ProjectOnXYPlane( const CoordinateSystem* const cSys ) const{
	return Ray{ this->Line::ProjectOnXYPlane( cSys ), this->properties_ };
}

void Ray::UpdateProperties( const VoxelData& data_, const double distance ){
	
	// Implement here handling of new properties
	if(  !data_.HasSpecialProperty() ||
		 data_.HasSpecificProperty( VoxelData::SpecialProperty::Metal ) )
		properties_.AttenuateSpectrum( data_, distance );
}

array<bool, ToUnderlying( Voxel::Face::End )> Ray::GetPossibleVoxelExits( void ) const{

	array<bool, ToUnderlying( Voxel::Face::End )> face_possiblities{ false };

	// Iterate all faces of voxel
	for( Voxel::Face currentFace = Voxel::Face::Begin; 
			 currentFace < Voxel::Face::End; ++currentFace ){

		bool possible = false;

		// Check if face can be an exit face of the ray
		switch( currentFace ){
			case Voxel::Face::YZ_Xp:
				if( direction_.X() > 0 ) 
					face_possiblities.at( ToUnderlying( currentFace ) ) = true; break;
			case Voxel::Face::YZ_Xm:
				if( direction_.X() < 0 ) 
					face_possiblities.at( ToUnderlying( currentFace ) ) = true; break;

			case Voxel::Face::XZ_Yp:
				if( direction_.Y() > 0 ) 
					face_possiblities.at( ToUnderlying( currentFace ) ) = true; break;
			case Voxel::Face::XZ_Ym:
				if( direction_.Y() < 0 )
					face_possiblities.at( ToUnderlying( currentFace ) ) = true; break;

			case Voxel::Face::XY_Zp:
				if( direction_.Z() > 0  )
					face_possiblities.at( ToUnderlying( currentFace ) ) = true; break;
			case Voxel::Face::XY_Zm:
				if( direction_.Z() < 0 ) 
					face_possiblities.at( ToUnderlying( currentFace ) ) = true; break;

			default: break;
		}
	}
		return face_possiblities;
}

	
void Ray::SetOrigin( const Point3D new_origin ){
	if( !new_origin.HasSameSystem( origin_ ) ){
		origin_ = new_origin.ConvertTo( origin_ );
		return;
	}
	
	origin_ = new_origin;
}


void Ray::SetDirection( const UnitVector3D new_direction ){
		if( !new_direction.HasSameSystem( direction_ ) ){
		direction_ = new_direction.ConvertTo( direction_ );
		return;
	}
	
	direction_ = new_direction;
}


vector<Ray> Ray::Scatter( RayScattering& scattering_information, 
													mutex& scattering_properties_mutex,
													const VoxelData voxel_data, 
													const double distance_traveled_mm, 
													const TomographyProperties tomography_properties, 
													const Point3D new_origin ){

	// Voxel's absorption with respect to water's absorption
	const double coefficient_factor = voxel_data.GetAbsorptionAtReferenceEnergy() / 
																		absorption_water_Per_mm;

	vector<Ray> scattered_rays;
	// Skip if scattering is really inpropable
	if( IsNearlyEqual( coefficient_factor, 0., 1e-6, Relative ) ) return scattered_rays;

	// Number of discrete scatter angles
	const size_t number_of_angles = 
		static_cast<size_t>( 2. * PI / scattering_information.angle_resolution() ) + 1;
	
	// For each angle store a vector with the amount of each energy in spectrum is scattered 
	vector<vector<size_t>> scattered_angles( number_of_angles, 
		vector<size_t>(  properties_.energy_spectrum_.GetNumberOfEnergies(), 0 ) );

	// Number of scatteres bins over all energies
	size_t scattered_bins_sum = 0;

	// Iterate energies in spectrum
	size_t energy_index = 0;
	for( const auto& [ energy, photons ] : properties_.energy_spectrum_.data() ){
		
		// No photons at current energy
		if( IsNearlyEqual( photons, 0., 1e-6, Relative ) ) continue;

		// Calculate scattering propability from compton scattering cross section
		const double cross_section_mm = 
			ScatteringCrossSection::GetInstance().GetCrossSection( energy );
		const double coefficient_1Permm = cross_section_mm * 
																			electron_density_water_1Permm3 * 
																			coefficient_factor;
		const double scatter_propability = 1. - exp( -coefficient_1Permm * distance_traveled_mm );

		// Iterate through energy bins
		for( size_t bin = 0; bin < simulation_properties.bins_per_energy; bin++ ){

			// Does scattering happen?
			if( integer_random_number_generator.DidARandomEventHappen( 
					scatter_propability * tomography_properties.scatter_propability_correction ) ){

				// Random angle
				const double angle = ForceRange( scattering_information.GetRandomAngle( energy, scattering_properties_mutex ), -PI, PI );			
				
				// If angle is almost zero -> treat as if no scattering happened
				if( IsNearlyEqual( angle, 0., 1e-3, Relative ) ) continue;

				// Get index of random angle
				const size_t angle_index = static_cast<size_t>( ( angle + PI ) / 
																		 scattering_information.angle_resolution() );
				
				// Increment count of scattering for current energy and angle
				scattered_angles.at( angle_index ).at( energy_index )++;
				scattered_bins_sum++;
			}
		}
		// Next energy 
		energy_index++;
	}

	// Attenuation coefficient at reference energy. For simple intensity
	const double cross_section_at_reference_energy_mm = 
		ScatteringCrossSection::GetInstance().GetCrossSection( reference_energy_for_mu_eV );
	const double coefficient_at_reference_energy_1Permm = 
		cross_section_at_reference_energy_mm * electron_density_water_1Permm3 * coefficient_factor;
	const double simple_fraction = exp( -distance_traveled_mm * 
																	coefficient_at_reference_energy_1Permm  );

	// Attenuate simple intensity
	properties_.simple_intensity_ *= simple_fraction;


	// Vector to store factors for attenuating this ray
	vector<double> energy_scalars( properties_.energy_spectrum_.GetNumberOfEnergies(), 1. );

	// Iterate all scatter angles with photons
	// Build one ray for each angle
	size_t angle_index = 0;
	for( const auto& scattered_energies : scattered_angles ){
		
		// Get angle from angle index
		const double angle = -PI + static_cast<double>( angle_index ) * 
			scattering_information.angle_resolution();
		
		// Vector pair for spectrum of scattered ray
		VectorPair new_energies{};		
		
		// How many bins in current ray
		size_t scattered_bins_for_current_angle = 0;

		// Iterate all energy bins that were scattered in current direction
		size_t energy_index = 0;
		bool current_angle_has_ray_with_nonzero_energies = false;

		for( const auto& number_of_scattered_bins : scattered_energies ){

			// Continue if no scattering for this energy happened
			if( number_of_scattered_bins == 0 ){
				energy_index++;
				continue;
			}

			// Accumulate number of bins over all energies
			scattered_bins_for_current_angle += number_of_scattered_bins;

			// Get current energy value
			const double energy = properties_.energy_spectrum_.GetEnergy( energy_index );
			
			// Calculate scattered photons energy via compton-wavelength
			const double new_energy = 1. / ( 1. / ( me_c2_eV ) * ( 1. - cos( angle ) )  + 1. / energy );

			// Calculate the amount of photons in this energies bins
			const double photons_per_bin = tomography_properties.scattered_ray_absorption_factor * 
											properties_.energy_spectrum_.data().at( energy_index ).y / 
												static_cast<double>( simulation_properties.bins_per_energy );
			
			// Scalers for energies in incoming ray. Only accounts for energy lost to new rays without
			// considering der angle dependent energy loss (Compton-Aporption). This is because the 
			// Compton-Absorption is already accounted for in the absorption routine
			energy_scalars.at( energy_index ) *= 
				pow( 1. - tomography_properties.scattered_ray_absorption_factor / 
					static_cast<double>( simulation_properties.bins_per_energy ), 
				static_cast<double>( number_of_scattered_bins ) );

			// Check if this bin is scattered inside the scattering plane 
			if( abs( scattering_information.GetRandomAngle( energy, scattering_properties_mutex ) ) <= scattering_information.max_angle_to_lie_in_plane() ){
				// Add bin to new ray			
				new_energies.first.push_back( new_energy );
				new_energies.second.push_back( number_of_scattered_bins * photons_per_bin );		
				current_angle_has_ray_with_nonzero_energies = true;
			}


			energy_index++;
		}

		// Create scattered ray only if is has energy
		if( current_angle_has_ray_with_nonzero_energies ){
			
			EnergySpectrum new_spectrum{ new_energies }; 

			// Discard scattered ray when its angle to the scattering plane is
			// too large meaning it would not hit the detector
			//if( !integer_random_number_generator.DidARandomEventHappen( 
			//	2. * scattering_information.max_angle_to_lie_in_plane() / PI ) ){
			//if( scattering_information.GetRandomAngle( energy, scattering_properties_mutex ) )
				
			//	angle_index++;
			//	continue;
			//}

			// Use complete amount of scattered bins to take scattering into account of simple intensity
			const double scattered_bins_fraction = 
					static_cast<double>( scattered_bins_for_current_angle ) / 
					static_cast<double>( scattered_bins_sum );

			// Create ray properties
			RayProperties new_properties{ new_spectrum };
			new_properties.voxel_hits_ = properties_.voxel_hits_;
			new_properties.simple_intensity_ = properties_.simple_intensity_ * 
																				 scattered_bins_fraction * 
																				 simple_fraction;
			new_properties.initial_power_ = new_spectrum.GetTotalPower();

			const UnitVector3D new_direction = direction_.RotateConstant(	
																			scattering_information.scattering_plane_normal(),
																			angle );

			// Save scattered ray																				
			scattered_rays.emplace_back( new_direction, new_origin, new_properties );
		}

		angle_index++;
	}


	energy_index = 0;
	for( const auto& energy_scalar : energy_scalars ){

		properties_.energy_spectrum_.ScaleEnergy( energy_index, energy_scalar );
		#ifdef TRANSMISSION_TRACKING
		properties_.only_scattering_spectrum.ScaleEnergy( energy_index, energy_scalar );
		#endif
		energy_index++;
	}

	return scattered_rays;

}