
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
using std::thread;
#include <mutex>
using std::ref;
using std::cref;

#include "model.h"
#include "coordinateSystemTree.h"
#include "intersections.h"
#include "propability.h"
#include "tomography.h"
#include "serialization.h"



  /*********************************************************************
	Implementations
 *********************************************************************/


/*
	model implementation
*/


const string model::FILE_PREAMBLE{ "CT_MODEL_FILE_PREAMBLE_Ver2"};

model::model( CoordinateSystem* const coordinate_system, const Index3D numVox3D_, const Tuple3D voxSize3D_, const string name_ ) :
	numVox3D( numVox3D_ ),
	voxSize3D( voxSize3D_ ),
	size3D( { (double) numVox3D.x * voxSize3D.x,
			 (double) numVox3D.y * voxSize3D.y,
			 (double) numVox3D.z * voxSize3D.z } ),
	numVox( numVox3D.x * numVox3D.y * numVox3D.z ),
	parameter( numVox, VoxelData{} ),
	cSys( coordinate_system ),
	attenuationMin( -1. ),
	attenuationMax( -1. ),
	name( name_ )
{
	if( cSys->IsGlobal() ) CheckForAndOutputError( MathError::Input, "Model coordinate system must be child of global system!" );
}


model::model( const vector<char>& binary_data, vector<char>::const_iterator& it ) :
	numVox3D( Index3D( binary_data, it ) ),
	voxSize3D( Tuple3D( binary_data, it ) ),
	size3D( Tuple3D( (double) numVox3D.x * voxSize3D.x,
			 (double) numVox3D.y * voxSize3D.y,
			 (double) numVox3D.z * voxSize3D.z ) ),
	numVox( numVox3D.x* numVox3D.y* numVox3D.z ),
	parameter( numVox, VoxelData{} ),
	cSys( CoordinateSystems().AddSystem( binary_data, it ) ),
	attenuationMin( DeSerializeBuildIn<double>( -1., binary_data, it ) ),
	attenuationMax( DeSerializeBuildIn<double>( -1., binary_data, it ) ),
	name( DeSerializeBuildIn( string{ "Default model name_"}, binary_data, it ) )
{
	
	if( numVox * sizeof( VoxelData ) == (size_t) (binary_data.end() - it) ){
		memcpy( parameter.data(), &( *it ), numVox * sizeof(VoxelData));
		it += static_cast<long long int>( numVox * sizeof( VoxelData ) );
	}
	else{

		for( size_t i = 0; i < numVox; i++ ){
			parameter[i] = VoxelData( binary_data, it );
		}
	}
}


string model::ToString( [[maybe_unused]] const unsigned int newline_tabulators ) const{
	return string( "" );
}

Voxel model::Vox( void ) const{
	return  Voxel{ Point3D{Tuple3D { 0, 0, 0 }, cSys}, size3D, VoxelData{} };
}

const VoxelData& model::operator() ( const size_t x, const size_t y, const size_t z ) const{
	if( x >= numVox3D.x ){ CheckForAndOutputError( MathError::Input, "x index exceeds model size!" ); return parameter[( (size_t) numVox3D.x * numVox3D.y * ( numVox3D.z - 1 ) ) + (size_t) numVox3D.x * ( numVox3D.y - 1 ) + ( numVox3D.x - 1 )]; };
	if( y >= numVox3D.y ){ CheckForAndOutputError( MathError::Input, "y index exceeds model size!" ); return parameter[( (size_t) numVox3D.x * numVox3D.y * ( numVox3D.z - 1 ) ) + (size_t) numVox3D.x * ( numVox3D.y - 1 ) + ( numVox3D.x - 1 )]; };
	if( z >= numVox3D.z ){ CheckForAndOutputError( MathError::Input, "z index exceeds model size!" ); return parameter[( (size_t) numVox3D.x * numVox3D.y * ( numVox3D.z - 1 ) ) + (size_t) numVox3D.x * ( numVox3D.y - 1 ) + ( numVox3D.x - 1 )]; };

	return parameter.at(( (size_t) numVox3D.x * numVox3D.y * z ) + (size_t) numVox3D.x * y + x);
}

