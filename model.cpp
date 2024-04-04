
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

Model::Model( CoordinateSystem* const coordinate_system, const Index3D numVox3D_, const Tuple3D voxSize3D_, const string name_, const VoxelData defaultData ) :
	number_of_voxel_3D_( numVox3D_ ),
	voxel_size_( voxSize3D_ ),
	size_{	static_cast<double>( number_of_voxel_3D_.x ) * voxel_size_.x,
			static_cast<double>( number_of_voxel_3D_.y ) * voxel_size_.y,
			static_cast<double>( number_of_voxel_3D_.z ) * voxel_size_.z } ,
	number_of_voxel_( number_of_voxel_3D_.x * number_of_voxel_3D_.y * number_of_voxel_3D_.z ),
	coordinate_system_( coordinate_system ),
	min_absorption_( defaultData.GetAbsorptionAtReferenceEnergy() ),
	max_absorption_( defaultData.GetAbsorptionAtReferenceEnergy() ),
	name_( name_ ),
	voxel_data_( number_of_voxel_, defaultData )
{
	if( coordinate_system_->IsGlobal() ) CheckForAndOutputError( MathError::Input, "model coordinate system must be child of global system!" );
}


Model::Model( const vector<char>& binary_data, vector<char>::const_iterator& it ) :
	number_of_voxel_3D_{ binary_data, it },
	voxel_size_{ binary_data, it },
	size_{	static_cast<double>( number_of_voxel_3D_.x ) * voxel_size_.x,
			static_cast<double>( number_of_voxel_3D_.y ) * voxel_size_.y,
			static_cast<double>( number_of_voxel_3D_.z ) * voxel_size_.z } ,
	number_of_voxel_( number_of_voxel_3D_.x* number_of_voxel_3D_.y* number_of_voxel_3D_.z ),
	coordinate_system_( CoordinateSystems().AddSystem( binary_data, it ) ),
	min_absorption_( DeSerializeBuildIn<double>( 0., binary_data, it ) ),
	max_absorption_(  DeSerializeBuildIn<double>( 1., binary_data, it )  ),
	name_( DeSerializeBuildIn<string>( string{ "Default model name_"}, binary_data, it ) ),
	voxel_data_( number_of_voxel_, VoxelData{} )
{
	
	if( number_of_voxel_ * sizeof( VoxelData ) == static_cast<size_t>( binary_data.end() - it ) ){
		memcpy( voxel_data_.data(), &( *it ), number_of_voxel_ * sizeof(VoxelData));
		it += static_cast<long long int>( number_of_voxel_ * sizeof( VoxelData ) );
	}
	else{

		for( size_t i = 0; i < number_of_voxel_; i++ ){
			voxel_data_[i] = VoxelData( binary_data, it );
		}
	}
}


