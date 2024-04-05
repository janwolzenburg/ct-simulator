#pragma once
/*********************************************************************
 * @file   model.h
 * @brief  classes for models
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/
#include <algorithm>
#include <optional>
using std::optional;

#include "generel.h"
#include "generelMath.h"
#include "coordinateSystem.h"
#include "voxel.h"
#include "ray.h"
#include "rayScattering.h"
#include "dataGrid.h"
#include "tomography.fwd.h"



  /*********************************************************************
	Definitions
 *********************************************************************/


/*!
 * @brief class for 3D-Models
*/
class Model : public MathematicalObject{
	
	public:

	static const string FILE_PREAMBLE;		/*!< string to prepend to file when storing as file*/


	public:

	/*!
	 * @brief constructor
	 * @param coordinate_system position and orientation of model in global system
	 * @param number_of_voxel_3D	amount of voxels in each spacial dimension
	 * @param voxel_size spacial size of voxels
	 * @param name name of model
	*/
	Model( CoordinateSystem* const coordinate_system, const Index3D number_of_voxel_3D, const Tuple3D voxel_size, const string name, const VoxelData defaultData = VoxelData{} );

	/*!
	 * @brief constructor from serialized data
	 * @param binary_data reference to vector with binary data
	 * @param current_byte iterator to start of data in vector
	*/
	Model( const vector<char>& binary_data, vector<char>::const_iterator& current_byte );

	/*!
	 * @brief default constructor
	*/
	Model( void ) : Model( GetDummySystem(), Index3D{ 1, 1, 1 }, Tuple3D{ 1, 1, 1 } , string{}, VoxelData{} ){};

	/*!
	 * @brief convert model's data to string
	 * @param newline_tabulators amount of tabulators to insert after each line break
	 * @return string with model's data
	*/
	string ConvertToString( const unsigned int newline_tabulators = 0 ) const override;
	
	/*!
	 * @brief serialize this object
	 * @param binary_data reference to vector where data will be appended
	 * @return written bytes
	*/
	size_t Serialize( vector<char>& binary_data ) const;

	/*!
	 * @brief get number of voxel
	 * @return voxel amount
	*/
	Index3D number_of_voxel_3D( void ) const{ return number_of_voxel_3D_; };

	/*!
	 * @brief get size of model
	 * @return model size
	*/
	Tuple3D size( void ) const{ return size_; };

	/*!
	 * @brief get size of voxel
	 * @return voxel size
	*/
	Tuple3D voxel_size( void ) const{ return voxel_size_; };

	/*!
	 * @brief get coordinate system of model
	 * @return coordinate system of base voxel
	*/
	CoordinateSystem* coordinate_system( void ) const{ return coordinate_system_; };

	/*!
	 * @brief get range of absorption in model
	 * @return range of absorption
	*/
	NumberRange GetAbsorptionRange( void ) const{ return NumberRange{ min_absorption_, max_absorption_ + ( min_absorption_ == max_absorption_ ) ? min_absorption_ + 1e-18 : 0. }; };

	/*!
	 * @brief get model name_
	 * @return name
	*/
	string name( void ) const{ return name_; };

	/*!
	 * @brief get the longest edge
	 * @return length of longest edge
	*/
	double GetLongestEdge( void ) const{ return Max( Max( size_.x, size_.y ), size_.z ); };

	/*!
	 * @brief check whether indices are defined in model
	 * @param voxel_indices Indices to check
	 * @return true when there exists a voxel with the indices within model
	*/
	bool AreIndicesValid( const Index3D voxel_indices ) const;

	/*!
	 * @brief checks if local voxel Coordinates are defined in model
	 * @param coordinates coordinates to check
	 * @return true if Coordinates are defined in model
	*/
	bool AreCoordinatesValid( const Tuple3D coordinates ) const{
		return coordinates.x >= 0 && coordinates.y >= 0 && coordinates.z >= 0 &&
			coordinates.x <= size_.x && coordinates.y <= size_.y && coordinates.z <= size_.z; };

	/*!
	 * @brief checks if local point is inside model
	 * @param point point to check
	 * @return true when point is inside model
	*/
	bool IsPointInside( const Point3D point ) const;

	/*!
	 * @brief get voxel indices for given Coordinates in local coordinate system
	 * @param point point in coordinate system of model
	 * @return indices of voxels where Coordinates are located
	*/
	Index3D GetVoxelIndices( const Point3D point ) const;

	/*!
	 * @brief element access
	 * @param voxel_indices index of voxel
	 * @return const reference to voxel data
	*/
	VoxelData GetVoxelData( const Index3D voxel_indices ) const;

	/*!
	 * @brief access voxel data_
	 * @param point Point in model
	 * @return const reference to voxel data_
	*/
	VoxelData GetVoxelData( const Point3D point ) const{ return GetVoxelData( GetVoxelIndices( point ) ); };

