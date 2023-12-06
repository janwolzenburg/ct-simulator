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
#include "projections.h"




  /*********************************************************************
	 Implementations
  *********************************************************************/

ofstream openAxis( path file_name, const bool reset ){
	ofstream outFile;

	if( file_name.empty() ) return outFile;

	if( file_name.extension() != ".axis" )
	file_name += ".axis";

	if( reset ) outFile.open( file_name, std::ios::out | std::ios::trunc );
	else outFile.open( file_name, std::ios::out | std::ios::app );

	return outFile;
}

void closeAxis( ofstream& axis ){
	axis.close();
}


string ReplaceSpace( const string text ){

	string new_text;

	for( char character : text ){
		if( character != ' ' ) new_text.push_back( character );
		else new_text.push_back( '_' );
	}

	return new_text;
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
string getObjectString<vector<Tuple2D>>( const vector<Tuple2D> data_ ){

	string str = "plot ";

	for( auto valIt = data_.cbegin(); valIt < data_.cend(); valIt++ ){
		str += to_string( valIt->x );

		if( valIt < data_.cend() - 1 ) str += ',';
	}

	str += ";";

	for( auto valIt = data_.cbegin(); valIt < data_.cend(); valIt++ ){
		str += to_string( valIt->y );

		if( valIt < data_.cend() - 1 ) str += ',';
	}

	
	return str;

}

template<>
string getObjectString<vector<RadonPoint>>( const vector<RadonPoint> data_ ){

	string str = "plotS ";

	vector<double> XValues, YValues, DataValues;

	for( const RadonPoint& p : data_ ){

		XValues.push_back( p.theta );
		YValues.push_back( p.distance );
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
string getObjectString<DataGrid<>>( const DataGrid<> data_, const bool image ){

	if( image ){

		string str = "image ";
		str += to_string( data_.size().c ) + "," + to_string( data_.start().c ) + "," + to_string( data_.resolution().c ) + ";";
		str += to_string( data_.size().r ) + "," + to_string( data_.start().r ) + "," + to_string( data_.resolution().r ) + ";";

		for( size_t row = 0; row < data_.size().r; row++ ){
			for( size_t column = 0; column < data_.size().c; column++ ){
				str += to_string( data_( GridIndex{ column, row } ) ) + ",";
			}
		}
		str.pop_back();
		return str;
	}
	else{
		string str = "plotS ";

		vector<double> XValues, YValues, DataValues;

		for( size_t row = 0; row < data_.size().r; row++ ){
			for( size_t column = 0; column < data_.size().c; column++ ){

				GridCoordinates coordinates = data_.GetCoordinates( GridIndex{ column, row });

				XValues.push_back( coordinates.c );
				YValues.push_back( coordinates.r );
				DataValues.push_back( data_( GridIndex{ column, row } ) );

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
void addObject<vector<Line>, double>( ofstream& axis, const string name, const vector<Line> lines, const string data, const double length ){

	for( const Line l : lines ){
		addSingleObject( axis, name, l, data, length );
	}

}

template<>
void addObject<vector<Ray>, double>(ofstream& axis, const string name, const vector<Ray> rays, const string data, const double length) {

	for (const Ray r : rays) {
		addSingleObject(axis, name, Line{ r }, data, length);
	}

}

template<>
void addObject<vector<DetectorPixel>, double>( ofstream& axis, const string name, const vector<DetectorPixel> allPixels, const string data, const double alpha ){
	int number = 0;
	for( const DetectorPixel singlePx : allPixels ){
		addSingleObject( axis, name+to_string(number++), BoundedSurface{singlePx}, data, alpha);
	}
}

template<>
void addObject<vector<BoundedSurface>, double>( ofstream& axis, const string name, const vector<BoundedSurface> surfaces, const string data, const double alpha ){
	for( const BoundedSurface surface : surfaces ){
		addSingleObject( axis, name, surface, data, alpha );
	}
}

template<>
void addObject<Gantry, int>( ofstream& axis, const string name, const Gantry gantry, const string data, const int specifiers ){

	if( specifiers & GANTRY_SPECIFIERS::ORIGIN )
		addSingleObject( axis, name + "Center", gantry.GetCenter(), data );

	if( specifiers & GANTRY_SPECIFIERS::BEAMS )
		addObject( axis, name + "Beams", gantry.tube().GetEmittedBeam( gantry.pixel_array(),  gantry.detector().properties().detector_focus_distance ), data, gantry.detector().properties().detector_focus_distance );
	
	if( specifiers & GANTRY_SPECIFIERS::DETECTOR_SURFACES )
		addObject( axis, name + "DetectorSurfaces", gantry.pixel_array(), data, .2 );

	if( specifiers & GANTRY_SPECIFIERS::DETECTOR_NORMALS ){
		vector<Line> pixelNormals;

		for( DetectorPixel currentPixel : gantry.pixel_array() ){
			pixelNormals.push_back( currentPixel.NormalLine() );
		}

		addObject( axis, name + "DetectorNormals", pixelNormals, data, 1.05 *  gantry.detector().properties().detector_focus_distance );
	}
	
	if( specifiers & GANTRY_SPECIFIERS::DETECTOR_CORNERS ){

		//addSingleObject( axis, "Corner", gantry.pixel_array().back().GetPoint(gantry.pixel_array().back().parameter_1_min(), gantry.pixel_array().back().parameter_2_max()), "b");

		for( DetectorPixel currentPixel : gantry.pixel_array() ){

			addSingleObject( axis, "Corner", currentPixel.GetPoint( currentPixel.parameter_1_max(), currentPixel.parameter_2_max() ), "b");
			addSingleObject( axis, "Corner", currentPixel.GetPoint( currentPixel.parameter_1_min(), currentPixel.parameter_2_max() ), "b");
		}
	
	}
}

template<>
void addObject<Model, double>( std::ofstream& axis, std::string name, Model mod, std::string voxel_data_, double threshold ){
	Voxel modVox = mod.GetModelVoxel();
	for( Voxel::Face i = Voxel::Face::Begin; i < Voxel::Face::End; ++i ){
		addSingleObject( axis, "modelFace" + to_string( ToUnderlying( i ) ), modVox.GetFace( i ), "b", 0.2 );
	}

	for( size_t iX = 0; iX < mod.number_of_voxel_3D().x; iX++ ){
		for( size_t iY = 0; iY < mod.number_of_voxel_3D().y; iY++ ){
			for( size_t iZ = 0; iZ < mod.number_of_voxel_3D().z; iZ++ ){
				Voxel voxel = mod.GetVoxel( Index3D{ iX, iY, iZ } );
				if( voxel.data().GetAbsorptionAtReferenceEnergy() >= static_cast<double>( threshold ) ){
					addSingleObject( axis, "voxel(" + to_string( iX ) + "," + to_string( iY ) + "," + to_string( iZ ) + ")", voxel.GetCenter(), voxel_data_ );
				}
			}
		}
	}
}