/*********************************************************************
 * @file   plotting.cpp
 * @brief  Implementations
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/


 /*********************************************************************
	 Includes
  *********************************************************************/

#include "plotting.h"
#include "gantry.h"
#include "radonTransform.h"




  /*********************************************************************
	 Implementations
  *********************************************************************/

ofstream openAxis( path file_name, const bool reset ){
	ofstream outFile;


	if( reset ) outFile.open( file_name, std::ios::out | std::ios::trunc );
	else outFile.open( file_name, std::ios::out | std::ios::app );

	return outFile;
}

void closeAxis( ofstream& axis ){
	axis.close();
}

template<>
string getObjectString<Point3D>( const Point3D p ){

	char tempCharArr[ 256 ];
	snprintf( tempCharArr, 256, "pnt (%.12f,%.12f,%.12f)", p.GetGlobalX(), p.GetGlobalY(), p.GetGlobalZ() );

	return string{ tempCharArr };

}

template<>
string getObjectString<Vector3D, Point3D>( const Vector3D v, const Point3D o ){

	char tempCharArr[256];
	snprintf( tempCharArr, 256, "vec (%.12f,%.12f,%.12f;%.12f,%.12f,%.12f)", v.GetGlobalX(), v.GetGlobalY(), v.GetGlobalZ(), o.GetGlobalX(), o.GetGlobalY(), o.GetGlobalZ() );

	return string{ tempCharArr };

}

template<>
string getObjectString<UnitVector3D, Point3D>( const UnitVector3D v, const Point3D o ){

	return getObjectString<Vector3D, Point3D>( Vector3D{ v }, o );

}

template<>
string getObjectString<Line, double>( const Line l, const double length ){

	char tempCharArr[ 256 ];
	snprintf( tempCharArr, 256, "lin (%.12f,%.12f,%.12f;%.12f,%.12f,%.12f;%.12f)", l.origin().GetGlobalX(), l.origin().GetGlobalY(), l.origin().GetGlobalZ(), l.direction().GetGlobalX(), l.direction().GetGlobalY(), l.direction().GetGlobalZ(), length );

	return string{ tempCharArr };

}

template<>
string getObjectString<Ray, double>(const Ray r, const double length) {

	char tempCharArr[256];
	snprintf(tempCharArr, 256, "lin (%.12f,%.12f,%.12f;%.12f,%.12f,%.12f;%.12f)", r.origin().GetGlobalX(), r.origin().GetGlobalY(), r.origin().GetGlobalZ(), r.direction().GetGlobalX(), r.direction().GetGlobalY(), r.direction().GetGlobalZ(), length);

	return string{ tempCharArr };

}

template<>
string getObjectString<BoundedSurface, double>(const BoundedSurface s, const double alpha) {

	char tempCharArr[256];
	snprintf(tempCharArr, 256, "sLi (%.12f,%.12f,%.12f;%.12f,%.12f,%.12f;%.12f,%.12f,%.12f;%.12f,%.12f,%.12f,%.12f;%.12f)", s.origin().GetGlobalX(), s.origin().GetGlobalY(), s.origin().GetGlobalZ(), s.direction_1().GetGlobalX(), s.direction_1().GetGlobalY(), s.direction_1().GetGlobalZ(), s.direction_2().GetGlobalX(), s.direction_2().GetGlobalY(), s.direction_2().GetGlobalZ(), s.parameter_1_min(), s.parameter_1_max(), s.parameter_2_min(), s.parameter_2_max(), alpha );

	return string{ tempCharArr };

}

template<>
string getObjectString<vector<Tuple2D>>( const vector<Tuple2D> data ){

	string str = "plot ";

	for( auto valIt = data.cbegin(); valIt < data.cend(); valIt++ ){
		str += to_string( valIt->x );

		if( valIt < data.cend() - 1 ) str += ',';
	}

	str += ";";

	for( auto valIt = data.cbegin(); valIt < data.cend(); valIt++ ){
		str += to_string( valIt->y );

		if( valIt < data.cend() - 1 ) str += ',';
	}

	
	return str;

}

template<>
string getObjectString<vector<radonPoint>>( const vector<radonPoint> data ){

	string str = "plotS ";

	vector<double> XValues, YValues, DataValues;

	for( const radonPoint& p : data ){

		XValues.push_back( p.coordinates.theta );
		YValues.push_back( p.coordinates.distance );
		DataValues.push_back( p.value );

		
	}

	for( auto xVal : XValues ) str += to_string( xVal ) + ',';
	str.back() = ';';

	for( auto yVal : YValues ) str += to_string( yVal ) + ',';
	str.back() = ';';

	for( auto dataVal : DataValues ) str += to_string( dataVal ) + ',';
	str.pop_back();

	return str;

}

