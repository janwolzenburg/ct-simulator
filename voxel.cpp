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
	voxData implementation
 */

voxData::voxData( const double attenuationAtFrequency, const double frequency, const specialProperty specProperty ) :
	attenuation( attenuationAtRefE( attenuationAtFrequency, frequency ) ),
	specialProperties( specProperty )
{}

voxData::voxData( const vector<char>& binary_data, vector<char>::const_iterator& it ) : 
	attenuation( DeSerializeBuildIn<double>( 0., binary_data, it ) ),
	specialProperties( DeSerializeBuildIn<specialEnumType>( specialProperty::UNDEFINED, binary_data, it ) )
{}

double voxData::attenuationAt( const double energy ) const{

	// If Metal attenuation is approx. 10 1/mm at 100 keV
	if( hasSpecificProperty( METAL ) ){
		return 10 * pow( 100000. / energy, 3. );
	}

	// Attenuation coefficient in voxel is at energy smaller than cut energy
	if constexpr( referenceEnergy < energyPhotoFXChange_eV ){
		return attenuation * pow( referenceEnergy / energy, 3. );
	}
	
	return attenuation * pow( energyPhotoFXChange_eV / energy, 3. );


}

size_t voxData::Serialize( vector<char>& binary_data ) const{
	size_t num_bytes = 0;
	num_bytes += SerializeBuildIn( attenuation, binary_data );
	num_bytes += SerializeBuildIn( specialProperties, binary_data );
	return num_bytes;
}

double voxData::attenuationAtRefE( const double attenuationAtEnergy, const double energy ) const{


	if constexpr( referenceEnergy < energyPhotoFXChange_eV ){
		return attenuationAtEnergy * pow( energy / referenceEnergy, 3. );
	}
	else{
		return attenuationAtEnergy * pow( energyPhotoFXChange_eV / referenceEnergy, 3. );
	}

}

bool voxData::hasSpecificProperty( const specialProperty property ) const{

	specialEnumType propertyToCheck = ToUnderlying( property );

	if( specialProperties & propertyToCheck ) return true;
	return false;

}


/*
	vox implementation
*/

vox::vox( const pnt3 o_, const Tuple3D size_, const voxData data_ ) :
	size( size_ ),
	data( data_ ),
	o( o_ ),
	faces{
				surfLim{ o.CSys()->EyVec(), o.CSys()->EzVec(), o + o.CSys()->ExVec() * size.x, 0, size.y, 0, size.z },
				surfLim{ o.CSys()->ExVec(), o.CSys()->EzVec(), o + o.CSys()->EyVec() * size.y, 0, size.x, 0, size.z },
				surfLim{ o.CSys()->ExVec(), o.CSys()->EyVec(), o + o.CSys()->EzVec() * size.z, 0, size.x, 0, size.y },
				surfLim{ o.CSys()->ExVec(), o.CSys()->EyVec(), o, 0, size.x, 0, size.y },
				surfLim{ o.CSys()->ExVec(), o.CSys()->EzVec(), o, 0, size.x, 0, size.z },
				surfLim{ o.CSys()->EyVec(), o.CSys()->EzVec(), o, 0, size.y, 0, size.z }
}
{
	if( size.x <= 0 || size.y <= 0 || size.z <= 0 ) checkErr( MATH_ERR::INPUT, "Size of voxel in each dimension must be greater than zero!" );
};


string vox::toStr( unsigned int newLineTabulators ) const{
	string str;
	string newLine = { '\n' };

	for( unsigned int i = 0; i < newLineTabulators; i++ ) newLine += '\t';


	char tempCharArr[ 256 ];
	snprintf( tempCharArr, 256, "(%.6f,%.6f,%.6f)", size.x, size.y, size.z );

	str += "o=" + newLine + o.toStr( newLineTabulators + 1 );
	str += "size=" + string( tempCharArr );
	str += newLine + "data=" + std::to_string( data.attenuationAtRefE() );
	str += newLine + "face[0]=" + newLine + faces[ 0 ].toStr( newLineTabulators + 1 );
	str += newLine + "face[1]=" + newLine + faces[ 1 ].toStr( newLineTabulators + 1 );
	str += newLine + "face[2]=" + newLine + faces[ 2 ].toStr( newLineTabulators + 1 );
	str += newLine + "face[3]=" + newLine + faces[ 3 ].toStr( newLineTabulators + 1 );
	str += newLine + "face[4]=" + newLine + faces[ 4 ].toStr( newLineTabulators + 1 );
	str += newLine + "face[5]=" + newLine + faces[ 5 ].toStr( newLineTabulators + 1 );

	return str;
}



bool vox::contains( const pnt3 p ) const{

	// Create copy of point in voxel's coordinate system
	pnt3 pHere { p.convertTo( this->O() ) };

	// Check all components
	return	o.X() <= pHere.X() && pHere.X() <= o.X() + size.x &&
			o.Y() <= pHere.Y() && pHere.Y() <= o.Y() + size.y &&
			o.Z() <= pHere.Z() && pHere.Z() <= o.Z() + size.z;

}