	/*!
	 * @brief set voxel data
	 * @param new_voxel_data data to set
	 * @param indices indices of target voxel
	 * @return true when indices are valid
	*/
	bool SetVoxelData( const VoxelData new_voxel_data, const Index3D voxel_indices );

	/*!
	 * @brief set voxel's special properties
	 * @param properties data to set
	 * @param voxel_indices indices of target voxel
	 * @return true when indices are valid
	*/
	bool SetVoxelProperties( const VoxelData::SpecialProperty properties, const Index3D voxel_indices );

	/*!
	 * @brief get voxel instance for given indices
	 * @param voxel_indices indices of voxel
	 * @return instance of voxel
	*/
	Voxel GetVoxel( const Index3D voxel_indices ) const;

	/*!
	 * @brief get voxel describing model boundaries
	 * @return voxel as complete model
	*/
	Voxel GetModelVoxel( void ) const;

	/*!
	 * @brief calculate ray transmission through model
	 * @param ray_to_transmit ray to trace through model
	 * @param tomography_parameter Simulation parameter used in ray tracing
	 * @param scattering_properties information for ray scattering
	 * @param dedicated_rng a dedicated RNG for this thread
	 * @param disable_scattering flag to override eneabled scattering in tomography_properties
	 * @return transmitted ray and rays, whcih were scattered
	*/
	pair<Ray, vector<Ray>> TransmitRay( const Ray& ray_to_transmit, const TomographyProperties& tomography_parameter, const RayScattering& scattering_properties, 
																		  RandomNumberGenerator& dedicated_rng, const bool disable_scattering = false ) const;

	/*!
	 * @brief crop model
	 * @param corner_1 start Coordinates of new model
	 * @param corner_2 end coordinate of new model
	 * @return true if succeeded
	*/
	bool Crop( const Tuple3D corner_1, const Tuple3D corner_2 );

	/*!
	 * @brief get slice through model
	 * @param slice_location where to slice
	 * @param number_of_points amount of points along the slice axis
	 * @param forced_resolution force a resolution
	 * @return grid with slice
	*/
	DataGrid<VoxelData> GetSlice( const Surface slice_location, const GridIndex number_of_points, const optional<GridCoordinates> forced_resolution = optional<GridCoordinates>{} ) const;

	/*!
	 * @brief alter special properties in the specified sphere
	 * @param property property to add
	 * @param center center of sphere
	 * @param radius radius of sphere
	*/
	void AddSpecialSphere( const VoxelData::SpecialProperty property, const Point3D center, const double radius );


	private:

	Index3D number_of_voxel_3D_;					/*!< amount of voxels in each dimension*/
	Tuple3D voxel_size_;									/*!< voxelsize in each dimension in mm*/
	Tuple3D size_;												/*!< size of complete model in mm*/
	size_t number_of_voxel_;							/*!< absolute amount of voxels in model*/
	CoordinateSystem* coordinate_system_;	/*!< coordinate system*/
	double min_absorption_;								/*!< absorption minimum in model*/
	double max_absorption_;								/*!< absorption maximum in model*/			
	string name_;													/*!< model name*/
	vector<VoxelData> voxel_data_;				/*!< voxel data*/


	private:

	/*!
	 * @brief slice a model
	 * @param current_x_index current x index. should start at 0
	 * @param current_x_index_mutex mutex for x index
	 * @param current_y_index current y index. should start at 0
	 * @param current_y_index_mutex mutex for y index
	 * @param real_start real start of model slice. should start at {INF, INF}
	 * @param real_start_mutex mutex for real_start
	 * @param real_end real end of model slice. should start at {-INF, -INF}
	 * @param real_end_mutex mutex for real_end
	 * @param slice slice to write voxel data to
	 * @param slice_mutex mutex for slice
	 * @param slice_plane plane to slice with
	 * @param model model to slice
	 */
	static void SliceThreaded(	size_t& current_x_index, mutex& current_x_index_mutex, size_t& current_y_index, mutex& current_y_index_mutex,
								GridCoordinates& real_start, mutex& real_start_mutex, GridCoordinates& real_end, mutex& real_end_mutex,
								DataGrid<VoxelData>& slice, mutex& slice_mutex,
								const Surface& slice_plane,
								const Model& model );

	/*!
	* @brief get voxel indices for given coordinates in local coordinate system
	* @param x x-coordinate
	* @param y y-coordinate
	* @param z	z-coordinate
	* @return indices of voxels where coordinates are located
	*/
	Index3D GetVoxelIndices( const Tuple3D locCoords ) const;

	/*!
	 * @brief get data index from indices organized in three dimensions
	 * @param indices 3D indides
	 * @return one dimensional index
	*/
	size_t GetDataIndex( const Index3D indices ) const{ 
		return number_of_voxel_3D_.x * number_of_voxel_3D_.y * indices.z + number_of_voxel_3D_.x * indices.y + indices.x; };

	/*!
	 * @brief element access
	 * @param index index of voxel
	 * @return reference to voxel data
	*/
	VoxelData& operator()( const Index3D index );

};