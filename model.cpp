
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
#include "propabilityDistribution.h"
#include "tomography.h"
#include "serialization.h"



  /*********************************************************************
	Implementations
 *********************************************************************/


/*
	model implementation
*/


const string Model::FILE_PREAMBLE{ "CT_MODEL_FILE_PREAMBLE_Ver2"};

Model::Model( CoordinateSystem* const coordinate_system, const Index3D numVox3D_, const Tuple3D voxSize3D_, const string name_ ) :
	number_of_voxel_3D_( numVox3D_ ),
	voxel_size_( voxSize3D_ ),
	size_{	static_cast<double>( number_of_voxel_3D_.x ) * voxel_size_.x,
			static_cast<double>( number_of_voxel_3D_.y ) * voxel_size_.y,
			static_cast<double>( number_of_voxel_3D_.z ) * voxel_size_.z } ,
	number_of_voxel_( number_of_voxel_3D_.x * number_of_voxel_3D_.y * number_of_voxel_3D_.z ),
	voxel_data_( number_of_voxel_, VoxelData{} ),
	coordinate_system_( coordinate_system ),
	attenuationRange_{ -2., -1. },
	name_( name_ )
{
	if( coordinate_system_->IsGlobal() ) CheckForAndOutputError( MathError::Input, "Model coordinate system must be child of global system!" );
}


Model::Model( const vector<char>& binary_data, vector<char>::const_iterator& it ) :
	number_of_voxel_3D_{ binary_data, it },
	voxel_size_{ binary_data, it },
	size_{	static_cast<double>( number_of_voxel_3D_.x ) * voxel_size_.x,
			static_cast<double>( number_of_voxel_3D_.y ) * voxel_size_.y,
			static_cast<double>( number_of_voxel_3D_.z ) * voxel_size_.z } ,
	number_of_voxel_( number_of_voxel_3D_.x* number_of_voxel_3D_.y* number_of_voxel_3D_.z ),
	voxel_data_( number_of_voxel_, VoxelData{} ),
	coordinate_system_( CoordinateSystems().AddSystem( binary_data, it ) ),
	attenuationRange_{ binary_data, it },
	name_( DeSerializeBuildIn( string{ "Default model name_"}, binary_data, it ) )
{
	
	if( number_of_voxel_ * sizeof( VoxelData ) == (size_t) (binary_data.end() - it) ){
		memcpy( voxel_data_.data(), &( *it ), number_of_voxel_ * sizeof(VoxelData));
		it += static_cast<long long int>( number_of_voxel_ * sizeof( VoxelData ) );
	}
	else{

		for( size_t i = 0; i < number_of_voxel_; i++ ){
			voxel_data_[i] = VoxelData( binary_data, it );
		}
	}
}


string Model::ToString( [[maybe_unused]] const unsigned int newline_tabulators ) const{
	return string( "" );
}

Voxel Model::GetModelVoxel( void ) const{
	return  Voxel{ Point3D{Tuple3D { 0, 0, 0 }, coordinate_system_}, size_, VoxelData{} };
}

VoxelData Model::GetVoxelData( const Index3D index ) const{
	if( !AreIndicesValid( index ) ){ 
		CheckForAndOutputError( MathError::Input, "index exceeds model size!" ); 
		return VoxelData{}; 
	}
	
	return voxel_data_.at( GetDataIndex( index ) );
}

VoxelData& Model::operator()( const Index3D index ){
	if( !AreIndicesValid( index ) ){ 
		CheckForAndOutputError( MathError::Input, "index exceeds model size!" ); 
		return voxel_data_.at( number_of_voxel_ - 1 ); 
	}
	
	return voxel_data_.at( GetDataIndex( index ) );
}

bool Model::AreIndicesValid( const Index3D indices ) const{
	if( indices.x >= number_of_voxel_3D_.x ||
		indices.y >= number_of_voxel_3D_.y ||
		indices.z >= number_of_voxel_3D_.z ){
		return false;
	}
	return true;
}

bool Model::IsPointInside( const Point3D p ) const{
	return AreCoordinatesValid( p.GetComponents( coordinate_system_ ) );
}