VoxelData& model::operator() ( const size_t x, const size_t y, const size_t z ){
	if( x >= numVox3D.x ){ CheckForAndOutputError( MathError::Input, "x index exceeds model size!" ); return parameter[ ( (size_t) numVox3D.x * numVox3D.y * ( numVox3D.z - 1 ) ) + (size_t) numVox3D.x * ( numVox3D.y - 1 ) + ( numVox3D.x - 1 ) ]; };
	if( y >= numVox3D.y ){ CheckForAndOutputError( MathError::Input, "y index exceeds model size!" ); return parameter[ ( (size_t) numVox3D.x * numVox3D.y * ( numVox3D.z - 1 ) ) + (size_t) numVox3D.x * ( numVox3D.y - 1 ) + ( numVox3D.x - 1 ) ]; };
	if( z >= numVox3D.z ){ CheckForAndOutputError( MathError::Input, "z index exceeds model size!" ); return parameter[ ( (size_t) numVox3D.x * numVox3D.y * ( numVox3D.z - 1 ) ) + (size_t) numVox3D.x * ( numVox3D.y - 1 ) + ( numVox3D.x - 1 ) ]; };

	return parameter.at( ( (size_t) numVox3D.x * numVox3D.y * z ) + (size_t) numVox3D.x * y + x );
}

bool model::checkIndices( const Index3D indices ) const{
	if( indices.x >= numVox3D.x ||
		indices.y >= numVox3D.y ||
		indices.z >= numVox3D.z ){
		return false;
	}
	return true;
}

bool model::pntInside( const Point3D p ) const{
	return validCoords( p.GetComponents( cSys ) );
}

Index3D model::getVoxelIndices( const Tuple3D locCoords ) const{
	if( locCoords.x < 0 || locCoords.y < 0 || locCoords.z < 0 ){
		CheckForAndOutputError( MathError::Input, "Only positive Coordinates allowed in model!" );
		return Index3D{ 0, 0, 0 };
	}

	Index3D indices{
		(size_t) ( locCoords.x / voxSize3D.x ),
		(size_t) ( locCoords.y / voxSize3D.y ),
		(size_t) ( locCoords.z / voxSize3D.z )
	};

	// Supress checkErr when index is exactly on the edge
	if( indices.x == numVox3D.x ) indices.x = numVox3D.x - 1;
	if( indices.y == numVox3D.y ) indices.y = numVox3D.y - 1;
	if( indices.z == numVox3D.z ) indices.z = numVox3D.z - 1;

	if( !checkIndices( indices ) ){
		CheckForAndOutputError( MathError::Input, "Coordinates exceed model size!" );
	}

	if( indices.x >= numVox3D.x ) indices.x = numVox3D.x - 1;
	if( indices.y >= numVox3D.y ) indices.y = numVox3D.y - 1;
	if( indices.z >= numVox3D.z ) indices.z = numVox3D.z - 1;

	return indices;
}

Index3D model::getVoxelIndices( const Point3D voxpnt ) const{
	return getVoxelIndices( voxpnt.GetComponents( cSys ) );
}

bool model::setVoxelData( const VoxelData newData, const Index3D indices ){

	if( !checkIndices( indices ) ) return false;

	this->operator()( indices ) = newData;

	if( newData.GetAttenuationAtReferenceEnergy() < attenuationMin || attenuationMax < 0 ) attenuationMin = newData.GetAttenuationAtReferenceEnergy();
	if( newData.GetAttenuationAtReferenceEnergy() > attenuationMax || attenuationMax < 0 ) attenuationMax = newData.GetAttenuationAtReferenceEnergy();

	return true;
}

bool model::setVoxelProperty( const VoxelData::specialProperty property, const Index3D indices ){

	if( !checkIndices( indices ) ) return false;

	this->operator()( indices ).AddSpecialProperty( property );

	return true;

}

