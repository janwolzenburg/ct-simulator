#pragma once

#include "cSysTree.h"
#include "surf.h"


class slicePlane{

	public:
	slicePlane( void ) :
		cSys( CSYS_TREE().addCSys( "Slice plane system" ) ),
		surface{ uvec3{ v3{ 1, 0, 0 }, cSys },
					uvec3{ v3{ 0, 1, 0 }, cSys },
					pnt3{  v3{0, 0, 0}, cSys },
					-300, 300, -300, 300 },
		rotationAngleX( 0. ),
		rotationAngleY( 0. ),
		positionZ( 0. ){

	};

	slicePlane( const vector<char>& binData, vector<char>::const_iterator& it ) :
		cSys{ CSYS_TREE().addCSys( binData, it ) },
		surface{ binData, it, cSys },
		rotationAngleX( deSerializeBuildIn<double>( (double) 0., binData, it ) ),
		rotationAngleY( deSerializeBuildIn<double>( (double) 0., binData, it ) ),
		positionZ( deSerializeBuildIn<double>( (double) 0., binData, it ) ){



	}


	size_t serialize( vector<char>& binData ) const{

		size_t numBytes = 0;
		numBytes += serializeBuildIn( FILE_PREAMBLE, binData );
		numBytes += cSys->serialize( binData );
		numBytes += surface.serialize( binData );
		numBytes += serializeBuildIn( rotationAngleX, binData );
		numBytes += serializeBuildIn( rotationAngleY, binData );
		numBytes += serializeBuildIn( positionZ, binData );

		return numBytes;

	};


	public:
	cartCSys* cSys;
	surfLim surface;
	double rotationAngleX;
	double rotationAngleY;
	double positionZ;

	static const string FILE_PREAMBLE;

};