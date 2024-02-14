#pragma once
/*********************************************************************
 * @file   voxel.cpp
 * @brief  Implementations
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
	
	for( auto& [matEnum, value] : VoxelData::special_property_names ){
		if( property_string == value )
			return matEnum;
	}

	return None;

}

VoxelData::VoxelData( const double absorption_at_energy, const double energy, const SpecialProperty specProperty ) :
	absorption_( -1. ),
	specialProperties_( specProperty )
{

	if( energy < change_energy_for_constant_mu )
		absorption_ = absorption_at_energy * pow( ( energy ) / change_energy_for_constant_mu, 3. );
	else
		absorption_ = absorption_at_energy;
}

VoxelData::VoxelData( const vector<char>& binary_data, vector<char>::const_iterator& it ) : 
	absorption_( DeSerializeBuildIn<double>( 0., binary_data, it ) ),
	specialProperties_( DeSerializeBuildIn<SpecialPropertyEnumType>( SpecialProperty::Undefined, binary_data, it ) )
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
	size_t num_bytes = 0;
	num_bytes += SerializeBuildIn<double>( absorption_, binary_data );
	num_bytes += SerializeBuildIn<typename std::underlying_type_t<VoxelData::SpecialProperty>>( specialProperties_, binary_data );
	return num_bytes;
}

double VoxelData::GetAbsorptionAtReferenceEnergy( const double absorptionAtEnergy, const double energy ){

	return absorptionAtEnergy * pow( ForceToMax( energy, change_energy_for_constant_mu ) / change_energy_for_constant_mu, 3. );
	
}

bool VoxelData::HasSpecificProperty( const SpecialProperty property ) const{

	SpecialPropertyEnumType propertyToCheck = ToUnderlying( property );

	if( specialProperties_ & propertyToCheck ) return true;
	return false;

}


/*
	Voxel implementation
*/

Voxel::Voxel( const Point3D o_, const Tuple3D size, const VoxelData data ) :
	size_( size ),
	data_( data ),
	origin_corner_( o_ ),
	faces{
				BoundedSurface{ origin_corner_.GetCoordinateSystem()->GetEy(), origin_corner_.GetCoordinateSystem()->GetEz(), origin_corner_ + origin_corner_.GetCoordinateSystem()->GetEx() * size_.x, 0, size_.y, 0, size_.z },
				BoundedSurface{ origin_corner_.GetCoordinateSystem()->GetEx(), origin_corner_.GetCoordinateSystem()->GetEz(), origin_corner_ + origin_corner_.GetCoordinateSystem()->GetEy() * size_.y, 0, size_.x, 0, size_.z },
				BoundedSurface{ origin_corner_.GetCoordinateSystem()->GetEx(), origin_corner_.GetCoordinateSystem()->GetEy(), origin_corner_ + origin_corner_.GetCoordinateSystem()->GetEz() * size_.z, 0, size_.x, 0, size_.y },
				BoundedSurface{ origin_corner_.GetCoordinateSystem()->GetEx(), origin_corner_.GetCoordinateSystem()->GetEy(), origin_corner_, 0, size_.x, 0, size_.y },
				BoundedSurface{ origin_corner_.GetCoordinateSystem()->GetEx(), origin_corner_.GetCoordinateSystem()->GetEz(), origin_corner_, 0, size_.x, 0, size_.z },
				BoundedSurface{ origin_corner_.GetCoordinateSystem()->GetEy(), origin_corner_.GetCoordinateSystem()->GetEz(), origin_corner_, 0, size_.y, 0, size_.z }
}
{
	if( size_.x <= 0 || size_.y <= 0 || size_.z <= 0 ) CheckForAndOutputError( MathError::Input, "Size of voxel in each dimension must be greater than zero!" );
};


string Voxel::ToString( unsigned int newline_tabulators ) const{
	string str;
	string newLine = { '\n' };

	for( unsigned int i = 0; i < newline_tabulators; i++ ) newLine += '\t';


	char tempCharArr[ 256 ];
	snprintf( tempCharArr, 256, "(%.6f,%.6f,%.6f)", size_.x, size_.y, size_.z );

	str += "o=" + newLine + origin_corner_.ToString( newline_tabulators + 1 );
	str += "size=" + string( tempCharArr );
	str += newLine + "data=" + std::to_string( data_.GetAbsorptionAtReferenceEnergy() );
	str += newLine + "face[0]=" + newLine + faces[ 0 ].ToString( newline_tabulators + 1 );
	str += newLine + "face[1]=" + newLine + faces[ 1 ].ToString( newline_tabulators + 1 );
	str += newLine + "face[2]=" + newLine + faces[ 2 ].ToString( newline_tabulators + 1 );
	str += newLine + "face[3]=" + newLine + faces[ 3 ].ToString( newline_tabulators + 1 );
	str += newLine + "face[4]=" + newLine + faces[ 4 ].ToString( newline_tabulators + 1 );
	str += newLine + "face[5]=" + newLine + faces[ 5 ].ToString( newline_tabulators + 1 );

	return str;
}



bool Voxel::Contains( const Point3D p ) const{

	// Create copy of point in voxel's coordinate system
	Point3D pHere { p.ConvertTo( this->origin_corner() ) };

	// Check all components
	return	origin_corner_.X() <= pHere.X() && pHere.X() <= origin_corner_.X() + size_.x &&
			origin_corner_.Y() <= pHere.Y() && pHere.Y() <= origin_corner_.Y() + size_.y &&
			origin_corner_.Z() <= pHere.Z() && pHere.Z() <= origin_corner_.Z() + size_.z;

}