Voxel model::getVoxel( const Index3D indices ) const{

	if( !checkIndices( indices ) ){
		CheckForAndOutputError( MathError::Input, "Indices exceed model!" );
		return Voxel{};
	}

	Point3D voxOrigin{ Tuple3D{ (double) indices.x * voxSize3D.x, (double) indices.y * voxSize3D.y, (double) indices.z * voxSize3D.z }, cSys };

	// Get voxel data_ and create voxel instance
	Voxel voxel{ voxOrigin, voxSize3D, getVoxelData( indices ) };

	return voxel;
}

Ray model::rayTransmission( const Ray tRay, const tomographyParameter& tomoParameter, const rayScattering& scatteringProperties ) const{

	Ray modelRay = tRay.ConvertTo( this->cSys );					// Current Ray in model's coordinate system

	// Find entrance_ in model
	const RayVoxelIntersection modelIsect{ Vox(), modelRay };

	//const rayVox_Intersection_Result rayEntrance = modelIsect.entrance_;
	if( !modelIsect.entrance_.intersection_exists_ ) return modelRay;			// Return if Ray does not intersect model


	// Iteration through model
	/* ---------------------------------------------------------------------------------------------------- */

	double currentRayStep = modelIsect.entrance_.line_parameter_ + tomoParameter.rayStepSize;		// Ray parameter at model entrance_
	const double lengthInModel = modelIsect.exit_.line_parameter_ - modelIsect.entrance_.line_parameter_;

	// Go a tiny step further down the Ray from intersection point with model and test if inside
	// Return when the point on the Ray is not inside the model meaning that the Ray just barely hit the model
	if( !pntInside( modelRay.GetPoint( currentRayStep ) ) ) return modelRay;


	// Current point on the Ray
	Point3D currentPntOnRay = modelRay.GetPoint( currentRayStep );		// Point of model entrance_


	const double meanFrequencyTube = modelRay.GetMeanFrequencyOfSpectrum();	// Mean frequency of Ray before it enters model
	const double meanVoxelSideLength = ( voxSize3D.x + voxSize3D.y + voxSize3D.z ) / 3.;
	const size_t meanVoxelAmount = (size_t) ( (double) ( numVox3D.x + numVox3D.y + numVox3D.z ) / 3. );

	const double scatterConstant = tomoParameter.scatterPropability * meanFrequencyTube / ( meanVoxelSideLength * static_cast<double>( meanVoxelAmount ) );


	// Iterate through model while current point is inside model
	while( pntInside( currentPntOnRay ) ){

		const Index3D currentVoxelIndices = getVoxelIndices( currentPntOnRay );		// Indices of current voxel

		const vector<Voxel::Face> possibleFaces = modelRay.GetPossibleVoxelExits();		// The possible exit_ faces

		double rayParameter = INFINITY;		// The smallest Ray parameter

		// Iterate all possible faces and get the Ray parameter at intersection
		for( const Voxel::Face& currentFace : possibleFaces ){

			double currentRayParameter = INFINITY;			// Set to infinity 
			double planePosistion;							// Position of current face

			
			// Switch faces
			switch( currentFace ){

				case Voxel::Face::YZ_Xp:
					planePosistion = ( static_cast<double>( currentVoxelIndices.x ) + 1. ) * this->voxSize3D.x;		// Position of positive yz-plane
					currentRayParameter = ( planePosistion - currentPntOnRay.X() ) / modelRay.direction().X();		// Ray parameter at intersection
					break;

				case Voxel::Face::YZ_Xm:
					planePosistion = ( static_cast<double>( currentVoxelIndices.x ) ) * this->voxSize3D.x;		// Position of negative yz-plane
					currentRayParameter = ( planePosistion - currentPntOnRay.X() ) / modelRay.direction().X();
					break;

				case Voxel::Face::XZ_Yp:
					planePosistion = ( static_cast<double>( currentVoxelIndices.y ) + 1. ) * this->voxSize3D.y;		// Position of positive xz-plane
					currentRayParameter = ( planePosistion - currentPntOnRay.Y() ) / modelRay.direction().Y();
					break;

				case Voxel::Face::XZ_Ym:
					planePosistion = ( static_cast<double>( currentVoxelIndices.y )     ) * this->voxSize3D.y;		// Position of negative xz-plane
					currentRayParameter = ( planePosistion - currentPntOnRay.Y() ) / modelRay.direction().Y();
					break;

				case Voxel::Face::XY_Zp:
					planePosistion = ( static_cast<double>( currentVoxelIndices.z ) + 1. ) * this->voxSize3D.z;		// Position of positive xy-plane
					currentRayParameter = ( planePosistion - currentPntOnRay.Z() ) / modelRay.direction().Z();
					break;

				case Voxel::Face::XY_Zm:
					planePosistion = ( static_cast<double>( currentVoxelIndices.z )   ) * this->voxSize3D.z;		// Position of negative xy-plane
					currentRayParameter = ( planePosistion - currentPntOnRay.Z() ) / modelRay.direction().Z();
					break;

				default: break;
			}

			// Set smallest Ray parameter
			if( currentRayParameter < rayParameter ) rayParameter = currentRayParameter;

		}
		
		// Exit found
		if( rayParameter < INFINITY ){

			const double distance = rayParameter;		// The distance is the rayParameter

			// Update Ray properties_ whith distance travelled in current voxel
			modelRay.UpdateProperties( this->getVoxelData( currentVoxelIndices ), distance );
			modelRay.IncrementHitCounter();

			currentRayStep += distance + tomoParameter.rayStepSize;				// New Step on Ray
			currentPntOnRay = modelRay.GetPoint( currentRayStep );	// New point on Ray
		
			// Scattering
			if( tomoParameter.scattering ){
			
				// Mean frequency of Ray
				const double meanFrequency = modelRay.GetMeanFrequencyOfSpectrum();

				// Calculate propability that the Ray is scattered
				const double scatterPropability = distance / meanFrequency * scatterConstant;

				// Does the Ray scatter?
				if( integerRNG.eventHappend( scatterPropability ) ){
					
					// Scatter the Ray
					return scatteringProperties.scatterRay( modelRay, currentPntOnRay );
				}
			}
		}
	}

	// New origin_ "outside" the model to return
	modelRay.origin( currentPntOnRay );

	return modelRay;
}