Index3D Model::GetVoxelIndices( const Tuple3D locCoords ) const{
	if( locCoords.x < 0 || locCoords.y < 0 || locCoords.z < 0 ){
		CheckForAndOutputError( MathError::Input, "Only positive Coordinates allowed in model!" );
		return Index3D{ 0, 0, 0 };
	}

	Index3D indices{
		(size_t) ( locCoords.x / voxel_size_.x ),
		(size_t) ( locCoords.y / voxel_size_.y ),
		(size_t) ( locCoords.z / voxel_size_.z )
	};


	// Supress checkErr when index is exactly on the edge
	if( indices.x == number_of_voxel_3D_.x ) indices.x = number_of_voxel_3D_.x - 1;
	if( indices.y == number_of_voxel_3D_.y ) indices.y = number_of_voxel_3D_.y - 1;
	if( indices.z == number_of_voxel_3D_.z ) indices.z = number_of_voxel_3D_.z - 1;

	if( !AreIndicesValid( indices ) ){
		CheckForAndOutputError( MathError::Input, "Coordinates exceed model size!" );
	}

	indices.x = ForceToMax( indices.x, number_of_voxel_3D_.x - 1 );
	indices.y = ForceToMax( indices.y, number_of_voxel_3D_.y - 1 );
	indices.z = ForceToMax( indices.z, number_of_voxel_3D_.z - 1 );


	return indices;
}

Index3D Model::GetVoxelIndices( const Point3D voxpnt ) const{
	return GetVoxelIndices( voxpnt.GetComponents( coordinate_system_ ) );
}

bool Model::SetVoxelData( const VoxelData newData, const Index3D indices ){

	if( !AreIndicesValid( indices ) ) return false;

	this->operator()( indices ) = newData;

	if( newData.GetAttenuationAtReferenceEnergy() < attenuationRange_.start()	|| attenuationRange_.start() < 0 ) attenuationRange_.start( newData.GetAttenuationAtReferenceEnergy() );
	if( newData.GetAttenuationAtReferenceEnergy() > attenuationRange_.end()		|| attenuationRange_.end() < 0 )   attenuationRange_.end( newData.GetAttenuationAtReferenceEnergy() );

	return true;
}

bool Model::SetVoxelProperties( const VoxelData::SpecialProperty property, const Index3D indices ){

	if( !AreIndicesValid( indices ) ) return false;

	this->operator()( indices ).AddSpecialProperty( property );

	return true;

}

Voxel Model::GetVoxel( const Index3D indices ) const{

	if( !AreIndicesValid( indices ) ){
		CheckForAndOutputError( MathError::Input, "Indices exceed model!" );
		return Voxel{};
	}

	Point3D voxOrigin{ Tuple3D{ (double) indices.x * voxel_size_.x, (double) indices.y * voxel_size_.y, (double) indices.z * voxel_size_.z }, coordinate_system_ };

	// Get voxel data and create voxel instance
	Voxel voxel{ voxOrigin, voxel_size_, GetVoxelData( indices ) };

	return voxel;
}

