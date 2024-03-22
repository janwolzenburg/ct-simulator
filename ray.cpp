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
	
	// implement here handling of new properties
	if(  !data_.HasSpecialProperty() ||
		 data_.HasSpecificProperty( VoxelData::SpecialProperty::Metal ) )
		properties_.AttenuateSpectrum( data_, distance );
}

array<bool, ToUnderlying( Voxel::Face::End )> Ray::GetPossibleVoxelExits( void ) const{

	array<bool, ToUnderlying( Voxel::Face::End )> face_possiblities{ false };

	// iterate all faces of voxel
	for( Voxel::Face currentFace = Voxel::Face::Begin; 
			 currentFace < Voxel::Face::End; ++currentFace ){

		bool possible = false;

		// check if face can be an exit face of the ray
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
													const VoxelData& voxel_data, 
													const double distance_traveled_mm, 
													const TomographyProperties& tomography_properties, 
													const Point3D& new_origin ){

	// voxel's absorption with respect to water's absorption
	const double coefficient_factor = voxel_data.GetAbsorptionAtReferenceEnergy() / 
																		absorption_water_Per_mm;

	// attenuation coefficient at reference energy. for simple intensity
	const double cross_section_at_reference_energy_mm = 
		ScatteringCrossSection::GetInstance().GetCrossSection( reference_energy_for_mu_eV );

	const double coefficient_at_reference_energy_1Permm = 
		cross_section_at_reference_energy_mm * electron_density_water_1Permm3 * 
		coefficient_factor;

	const double simple_fraction = exp( -distance_traveled_mm * 
																 coefficient_at_reference_energy_1Permm  );

	properties_.simple_intensity_ *= simple_fraction;													


	// ---------------------------- Scattering -------------------------
	
	vector<Ray> scattered_rays;
	// skip if scattering is extremely inpropable
	if( IsNearlyEqual( coefficient_factor, 0., 1e-6, Relative ) ) return scattered_rays;
	

	vector<pair<double, pair<double, double>>> scattered_angles;

	// number of scatteres bins over all energies
	size_t scattered_bins_sum = 0;

	// iterate energies in spectrum
	size_t energy_index = 0;
	for( const auto& [ energy, photons ] : properties_.energy_spectrum_.data() ){
		
		// no photons at current energy
		if( IsNearlyEqual( photons, 0., 1e-6, Relative ) ) continue;

		// calculate scattering propability from compton scattering cross section
		const double cross_section_mm = 
			ScatteringCrossSection::GetInstance().GetCrossSection( energy );

		const double coefficient_1Permm = cross_section_mm * 
																			electron_density_water_1Permm3 * 
																			coefficient_factor;

		const double scatter_propability =  1. - exp( -coefficient_1Permm * 
																								  distance_traveled_mm );

		// iterate through energy bins
		for( size_t bin = 0; bin < simulation_properties.bins_per_energy; bin++ ){

			// does scattering happen?
			if( !integer_random_number_generator.DidARandomEventHappen( 
					scatter_propability * tomography_properties.scatter_propability_correction ) )
				continue;
			
			// check if angle is scattered inside scattering plane
			if( abs( scattering_information.GetRandomAngle( 
																energy, scattering_properties_mutex ) ) <= 
																scattering_information.max_angle_to_lie_in_plane() ){
			
				// random angle inside scattering plane
				const double angle = ForceRange( scattering_information.GetRandomAngle( 
																				 energy, scattering_properties_mutex ), -PI, PI );
				
				// if angle is almost zero -> treat as if no scattering happened
				if( IsNearlyEqual( angle, 0., 1e-3, Relative ) ) continue;

				// calculate scattered photons energy via compton-wavelength
				const double new_energy = 
					1. / ( 1. / ( me_c2_eV ) * ( 1. - cos( angle ) )  + 1. / energy );
					
				// new photonflow
				const double new_photonflow = 
					tomography_properties.scattered_ray_absorption_factor * photons / 
					static_cast<double>( simulation_properties.bins_per_energy );

				scattered_angles.emplace_back( angle, 
																			 pair<double, double>{ new_energy, new_photonflow });
			}

			// scalar for energy in incoming ray. only accounts for energy lost to new rays
			// without considering der angle dependent energy loss (Compton-Aporption). 
			// this is because the compton-absorption is already accounted for in the absorption 
			// routine
			const double energy_scalar = 
				1. - tomography_properties.scattered_ray_absorption_factor / 
				static_cast<double>( simulation_properties.bins_per_energy );

			properties_.energy_spectrum_.ScaleEnergy( energy_index, energy_scalar );
			#ifdef TRANSMISSION_TRACKING
			properties_.only_scattering_spectrum.ScaleEnergy( energy_index, energy_scalar );
			#endif
			
			scattered_bins_sum++;
		}

		energy_index++;
	}

	// no ray scattered in scattering plane -> return empty
	if( scattered_angles.size() == 0 )
		return {};

	// sort scattered angles
	std::sort( scattered_angles.begin(), scattered_angles.end(), 
						 []( const auto& a, const auto& b ){ return a.first > b.first; } );

	double previous_angle = -1.;	// angle currently handled
	size_t scattered_bins_for_current_angle = 0;

	// photonflows for scattered ray
	vector<Tuple2D> spectral_photonflows;

	// iterate through sorted angles
	for( size_t angle_index = 0; angle_index < scattered_angles.size(); angle_index++ ){
		
		const double angle = scattered_angles.at( angle_index ).first;
		const double energy = scattered_angles.at( angle_index ).second.first;
		const double photonflow = scattered_angles.at( angle_index ).second.second;
		scattered_bins_for_current_angle++;

		// angle already in spectrum or first angle
		if( previous_angle == angle || angle_index == 0 ){
			
			// iterate through current spectrum
			// energy index tracks where to put add photonflow to
			size_t energy_index = 0;
			for(; energy_index < spectral_photonflows.size(); 
																		energy_index++ ){
				// energy is in current spectrum
				if( energy == spectral_photonflows.at( energy_index ).x )
					break;	// current index is index of energy
			}

			// energy is not in spectrum -> add
			if( energy_index == spectral_photonflows.size() ){
				spectral_photonflows.emplace_back( energy, photonflow );
			}
			else{
				spectral_photonflows.at( energy_index ).y += photonflow;
			}
		}
		else{
			spectral_photonflows.emplace_back( energy, photonflow );
		}
		
		// flag to know whether the current angle has power and needs to be created
		bool build_ray = false;
		
		// at least one angle left
		if( angle_index < scattered_angles.size() - 1 ){
			// next ray has different angle
			if( scattered_angles.at( angle_index + 1 ).first != angle )
				build_ray = true;
		}
		// last angle
		else if( angle_index == scattered_angles.size() - 1 )
			build_ray = true;

		// build scattered ray. When angle is finished or last element is reached
		if( build_ray ){

			// use complete amount of scattered bins to take scattering into account of 
			// simple intensity
			const double scattered_bins_fraction = 
					static_cast<double>( scattered_bins_for_current_angle ) / 
					static_cast<double>( scattered_bins_sum );

			const EnergySpectrum new_spectrum{ spectral_photonflows }; 

			// create ray properties
			RayProperties new_properties{ new_spectrum };
			new_properties.voxel_hits_ = properties_.voxel_hits_;
			new_properties.simple_intensity_ = properties_.simple_intensity_ * 
																				 scattered_bins_fraction * 
																				 simple_fraction;
			new_properties.initial_power_ = new_spectrum.GetTotalPower();

			const UnitVector3D new_direction = 
				direction_.RotateConstant( scattering_information.scattering_plane_normal(),
																	 angle );

			// save scattered ray																				
			scattered_rays.emplace_back( new_direction, new_origin, new_properties );
	
			// reset for next angle
			scattered_bins_for_current_angle = 0;
			spectral_photonflows.clear();				
		}
		
		// remember previous angle
		previous_angle = angle;
	
	}

	return scattered_rays;
}


void Ray::SetExpectedPixelIndex( const size_t pixel_index, const bool definitely_hits ){
	properties_.expected_detector_pixel_index_ = pixel_index;
	properties_.definitely_hits_expected_pixel_ = definitely_hits;
}