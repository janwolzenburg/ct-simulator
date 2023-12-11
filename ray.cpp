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


	// Iterate energies in spectrum
	for( const auto& [ energy, photons ] : properties_.energy_spectrum_.data() ){
		

		// No photons at current energy
		if( IsNearlyEqual( photons, 0., 1e-6, Relative ) ) continue;

		const double cross_section_mm = ScatteringCrossSection::GetInstance().GetCrossSection( energy );
		const double coefficient_1Permm = cross_section_mm * electron_density_water_1Permm3 * coefficient_factor;
		const double scatter_propability = 1. - exp( -coefficient_1Permm * distance_traveled_mm );

		const double photons_per_bin = tomography_properties.scattered_ray_absorption_factor * photons / static_cast<double>( bins_per_energy );
		const double power_in_bin = energy * photons_per_bin;
		const double power_fraction = power_in_bin / properties_.energy_spectrum_.GetTotalPowerIn_eVPerSecond();

		for( size_t bin = 0; bin < bins_per_energy; bin++ ){

			if( integer_random_number_generator.DidARandomEventHappen( scatter_propability * tomography_properties.scatter_propability_correction ) ){

				const double angle = scattering_information.GetRandomAngle( energy );
				const UnitVector3D newDirection = direction_.RotateConstant(	scattering_information.scattering_plane_normal(),
																				angle );
				
				if( IsNearlyEqual( angle, 0., 1e-3, Relative ) ) continue;

				const double new_energy = 1. / ( 1. / ( me_c2_eV ) * ( 1. - cos( angle ) )  + 1. / energy );
				
				const EnergySpectrum new_spectrum{ VectorPair{ { new_energy }, { photons_per_bin } } }; 
				RayProperties new_properties{ new_spectrum };
				new_properties.voxel_hits_ = properties_.voxel_hits_;
				new_properties.simple_intensity_ = properties_.simple_intensity_ * power_fraction * scattered_ray_intensity_factor;
				new_properties.initial_power_ = new_spectrum.GetTotalPower();

				scattered_rays.emplace_back( newDirection, newOrigin, new_properties );

				properties_.energy_spectrum_.ScaleEnergy( energy, 1. - tomography_properties.scattered_ray_absorption_factor / static_cast<double>( bins_per_energy ) );
				properties_.only_scattering_spectrum.ScaleEnergy( energy, 1. - tomography_properties.scattered_ray_absorption_factor / static_cast<double>( bins_per_energy ) );
			}
		}
	}

	return scattered_rays;

}