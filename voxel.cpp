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

VoxelData::VoxelData( const double attenuationAtFrequency, const double frequency, const SpecialProperty specProperty ) :
	attenuation_( GetAttenuationAtReferenceEnergy( attenuationAtFrequency, frequency ) ),
	specialProperties_( specProperty )
{}

VoxelData::VoxelData( const vector<char>& binary_data, vector<char>::const_iterator& it ) : 
	attenuation_( DeSerializeBuildIn<double>( 0., binary_data, it ) ),
	specialProperties_( DeSerializeBuildIn<SpecialPropertyEnumType>( SpecialProperty::Undefined, binary_data, it ) )
{}

double VoxelData::GetAttenuationAtEnergy( const double energy ) const{

	// If Plumbum attenuation is approx. 0.1134 1/mm at 100 keV
	if( HasSpecificProperty( Metal ) ){
		return 11.34 * pow( 100000. / energy, 3. );
	}

	// Attenuation coefficient in voxel is at energy smaller than cut energy
	if constexpr( reference_energy_eV_ < photoeffect_change_energy_eV ){
		return attenuation_ * pow( reference_energy_eV_ / energy, 3. );
	}
	
	return attenuation_ * pow( photoeffect_change_energy_eV / energy, 3. );


}

size_t VoxelData::Serialize( vector<char>& binary_data ) const{
	size_t num_bytes = 0;
	num_bytes += SerializeBuildIn( attenuation_, binary_data );
	num_bytes += SerializeBuildIn( specialProperties_, binary_data );
	return num_bytes;
}

double VoxelData::GetAttenuationAtReferenceEnergy( const double attenuationAtEnergy, const double energy ) const{


	if constexpr( reference_energy_eV_ < photoeffect_change_energy_eV ){
		return attenuationAtEnergy * pow( energy / reference_energy_eV_, 3. );
	}
	else{
		return attenuationAtEnergy * pow( photoeffect_change_energy_eV / reference_energy_eV_, 3. );
	}

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
	str += newLine + "data=" + std::to_string( data_.GetAttenuationAtReferenceEnergy() );
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