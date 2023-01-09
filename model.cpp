
/*********************************************************************
 * @file   model.cpp
 * @brief  Implementations
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/


  /*********************************************************************
	Includes
 *********************************************************************/
#include <fstream>
#include "model.h"
#include "cSysTree.h"
#include "rays.h"
#include "intersections.h"



  /*********************************************************************
	Implementations
 *********************************************************************/


/*
	model implementation
*/

model::model( cartCSys* const cSys_, const idx3 numVox3D_, const v3 voxSize3D_ )
	: numVox3D( numVox3D_ ),
	voxSize3D( voxSize3D_ ),
	size3D( { (double) numVox3D.x * voxSize3D.x,
			 (double) numVox3D.y * voxSize3D.y,
			 (double) numVox3D.z * voxSize3D.z } ),
	numVox( (size_t) numVox3D.x* numVox3D.y* numVox3D.z ),
	parameter( new voxData[ numVox ] ),
	importSuccess( false ),
	cSys( cSys_ ){
	if( cSys->isGlobal() ) checkErr( MATH_ERR::INPUT, "Model coordinate system must be child of global system!" );
};

model::model( const model& mod ) : model( mod.cSys, mod.numVox3D, mod.voxSize3D ){
	memcpy( parameter, mod.parameter, numVox * sizeof( voxData ) );		// Copy data
};

model::model( const path file ) : model(){
	// File handle
	std::ifstream inFile;
	inFile.open( file, std::ios::binary );
	if( inFile.fail() ) return;

	inFile.read( (char*) &size3D.x, sizeof( double ) );
	inFile.read( (char*) &size3D.y, sizeof( double ) );
	inFile.read( (char*) &size3D.z, sizeof( double ) );

	inFile.read( (char*) &voxSize3D.x, sizeof( double ) );
	inFile.read( (char*) &voxSize3D.y, sizeof( double ) );
	inFile.read( (char*) &voxSize3D.z, sizeof( double ) );

	inFile.read( (char*) &numVox3D.x, sizeof( size_t ) );
	inFile.read( (char*) &numVox3D.y, sizeof( size_t ) );
	inFile.read( (char*) &numVox3D.z, sizeof( size_t ) );

	if( inFile.eof() ){
		inFile.close();
		return;
	}
	numVox = (size_t) numVox3D.x * numVox3D.y * numVox3D.z;

	delete[] parameter;
	parameter = new voxData[ numVox ];

	// Read voxel data
	inFile.read( (char*) parameter, (std::streamsize) numVox * sizeof( voxData ) );
	if( inFile.eof() ){
		inFile.close();
		return;
	}

	inFile.close();

	importSuccess = true;
}

model::model( void ) : model( DUMMY_CSYS(), idx3{ 1, 1, 1 }, v3{ 1, 1, 1 } ){}

model::~model(){
	delete[] parameter;
};

std::string model::toStr( [[maybe_unused]] const unsigned int newLineTabulators ) const{
	return std::string( "" );
};

model& model::operator=( const model& mod ){
	cSys = mod.cSys;
	numVox3D = mod.numVox3D;
	voxSize3D = mod.voxSize3D;

	size3D = mod.size3D;
	numVox = mod.numVox;

	delete parameter;
	parameter = new voxData[ numVox ];
	memcpy( parameter, mod.parameter, numVox * sizeof( voxData ) );		// Copy data

	return *this;
};

voxData& model::operator() ( const size_t x, const size_t y, const size_t z ){
	if( x >= numVox3D.x ){ checkErr( MATH_ERR::INPUT, "x index exceeds model size!" ); return parameter[ ( (size_t) numVox3D.x * numVox3D.y * ( numVox3D.z - 1 ) ) + (size_t) numVox3D.x * ( numVox3D.y - 1 ) + ( numVox3D.x - 1 ) ]; };
	if( y >= numVox3D.y ){ checkErr( MATH_ERR::INPUT, "y index exceeds model size!" ); return parameter[ ( (size_t) numVox3D.x * numVox3D.y * ( numVox3D.z - 1 ) ) + (size_t) numVox3D.x * ( numVox3D.y - 1 ) + ( numVox3D.x - 1 ) ]; };
	if( z >= numVox3D.z ){ checkErr( MATH_ERR::INPUT, "z index exceeds model size!" ); return parameter[ ( (size_t) numVox3D.x * numVox3D.y * ( numVox3D.z - 1 ) ) + (size_t) numVox3D.x * ( numVox3D.y - 1 ) + ( numVox3D.x - 1 ) ]; };

	return parameter[ ( (size_t) numVox3D.x * numVox3D.y * z ) + (size_t) numVox3D.x * y + x ];
};

