#pragma once

#include "cSysTree.h"
#include "surf.h"


class slicePlane{


	public:

	slicePlane( void );

	slicePlane( const vector<char>& binData, vector<char>::const_iterator& it );

	size_t serialize( vector<char>& binData ) const;

	void changeLimits( const range a, const range b ){
		surface = surfLim( surface, a.start, a.end, b.start, b.end );
	}

	public:

	cartCSys* cSys;
	surfLim surface;
	double rotationAngleX;
	double rotationAngleY;
	double positionZ;

	static const string FILE_PREAMBLE;

};