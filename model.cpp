
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
#include <thread>
#include <mutex>
using std::ref;
using std::cref;

#include "model.h"
#include "cSysTree.h"
#include "rays.h"
#include "intersections.h"
#include "propability.h"
#include "scattering.h"
#include "tomography.h"



  /*********************************************************************
	Implementations
 *********************************************************************/


/*
	model implementation
*/


const string model::FILE_PREAMBLE{ "CT_MODEL_FILE_PREAMBLE_Ver2"};

model::model( cartCSys* const cSys_, const idx3 numVox3D_, const v3 voxSize3D_, const string name_ ) :
	numVox3D( numVox3D_ ),
	voxSize3D( voxSize3D_ ),
	size3D( { (double) numVox3D.x * voxSize3D.x,
			 (double) numVox3D.y * voxSize3D.y,
			 (double) numVox3D.z * voxSize3D.z } ),
	numVox( numVox3D.x * numVox3D.y * numVox3D.z ),
	parameter( new voxData[ numVox ] ),
	cSys( cSys_ ),
	attenuationMin( -1. ),
	attenuationMax( -1. ),
	name( name_ )
{
	if( cSys->isGlobal() ) checkErr( MATH_ERR::INPUT, "Model coordinate system must be child of global system!" );
}

model::model( const model& mod ) : model( mod.cSys, mod.numVox3D, mod.voxSize3D, mod.name )
	
{
	attenuationMin = mod.attenuationMin;
	attenuationMax = mod.attenuationMax;
	
	memcpy( parameter, mod.parameter, numVox * sizeof( voxData ) );		// Copy data
}

model::model( const vector<char>& binData, vector<char>::const_iterator& it ) :
	numVox3D( idx3( binData, it ) ),
	voxSize3D( v3( binData, it ) ),
	size3D( v3( (double) numVox3D.x * voxSize3D.x,
			 (double) numVox3D.y * voxSize3D.y,
			 (double) numVox3D.z * voxSize3D.z ) ),
	numVox( numVox3D.x* numVox3D.y* numVox3D.z ),
	parameter( new voxData[numVox] ),
	cSys( CSYS_TREE().addCSys( binData, it ) ),
	attenuationMin( deSerializeBuildIn<double>( -1., binData, it ) ),
	attenuationMax( deSerializeBuildIn<double>( -1., binData, it ) ),
	name( deSerializeBuildIn( string{ "Default model name"}, binData, it ) )
{
	
	if( numVox * sizeof( voxData ) == (size_t) (binData.end() - it) ){
		memcpy( parameter, &(*it) , numVox * sizeof(voxData));
		it += numVox * sizeof( voxData );
	}
	else{

		for( size_t i = 0; i < numVox; i++ ){
			parameter[i] = voxData( binData, it );
		}
	}
}

model::~model(){
	delete[] parameter;
}

string model::toStr( [[maybe_unused]] const unsigned int newLineTabulators ) const{
	return string( "" );
}

model& model::operator=( const model& mod ){
	cSys = mod.cSys;
	numVox3D = mod.numVox3D;
	voxSize3D = mod.voxSize3D;

	size3D = mod.size3D;
	numVox = mod.numVox;

	attenuationMin = mod.attenuationMin;
	attenuationMax = mod.attenuationMax;
	name = mod.name;

	delete[] parameter;
	parameter = new voxData[ numVox ];
	memcpy( parameter, mod.parameter, numVox * sizeof( voxData ) );		// Copy data

	return *this;
}

model& model::operator=( model&& mod ) noexcept{
	
	if( this == &mod ) return *this;

	cSys = mod.cSys;
	numVox3D = mod.numVox3D;
	voxSize3D = mod.voxSize3D;

	size3D = mod.size3D;
	numVox = mod.numVox;

	attenuationMin = mod.attenuationMin;
	attenuationMax = mod.attenuationMax;
	name = mod.name;

	delete[] parameter;
	parameter = mod.parameter;
	
	mod.parameter = nullptr;
	mod.numVox = 0;

	return *this;
}

vox model::Vox( void ) const{
	return  vox{ pnt3{v3 { 0, 0, 0 }, cSys}, size3D, voxData{} };
}