Ray Model::TransmitRay( const Ray& tRay, const TomographyProperties& tomoParameter, const RayScattering& scatteringProperties ) const{

	Ray modelRay = std::move( tRay.ConvertTo( this->coordinate_system_ ) );					// Current Ray in model's coordinate system

	// Find entrance_ in model
	const RayVoxelIntersection modelIsect{ GetModelVoxel(), modelRay };

	//const rayVox_Intersection_Result rayEntrance = modelIsect.entrance_;
	if( !modelIsect.entrance_.intersection_exists_ ) return modelRay;			// Return if Ray does not intersect model


	// Iteration through model
	/* ---------------------------------------------------------------------------------------------------- */

	double currentRayStep = modelIsect.entrance_.line_parameter_ + tomoParameter.ray_step_length;		// Ray parameter at model entrance_
	const double lengthInModel = modelIsect.exit_.line_parameter_ - modelIsect.entrance_.line_parameter_;

	// Go a tiny step further down the Ray from intersection point with model and test if inside
	// Return when the point on the Ray is not inside the model meaning that the Ray just barely hit the model
	if( !IsPointInside( modelRay.GetPoint( currentRayStep ) ) ) return modelRay;


	// Current point on the Ray
	Point3D currentPntOnRay = std::move( modelRay.GetPoint( currentRayStep ) );		// Point of model entrance_


	const double meanFrequencyTube = modelRay.GetMeanFrequencyOfSpectrum();	// Mean frequency of Ray before it enters model
	const double meanVoxelSideLength = ( voxel_size_.x + voxel_size_.y + voxel_size_.z ) / 3.;
	const size_t meanVoxelAmount = (size_t) ( (double) ( number_of_voxel_3D_.x + number_of_voxel_3D_.y + number_of_voxel_3D_.z ) / 3. );

	const double scatterConstant = tomoParameter.scatter_propability * meanFrequencyTube / ( meanVoxelSideLength * static_cast<double>( meanVoxelAmount ) );


	// Iterate through model while current point is inside model
	while( IsPointInside( currentPntOnRay ) ){

		const Index3D currentVoxelIndices = GetVoxelIndices( currentPntOnRay );		// Indices of current voxel

		const array<bool, ToUnderlying( Voxel::Face::End )> possibleFaces = std::move( modelRay.GetPossibleVoxelExits() );		// The possible exit_ faces

		double rayParameter = INFINITY;		// The smallest Ray parameter

		// Iterate all possible faces and get the Ray parameter at intersection
		for( Voxel::Face currentFace = Voxel::Face::Begin; currentFace < Voxel::Face::End; ++currentFace ){

			double currentRayParameter = INFINITY;			// Set to infinity 
			double planePosistion;							// Position of current face

			// Switch faces
			switch( currentFace ){

				case Voxel::Face::YZ_Xp:
					if( possibleFaces.at( ToUnderlying( currentFace ) ) == false ) break;
					planePosistion = ( static_cast<double>( currentVoxelIndices.x ) + 1. ) * this->voxel_size_.x;		// Position of positive yz-plane
					currentRayParameter = ( planePosistion - currentPntOnRay.X() ) / modelRay.direction().X();		// Ray parameter at intersection
					break;

				case Voxel::Face::YZ_Xm:
					if( possibleFaces.at( ToUnderlying( currentFace ) ) == false ) break;
					planePosistion = ( static_cast<double>( currentVoxelIndices.x ) ) * this->voxel_size_.x;		// Position of negative yz-plane
					currentRayParameter = ( planePosistion - currentPntOnRay.X() ) / modelRay.direction().X();
					break;

				case Voxel::Face::XZ_Yp:
					if( possibleFaces.at( ToUnderlying( currentFace ) ) == false ) break;
					planePosistion = ( static_cast<double>( currentVoxelIndices.y ) + 1. ) * this->voxel_size_.y;		// Position of positive xz-plane
					currentRayParameter = ( planePosistion - currentPntOnRay.Y() ) / modelRay.direction().Y();
					break;

				case Voxel::Face::XZ_Ym:
					if( possibleFaces.at( ToUnderlying( currentFace ) ) == false ) break;
					planePosistion = ( static_cast<double>( currentVoxelIndices.y )     ) * this->voxel_size_.y;		// Position of negative xz-plane
					currentRayParameter = ( planePosistion - currentPntOnRay.Y() ) / modelRay.direction().Y();
					break;

				case Voxel::Face::XY_Zp:
					if( possibleFaces.at( ToUnderlying( currentFace ) ) == false ) break;
					planePosistion = ( static_cast<double>( currentVoxelIndices.z ) + 1. ) * this->voxel_size_.z;		// Position of positive xy-plane
					currentRayParameter = ( planePosistion - currentPntOnRay.Z() ) / modelRay.direction().Z();
					break;

				case Voxel::Face::XY_Zm:
					if( possibleFaces.at( ToUnderlying( currentFace ) ) == false ) break;
					planePosistion = ( static_cast<double>( currentVoxelIndices.z )   ) * this->voxel_size_.z;		// Position of negative xy-plane
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
			modelRay.UpdateProperties( this->GetVoxelData( currentVoxelIndices ), distance );
			modelRay.IncrementHitCounter();

			currentRayStep += distance + tomoParameter.ray_step_length;				// New Step on Ray
			currentPntOnRay = modelRay.GetPoint( currentRayStep );	// New point on Ray
		
			// Scattering
			if( tomoParameter.scattering_enabled ){
			
				// Mean frequency of Ray
				const double meanFrequency = modelRay.GetMeanFrequencyOfSpectrum();

				// Calculate propability that the Ray is scattered
				const double scatterPropability = distance / meanFrequency * scatterConstant;

				// Does the Ray scatter?
				if( integer_random_number_generator.DidARandomEventHappen( scatterPropability ) ){
					
					// Scatter the Ray
					return scatteringProperties.ScatterRay( modelRay, currentPntOnRay );
				}
			}
		}
	}

	// New origin_ "outside" the model to return
	modelRay.origin( currentPntOnRay );

	return modelRay;
}

bool Model::Crop( const Tuple3D minCoords, const Tuple3D maxCoords ){
	if( !AreCoordinatesValid( minCoords ) || !AreCoordinatesValid( maxCoords ) ) return false;

	Index3D minIdcs = GetVoxelIndices( minCoords );			// Minimum boundary indices
	Index3D maxIdcs = GetVoxelIndices( maxCoords );			// Maximum boundary indices

	// Parameters of cropped model
	Index3D newVoxNum3D{ maxIdcs.x - minIdcs.x + 1, maxIdcs.y - minIdcs.y + 1, maxIdcs.z - minIdcs.z + 1 };

	// New model
	Model newModel{ coordinate_system_, newVoxNum3D, voxel_size_ };


	// Copy data to new model
	for( size_t z = 0; z < newVoxNum3D.z; z++ ){
		for( size_t y = 0; y < newVoxNum3D.y; y++ ){
			for( size_t x = 0; x < newVoxNum3D.x; x++ ){
				newModel.SetVoxelData( GetVoxelData( { x + minIdcs.x, y + minIdcs.y, z + minIdcs.z } ), { x, y, z } );
			}
		}
	}

	*this = std::move( newModel );

	return true;
}


size_t Model::Serialize( vector<char>& binary_data ) const{

	size_t expectedSize = FILE_PREAMBLE.size() + 1;
	expectedSize += sizeof( number_of_voxel_3D_ );
	expectedSize += sizeof( voxel_size_ );
	expectedSize += sizeof( CoordinateSystem );
	expectedSize += sizeof( attenuationRange_ );
	expectedSize += name_.size() + 1;
	expectedSize += number_of_voxel_ * sizeof( voxel_data_.front() );

	binary_data.reserve( expectedSize );

	size_t num_bytes = 0;
	num_bytes += SerializeBuildIn( FILE_PREAMBLE, binary_data );
	num_bytes += number_of_voxel_3D_.Serialize( binary_data );
	num_bytes += voxel_size_.Serialize( binary_data );
	num_bytes += coordinate_system_->Serialize( binary_data );
	num_bytes += attenuationRange_.Serialize( binary_data );
	num_bytes += SerializeBuildIn( name_, binary_data );

	
	binary_data.insert( binary_data.end(), (char*) voxel_data_.data(), (char*) voxel_data_.data() + sizeof(VoxelData) * number_of_voxel_);
	//binary_data.insert( binary_data.end(), parameter.cbegin(), parameter.cend() );

	return num_bytes;

}


void Model::SliceThreaded(	size_t& xIdx, mutex& currentXMutex, size_t& yIdx, mutex& currentYMutex,
									GridCoordinates& realStart, mutex& realStartMutex, GridCoordinates& realEnd, mutex& realEndMutex,
									DataGrid<VoxelData>& slice, mutex& sliceMutex,
									const Surface& slicePlane,
									const Model& modelRef ){

	// Iterate through all columns
	while( xIdx < slice.size().c ){

		size_t localXIdx, localYIdx;

		// Lock
		currentXMutex.lock(); currentYMutex.lock();

		localXIdx = xIdx;
		localYIdx = yIdx;

		if( yIdx >= slice.size().r ){
			xIdx++;
			yIdx = 0;
		}
		else{
			yIdx++;
		}

		//Unlock
		currentXMutex.unlock(); currentYMutex.unlock();


		// Check 
		if( localXIdx > slice.size().c - 1 || localYIdx > slice.size().r - 1 ) continue;
		
		// Grid indices
		const GridIndex gridIndices( localXIdx, localYIdx );

		// Get point on surface for current grid indices
		const GridCoordinates surfaceCoordinate = slice.GetCoordinates( gridIndices );
		const Point3D currentPoint = slicePlane.GetPoint( surfaceCoordinate.c, surfaceCoordinate.r );


		// Are cooradinates defined in model?
		if( !modelRef.IsPointInside( currentPoint ) ){
			slice.SetData( gridIndices, VoxelData( 0., 1., VoxelData::Undefined ) );
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
		const VoxelData currentValue = modelRef.GetVoxelData( currentPoint );

		// Add pixel Coordinates and pixel value to slice
		sliceMutex.lock();
		slice.SetData( gridIndices, currentValue );
		sliceMutex.unlock();


	}

}

DataGrid<VoxelData> Model::GetSlice( const Surface sliceLocation, const double resolution ) const{

	// Distance between corners furthest away from each other
	const double cornerDistance = sqrt( pow( size_.x, 2. ) + pow( size_.y, 2. ) + pow( size_.z, 2. ) );
	// Worst case: origin_ of plane at one corner and plane orianted in a way that it just slices corner on the other side of model cube


	// Surface in model's system
	const Surface localSurface = sliceLocation.ConvertTo( coordinate_system_ );


	GridCoordinates sliceStart( -cornerDistance, -cornerDistance );
	GridCoordinates sliceEnd( cornerDistance, cornerDistance );
	GridCoordinates sliceResolution( resolution, resolution );

	DataGrid<VoxelData> largeSlice( NumberRange( sliceStart.c, sliceEnd.c ), NumberRange( sliceStart.r, sliceEnd.r ), sliceResolution, VoxelData() );

	// Update Slice start, end and resolution because grid is discrete and fits the end and resolution the its range
	sliceStart = largeSlice.start();
	sliceEnd = largeSlice.GetEnd();
	sliceResolution = largeSlice.resolution();


	GridCoordinates realStart( INFINITY, INFINITY );
	GridCoordinates realEnd( -INFINITY, -INFINITY );

	size_t xIdx = 0, yIdx = 0;

	mutex currentXMutex, currentYMutex;
	mutex sliceMutex;

	mutex realStartMutex, realEndMutex;


	// Computation in threads
	vector<std::thread> threads;

	for( size_t threadIdx = 0; threadIdx < std::thread::hardware_concurrency(); threadIdx++ ){
		threads.emplace_back( SliceThreaded,	ref( xIdx ), ref( currentXMutex ), ref( yIdx ), ref( currentYMutex ),
													ref( realStart ), ref( realStartMutex), ref( realEnd ), ref( realEndMutex ),
													ref( largeSlice ), ref( sliceMutex ),
													cref( localSurface ), cref( *this ) );
	}

	for( std::thread& currentThread : threads ) currentThread.join();

	// Check if restart and end have infinity in them
	if( realStart.c == INFINITY || realStart.r == INFINITY || realEnd.c == -INFINITY || realEnd.r == -INFINITY ){
		// This means that no model voxel is in slice
		return DataGrid<VoxelData>{ GridIndex{ 0, 0 }, GridCoordinates{ 0., 0. }, GridCoordinates{ 1., 1. } };

	}

	// Write data to smaller grid
	DataGrid<VoxelData> slice{ NumberRange{ realStart.c, realEnd.c }, NumberRange{ realStart.r, realEnd.r }, sliceResolution, VoxelData() };


	// Iterate grid
	GridCoordinates coords;
	VoxelData currentData;

	for( size_t colIdx = 0; colIdx < slice.size().c; colIdx++ ){

		for( size_t rowIdx = 0; rowIdx < slice.size().r; rowIdx++ ){

			coords = slice.GetCoordinates( GridIndex( colIdx, rowIdx ) );
			currentData = largeSlice.GetData( coords );

			if( currentData.HasSpecificProperty( VoxelData::Undefined ) )
				slice.SetData( coords, VoxelData{ largeSlice.max_value().GetAttenuationAtReferenceEnergy(), VoxelData::reference_energy_eV() } );
			else
				slice.SetData( coords, currentData );
		}
	}


	return slice;
}


void Model::AddSpecialSphere( const VoxelData::SpecialProperty property, const Point3D center, const double radius ){
	
	// Exit when coords invalid
	if ( !IsPointInside( center ) ) return;
	
	// Center indices
	const Index3D centerIdx = GetVoxelIndices( center );

	// Index distance from center in each dimension
	Index3D indexDelta{
		ForceToMax( (size_t) ceil( radius / voxel_size_.x ), Min( centerIdx.x, number_of_voxel_3D_.x - centerIdx.x ) ),
		ForceToMax( (size_t) ceil( radius / voxel_size_.y ), Min( centerIdx.y, number_of_voxel_3D_.y - centerIdx.y ) ),
		ForceToMax( (size_t) ceil( radius / voxel_size_.z ), Min( centerIdx.z, number_of_voxel_3D_.z - centerIdx.z ) )
	};

	for( size_t x = centerIdx.x - indexDelta.x; x < centerIdx.x + indexDelta.x; x++ ){
		for( size_t y = centerIdx.y - indexDelta.y; x < centerIdx.y + indexDelta.y; y++ ){
			for( size_t z = centerIdx.z - indexDelta.z; x < centerIdx.z + indexDelta.z; z++ ){

				Point3D p( Tuple3D( (double) x * voxel_size_.x , (double) y * voxel_size_.y , (double) z * voxel_size_.z ), coordinate_system_ );

				if( ( center - p ).length() <= radius )   (*this).operator()( { x, y, z } ).AddSpecialProperty( property );
				
			}
		}
	}
}