/*********************************************************************
 * @file   model.cpp
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

Model::Model( CoordinateSystem* const coordinate_system, const Index3D number_of_voxel_3D, const Tuple3D voxel_size, const string name, const VoxelData default_data ) :
	number_of_voxel_3D_( number_of_voxel_3D ),
	voxel_size_( voxel_size ),
	size_{	static_cast<double>( number_of_voxel_3D_.x ) * voxel_size_.x,
			static_cast<double>( number_of_voxel_3D_.y ) * voxel_size_.y,
			static_cast<double>( number_of_voxel_3D_.z ) * voxel_size_.z } ,
	number_of_voxel_( number_of_voxel_3D_.x * number_of_voxel_3D_.y * number_of_voxel_3D_.z ),
	coordinate_system_( coordinate_system ),
	min_absorption_( default_data.GetAbsorptionAtReferenceEnergy() ),
	max_absorption_( default_data.GetAbsorptionAtReferenceEnergy() ),
	name_( name ),
	voxel_data_( number_of_voxel_, default_data )
{
	if( coordinate_system_->IsGlobal() ) CheckForAndOutputError( MathError::Input, "model coordinate system must be child of global system!" );
}


Model::Model( const vector<char>& binary_data, vector<char>::const_iterator& current_byte ) :
	number_of_voxel_3D_{ binary_data, current_byte },
	voxel_size_{ binary_data, current_byte },
	size_{	static_cast<double>( number_of_voxel_3D_.x ) * voxel_size_.x,
			static_cast<double>( number_of_voxel_3D_.y ) * voxel_size_.y,
			static_cast<double>( number_of_voxel_3D_.z ) * voxel_size_.z } ,
	number_of_voxel_( number_of_voxel_3D_.x* number_of_voxel_3D_.y* number_of_voxel_3D_.z ),
	coordinate_system_( GetCoordinateSystemTree().AddSystem( binary_data, current_byte ) ),
	min_absorption_( DeSerializeBuildIn<double>( 0., binary_data, current_byte ) ),
	max_absorption_(  DeSerializeBuildIn<double>( 1., binary_data, current_byte )  ),
	name_( DeSerializeBuildIn<string>( string{ "Default model name"}, binary_data, current_byte ) ),
	voxel_data_( number_of_voxel_, VoxelData{} )
{
	
	if( number_of_voxel_ * sizeof( VoxelData ) == static_cast<size_t>( binary_data.end() - current_byte ) ){
		memcpy( voxel_data_.data(), &( *current_byte ), number_of_voxel_ * sizeof(VoxelData));
		current_byte += static_cast<long long int>( number_of_voxel_ * sizeof( VoxelData ) );
	}
	else{

		for( size_t i = 0; i < number_of_voxel_; i++ ){
			voxel_data_[i] = VoxelData( binary_data, current_byte );
		}
	}
}


string Model::ConvertToString( [[maybe_unused]] const unsigned int newline_tabulators ) const{
	return string{ "" };
}

Voxel Model::GetModelVoxel( void ) const{
	return  Voxel{ Point3D{Tuple3D { 0, 0, 0 }, coordinate_system_}, size_, VoxelData{} };
}

VoxelData Model::GetVoxelData( const Index3D voxel_indices ) const{
	if( !AreIndicesValid( voxel_indices ) ){ 
		CheckForAndOutputError( MathError::Input, "voxel_indices exceeds model size!" ); 
		return VoxelData{}; 
	}
	
	return voxel_data_.at( GetDataIndex( voxel_indices ) );
}

VoxelData& Model::operator()( const Index3D voxel_indices ){
	if( !AreIndicesValid( voxel_indices ) ){ 
		CheckForAndOutputError( MathError::Input, "voxel_indices exceeds model size!" ); 
		return voxel_data_.at( number_of_voxel_ - 1 ); 
	}
	
	return voxel_data_.at( GetDataIndex( voxel_indices ) );
}

bool Model::AreIndicesValid( const Index3D voxel_indices ) const{
	if( voxel_indices.x >= number_of_voxel_3D_.x ||
		voxel_indices.y >= number_of_voxel_3D_.y ||
		voxel_indices.z >= number_of_voxel_3D_.z ){
		return false;
	}
	return true;
}

bool Model::IsPointInside( const Point3D point ) const{
	return AreCoordinatesValid( point.GetComponents( coordinate_system_ ) );
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

Index3D Model::GetVoxelIndices( const Point3D point ) const{
	return GetVoxelIndices( point.GetComponents( coordinate_system_ ) );
}

bool Model::SetVoxelData( const VoxelData new_voxel_data, const Index3D voxel_indices ){

	if( !AreIndicesValid( voxel_indices ) ) return false;

	this->operator()( voxel_indices ) = new_voxel_data;

	if( new_voxel_data.GetAbsorptionAtReferenceEnergy() < min_absorption_ ) min_absorption_ =  new_voxel_data.GetAbsorptionAtReferenceEnergy();
	if( new_voxel_data.GetAbsorptionAtReferenceEnergy() > max_absorption_ ) max_absorption_ = new_voxel_data.GetAbsorptionAtReferenceEnergy() ;

	return true;
}

bool Model::SetVoxelProperties( const SpecialProperty property, const Index3D voxel_indices ){

	if( !AreIndicesValid( voxel_indices ) ) return false;

	this->operator()( voxel_indices ).AddSpecialProperty( property );

	return true;

}

Voxel Model::GetVoxel( const Index3D voxel_indices ) const{

	if( !AreIndicesValid( voxel_indices ) ){
		CheckForAndOutputError( MathError::Input, "voxel_indices exceed model!" );
		return Voxel{};
	}

	const Point3D voxel_origin{ Tuple3D{ static_cast<double>( voxel_indices.x ) * voxel_size_.x, static_cast<double>( voxel_indices.y ) * voxel_size_.y, static_cast<double>( voxel_indices.z ) * voxel_size_.z }, coordinate_system_ };

	// get voxel data and create voxel instance
	const Voxel voxel{ voxel_origin, voxel_size_, GetVoxelData( voxel_indices ) };

	return voxel;
}

pair<Ray, vector<Ray>> Model::TransmitRay( 
																const Ray& ray, 
																const TomographyProperties& tomography_properties,
																const RayScattering& scattering_properties,
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
														 scattering_properties,
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

bool Model::Crop( const Tuple3D corner_1, const Tuple3D corner_2 ){
	if( !AreCoordinatesValid( corner_1 ) || !AreCoordinatesValid( corner_2 ) ) return false;

	const Index3D corner_1_indices = GetVoxelIndices( corner_1 );			// minimum boundary indices
	const Index3D corner_2_indices = GetVoxelIndices( corner_2 );			// maximum boundary indices

	// parameters of cropped model
	const Index3D new_number_of_voxel{ corner_2_indices.x - corner_1_indices.x + 1, corner_2_indices.y - corner_1_indices.y + 1, corner_2_indices.z - corner_1_indices.z + 1 };

	// new model
	Model cropped_model{ coordinate_system_, new_number_of_voxel, voxel_size_, name_, VoxelData{} };


	// copy data to new model
	for( size_t z = 0; z < new_number_of_voxel.z; z++ ){
		for( size_t y = 0; y < new_number_of_voxel.y; y++ ){
			for( size_t x = 0; x < new_number_of_voxel.x; x++ ){
				cropped_model.SetVoxelData( GetVoxelData( { x + corner_1_indices.x, y + corner_1_indices.y, z + corner_1_indices.z } ), { x, y, z } );
			}
		}
	}

	*this = std::move( cropped_model );

	return true;
}


size_t Model::Serialize( vector<char>& binary_data ) const{

	size_t expected_size = 0;
	expected_size += sizeof( number_of_voxel_3D_ );
	expected_size += sizeof( voxel_size_ );
	expected_size += sizeof( CoordinateSystem );
	expected_size += 2* sizeof( min_absorption_ );
	expected_size += name_.size() + 1;
	expected_size += number_of_voxel_ * sizeof( voxel_data_.front() );

	binary_data.reserve( expected_size );

	size_t number_of_bytes = 0;
	number_of_bytes += number_of_voxel_3D_.Serialize( binary_data );
	number_of_bytes += voxel_size_.Serialize( binary_data );
	number_of_bytes += coordinate_system_->Serialize( binary_data );
	number_of_bytes += SerializeBuildIn<double>( min_absorption_, binary_data );
	number_of_bytes += SerializeBuildIn<double>( max_absorption_, binary_data );
	number_of_bytes += SerializeBuildIn<string>( name_, binary_data );

	
	binary_data.insert( binary_data.end(), (char*) voxel_data_.data(), (char*) voxel_data_.data() + sizeof(VoxelData) * number_of_voxel_);

	return number_of_bytes;

}


void Model::SliceThreaded( size_t& current_x_index, mutex& current_x_index_mutex, size_t& current_y_index, mutex& current_y_index_mutex,
													 GridCoordinates& real_start, mutex& real_start_mutex, GridCoordinates& real_end, mutex& real_end_mutex,
													 DataGrid<VoxelData>& slice, mutex& slice_mutex,
													 const Surface& slice_plane,
													 const Model& model ){

	// iterate through all columns
	while( current_x_index < slice.size().c ){

		size_t local_x_index, local_y_index;

		// lock
		current_x_index_mutex.lock(); current_y_index_mutex.lock();

		local_x_index = current_x_index;
		local_y_index = current_y_index;

		if( current_y_index >= slice.size().r ){
			current_x_index++;
			current_y_index = 0;
		}
		else{
			current_y_index++;
		}

		// unlock
		current_x_index_mutex.unlock(); current_y_index_mutex.unlock();


		// check 
		if( local_x_index > slice.size().c - 1 || local_y_index > slice.size().r - 1 ) continue;
		
		// grid indices
		const GridIndex grid_indices( local_x_index, local_y_index );

		// get point on surface for current grid indices
		const GridCoordinates surface_coordinates = slice.GetCoordinates( grid_indices );
		const Point3D current_point_on_surface = slice_plane.GetPoint( surface_coordinates.c , surface_coordinates.r  );


		// are cooradinates defined in model?
		if( !model.IsPointInside( current_point_on_surface ) ){
			slice.SetData( grid_indices, VoxelData( 0., 1., SpecialProperty::Undefined ) );
			continue;	// goto next iteration
		}


		// check if current x and y values in plane are new real start/end

		if( surface_coordinates.c < real_start.c ){
			WriteThreadVar( real_start.c, surface_coordinates.c, real_start_mutex );
		}
		
		if( surface_coordinates.c > real_end.c ){
			WriteThreadVar( real_end.c, surface_coordinates.c, real_end_mutex );
		}

		if( surface_coordinates.r < real_start.r ){
			WriteThreadVar( real_start.r, surface_coordinates.r, real_start_mutex );
		}

		if( surface_coordinates.r > real_end.r ){
			WriteThreadVar( real_end.r, surface_coordinates.r, real_end_mutex );
		}

		// current voxel value
		const VoxelData current_voxel_data = model.GetVoxelData( current_point_on_surface );

		// add pixel Coordinates and pixel value to slice
		slice_mutex.lock();
		slice.SetData( grid_indices, current_voxel_data );
		slice_mutex.unlock();


	}

}

DataGrid<VoxelData> Model::GetSlice( const Surface slice_location, const GridIndex number_of_points, const optional<GridCoordinates> forced_resolution ) const{

	// surface in model's system
	const Surface local_surface = slice_location.ConvertTo( coordinate_system_ );
	 
	 GridCoordinates slice_start;
	 GridCoordinates slice_end;
	 GridCoordinates slice_resolution;

	if( !forced_resolution.has_value() ){
		// distance between corners furthest away from each other
		const double corner_distance = size_.x + size_.y + size_.z;
		// worst case: origin_ of plane at one corner and plane orianted in a way that current_byte just slices corner on the other side of model cube

		const size_t max_number_of_points = Max( number_of_points.c, number_of_points.r );
		slice_start = { - corner_distance / 2., -corner_distance / 2. };
		slice_end = { corner_distance / 2., corner_distance / 2. };
		slice_resolution = { ( slice_end.c - slice_start.c ) / static_cast<double>( max_number_of_points ) / 4., ( slice_end.r - slice_start.r ) / static_cast<double>( max_number_of_points ) / 4. };

	}
	else{

		slice_resolution = forced_resolution.value();
		slice_start = {	- ( static_cast<double>( number_of_points.c - 1) * slice_resolution.c ) / 2.,
						- ( static_cast<double>( number_of_points.r - 1) * slice_resolution.r ) / 2. };
		slice_end = {	  ( static_cast<double>( number_of_points.c - 1) * slice_resolution.c ) / 2.,
						  ( static_cast<double>( number_of_points.r - 1) * slice_resolution.r ) / 2. };
	}


	DataGrid<VoxelData> large_slice{ NumberRange( slice_start.c, slice_end.c ), NumberRange( slice_start.r, slice_end.r ), slice_resolution, VoxelData() };

	// update Slice start, end and resolution because grid is discrete and fits the end and resolution the its range
	slice_start = large_slice.start();
	slice_end = large_slice.GetEnd();
	slice_resolution = large_slice.resolution();


	GridCoordinates real_start( INFINITY, INFINITY );
	GridCoordinates real_end( -INFINITY, -INFINITY );
	mutex real_start_mutex, real_end_mutex;

	size_t current_x_index = 0, current_y_index = 0;
	mutex current_x_index_mutex, current_y_index_mutex;

	mutex slice_mutex;


	// computation in threads
	vector<std::thread> threads;

	for( size_t thread_index = 0; thread_index < std::thread::hardware_concurrency(); thread_index++ ){
		threads.emplace_back( SliceThreaded,	ref( current_x_index ), ref( current_x_index_mutex ), ref( current_y_index ), ref( current_y_index_mutex ),
													ref( real_start ), ref( real_start_mutex), ref( real_end ), ref( real_end_mutex ),
													ref( large_slice ), ref( slice_mutex ),
													cref( local_surface ), cref( *this ) );
	}

	for( std::thread& current_thread : threads ) current_thread.join();

	// check if restart and end have infinity in them
	if( real_start.c == INFINITY || real_start.r == INFINITY || real_end.c == -INFINITY || real_end.r == -INFINITY ){
		// this means that no model voxel is in slice
		return DataGrid<VoxelData>{ GridIndex{ 0, 0 }, GridCoordinates{ 0., 0. }, GridCoordinates{ 1., 1. } };

	}

	// write data to smaller grid
	DataGrid<VoxelData> slice{ NumberRange{ real_start.c, real_end.c }, NumberRange{ real_start.r, real_end.r }, slice_resolution, VoxelData() };


	// iterate grid
	GridCoordinates coordinates;
	VoxelData current_voxel_data;

	for( size_t column_index = 0; column_index < slice.size().c; column_index++ ){

		for( size_t row_index = 0; row_index < slice.size().r; row_index++ ){

			coordinates = slice.GetCoordinates( GridIndex( column_index, row_index ) );
			current_voxel_data = large_slice.GetData( coordinates );

			if( current_voxel_data.HasSpecificProperty( SpecialProperty::Undefined ) )
				slice.SetData( coordinates, VoxelData{ large_slice.max_value().GetAbsorptionAtReferenceEnergy(), reference_energy_for_mu_eV, SpecialProperty::Undefined } );
			else
				slice.SetData( coordinates, current_voxel_data );
		}
	}


	return slice;
}


void Model::AddSpecialSphere( const SpecialProperty property, const Point3D center, const double radius ){
	
	// exit when coordinates invalid
	if ( !IsPointInside( center ) ) return;
	
	// center indices
	const Index3D center_index = GetVoxelIndices( center );

	// index distance from center in each dimension
	Index3D index_delta{
		ForceToMax( static_cast<size_t>( ceil( radius / voxel_size_.x ) ), Min( center_index.x, number_of_voxel_3D_.x - center_index.x ) ),
		ForceToMax( static_cast<size_t>( ceil( radius / voxel_size_.y ) ), Min( center_index.y, number_of_voxel_3D_.y - center_index.y ) ),
		ForceToMax( static_cast<size_t>( ceil( radius / voxel_size_.z ) ), Min( center_index.z, number_of_voxel_3D_.z - center_index.z ) )
	};

	for( size_t x = center_index.x - index_delta.x; x < center_index.x + index_delta.x; x++ ){
		for( size_t y = center_index.y - index_delta.y; x < center_index.y + index_delta.y; y++ ){
			for( size_t z = center_index.z - index_delta.z; x < center_index.z + index_delta.z; z++ ){

				const Point3D current_point{ Tuple3D( static_cast<double>( x ) * voxel_size_.x, static_cast<double>( y ) * voxel_size_.y, static_cast<double>( z ) * voxel_size_.z ), coordinate_system_ };

				if( ( center - current_point ).length() <= radius )   (*this).operator()( { x, y, z } ).AddSpecialProperty( property );
				
			}
		}
	}
}