const voxData& model::operator() ( const size_t x, const size_t y, const size_t z ) const{
	if( x >= numVox3D.x ){ checkErr( MATH_ERR::INPUT, "x index exceeds model size!" ); return parameter[( (size_t) numVox3D.x * numVox3D.y * ( numVox3D.z - 1 ) ) + (size_t) numVox3D.x * ( numVox3D.y - 1 ) + ( numVox3D.x - 1 )]; };
	if( y >= numVox3D.y ){ checkErr( MATH_ERR::INPUT, "y index exceeds model size!" ); return parameter[( (size_t) numVox3D.x * numVox3D.y * ( numVox3D.z - 1 ) ) + (size_t) numVox3D.x * ( numVox3D.y - 1 ) + ( numVox3D.x - 1 )]; };
	if( z >= numVox3D.z ){ checkErr( MATH_ERR::INPUT, "z index exceeds model size!" ); return parameter[( (size_t) numVox3D.x * numVox3D.y * ( numVox3D.z - 1 ) ) + (size_t) numVox3D.x * ( numVox3D.y - 1 ) + ( numVox3D.x - 1 )]; };

	return parameter[( (size_t) numVox3D.x * numVox3D.y * z ) + (size_t) numVox3D.x * y + x];
}

voxData& model::operator() ( const size_t x, const size_t y, const size_t z ){
	if( x >= numVox3D.x ){ checkErr( MATH_ERR::INPUT, "x index exceeds model size!" ); return parameter[ ( (size_t) numVox3D.x * numVox3D.y * ( numVox3D.z - 1 ) ) + (size_t) numVox3D.x * ( numVox3D.y - 1 ) + ( numVox3D.x - 1 ) ]; };
	if( y >= numVox3D.y ){ checkErr( MATH_ERR::INPUT, "y index exceeds model size!" ); return parameter[ ( (size_t) numVox3D.x * numVox3D.y * ( numVox3D.z - 1 ) ) + (size_t) numVox3D.x * ( numVox3D.y - 1 ) + ( numVox3D.x - 1 ) ]; };
	if( z >= numVox3D.z ){ checkErr( MATH_ERR::INPUT, "z index exceeds model size!" ); return parameter[ ( (size_t) numVox3D.x * numVox3D.y * ( numVox3D.z - 1 ) ) + (size_t) numVox3D.x * ( numVox3D.y - 1 ) + ( numVox3D.x - 1 ) ]; };

	return parameter[ ( (size_t) numVox3D.x * numVox3D.y * z ) + (size_t) numVox3D.x * y + x ];
}

bool model::checkIndices( const idx3 indices ) const{
	if( indices.x >= numVox3D.x ||
		indices.y >= numVox3D.y ||
		indices.z >= numVox3D.z ){
		return false;
	}
	return true;
}

bool model::pntInside( const pnt3 p ) const{
	return validCoords( p.XYZ( cSys ) );
}

idx3 model::getVoxelIndices( const v3 locCoords ) const{
	if( locCoords.x < 0 || locCoords.y < 0 || locCoords.z < 0 ){
		checkErr( MATH_ERR::INPUT, "Only positive coordinates allowed in model!" );
		return idx3{ 0, 0, 0 };
	}

	idx3 indices{
		(size_t) ( locCoords.x / voxSize3D.x ),
		(size_t) ( locCoords.y / voxSize3D.y ),
		(size_t) ( locCoords.z / voxSize3D.z )
	};

	// Supress checkErr when index is exactly on the edge
	if( indices.x == numVox3D.x ) indices.x = numVox3D.x - 1;
	if( indices.y == numVox3D.y ) indices.y = numVox3D.y - 1;
	if( indices.z == numVox3D.z ) indices.z = numVox3D.z - 1;

	if( !checkIndices( indices ) ){
		checkErr( MATH_ERR::INPUT, "Coordinates exceed model size!" );
	}

	if( indices.x >= numVox3D.x ) indices.x = numVox3D.x - 1;
	if( indices.y >= numVox3D.y ) indices.y = numVox3D.y - 1;
	if( indices.z >= numVox3D.z ) indices.z = numVox3D.z - 1;

	return indices;
}

idx3 model::getVoxelIndices( const pnt3 voxpnt ) const{
	return getVoxelIndices( voxpnt.XYZ( cSys ) );
}

bool model::setVoxelData( const voxData newData, const idx3 indices ){

	if( !checkIndices( indices ) ) return false;

	this->operator()( indices ) = newData;

	if( newData.attenuationAtRefE() < attenuationMin || attenuationMax < 0 ) attenuationMin = newData.attenuationAtRefE();
	if( newData.attenuationAtRefE() > attenuationMax || attenuationMax < 0 ) attenuationMax = newData.attenuationAtRefE();

	return true;
}

bool model::setVoxelProperty( const voxData::specialProperty property, const idx3 indices ){

	if( !checkIndices( indices ) ) return false;

	this->operator()( indices ).addSpecialProperty( property );

	return true;

}

