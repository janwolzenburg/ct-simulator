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
 #include "vec3D.h"
 #include "cSysTree.h"
 #include "voxel.h"



  /*********************************************************************
	Implementations
 *********************************************************************/


 /*
	voxData implementation
 */


//constexpr double voxData::referenceEnergy = 120000.;									/*!< Energy at which the attenuation coefficient is valid */
//constexpr double voxData::referenceEnergy_3 = pow( voxData::referenceEnergy, 3. );

voxData::voxData( const double attenuationAtFrequency, const double frequency, const specialProperty specProperty ) :
	attenuation( attenuationAtRefE( attenuationAtFrequency, frequency ) ),
	specialProperties( specProperty )
{}

voxData::voxData( const vector<char>& binData, vector<char>::const_iterator& it ) : 
	attenuation( deSerializeBuildIn<double>( 0., binData, it ) ),
	specialProperties( deSerializeBuildIn<specialEnumType>( specialProperty::UNDEFINED, binData, it ) )
{

}

voxData::voxData( void ) : 
	attenuation( -1 ),
	specialProperties( UNDEFINED )
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


};

size_t voxData::serialize( vector<char>& binData ) const{
	size_t numBytes = 0;
	numBytes += serializeBuildIn( attenuation, binData );
	numBytes += serializeBuildIn( specialProperties, binData );
	return numBytes;
};

double voxData::attenuationAtRefE( const double attenuationAtEnergy, const double energy ) const{


	if constexpr( referenceEnergy < energyPhotoFXChange_eV ){
		return attenuationAtEnergy * pow( energy / referenceEnergy, 3. );
	}
	else{
		return attenuationAtEnergy * pow( energyPhotoFXChange_eV / referenceEnergy, 3. );
	}

};

void voxData::addSpecialProperty( const specialProperty property ){

	specialProperties |= toUnderlying( property );

};

void voxData::removeSpecialProperty( const specialProperty property ){

	specialProperties &= ~toUnderlying( property );

};

bool voxData::hasSpecificProperty( const specialProperty property ) const{

	specialEnumType propertyToCheck = toUnderlying( property );

	if( specialProperties & propertyToCheck ) return true;
	return false;

};

bool voxData::hasSpecialProperty( void ) const{

	return specialProperties != NONE;

};

/*
	vox implementation
*/

vox::vox( const pnt3 o_, const v3 size_, const voxData data_ ) :
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

vox::vox( void )
	: vox( pnt3{ v3{ 0, 0, 0 }, DUMMY_CSYS() }, v3{1, 1, 1}, voxData{} ){};

vox::vox( const vox& v )
	: vox( v.o, v.size, v.data ){};

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

vox& vox::operator=( const vox& v ){
	o = v.o;
	size = v.size;
	data = v.data;

	memcpy( faces, v.faces, toUnderlying( FACE_ID::END ) * sizeof( surfLim ) );
	return *this;
};

surfLim vox::getFace( FACE_ID id_ ) const{
	return faces[ toUnderlying( id_ ) ];
}

pnt3 vox::getCenter( void ) const{
	return o + vec3{ v3{ size.x / 2, size.y / 2, size.z / 2 } , o.CSys() };
}

bool vox::contains( const pnt3 p ) const{

	// Create copy of point in voxel's coordinate system
	pnt3 pHere { p.convertTo( this->O() ) };

	// Check all components
	return	o.X() <= pHere.X() && pHere.X() <= o.X() + size.x &&
			o.Y() <= pHere.Y() && pHere.Y() <= o.Y() + size.y &&
			o.Z() <= pHere.Z() && pHere.Z() <= o.Z() + size.z;

}