bool model::crop( const Tuple3D minCoords, const Tuple3D maxCoords ){
	if( !validCoords( minCoords ) || !validCoords( maxCoords ) ) return false;

	Index3D minIdcs = getVoxelIndices( minCoords );			// Minimum boundary indices
	Index3D maxIdcs = getVoxelIndices( maxCoords );			// Maximum boundary indices

	// Parameters of cropped model
	Index3D newVoxNum3D{ maxIdcs.x - minIdcs.x + 1, maxIdcs.y - minIdcs.y + 1, maxIdcs.z - minIdcs.z + 1 };

	// New model
	model newModel{ cSys, newVoxNum3D, voxSize3D };


	// Copy data_ to new model
	for( size_t z = 0; z < newVoxNum3D.z; z++ ){
		for( size_t y = 0; y < newVoxNum3D.y; y++ ){
			for( size_t x = 0; x < newVoxNum3D.x; x++ ){
				newModel( x, y, z ) = ( *this )( x + minIdcs.x, y + minIdcs.y, z + minIdcs.z );
			}
		}
	}

	*this = std::move( newModel );

	return true;
}


size_t model::Serialize( vector<char>& binary_data ) const{

	size_t expectedSize = FILE_PREAMBLE.size() + 1;
	expectedSize += sizeof( numVox3D );
	expectedSize += sizeof( voxSize3D );
	expectedSize += sizeof( CoordinateSystem );
	expectedSize += sizeof( attenuationMin );
	expectedSize += sizeof( attenuationMax );
	expectedSize += name.size() + 1;
	expectedSize += numVox * sizeof( parameter.front() );

	binary_data.reserve( expectedSize );

	size_t num_bytes = 0;
	num_bytes += SerializeBuildIn( FILE_PREAMBLE, binary_data );
	num_bytes += numVox3D.Serialize( binary_data );
	num_bytes += voxSize3D.Serialize( binary_data );
	num_bytes += cSys->Serialize( binary_data );
	num_bytes += SerializeBuildIn( attenuationMin, binary_data );
	num_bytes += SerializeBuildIn( attenuationMax, binary_data );
	num_bytes += SerializeBuildIn( name, binary_data );

	
	binary_data.insert( binary_data.end(), (char*) parameter.data(), (char*) parameter.data() + sizeof(VoxelData) * numVox);
	//binary_data.insert( binary_data.end(), parameter.cbegin(), parameter.cend() );

	return num_bytes;

}


