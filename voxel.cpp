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


const double voxData::referenceEnergy = 120000.;
const double voxData::freqAtRefE = referenceEnergy / h_Js;
const double voxData::freqAtRefE_3 = pow( freqAtRefE, 3 );

voxData::voxData( const double attenuationAtFrequency, const double frequency ) :
	attenuation( attenuationAtRefE( attenuationAtFrequency, frequency ) )
{}

voxData::voxData( const vector<char>& binData, vector<char>::const_iterator& it ){
	deSerializeBuildIn( attenuation, (double) -1, binData, it );
}

voxData::voxData( void ) : 
	attenuation( -1 )
{}

double voxData::attenuationAt( const double frequency ) const{

	//const double constant = attenuation * freqAtRefE_3;

	return attenuation * pow( freqAtRefE / frequency, 3. );
};

size_t voxData::serialize( vector<char>& binData ) const{
	size_t numBytes = 0;
	numBytes += serializeBuildIn( attenuation, binData );
	return numBytes;
};

double voxData::attenuationAtRefE( const double attenuationAtFrequency, const double frequency ) const{

	return attenuationAtFrequency * pow( frequency / freqAtRefE, 3. );
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