voxData model::operator() ( const size_t x, const size_t y, const size_t z ) const{
	if( x >= numVox3D.x ){ checkErr( MATH_ERR::INPUT, "x index exceeds model size!" ); return parameter[ ( (size_t) numVox3D.x * numVox3D.y * ( numVox3D.z - 1 ) ) + (size_t) numVox3D.x * ( numVox3D.y - 1 ) + ( numVox3D.x - 1 ) ]; };
	if( y >= numVox3D.y ){ checkErr( MATH_ERR::INPUT, "y index exceeds model size!" ); return parameter[ ( (size_t) numVox3D.x * numVox3D.y * ( numVox3D.z - 1 ) ) + (size_t) numVox3D.x * ( numVox3D.y - 1 ) + ( numVox3D.x - 1 ) ]; };
	if( z >= numVox3D.z ){ checkErr( MATH_ERR::INPUT, "z index exceeds model size!" ); return parameter[ ( (size_t) numVox3D.x * numVox3D.y * ( numVox3D.z - 1 ) ) + (size_t) numVox3D.x * ( numVox3D.y - 1 ) + ( numVox3D.x - 1 ) ]; };

	return parameter[ ( (size_t) numVox3D.x * numVox3D.y * z ) + (size_t) numVox3D.x * y + x ];
};

voxData& model::operator() ( const idx3 indices ){
	return ( *this )( indices.x, indices.y, indices.z );
}

voxData model::operator() ( const idx3 indices ) const{
	return ( *this )( indices.x, indices.y, indices.z );
}

vox model::Vox( void ) const{
	return  vox{ pnt3{v3 { 0, 0, 0 }, cSys}, size3D, voxData{} };
}

bool model::checkIndices( const idx3 indices ) const{
	if( indices.x >= numVox3D.x ||
		indices.y >= numVox3D.y ||
		indices.z >= numVox3D.z ){
		return false;
	}

	return true;
}

bool model::validCoords( const v3 voxCoords ) const{
	return voxCoords.x >= 0 && voxCoords.y >= 0 && voxCoords.z >= 0 &&
		voxCoords.x < size3D.x&& voxCoords.y < size3D.y&& voxCoords.z < size3D.z;
}

idx3 model::getVoxelIndices( const pnt3 voxpnt ) const{
	return getVoxelIndices( voxpnt.XYZ( cSys ) );
}

vox model::getVoxel( const idx3 indices ) const{
	if( indices.x >= numVox3D.x || indices.y >= numVox3D.y || indices.z >= numVox3D.z ){
		checkErr( MATH_ERR::INPUT, "Indices exceed model!" );
		return vox();
	}

	pnt3 voxOrigin{ v3{ (double) indices.x * voxSize3D.x, (double) indices.y * voxSize3D.y, (double) indices.z * voxSize3D.z }, cSys };

	// Get voxel data and create voxel instance
	vox voxel( voxOrigin, voxSize3D, ( *this )( indices ) );

	return voxel;
}

bool model::pntInside( const pnt3 p ) const{
	return validCoords( p.XYZ( cSys ) );
}