void model::sliceThreadFunction(	size_t& xIdx, mutex& currentXMutex, size_t& yIdx, mutex& currentYMutex,
									GridCoordinates& realStart, mutex& realStartMutex, GridCoordinates& realEnd, mutex& realEndMutex,
									grid<VoxelData>& slice, mutex& sliceMutex,
									const Surface& slicePlane,
									const model& modelRef ){

	// Iterate through all columns
	while( xIdx < slice.Size().c ){

		size_t localXIdx, localYIdx;

		// Lock
		currentXMutex.lock(); currentYMutex.lock();

		localXIdx = xIdx;
		localYIdx = yIdx;

		if( yIdx >= slice.Size().r ){
			xIdx++;
			yIdx = 0;
		}
		else{
			yIdx++;
		}

		//Unlock
		currentXMutex.unlock(); currentYMutex.unlock();


		// Check 
		if( localXIdx > slice.Size().c - 1 || localYIdx > slice.Size().r - 1 ) continue;
		
		// Grid indices
		const GridIndex gridIndices( localXIdx, localYIdx );

		// Get point on surface for current grid indices
		const GridCoordinates surfaceCoordinate = slice.getCoordinates( gridIndices );
		const Point3D currentPoint = slicePlane.GetPoint( surfaceCoordinate.c, surfaceCoordinate.r );


		// Are cooradinates defined in model?
		if( !modelRef.pntInside( currentPoint ) ){
			slice.setData( gridIndices, VoxelData( 0., 1., VoxelData::UNDEFINED ) );
			continue;	// Goto next iteration
		}


		// Check if current x and y values in plane are new real start/end

		if( surfaceCoordinate.c < realStart.c ){
			WriteThreadVar( realStart.c, surfaceCoordinate.c, realStartMutex );
		}
		
		if( surfaceCoordinate.c > realEnd.c ){
			WriteThreadVar( realEnd.c, surfaceCoordinate.c, realEndMutex );
		}

		if( surfaceCoordinate.r < realStart.r ){
			WriteThreadVar( realStart.r, surfaceCoordinate.r, realStartMutex );
		}

		if( surfaceCoordinate.r > realEnd.r ){
			WriteThreadVar( realEnd.r, surfaceCoordinate.r, realEndMutex );
		}

		// Current voxel value
		const VoxelData currentValue = modelRef.getVoxelData( currentPoint );

		// Add pixel Coordinates and pixel value to slice
		sliceMutex.lock();
		slice.setData( gridIndices, currentValue );
		sliceMutex.unlock();


	}

}