vox model::getVoxel( const idx3 indices ) const{

	if( !checkIndices( indices ) ){
		checkErr( MATH_ERR::INPUT, "Indices exceed model!" );
		return vox{};
	}

	pnt3 voxOrigin{ v3{ (double) indices.x * voxSize3D.x, (double) indices.y * voxSize3D.y, (double) indices.z * voxSize3D.z }, cSys };

	// Get voxel data and create voxel instance
	vox voxel{ voxOrigin, voxSize3D, getVoxelData( indices ) };

	return voxel;
}

ray model::rayTransmission( const ray tRay, const tomographyParameter& tomoParameter, const rayScattering& scatteringProperties ) const{

	ray modelRay = tRay.convertTo( this->cSys );					// Current ray in model's coordinate system

	// Find entrance in model
	const rayVoxelIntersection modelIsect{ Vox(), modelRay };

	const rayVox_Intersection_Result rayEntrance = modelIsect.entrance;
	if( !rayEntrance.hasSolution ) return modelRay;			// Return if ray does not intersect model


	// Iteration through model
	/* ---------------------------------------------------------------------------------------------------- */

	double currentRayStep = rayEntrance.linePara;					// Ray parameter at model entrance
	const double lengthInModel = modelIsect.exit.linePara - modelIsect.entrance.linePara;

	// Get first point inside the model
	while( !pntInside( modelRay.getPnt( currentRayStep ) ) && lengthInModel > tomoParameter.rayStepSize ){
		currentRayStep += tomoParameter.rayStepSize;
	}

	// Current point on the ray
	pnt3 currentPntOnRay = modelRay.getPnt( currentRayStep );		// Point of model entrance


	const double meanFrequencyTube = modelRay.getMeanFrequency();	// Mean frequency of ray before it enters model
	const double meanVoxelSideLength = ( voxSize3D.x + voxSize3D.y + voxSize3D.z ) / 3.;
	const size_t meanVoxelAmount = (size_t) ( (double) ( numVox3D.x + numVox3D.y + numVox3D.z ) / 3. );

	const double scatterConstant = tomoParameter.scatterPropability * meanFrequencyTube / ( meanVoxelSideLength * meanVoxelAmount );


	// Iterate through model while current point is inside model
	while( pntInside( currentPntOnRay ) ){

		const idx3 currentVoxelIndices = getVoxelIndices( currentPntOnRay );		// Indices of current voxel

		const vector<FACE_ID> possibleFaces = modelRay.getPossibleVoxelExits();		// The possible exit faces

		double rayParameter = INFINITY;		// The smallest ray parameter

		// Iterate all possible faces and get the ray parameter at intersection
		for( const FACE_ID& currentFace : possibleFaces ){

			double currentRayParameter = INFINITY;			// Set to infinity 
			double planePosistion;							// Position of current face

			
			// Switch faces
			switch( currentFace ){

				case FACE_ID::YZ_Xp:
					planePosistion = ( currentVoxelIndices.x + 1 ) * this->voxSize3D.x;		// Position of positive yz-plane
					currentRayParameter = ( planePosistion - currentPntOnRay.X() ) / modelRay.R().X();		// Ray parameter at intersection
					break;

				case FACE_ID::YZ_Xm:
					planePosistion = ( currentVoxelIndices.x     ) * this->voxSize3D.x;		// Position of negative yz-plane
					currentRayParameter = ( planePosistion - currentPntOnRay.X() ) / modelRay.R().X();
					break;

				case FACE_ID::XZ_Yp:
					planePosistion = ( currentVoxelIndices.y + 1 ) * this->voxSize3D.y;		// Position of positive xz-plane
					currentRayParameter = ( planePosistion - currentPntOnRay.Y() ) / modelRay.R().Y();
					break;

				case FACE_ID::XZ_Ym:
					planePosistion = ( currentVoxelIndices.y     ) * this->voxSize3D.y;		// Position of negative xz-plane
					currentRayParameter = ( planePosistion - currentPntOnRay.Y() ) / modelRay.R().Y();
					break;

				case FACE_ID::XY_Zp:
					planePosistion = ( currentVoxelIndices.z + 1 ) * this->voxSize3D.z;		// Position of positive xy-plane
					currentRayParameter = ( planePosistion - currentPntOnRay.Z() ) / modelRay.R().Z();
					break;

				case FACE_ID::XY_Zm:
					planePosistion = ( currentVoxelIndices.z    ) * this->voxSize3D.z;		// Position of negative xy-plane
					currentRayParameter = ( planePosistion - currentPntOnRay.Z() ) / modelRay.R().Z();
					break;

				default: break;
			}

			// Set smallest ray parameter
			if( currentRayParameter < rayParameter ) rayParameter = currentRayParameter;

		}
		
		// Exit found
		if( rayParameter < INFINITY ){

			const double distance = rayParameter;		// The distance is the rayParameter

			// Update ray properties whith distance travelled in current voxel
			modelRay.updateProperties( this->getVoxelData( currentVoxelIndices ), distance );
			modelRay.incrementHitCounter();

			currentRayStep += distance + tomoParameter.rayStepSize;				// New Step on ray
			currentPntOnRay = modelRay.getPnt( currentRayStep );	// New point on ray
		
			// Scattering
			if( tomoParameter.scattering ){
			
				// Mean frequency of ray
				const double meanFrequency = modelRay.getMeanFrequency();

				// Calculate propability that the ray is scattered
				const double scatterPropability = distance / meanFrequency * scatterConstant;

				// Does the ray scatter?
				if( integerRNG.eventHappend( scatterPropability ) ){
					
					// Scatter the ray
					return scatteringProperties.scatterRay( modelRay, currentPntOnRay );
				}
			}
		}
	}

	// New origin "outside" the model to return
	modelRay.setOrigin( currentPntOnRay );

	return modelRay;
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


size_t model::serialize( vector<char>& binData ) const{

	size_t expectedSize = FILE_PREAMBLE.size() + 1;
	expectedSize += sizeof( numVox3D );
	expectedSize += sizeof( voxSize3D );
	expectedSize += sizeof( cartCSys );
	expectedSize += sizeof( attenuationMin );
	expectedSize += sizeof( attenuationMax );
	expectedSize += name.size() + 1;
	expectedSize += numVox * sizeof( *(parameter) );

	binData.reserve( expectedSize );

	size_t numBytes = 0;
	numBytes += serializeBuildIn( FILE_PREAMBLE, binData );
	numBytes += numVox3D.serialize( binData );
	numBytes += voxSize3D.serialize( binData );
	numBytes += cSys->serialize( binData );
	numBytes += serializeBuildIn( attenuationMin, binData );
	numBytes += serializeBuildIn( attenuationMax, binData );
	numBytes += serializeBuildIn( name, binData );

	binData.insert( binData.end(), (char*) parameter, (char*) parameter + sizeof( voxData ) * numVox );

	return numBytes;

}


void sliceThreadFunction(	size_t& xIdx, mutex& currentXMutex, size_t& yIdx, mutex& currentYMutex,
							v2CR& realStart, mutex& realStartMutex, v2CR& realEnd, mutex& realEndMutex,
							grid<voxData>& slice, mutex& sliceMutex,
							const surf& slicePlane,
							const model& modelRef ){

	// Iterate through all columns
	while( xIdx < slice.Size().col ){

		size_t localXIdx, localYIdx;

		// Lock
		currentXMutex.lock(); currentYMutex.lock();

		localXIdx = xIdx;
		localYIdx = yIdx;

		if( yIdx >= slice.Size().row ){
			xIdx++;
			yIdx = 0;
		}
		else{
			yIdx++;
		}

		//Unlock
		currentXMutex.unlock(); currentYMutex.unlock();


		// Check 
		if( localXIdx > slice.Size().col - 1 || localYIdx > slice.Size().row - 1 ) continue;
		
		// Grid indices
		const idx2CR gridIndices( localXIdx, localYIdx );

		// Get point on surface for current grid indices
		const v2CR surfaceCoordinate = slice.getCoordinates( gridIndices );
		const pnt3 currentPoint = slicePlane.getPnt( surfaceCoordinate.col, surfaceCoordinate.row );


		// Are cooradinates defined in model?
		if( !modelRef.pntInside( currentPoint ) ){
			slice.setData( gridIndices, voxData( 0., 1., voxData::UNDEFINED ) );
			continue;	// Goto next iteration
		}


		// Check if current x and y values in plane are new real start/end

		if( surfaceCoordinate.col < realStart.col ){
			writeThreadVar( realStart.col, surfaceCoordinate.col, realStartMutex );
		}
		
		if( surfaceCoordinate.col > realEnd.col ){
			writeThreadVar( realEnd.col, surfaceCoordinate.col, realEndMutex );
		}

		if( surfaceCoordinate.row < realStart.row ){
			writeThreadVar( realStart.row, surfaceCoordinate.row, realStartMutex );
		}

		if( surfaceCoordinate.row > realEnd.row ){
			writeThreadVar( realEnd.row, surfaceCoordinate.row, realEndMutex );
		}

		// Current voxel value
		const voxData currentValue = modelRef.getVoxelData( currentPoint );

		// Add pixel coordinates and pixel value to slice
		sliceMutex.lock();
		slice.setData( gridIndices, currentValue );
		sliceMutex.unlock();


	}

}

grid<voxData> model::getSlice( const surf sliceLocation, const double resolution ) const{

	// Distance between corners furthest away from each other
	const double cornerDistance = sqrt( pow( size3D.x, 2. ) + pow( size3D.y, 2. ) + pow( size3D.z, 2. ) );
	// Worst case: origin of plane at one corner and plane orianted in a way that it just slices corner on the other side of model cube


	// Surface in model's system
	const surf localSurface = sliceLocation.convertTo( cSys );


	v2CR sliceStart( -cornerDistance, -cornerDistance );
	v2CR sliceEnd( cornerDistance, cornerDistance );
	v2CR sliceResolution( resolution, resolution );

	grid<voxData> largeSlice( range( sliceStart.col, sliceEnd.col ), range( sliceStart.row, sliceEnd.row ), sliceResolution, voxData() );

	// Update Slice start, end and resolution because grid is discrete and fits the end and resolution the its range
	sliceStart = largeSlice.Start();
	sliceEnd = largeSlice.End();
	sliceResolution = largeSlice.Resolution();


	v2CR realStart( INFINITY, INFINITY );
	v2CR realEnd( -INFINITY, -INFINITY );

	size_t xIdx = 0, yIdx = 0;

	mutex currentXMutex, currentYMutex;
	mutex sliceMutex;

	mutex realStartMutex, realEndMutex;


	// Computation in threads
	vector<std::thread> threads;

	for( size_t threadIdx = 0; threadIdx < numThreads; threadIdx++ ){
		threads.emplace_back( sliceThreadFunction,	ref( xIdx ), ref( currentXMutex ), ref( yIdx ), ref( currentYMutex ),
													ref( realStart ), ref( realStartMutex), ref( realEnd ), ref( realEndMutex ),
													ref( largeSlice ), ref( sliceMutex ),
													cref( localSurface ), cref( *this ));
	}

	for( std::thread& currentThread : threads ) currentThread.join();

	// Check if restart and end have infinity in them
	if( realStart.col == INFINITY || realStart.row == INFINITY || realEnd.col == -INFINITY || realEnd.row == -INFINITY ){
		// This means that no model voxel is in slice
		return grid<voxData>( idx2CR( 0, 0 ), v2CR( 0., 0. ), v2CR( 1., 1. ) );

	}

	// Write data to smaller grid
	grid<voxData> slice( range( realStart.col, realEnd.col ), range( realStart.row, realEnd.row ), sliceResolution, voxData() );


	// Iterate grid
	v2CR coords;
	voxData currentData;

	for( size_t colIdx = 0; colIdx < slice.Size().col; colIdx++ ){

		for( size_t rowIdx = 0; rowIdx < slice.Size().row; rowIdx++ ){

			coords = slice.getCoordinates( idx2CR( colIdx, rowIdx ) );
			currentData = largeSlice.getData( coords );

			if( currentData.hasSpecificProperty( voxData::UNDEFINED ) )
				slice.setData( coords, voxData( largeSlice.MaxValue().attenuationAtRefE(), voxData::ReferencyEnergy()));
			else
				slice.setData( coords, currentData );
		}
	}


	return slice;
}


void model::addSpecialSphere( const voxData::specialProperty property, const pnt3 center, const double radius ){
	
	// Exit when coords invalid
	if ( !pntInside( center ) ) return;
	
	// Center indices
	const idx3 centerIdx = getVoxelIndices( center );

	// Index distance from center in each dimension
	idx3 indexDelta{
		Fmax( (size_t) ceil( radius / voxSize3D.x ), Min( centerIdx.x, numVox3D.x - centerIdx.x ) ),
		Fmax( (size_t) ceil( radius / voxSize3D.y ), Min( centerIdx.y, numVox3D.y - centerIdx.y ) ),
		Fmax( (size_t) ceil( radius / voxSize3D.z ), Min( centerIdx.z, numVox3D.z - centerIdx.z ) )
	};

	for( size_t x = centerIdx.x - indexDelta.x; x < centerIdx.x + indexDelta.x; x++ ){
		for( size_t y = centerIdx.y - indexDelta.y; x < centerIdx.y + indexDelta.y; y++ ){
			for( size_t z = centerIdx.z - indexDelta.z; x < centerIdx.z + indexDelta.z; z++ ){

				pnt3 p( v3( (double) x * voxSize3D.x , (double) y * voxSize3D.y , (double) z * voxSize3D.z ), cSys );

				if( ( center - p ).Length() <= radius )  (*this).operator()( x, y, z ).addSpecialProperty( property );
				
			}
		}
	}
}