vector<ray> model::rayTransmission( const ray tRay, const bool enableScattering ) const{
	vector<ray> rays(0);									// Vector of rays after/during transmission to be processed further

	// Find entrance in model
	rayVoxelIntersection modelIsect{ Vox(), tRay };

	rayVox_Intersection_Result rayEntrance = modelIsect.Entrance();
	if( !rayEntrance.hasSolution ) return vector<ray>(0);			// Return if ray does not intersect model


	// Iteration through model
	/* ---------------------------------------------------------------------------------------------------- */

	ray currentRay = tRay;											// Current ray in model's coordinate system
	double currentRayStep = rayEntrance.linPara + rayStepSize;		// Ray parameter at model entrance
	pnt3 currentPntOnRay = currentRay.getPnt( currentRayStep );		// Point of model entrance

	rayVox_Intersection_Result rayExit;						// Exit of ray
	idx3 currentVoxIndices{ 0, 0, 0 };						// Indices of current voxel
	vox currentVox;											// Current voxel

	double distance;										// Distance of ray inside voxel


	// Iterate through model while current point is inside model
	while( pntInside( currentPntOnRay ) ){
		// Get current voxel
		currentVoxIndices = getVoxelIndices( currentPntOnRay );
		currentVox = getVoxel( currentVoxIndices );

		// Find exit

		rayExit = rayVoxelIntersection{ currentVox, currentRay }.Exit();

		if( !rayExit.hasSolution ){
			checkErr( MATH_ERR::OPERATION, "No exit out of current voxel found!" );
			return vector<ray>(0);
		}

		// Get ray parameter of voxel exit
		currentRayStep = rayExit.linPara;

		// Calculate distance between entrance and exit
		distance = ( rayEntrance.isectPnt - rayExit.isectPnt ).Length();

		// Update ray properties
		currentRay.updateProperties( currentVox.Data(), distance );
	
		// Exit of this voxel is entrance of next voxel
		rayEntrance = rayExit;

		// "Enter" next voxel
		currentRayStep += rayStepSize;
		currentPntOnRay = currentRay.getPnt( currentRayStep );

		//TODO: Scattering on voxel surfaces
		if( enableScattering ){
			// Do scattering
		}


	}

	// New origin "outside" the model to return
	currentRay.O( currentPntOnRay );
	rays.push_back( currentRay );

	return rays;
}

bool model::exportToFile( const path file ) const{
	// File handle
	std::ofstream outFile;
	outFile.open( file, std::ios::trunc | std::ios::binary );
	if( outFile.fail() ) return false;

	//cout << endl << "Writing model file..." << endl;

	outFile.write( (char*) &size3D.x, sizeof( double ) );
	outFile.write( (char*) &size3D.y, sizeof( double ) );
	outFile.write( (char*) &size3D.z, sizeof( double ) );

	outFile.write( (char*) &voxSize3D.x, sizeof( double ) );
	outFile.write( (char*) &voxSize3D.y, sizeof( double ) );
	outFile.write( (char*) &voxSize3D.z, sizeof( double ) );

	outFile.write( (char*) &numVox3D.x, sizeof( size_t ) );
	outFile.write( (char*) &numVox3D.y, sizeof( size_t ) );
	outFile.write( (char*) &numVox3D.z, sizeof( size_t ) );


	// Write parameter data
	outFile.write( (char*) parameter, (std::streamsize) numVox * sizeof( voxData ) );
	if( outFile.fail() ) return false;


	outFile.close();

	return true;
}

bool model::crop( const v3 minCoords, const v3 maxCoords ){
	if( !validCoords( minCoords ) || !validCoords( maxCoords ) ) return false;

	idx3 minIdcs = getVoxelIndices( minCoords );			// Minimum boundary indices
	idx3 maxIdcs = getVoxelIndices( maxCoords );			// Maximum boundary indices

	// Parameters of cropped model
	idx3 newVoxNum3D{ maxIdcs.x - minIdcs.x + 1, maxIdcs.y - minIdcs.y + 1, maxIdcs.z - minIdcs.z + 1 };

	// New model
	model newModel( cSys, newVoxNum3D, voxSize3D );


	// Copy data to new model
	for( size_t z = 0; z < newVoxNum3D.z; z++ ){
		for( size_t y = 0; y < newVoxNum3D.y; y++ ){
			for( size_t x = 0; x < newVoxNum3D.x; x++ ){
				newModel( x, y, z ) = ( *this )( x + minIdcs.x, y + minIdcs.y, z + minIdcs.z );
			}
		}
	}

	*this = newModel;

	return true;
}

idx3 model::getVoxelIndices( const v3 locCoords ) const{
	if( locCoords.x < 0 || locCoords.y < 0 || locCoords.z < 0 ){
		checkErr( MATH_ERR::INPUT, "Only positive coordinates allowed in model!" );
		return idx3{0, 0, 0};
	}

	idx3 indices{
		(size_t) ( locCoords.x / voxSize3D.x ),
		(size_t) ( locCoords.y / voxSize3D.y ),
		(size_t) ( locCoords.z / voxSize3D.z )
	};

	if( !checkIndices( indices ) ) checkErr( MATH_ERR::INPUT, "Coordinates exceed model size!" );

	if( indices.x >= numVox3D.x ) indices.x = numVox3D.x - 1;
	if( indices.y >= numVox3D.y ) indices.y = numVox3D.y - 1;
	if( indices.z >= numVox3D.z ) indices.z = numVox3D.z - 1;

	return indices;
}
