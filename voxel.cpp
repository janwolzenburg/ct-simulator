#pragma once
/*********************************************************************
 * @file   voxel.cpp
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/

 #include "voxel.h"
 #include "serialization.h"


  /*********************************************************************
	Implementations
 *********************************************************************/


 /*
	VoxelData implementation
 */

 const std::map < VoxelData::SpecialProperty, string> VoxelData::special_property_names{
		{ None,			"None" },
		{ Metal,		"Iron" },
		{ Undefined,	"Undefined" }
};

double VoxelData::artefact_impact_factor_ = 1.;

VoxelData::SpecialProperty VoxelData::GetPropertyEnum( const string property_string ){
	
	for( const auto& [material_enumeration, material_string] : VoxelData::special_property_names ){
		if( property_string == material_string )
			return material_enumeration;
	}

	return None;

}

VoxelData::VoxelData( const double absorption_at_energy, const double energy, const SpecialProperty special_properties ) :
	absorption_( -1. ),
	specialProperties_( special_properties )
{

	if( energy < change_energy_for_constant_mu )
		absorption_ = absorption_at_energy * pow( ( energy ) / change_energy_for_constant_mu, 3. );
	else
		absorption_ = absorption_at_energy;
}

VoxelData::VoxelData( const vector<char>& binary_data, vector<char>::const_iterator& current_byte ) : 
	absorption_( DeSerializeBuildIn<double>( 0., binary_data, current_byte ) ),
	specialProperties_( DeSerializeBuildIn<SpecialPropertyEnumType>( SpecialProperty::Undefined, binary_data, current_byte ) )
{}

double VoxelData::GetAbsorptionAtEnergy( const double energy ) const{

	// titan absorption is approx. 0.0135 1/mm above 110 keV
	if( HasSpecificProperty( Metal ) ){
		return artefact_impact_factor_ * absorption_titan_Per_mm * pow( change_energy_for_constant_mu / ForceToMax( energy, change_energy_for_constant_mu ), 3. );
	}

	return absorption_ * pow( change_energy_for_constant_mu / ForceToMax( energy, change_energy_for_constant_mu ), 3. );

}

void VoxelData::SetArtefactImpactFactor( const double artefact_impact_factor ){

	// impact factor from 0 to 10 should map from mu_water / mu_iron to 1.

	artefact_impact_factor_ = ForceToMin( artefact_impact_factor * ( 1 - absorption_water_Per_mm / absorption_titan_Per_mm ) / 10. + absorption_water_Per_mm / absorption_titan_Per_mm, 0. );
}

size_t VoxelData::Serialize( vector<char>& binary_data ) const{
	size_t number_of_bytes = 0;
	number_of_bytes += SerializeBuildIn<double>( absorption_, binary_data );
	number_of_bytes += SerializeBuildIn<typename std::underlying_type_t<VoxelData::SpecialProperty>>( specialProperties_, binary_data );
	return number_of_bytes;
}

double VoxelData::GetAbsorptionAtReferenceEnergy( const double absorption_at_energy, const double energy ){

	return absorption_at_energy * pow( ForceToMax( energy, change_energy_for_constant_mu ) / change_energy_for_constant_mu, 3. );
	
}

bool VoxelData::HasSpecificProperty( const SpecialProperty property ) const{

	SpecialPropertyEnumType propertyToCheck = ConvertToUnderlying( property );

	if( specialProperties_ & propertyToCheck ) return true;
	return false;

}


/*
	Voxel implementation
*/

Voxel::Voxel( const Point3D origin, const Tuple3D size, const VoxelData data ) :
	size_( size ),
	data_( data ),
	origin_corner_( origin ),
	faces{
				BoundedSurface{ origin_corner_.GetCoordinateSystem()->GetEy(), origin_corner_.GetCoordinateSystem()->GetEz(), origin_corner_ + origin_corner_.GetCoordinateSystem()->GetEx() * size_.x, 0, size_.y, 0, size_.z },
				BoundedSurface{ origin_corner_.GetCoordinateSystem()->GetEx(), origin_corner_.GetCoordinateSystem()->GetEz(), origin_corner_ + origin_corner_.GetCoordinateSystem()->GetEy() * size_.y, 0, size_.x, 0, size_.z },
				BoundedSurface{ origin_corner_.GetCoordinateSystem()->GetEx(), origin_corner_.GetCoordinateSystem()->GetEy(), origin_corner_ + origin_corner_.GetCoordinateSystem()->GetEz() * size_.z, 0, size_.x, 0, size_.y },
				BoundedSurface{ origin_corner_.GetCoordinateSystem()->GetEx(), origin_corner_.GetCoordinateSystem()->GetEy(), origin_corner_, 0, size_.x, 0, size_.y },
				BoundedSurface{ origin_corner_.GetCoordinateSystem()->GetEx(), origin_corner_.GetCoordinateSystem()->GetEz(), origin_corner_, 0, size_.x, 0, size_.z },
				BoundedSurface{ origin_corner_.GetCoordinateSystem()->GetEy(), origin_corner_.GetCoordinateSystem()->GetEz(), origin_corner_, 0, size_.y, 0, size_.z }
}
{
	if( size_.x <= 0 || size_.y <= 0 || size_.z <= 0 ) CheckForAndOutputError( MathError::Input, "size of voxel in each dimension must be greater than zero!" );
};


string Voxel::ConvertToString( unsigned int newline_tabulators ) const{
	string new_string;
	string new_line = { '\n' };

	for( unsigned int i = 0; i < newline_tabulators; i++ ) new_line += '\t';


	char tempory_character_array[ 256 ];
	snprintf( tempory_character_array, 256, "(%.6f,%.6f,%.6f)", size_.x, size_.y, size_.z );

	new_string += "o=" + new_line + origin_corner_.ConvertToString( newline_tabulators + 1 );
	new_string += "size=" + string( tempory_character_array );
	new_string += new_line + "data=" + std::to_string( data_.GetAbsorptionAtReferenceEnergy() );
	new_string += new_line + "face[0]=" + new_line + faces[ 0 ].ConvertToString( newline_tabulators + 1 );
	new_string += new_line + "face[1]=" + new_line + faces[ 1 ].ConvertToString( newline_tabulators + 1 );
	new_string += new_line + "face[2]=" + new_line + faces[ 2 ].ConvertToString( newline_tabulators + 1 );
	new_string += new_line + "face[3]=" + new_line + faces[ 3 ].ConvertToString( newline_tabulators + 1 );
	new_string += new_line + "face[4]=" + new_line + faces[ 4 ].ConvertToString( newline_tabulators + 1 );
	new_string += new_line + "face[5]=" + new_line + faces[ 5 ].ConvertToString( newline_tabulators + 1 );

	return new_string;
}



bool Voxel::Contains( const Point3D point ) const{

	// create copy of point in voxel's coordinate system
	const Point3D converted_point { point.ConvertTo( this->origin_corner() ) };

	// check all components
	return	origin_corner_.X() <= converted_point.X() && converted_point.X() <= origin_corner_.X() + size_.x &&
			origin_corner_.Y() <= converted_point.Y() && converted_point.Y() <= origin_corner_.Y() + size_.y &&
			origin_corner_.Z() <= converted_point.Z() && converted_point.Z() <= origin_corner_.Z() + size_.z;

}