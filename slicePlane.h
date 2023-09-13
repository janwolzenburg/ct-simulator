#pragma once

#include "cSysTree.h"
#include "surf.h"


class slicePlane{


	public:

	slicePlane( void );

	slicePlane( const vector<char>& binData, vector<char>::const_iterator& it );

	size_t serialize( vector<char>& binData ) const;

	void setSize( const double newSize ){
		size = newSize;
		updateSurface();
	}

	public:

	cartCSys* cSys;
	surfLim surface;
	double rotationAngleX;
	double rotationAngleY;
	double positionZ;
	double size;

	static const string FILE_PREAMBLE;

	private:

	void updateSurface( void ){
		surface = surfLim( surface, -size / 2, size / 2, -size / 2, size / 2 );
	}

};