template<>
string getObjectString<grid<>>( const grid<> data, const bool image ){

	if( image ){

		string str = "image ";
		str += to_string( data.Size().c ) + "," + to_string( data.Start().c ) + "," + to_string( data.Resolution().c ) + ";";
		str += to_string( data.Size().r ) + "," + to_string( data.Start().r ) + "," + to_string( data.Resolution().r ) + ";";

		for( size_t row = 0; row < data.Size().r; row++ ){
			for( size_t column = 0; column < data.Size().c; column++ ){
				str += to_string( data( GridIndex{ column, row } ) ) + ",";
			}
		}
		str.pop_back();
		return str;
	}
	else{
		string str = "plotS ";

		vector<double> XValues, YValues, DataValues;

		for( size_t row = 0; row < data.Size().r; row++ ){
			for( size_t column = 0; column < data.Size().c; column++ ){

				GridCoordinates coordinates = data.getCoordinates( GridIndex{ column, row });

				XValues.push_back( coordinates.c );
				YValues.push_back( coordinates.r );
				DataValues.push_back( data( GridIndex{ column, row } ) );

			}
		}

		for( auto xVal : XValues ) str += to_string( xVal ) + ',';
		str.back() = ';';

		for( auto yVal : YValues ) str += to_string( yVal ) + ',';
		str.back() = ';';

		for( auto dataVal : DataValues ) str += to_string( dataVal ) + ',';
		str.pop_back();

		return str;
	}
}

template<>
void addObject<vector<Line>, double>( ofstream& axis, const string name, const vector<Line> lines, const string parameter, const double length ){

	for( const Line l : lines ){
		addSingleObject( axis, name, l, parameter, length );
	}

}

template<>
void addObject<vector<Ray>, double>(ofstream& axis, const string name, const vector<Ray> rays, const string parameter, const double length) {

	for (const Ray r : rays) {
		addSingleObject(axis, name, Line{ r }, parameter, length);
	}

}

template<>
void addObject<vector<pixel>, double>( ofstream& axis, const string name, const vector<pixel> allPixels, const string parameter, const double alpha ){
	for( const pixel singlePx : allPixels ){
		addSingleObject( axis, name, BoundedSurface{ singlePx }, parameter, alpha );
	}
}

template<>
void addObject<vector<BoundedSurface>, double>( ofstream& axis, const string name, const vector<BoundedSurface> surfaces, const string parameter, const double alpha ){
	for( const BoundedSurface surface : surfaces ){
		addSingleObject( axis, name, surface, parameter, alpha );
	}
}

template<>
void addObject<gantry, int>( ofstream& axis, const string name, const gantry gantry, const string parameter, const int specifiers ){

	if( specifiers & GANTRY_SPECIFIERS::ORIGIN )
		addSingleObject( axis, name + "Center", gantry.Center(), parameter );

	if( specifiers & GANTRY_SPECIFIERS::BEAMS )
		addObject( axis, name + "Beams", gantry.getBeam( 1. ), parameter, 2.*gantry.Radius() );
	
	if( specifiers & GANTRY_SPECIFIERS::DETECTOR_SURFACES )
		addObject( axis, name + "DetectorSurfaces", gantry.getPixel(), parameter, .2 );

	if( specifiers & GANTRY_SPECIFIERS::DETECTOR_NORMALS ){
		vector<Line> pixelNormals;

		for( pixel currentPixel : gantry.getPixel() ){
			pixelNormals.push_back( currentPixel.NormalLine() );
		}

		addObject( axis, name + "DetectorNormals", pixelNormals, parameter, 2.1 * gantry.Radius() );
	}

}


template<>
void addObject<model, double>( std::ofstream& axis, std::string name, model mod, std::string parameter, double threshold ){
	vox modVox = mod.Vox();
	for( FACE_ID i = FACE_ID::BEGIN; i < FACE_ID::END; ++i ){
		addSingleObject( axis, "modelFace" + to_string( ToUnderlying( i ) ), modVox.getFace( i ), "b", 0.2 );
	}

	for( size_t iX = 0; iX < mod.NumVox().x; iX++ ){
		for( size_t iY = 0; iY < mod.NumVox().y; iY++ ){
			for( size_t iZ = 0; iZ < mod.NumVox().z; iZ++ ){
				vox voxel = mod.getVoxel( Index3D{ iX, iY, iZ } );
				if( voxel.Data().attenuationAtRefE() >= (double) threshold ){
					addSingleObject( axis, "voxel(" + to_string( iX ) + "," + to_string( iY ) + "," + to_string( iZ ) + ")", voxel.getCenter(), parameter );
				}
			}
		}
	}
}