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

ofstream openAxis( path fileName, const bool reset ){
	ofstream outFile;


	if( reset ) outFile.open( fileName, std::ios::out | std::ios::trunc );
	else outFile.open( fileName, std::ios::out | std::ios::app );

	return outFile;
}

void closeAxis( ofstream& axis ){
	axis.close();
}

template<>
string getObjectString<pnt3>( const pnt3 p ){

	char tempCharArr[ 256 ];
	snprintf( tempCharArr, 256, "pnt (%.12f,%.12f,%.12f)", p.gX(), p.gY(), p.gZ() );

	return string{ tempCharArr };

}

template<>
string getObjectString<ray, double>(const ray r, const double length) {

	char tempCharArr[256];
	snprintf(tempCharArr, 256, "lin (%.12f,%.12f,%.12f;%.12f,%.12f,%.12f;%.12f)", r.O().gX(), r.O().gY(), r.O().gZ(), r.R().gX(), r.R().gY(), r.R().gZ(), length);

	return string{ tempCharArr };

}

template<>
string getObjectString<surfLim, double>(const surfLim s, const double alpha) {

	char tempCharArr[256];
	snprintf(tempCharArr, 256, "sLi (%.12f,%.12f,%.12f;%.12f,%.12f,%.12f;%.12f,%.12f,%.12f;%.12f,%.12f,%.12f,%.12f;%.12f)", s.O().gX(), s.O().gY(), s.O().gZ(), s.R1().gX(), s.R1().gY(), s.R1().gZ(), s.R2().gX(), s.R2().gY(), s.R2().gZ(), s.AMin(), s.AMax(), s.BMin(), s.BMax(), alpha );

	return string{ tempCharArr };

}

template<>
string getObjectString<vector<v2>>( const vector<v2> data ){

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
string getObjectString<grid>( const grid data, const bool image ){

	if( image ){

		string str = "image ";
		str += to_string( data.Size().c ) + "," + to_string( data.Start().c ) + "," + to_string( data.Resolution().c ) + ";";
		str += to_string( data.Size().r ) + "," + to_string( data.Start().r ) + "," + to_string( data.Resolution().r ) + ";";

		for( size_t row = 0; row < data.Size().r; row++ ){
			for( size_t column = 0; column < data.Size().c; column++ ){
				str += to_string( data( idx2RC{ column, row } ) ) + ",";
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

				v2RC coordinates = data.getCoordinates( idx2RC{ column, row });

				XValues.push_back( coordinates.c );
				YValues.push_back( coordinates.r );
				DataValues.push_back( data( idx2RC{ column, row } ) );

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
void addObject<vector<ray>, double>(ofstream& axis, const string name, const vector<ray> rays, const string parameter, const double length) {

	for (const ray r : rays) {
		addSingleObject(axis, name, r, parameter, length);
	}

}

template<>
void addObject<vector<pixel>, double>( ofstream& axis, const string name, const vector<pixel> allPixels, const string parameter, const double alpha ){
	for( const pixel singlePx : allPixels ){
		addSingleObject( axis, name, surfLim{ singlePx }, parameter, alpha );
	}
}

template<>
void addObject<gantry, int>( ofstream& axis, const string name, const gantry gantry, const string parameter, const int specifiers ){

	addObject( axis, name + "Beams", gantry.getBeam(), parameter, 2.*gantry.Radius() );
	addObject( axis, name + "Detector", gantry.getPixel(), parameter, .2 );
	addSingleObject( axis, name + "Center", gantry.Center(), parameter );

}


template<>
void addObject<model, double>( std::ofstream& axis, std::string name, model mod, std::string parameter, double threshold ){
	vox modVox = mod.Vox();
	for( FACE_ID i = FACE_ID::BEGIN; i < FACE_ID::END; ++i ){
		addSingleObject( axis, "modelFace" + to_string( toUnderlying( i ) ), modVox.getFace( i ), "b", 0.2 );
	}

	for( size_t iX = 0; iX < mod.NumVox().x; iX++ ){
		for( size_t iY = 0; iY < mod.NumVox().y; iY++ ){
			for( size_t iZ = 0; iZ < mod.NumVox().z; iZ++ ){
				vox voxel = mod.getVoxel( idx3{ iX, iY, iZ } );
				if( voxel.Data().k >= (double) threshold ){
					addSingleObject( axis, "voxel(" + to_string( iX ) + "," + to_string( iY ) + "," + to_string( iZ ) + ")", voxel.getCenter(), parameter );
				}
			}
		}
	}
}