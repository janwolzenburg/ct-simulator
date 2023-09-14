#pragma once

#include "cSysTree.h"
#include "surf.h"


class slicePlane{


	public:

	slicePlane( void );

	slicePlane( const vector<char>& binData, vector<char>::const_iterator& it );

	size_t serialize( vector<char>& binData ) const;

	public:

	cartCSys* cSys;
	surf surface;
	double rotationAngleX;
	double rotationAngleY;
	double positionZ;

	static const string FILE_PREAMBLE;


};