grid<VoxelData> model::getSlice( const Surface sliceLocation, const double resolution ) const{

	// Distance between corners furthest away from each other
	const double cornerDistance = sqrt( pow( size3D.x, 2. ) + pow( size3D.y, 2. ) + pow( size3D.z, 2. ) );
	// Worst case: origin_ of plane at one corner and plane orianted in a way that it just slices corner on the other side of model cube


	// Surface in model's system
	const Surface localSurface = sliceLocation.ConvertTo( cSys );


	GridCoordinates sliceStart( -cornerDistance, -cornerDistance );
	GridCoordinates sliceEnd( cornerDistance, cornerDistance );
	GridCoordinates sliceResolution( resolution, resolution );

	grid<VoxelData> largeSlice( NumberRange( sliceStart.c, sliceEnd.c ), NumberRange( sliceStart.r, sliceEnd.r ), sliceResolution, VoxelData() );

	// Update Slice start, end and resolution because grid is discrete and fits the end and resolution the its range
	sliceStart = largeSlice.Start();
	sliceEnd = largeSlice.End();
	sliceResolution = largeSlice.Resolution();


	GridCoordinates realStart( INFINITY, INFINITY );
	GridCoordinates realEnd( -INFINITY, -INFINITY );

	size_t xIdx = 0, yIdx = 0;

	mutex currentXMutex, currentYMutex;
	mutex sliceMutex;

	mutex realStartMutex, realEndMutex;


	// Computation in threads
	vector<std::thread> threads;

	for( size_t threadIdx = 0; threadIdx < std::thread::hardware_concurrency(); threadIdx++ ){
		threads.emplace_back( sliceThreadFunction,	ref( xIdx ), ref( currentXMutex ), ref( yIdx ), ref( currentYMutex ),
													ref( realStart ), ref( realStartMutex), ref( realEnd ), ref( realEndMutex ),
													ref( largeSlice ), ref( sliceMutex ),
													cref( localSurface ), cref( *this ));
	}

	for( std::thread& currentThread : threads ) currentThread.join();

	// Check if restart and end have infinity in them
	if( realStart.c == INFINITY || realStart.r == INFINITY || realEnd.c == -INFINITY || realEnd.r == -INFINITY ){
		// This means that no model voxel is in slice
		return grid<VoxelData>( GridIndex( 0, 0 ), GridCoordinates( 0., 0. ), GridCoordinates( 1., 1. ) );

	}

	// Write data_ to smaller grid
	grid<VoxelData> slice( NumberRange( realStart.c, realEnd.c ), NumberRange( realStart.r, realEnd.r ), sliceResolution, VoxelData() );


	// Iterate grid
	GridCoordinates coords;
	VoxelData currentData;

	for( size_t colIdx = 0; colIdx < slice.Size().c; colIdx++ ){

		for( size_t rowIdx = 0; rowIdx < slice.Size().r; rowIdx++ ){

			coords = slice.getCoordinates( GridIndex( colIdx, rowIdx ) );
			currentData = largeSlice.getData( coords );

			if( currentData.HasSpecificProperty( VoxelData::UNDEFINED ) )
				slice.setData( coords, VoxelData( largeSlice.MaxValue().GetAttenuationAtReferenceEnergy(), VoxelData::reference_energy_eV()));
			else
				slice.setData( coords, currentData );
		}
	}


	return slice;
}


void model::addSpecialSphere( const VoxelData::specialProperty property, const Point3D center, const double radius ){
	
	// Exit when coords invalid
	if ( !pntInside( center ) ) return;
	
	// Center indices
	const Index3D centerIdx = getVoxelIndices( center );

	// Index distance from center in each dimension
	Index3D indexDelta{
		ForceToMax( (size_t) ceil( radius / voxSize3D.x ), Min( centerIdx.x, numVox3D.x - centerIdx.x ) ),
		ForceToMax( (size_t) ceil( radius / voxSize3D.y ), Min( centerIdx.y, numVox3D.y - centerIdx.y ) ),
		ForceToMax( (size_t) ceil( radius / voxSize3D.z ), Min( centerIdx.z, numVox3D.z - centerIdx.z ) )
	};

	for( size_t x = centerIdx.x - indexDelta.x; x < centerIdx.x + indexDelta.x; x++ ){
		for( size_t y = centerIdx.y - indexDelta.y; x < centerIdx.y + indexDelta.y; y++ ){
			for( size_t z = centerIdx.z - indexDelta.z; x < centerIdx.z + indexDelta.z; z++ ){

				Point3D p( Tuple3D( (double) x * voxSize3D.x , (double) y * voxSize3D.y , (double) z * voxSize3D.z ), cSys );

				if( ( center - p ).length() <= radius )  (*this).operator()( x, y, z ).AddSpecialProperty( property );
				
			}
		}
	}
}