string Model::ConvertToString( [[maybe_unused]] const unsigned int newline_tabulators ) const{
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

Index3D Model::GetVoxelIndices( const Tuple3D local_coordinates ) const{

	if( local_coordinates.x < 0 || local_coordinates.y < 0 || 
			local_coordinates.z < 0 ){
		CheckForAndOutputError( MathError::Input, 
			"only positive coordinates allowed in model!" );
		return Index3D{ 0, 0, 0 };
	}

	Index3D indices{
		static_cast<size_t>( local_coordinates.x / voxel_size_.x ),
		static_cast<size_t>( local_coordinates.y / voxel_size_.y ),
		static_cast<size_t>( local_coordinates.z / voxel_size_.z )
	};


	// surpress error when index is exactly on the edge
	if( indices.x == number_of_voxel_3D_.x ) indices.x = number_of_voxel_3D_.x - 1;
	if( indices.y == number_of_voxel_3D_.y ) indices.y = number_of_voxel_3D_.y - 1;
	if( indices.z == number_of_voxel_3D_.z ) indices.z = number_of_voxel_3D_.z - 1;

	// are indices too big?
	if( !AreIndicesValid( indices ) )
		CheckForAndOutputError( MathError::Input, "coordinates exceed model size!" );

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

	if( newData.GetAbsorptionAtReferenceEnergy() < min_absorption_ ) min_absorption_ =  newData.GetAbsorptionAtReferenceEnergy();
	if( newData.GetAbsorptionAtReferenceEnergy() > max_absorption_ ) max_absorption_ = newData.GetAbsorptionAtReferenceEnergy() ;

	return true;
}

bool Model::SetVoxelProperties( const VoxelData::SpecialProperty property, const Index3D indices ){

	if( !AreIndicesValid( indices ) ) return false;

	this->operator()( indices ).AddSpecialProperty( property );

	return true;

}

Voxel Model::GetVoxel( const Index3D indices ) const{

	if( !AreIndicesValid( indices ) ){
		CheckForAndOutputError( MathError::Input, "indices exceed model!" );
		return Voxel{};
	}

	Point3D voxOrigin{ Tuple3D{ static_cast<double>( indices.x ) * voxel_size_.x, static_cast<double>( indices.y ) * voxel_size_.y, static_cast<double>( indices.z ) * voxel_size_.z }, coordinate_system_ };

	// get voxel data and create voxel instance
	Voxel voxel{ voxOrigin, voxel_size_, GetVoxelData( indices ) };

	return voxel;
}

pair<Ray, vector<Ray>> Model::TransmitRay( 
																const Ray& ray, 
																const TomographyProperties& tomography_properties,
																RayScattering& scattering_properties, 
																mutex& scattering_properties_mutex,
																RandomNumberGenerator& dedicated_rng,
																const bool disable_scattering ) const{

	// current ray in model's coordinate system
	Ray local_ray = std::move( ray.ConvertTo( this->coordinate_system_ ) );					

	// find entrance inside model
	const RayVoxelIntersection model_intersection{ GetModelVoxel(), local_ray };

	// return if ray does not intersect model
	if( !model_intersection.entrance_.intersection_exists_ ) return { local_ray, {} };		


	// iteration through model
	/* ------------------------------------------------------------------------------- */

	// ray parameter at model entrance plus a tiny bit
	double current_ray_step = model_intersection.entrance_.line_parameter_ + 
														simulation_properties.ray_step_size_mm;

	// return when the point on the ray is not inside the model meaning that the ray 
	// just barely hit the model
	if( !IsPointInside( local_ray.GetPoint( current_ray_step ) ) ) 
		return { local_ray, {} };

	// current point on the ray is model entrance
	Point3D current_point_on_ray = std::move( local_ray.GetPoint( current_ray_step ) );

	#ifdef TRANSMISSION_TRACKING
	if( !IsPointInside( local_ray.origin() ) ){
		local_ray.ray_tracing().tracing_steps.emplace_back( false, 
			model_intersection.entrance_.line_parameter_, GetModelVoxel().data(), 
			local_ray.origin(), model_intersection.entrance_.intersection_point_, 
			local_ray.properties().simple_intensity(), 
			local_ray.properties().energy_spectrum().GetTotalPower(),
			local_ray.properties().energy_spectrum() );
	}
	#endif

	vector<Ray> all_scattered_rays;	// vector for all scattered rays

	// the possible exit faces of voxel
	const array<bool, ConvertToUnderlying( Voxel::Face::End )> possible_exit_faces = 
		std::move( local_ray.GetPossibleVoxelExits() );

	// iterate through model while current point is inside model
	while( IsPointInside( current_point_on_ray ) ){

		// indices of current voxel
		const Index3D current_voxel_indices = GetVoxelIndices( current_point_on_ray );				
		double distance_to_exit = INFINITY;		// the smallest ray parameter

		// iterate all possible faces and get the ray parameter at intersection
		for( Voxel::Face current_face = Voxel::Face::Begin; 
			   current_face < Voxel::Face::End; ++current_face ){

			// distance to the current face. Will become finite if exit found
			double distance_to_current_face = INFINITY;			
			double exit_face_position;											// face position along one axis

			// switch faces. check only the possible faces
			switch( current_face ){

				case Voxel::Face::YZ_Xp:
					if( possible_exit_faces.at( ConvertToUnderlying( current_face ) ) == false ) break;
					exit_face_position = ( static_cast<double>( current_voxel_indices.x ) + 1. ) *
															 this->voxel_size_.x;		// position of positive yz-plane
					distance_to_current_face = ( exit_face_position - current_point_on_ray.X() ) /
																		 local_ray.direction().X();
					break;

				case Voxel::Face::YZ_Xm:
					if( possible_exit_faces.at( ConvertToUnderlying( current_face ) ) == false ) break;
					exit_face_position = ( static_cast<double>( current_voxel_indices.x ) ) * 
															 this->voxel_size_.x;		// position of negative yz-plane
					distance_to_current_face = ( exit_face_position - current_point_on_ray.X() ) / 
																		 local_ray.direction().X();
					break;

				case Voxel::Face::XZ_Yp:
					if( possible_exit_faces.at( ConvertToUnderlying( current_face ) ) == false ) break;
					exit_face_position = ( static_cast<double>( current_voxel_indices.y ) + 1. ) * 
															 this->voxel_size_.y;		// position of positive xz-plane
					distance_to_current_face = ( exit_face_position - current_point_on_ray.Y() ) / 
																		 local_ray.direction().Y();
					break;

				case Voxel::Face::XZ_Ym:
					if( possible_exit_faces.at( ConvertToUnderlying( current_face ) ) == false ) break;
					exit_face_position = ( static_cast<double>( current_voxel_indices.y ) ) * 
															 this->voxel_size_.y;		// position of negative xz-plane
					distance_to_current_face = ( exit_face_position - current_point_on_ray.Y() ) / 
																		 local_ray.direction().Y();
					break;

				case Voxel::Face::XY_Zp:
					if( possible_exit_faces.at( ConvertToUnderlying( current_face ) ) == false ) break;
					exit_face_position = ( static_cast<double>( current_voxel_indices.z ) + 1. ) * 
															 this->voxel_size_.z;		// position of positive xy-plane
					distance_to_current_face = ( exit_face_position - current_point_on_ray.Z() ) / 
																		 local_ray.direction().Z();
					break;

				case Voxel::Face::XY_Zm:
					if( possible_exit_faces.at( ConvertToUnderlying( current_face ) ) == false ) break;
					exit_face_position = ( static_cast<double>( current_voxel_indices.z )   ) * 
															 this->voxel_size_.z;		// position of negative xy-plane
					distance_to_current_face = ( exit_face_position - current_point_on_ray.Z() ) / 
																		 local_ray.direction().Z();
					break;

				default: break;
			}

			// there may be more than one intersectiones with the unbounded faces
			// the intersected face with the smallest distance from current point along the ray
			// is the exit face
			if( distance_to_current_face < distance_to_exit ) 
				distance_to_exit = distance_to_current_face;

		}
		
		// if exit found
		if( distance_to_exit < INFINITY ){

			// the distance traveled in this voxel. add step size, because ray origin is
			// this amount inside the voxel
			const double distance_in_voxel = distance_to_exit +
																			 simulation_properties.ray_step_size_mm;		

			// the current voxel's properties
			const VoxelData current_voxel_data = this->GetVoxelData( current_voxel_indices );

			// update ray's properties with distance traveled in current voxel
			local_ray.UpdateProperties( current_voxel_data, distance_in_voxel );
			local_ray.IncrementHitCounter();

			#ifdef TRANSMISSION_TRACKING
			local_ray.ray_tracing().tracing_steps.emplace_back( true, 
					distance_in_voxel, current_voxel_data, current_point_on_ray, 
					local_ray.GetPointFast( current_ray_step ), 
					local_ray.properties().simple_intensity(), 
					local_ray.properties().energy_spectrum().GetTotalPower(),
					local_ray.properties().energy_spectrum() );
			#endif

			// new ray parameter the distance travelled in this voxel
			current_ray_step += distance_in_voxel;				

			// the new point on the ray
			current_point_on_ray = std::move( local_ray.GetPointFast( current_ray_step ) );

			// scattering. only when enabled, not overriden and current point is inside model
			if( tomography_properties.scattering_enabled && 
				  !disable_scattering && 
					IsPointInside( current_point_on_ray ) ){

				// scatter the ray
				const vector<Ray> scattered_rays = std::move( 
					local_ray.Scatter( 
														 scattering_properties, scattering_properties_mutex,
														 current_voxel_data, distance_in_voxel, 
														 tomography_properties, current_point_on_ray,
														 dedicated_rng ) );

				// append scattered rays
				all_scattered_rays.insert( all_scattered_rays.end(), 
																	 make_move_iterator( scattered_rays.begin() ), 
																	 make_move_iterator( scattered_rays.end() ) );
			}
		}
	}

	// ray is now outside the model
	/* ------------------------------------------------------------------------------- */

	// new origin of ray "outside" the model
	local_ray.origin( current_point_on_ray );

	#ifdef TRANSMISSION_TRACKING
	local_ray.ray_tracing().tracing_steps.emplace_back( false, 
		-1, GetModelVoxel().data(), current_point_on_ray, current_point_on_ray, 
		local_ray.properties().simple_intensity(), 
		local_ray.properties().energy_spectrum().GetTotalPower(),
		local_ray.properties().energy_spectrum() );
	#endif

	return { local_ray, all_scattered_rays };
}

bool Model::Crop( const Tuple3D minCoords, const Tuple3D maxCoords ){
	if( !AreCoordinatesValid( minCoords ) || !AreCoordinatesValid( maxCoords ) ) return false;

	Index3D minIdcs = GetVoxelIndices( minCoords );			// minimum boundary indices
	Index3D maxIdcs = GetVoxelIndices( maxCoords );			// maximum boundary indices

	// parameters of cropped model
	Index3D newVoxNum3D{ maxIdcs.x - minIdcs.x + 1, maxIdcs.y - minIdcs.y + 1, maxIdcs.z - minIdcs.z + 1 };

	// new model
	Model newModel{ coordinate_system_, newVoxNum3D, voxel_size_, name_, VoxelData{} };


	// copy data to new model
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

	size_t expectedSize = 0;
	expectedSize += sizeof( number_of_voxel_3D_ );
	expectedSize += sizeof( voxel_size_ );
	expectedSize += sizeof( CoordinateSystem );
	expectedSize += 2* sizeof( min_absorption_ );
	expectedSize += name_.size() + 1;
	expectedSize += number_of_voxel_ * sizeof( voxel_data_.front() );

	binary_data.reserve( expectedSize );

	size_t num_bytes = 0;
	num_bytes += number_of_voxel_3D_.Serialize( binary_data );
	num_bytes += voxel_size_.Serialize( binary_data );
	num_bytes += coordinate_system_->Serialize( binary_data );
	num_bytes += SerializeBuildIn<double>( min_absorption_, binary_data );
	num_bytes += SerializeBuildIn<double>( max_absorption_, binary_data );
	num_bytes += SerializeBuildIn<string>( name_, binary_data );

	
	binary_data.insert( binary_data.end(), (char*) voxel_data_.data(), (char*) voxel_data_.data() + sizeof(VoxelData) * number_of_voxel_);
	//binary_data.insert( binary_data.end(), parameter.cbegin(), parameter.cend() );

	return num_bytes;

}


void Model::SliceThreaded(	size_t& xIdx, mutex& currentXMutex, size_t& yIdx, mutex& currentYMutex,
									GridCoordinates& realStart, mutex& realStartMutex, GridCoordinates& realEnd, mutex& realEndMutex,
									DataGrid<VoxelData>& slice, mutex& sliceMutex,
									const Surface& slicePlane,
									const Model& modelRef ){

	// iterate through all columns
	while( xIdx < slice.size().c ){

		size_t localXIdx, localYIdx;

		// lock
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

		// unlock
		currentXMutex.unlock(); currentYMutex.unlock();


		// check 
		if( localXIdx > slice.size().c - 1 || localYIdx > slice.size().r - 1 ) continue;
		
		// grid indices
		const GridIndex gridIndices( localXIdx, localYIdx );

		// get point on surface for current grid indices
		const GridCoordinates surfaceCoordinate = slice.GetCoordinates( gridIndices );
		const Point3D currentPoint = slicePlane.GetPoint( surfaceCoordinate.c , surfaceCoordinate.r  );


		// are cooradinates defined in model?
		if( !modelRef.IsPointInside( currentPoint ) ){
			slice.SetData( gridIndices, VoxelData( 0., 1., VoxelData::Undefined ) );
			continue;	// goto next iteration
		}


		// check if current x and y values in plane are new real start/end

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

		// current voxel value
		const VoxelData currentValue = modelRef.GetVoxelData( currentPoint );

		// add pixel Coordinates and pixel value to slice
		sliceMutex.lock();
		slice.SetData( gridIndices, currentValue );
		sliceMutex.unlock();


	}

}

DataGrid<VoxelData> Model::GetSlice( const Surface sliceLocation, const GridIndex number_of_points, const optional<GridCoordinates> forced_resolution ) const{

	// surface in model's system
	const Surface localSurface = sliceLocation.ConvertTo( coordinate_system_ );
	 
	 GridCoordinates sliceStart;
	 GridCoordinates sliceEnd;
	 GridCoordinates sliceResolution;

	if( !forced_resolution.has_value() ){
		// distance between corners furthest away from each other
		double cornerDistance = size_.x + size_.y + size_.z; //sqrt( pow( size_.x, 2. ) + pow( size_.y, 2. ) + pow( size_.z, 2. ) );
		// worst case: origin_ of plane at one corner and plane orianted in a way that it just slices corner on the other side of model cube

		size_t Mmax_number_of_points = Max( number_of_points.c, number_of_points.r );
		sliceStart = { - cornerDistance / 2., -cornerDistance / 2. };
		sliceEnd = { cornerDistance / 2., cornerDistance / 2. };
		sliceResolution = { ( sliceEnd.c - sliceStart.c ) / Mmax_number_of_points / 4., ( sliceEnd.r - sliceStart.r ) / Mmax_number_of_points / 4. };

	}
	else{

		sliceResolution = forced_resolution.value();
		sliceStart = {	- ( static_cast<double>( number_of_points.c - 1) * sliceResolution.c ) / 2.,
						- ( static_cast<double>( number_of_points.r - 1) * sliceResolution.r ) / 2. };
		sliceEnd = {	  ( static_cast<double>( number_of_points.c - 1) * sliceResolution.c ) / 2.,
						  ( static_cast<double>( number_of_points.r - 1) * sliceResolution.r ) / 2. };
	}


	DataGrid<VoxelData> largeSlice{ NumberRange( sliceStart.c, sliceEnd.c ), NumberRange( sliceStart.r, sliceEnd.r ), sliceResolution, VoxelData() };

	// update Slice start, end and resolution because grid is discrete and fits the end and resolution the its range
	sliceStart = largeSlice.start();
	sliceEnd = largeSlice.GetEnd();
	sliceResolution = largeSlice.resolution();


	GridCoordinates realStart( INFINITY, INFINITY );
	GridCoordinates realEnd( -INFINITY, -INFINITY );

	size_t xIdx = 0, yIdx = 0;

	mutex currentXMutex, currentYMutex;
	mutex sliceMutex;

	mutex realStartMutex, realEndMutex;


	// computation in threads
	vector<std::thread> threads;

	for( size_t threadIdx = 0; threadIdx < std::thread::hardware_concurrency(); threadIdx++ ){
		threads.emplace_back( SliceThreaded,	ref( xIdx ), ref( currentXMutex ), ref( yIdx ), ref( currentYMutex ),
													ref( realStart ), ref( realStartMutex), ref( realEnd ), ref( realEndMutex ),
													ref( largeSlice ), ref( sliceMutex ),
													cref( localSurface ), cref( *this ) );
	}

	for( std::thread& currentThread : threads ) currentThread.join();

	// check if restart and end have infinity in them
	if( realStart.c == INFINITY || realStart.r == INFINITY || realEnd.c == -INFINITY || realEnd.r == -INFINITY ){
		// this means that no model voxel is in slice
		return DataGrid<VoxelData>{ GridIndex{ 0, 0 }, GridCoordinates{ 0., 0. }, GridCoordinates{ 1., 1. } };

	}

	// write data to smaller grid
	DataGrid<VoxelData> slice{ NumberRange{ realStart.c, realEnd.c }, NumberRange{ realStart.r, realEnd.r }, sliceResolution, VoxelData() };


	// iterate grid
	GridCoordinates coords;
	VoxelData currentData;

	for( size_t colIdx = 0; colIdx < slice.size().c; colIdx++ ){

		for( size_t rowIdx = 0; rowIdx < slice.size().r; rowIdx++ ){

			coords = slice.GetCoordinates( GridIndex( colIdx, rowIdx ) );
			currentData = largeSlice.GetData( coords );

			if( currentData.HasSpecificProperty( VoxelData::Undefined ) )
				slice.SetData( coords, VoxelData{ largeSlice.max_value().GetAbsorptionAtReferenceEnergy(), reference_energy_for_mu_eV, VoxelData::Undefined } );
			else
				slice.SetData( coords, currentData );
		}
	}


	return slice;
}


void Model::AddSpecialSphere( const VoxelData::SpecialProperty property, const Point3D center, const double radius ){
	
	// exit when coords invalid
	if ( !IsPointInside( center ) ) return;
	
	// center indices
	const Index3D centerIdx = GetVoxelIndices( center );

	// index distance from center in each dimension
	Index3D indexDelta{
		ForceToMax( static_cast<size_t>( ceil( radius / voxel_size_.x ) ), Min( centerIdx.x, number_of_voxel_3D_.x - centerIdx.x ) ),
		ForceToMax( static_cast<size_t>( ceil( radius / voxel_size_.y ) ), Min( centerIdx.y, number_of_voxel_3D_.y - centerIdx.y ) ),
		ForceToMax( static_cast<size_t>( ceil( radius / voxel_size_.z ) ), Min( centerIdx.z, number_of_voxel_3D_.z - centerIdx.z ) )
	};

	for( size_t x = centerIdx.x - indexDelta.x; x < centerIdx.x + indexDelta.x; x++ ){
		for( size_t y = centerIdx.y - indexDelta.y; x < centerIdx.y + indexDelta.y; y++ ){
			for( size_t z = centerIdx.z - indexDelta.z; x < centerIdx.z + indexDelta.z; z++ ){

				Point3D p( Tuple3D( static_cast<double>( x ) * voxel_size_.x , static_cast<double>( y ) * voxel_size_.y , static_cast<double>( z ) * voxel_size_.z ), coordinate_system_ );

				if( ( center - p ).length() <= radius )   (*this).operator()( { x, y, z } ).AddSpecialProperty( property );
				
			}
		}
	}
}