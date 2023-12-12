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

void RayProperties::AttenuateSpectrum( const VoxelData& voxelData, const double distance ){
	energy_spectrum_.GetAbsorped( voxelData, distance );
	simple_intensity_ *= exp( -voxelData.GetAbsorptionAtReferenceEnergy() * distance );
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
	for( Voxel::Face currentFace = Voxel::Face::Begin; currentFace < Voxel::Face::End; ++currentFace ){

		bool possible = false;

		// Check if face can be an exit_ face of the tRay
		switch( currentFace ){
			case Voxel::Face::YZ_Xp:
				if( direction_.X() > 0 ) face_possiblities.at( ToUnderlying( currentFace ) ) = true; break;
			case Voxel::Face::YZ_Xm:
				if( direction_.X() < 0 ) face_possiblities.at( ToUnderlying( currentFace ) ) = true; break;

			case Voxel::Face::XZ_Yp:
				if( direction_.Y() > 0 ) face_possiblities.at( ToUnderlying( currentFace ) ) = true; break;
			case Voxel::Face::XZ_Ym:
				if( direction_.Y() < 0 ) face_possiblities.at( ToUnderlying( currentFace ) ) = true; break;

			case Voxel::Face::XY_Zp:
				if( direction_.Z() > 0  )face_possiblities.at( ToUnderlying( currentFace ) ) = true; break;
			case Voxel::Face::XY_Zm:
				if( direction_.Z() < 0 ) face_possiblities.at( ToUnderlying( currentFace ) ) = true; break;

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


vector<Ray> Ray::Scatter( const RayScattering& scattering_information, const VoxelData voxel_data, const double distance_traveled_mm, const TomographyProperties tomography_properties, const Point3D newOrigin ){

	const double coefficient_factor = voxel_data.GetAbsorptionAtReferenceEnergy() / mu_water;

	vector<Ray> scattered_rays;
	if( IsNearlyEqual( coefficient_factor, 0., 1e-6, Relative ) ) return scattered_rays;

	const size_t number_of_angles = static_cast<size_t>( 2. * PI / scattering_information.angle_resolution() ) + 1;
	vector<vector<size_t>> scattered_angles( number_of_angles, vector<size_t>(  properties_.energy_spectrum_.GetNumberOfEnergies(), 0) );

	size_t energy_index = 0;
	// Iterate energies in spectrum
	for( const auto& [ energy, photons ] : properties_.energy_spectrum_.data() ){
		
		// No photons at current energy
		if( IsNearlyEqual( photons, 0., 1e-6, Relative ) ) continue;

		const double cross_section_mm = ScatteringCrossSection::GetInstance().GetCrossSection( energy );
		const double coefficient_1Permm = cross_section_mm * electron_density_water_1Permm3 * coefficient_factor;
		const double scatter_propability = 1. - exp( -coefficient_1Permm * distance_traveled_mm );

		for( size_t bin = 0; bin < bins_per_energy; bin++ ){

			if( integer_random_number_generator.DidARandomEventHappen( scatter_propability * tomography_properties.scatter_propability_correction ) ){

				const double angle = ForceRange( scattering_information.GetRandomAngle( energy ), -PI, PI );			
				if( IsNearlyEqual( angle, 0., 1e-3, Relative ) ) continue;

				const size_t angle_index = static_cast<size_t>( ( angle + PI ) /  scattering_information.angle_resolution() );
				scattered_angles.at( angle_index ).at( energy_index )++;

			}
		}

		energy_index++;
	}

	
	vector<double> energy_scalars( properties_.energy_spectrum_.GetNumberOfEnergies(), 1. );

	size_t angle_index = 0;
	for( const auto& scattered_energies : scattered_angles ){
		
		const double angle = -PI + static_cast<double>( angle_index ) *  scattering_information.angle_resolution();
		const UnitVector3D newDirection = direction_.RotateConstant(	scattering_information.scattering_plane_normal(),
																			angle );

		VectorPair new_energies{};
		double power_sum = 0.;

		size_t energy_index = 0;
		for( auto number_of_scattered_bins : scattered_energies ){

			if( number_of_scattered_bins == 0 ) continue;

			const double energy = properties_.energy_spectrum_.GetEnergy( energy_index );
			const double new_energy = 1. / ( 1. / ( me_c2_eV ) * ( 1. - cos( angle ) )  + 1. / energy );

			const double photons_per_bin = tomography_properties.scattered_ray_absorption_factor * 
											properties_.energy_spectrum_.data().at( energy_index ).y / static_cast<double>( bins_per_energy );
			power_sum += energy * photons_per_bin;
			
			new_energies.first.push_back( new_energy );
			new_energies.second.push_back( number_of_scattered_bins * photons_per_bin );
		
			energy_scalars.at( energy_index ) *= 1. - tomography_properties.scattered_ray_absorption_factor / static_cast<double>( bins_per_energy );
			energy_index++;
		}

		const double power_fraction = power_sum / properties_.energy_spectrum_.GetTotalPowerIn_eVPerSecond();
		const EnergySpectrum new_spectrum{ new_energies }; 
		RayProperties new_properties{ new_spectrum };
		new_properties.voxel_hits_ = properties_.voxel_hits_;
		new_properties.simple_intensity_ = properties_.simple_intensity_ * power_fraction * scattered_ray_intensity_factor;
		new_properties.initial_power_ = new_spectrum.GetTotalPower();

		scattered_rays.emplace_back( newDirection, newOrigin, new_properties );

		angle_index++;
	}

	energy_index = 0;
	for( const auto& energy_scalar : energy_scalars ){

		properties_.energy_spectrum_.ScaleEnergy( energy_index, energy_scalar );
		properties_.only_scattering_spectrum.ScaleEnergy( energy_index, energy_scalar );
		energy_index++;
	}

				


	return scattered_rays;

}