#pragma once
/*********************************************************************
 * @file   model.h
 * @brief  Classes for models
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/
#include <algorithm>

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
 * @brief Class for 3D-Models
*/
class Model : public MathematicalObject{
	
	public:

	static const string FILE_PREAMBLE; /*!<String to prepend to file when storing as file*/		/*!<String at the beginning of file with exported model*/


	public:

	/*!
	 * @brief Constructor
	 * @param coordinate_system Position and orientation of model in global system
	 * @param number_of_voxel_3D	Amount of voxels in each spacial dimension
	 * @param voxel_size Spacial size of voxels
	 * @param name Name of model
	*/
	Model( CoordinateSystem* const coordinate_system, const Index3D number_of_voxel_3D, const Tuple3D voxel_size, const string name = "Default model name_" );

	/*!
	 * @brief Constructor from serialized data
	 * @param binary_data Reference to vector with binary data
	 * @param current_byte Iterator to start of data in vector
	*/
	Model( const vector<char>& binary_data, vector<char>::const_iterator& current_byte );

	/*!
	 * @brief Default constructor
	*/
	Model( void ) : Model( DummySystem(), Index3D{ 1, 1, 1 }, Tuple3D{ 1, 1, 1 } ){};

	/*!
	 * @brief Convert model's data to string
	 * @param newline_tabulators Amount of tabulators to insert after each Line break
	 * @return String with model's data
	*/
	string ToString( const unsigned int newline_tabulators = 0 ) const override;
	
	/*!
	 * @brief Serialize this object
	 * @param binary_data Reference to vector where data will be appended
	*/
	size_t Serialize( vector<char>& binary_data ) const;

	/*!
	 * @brief Get number_of_pixel of voxels
	 * @return Voxel amount
	*/
	Index3D number_of_voxel_3D( void ) const{ return number_of_voxel_3D_; };

	/*!
	 * @brief Get size of model
	 * @return Model size
	*/
	Tuple3D size( void ) const{ return size_; };

	/*!
	 * @brief Get size of voxel
	 * @return Voxel size
	*/
	Tuple3D voxel_size( void ) const{ return voxel_size_; };

	/*!
	 * @brief Get coordinate system of model
	 * @return coordinate system of base voxel
	*/
	CoordinateSystem* coordinate_system( void ) const{ return coordinate_system_; };

	/*!
	 * @brief Get range of attenuation in model
	 * @return Range of attenuation
	*/
	NumberRange attenuationRange( void ) const{ return NumberRange{ min_attenuation_, max_attenuation_ }; };

	/*!
	 * @brief Get model name_
	 * @return Name
	*/
	string name( void ) const{ return name_; };

	/*!
	 * @brief Get the longest edge
	 * @return Length of longest edge
	*/
	double GetLongestEdge( void ) const{ return Max( Max( size_.x, size_.y ), size_.z ); };

	/*!
	 * @brief Check whether indices are defined in model
	 * @param voxel_indices Indices to check
	 * @return True when there exists a voxel with the indices within model
	*/
	bool AreIndicesValid( const Index3D voxel_indices ) const;

	/*!
	 * @brief Checks if local voxel Coordinates are defined in model
	 * @param coordinates Coordinates to check
	 * @return True if Coordinates are defined in model
	*/
	bool AreCoordinatesValid( const Tuple3D coordinates ) const{
		return coordinates.x >= 0 && coordinates.y >= 0 && coordinates.z >= 0 &&
			coordinates.x < size_.x && coordinates.y < size_.y && coordinates.z < size_.z; };

	/*!
	 * @brief Checks if local point is inside model
	 * @param point Point to check
	 * @return True when point is inside model
	*/
	bool IsPointInside( const Point3D point ) const;

	/*!
	 * @brief Get voxel indices for given Coordinates in local coordinate system
	 * @param point Point in coordinate system of model
	 * @return Indices of voxels where Coordinates are located
	*/
	Index3D GetVoxelIndices( const Point3D point ) const;

	/*!
	 * @brief Element access
	 * @param voxel_indices index of voxel
	 * @return Const reference to voxel data_
	*/
	VoxelData GetVoxelData( const Index3D voxel_indices ) const;

	/*!
	 * @brief Access voxel data_
	 * @param point Point in model
	 * @return Const reference to voxel data_
	*/
	VoxelData GetVoxelData( const Point3D point ) const{ return GetVoxelData( GetVoxelIndices( point ) ); };

	/*!
	 * @brief Set voxel data_
	 * @param new_voxel_data Data to set
	 * @param indices Indices of target voxel
	 * @return True when indices are valid
	*/
	bool SetVoxelData( const VoxelData new_voxel_data, const Index3D voxel_indices );

	/*!
	 * @brief Set voxel's special properties
	 * @param properties Data to set
	 * @param voxel_indices Indices of target voxel
	 * @return True when indices are valid
	*/
	bool SetVoxelProperties( const VoxelData::SpecialProperty properties, const Index3D voxel_indices );

	/*!
	 * @brief Get voxel instance for given indices
	 * @param voxel_indices Indices of voxel
	 * @return Instance of voxel
	*/
	Voxel GetVoxel( const Index3D voxel_indices ) const;

	/*!
	 * @brief Get voxel describing model boundaries
	 * @return Voxel as complete model
	*/
	Voxel GetModelVoxel( void ) const;

	/*!
	 * @brief Calculate Ray transmission through model
	 * @param ray_to_transmit Ray to trace through model
	 * @param tomography_parameter Simulation parameter used in ray tracing
	 * @param scattering_properties Information for ray scattering
	 * @return Ray that has been scattered or left the model
	*/
	Ray TransmitRay( const Ray& ray_to_transmit, const TomographyProperties& tomography_parameter, const RayScattering& scattering_properties ) const;

	/*!
	 * @brief Crop model
	 * @param corner_1 Start Coordinates of new model
	 * @param corner_2	End coordinate of new model
	 * @return True if succeeded
	*/
	bool Crop( const Tuple3D corner_1, const Tuple3D corner_2 );


	/*!
	 * @brief Get slice through model
	 * @param slice_location Where to slice
	 * @param resolution GetResolution of grid
	 * @return Grid with slice
	*/
	DataGrid<VoxelData> GetSlice( const Surface slice_location, const double resolution ) const; 

	/*!
	 * @brief Alter special properties in the specified sphere
	 * @param property Property to add
	 * @param center Center of sphere
	 * @param radius Radius of sphere
	*/
	void AddSpecialSphere( const VoxelData::SpecialProperty property, const Point3D center, const double radius );


	private:

	Index3D number_of_voxel_3D_;				/*!<Amount of voxels in each dimension*/
	Tuple3D voxel_size_;						/*!<Voxelsize in each dimension in mm*/
	Tuple3D size_;								/*!<Size of complete model in mm*/
	size_t number_of_voxel_;					/*!<Absolute amount of voxels in model*/
	vector<VoxelData> voxel_data_;				/*!<Voxel data*/
	CoordinateSystem* coordinate_system_;		/*!<Coordinate system*/
	double min_attenuation_;					/*!<Attenuation minimum in model*/
	double max_attenuation_;					/*!<Attenuation maximum in model*/			
	string name_;								/*!<Model name*/


	private:

	/*!
	 * @brief Function for multi threaded model slicing
	*/
	static void SliceThreaded(	size_t& xIdx, mutex& currentXMutex, size_t& yIdx, mutex& currentYMutex,
								GridCoordinates& realStart, mutex& realStartMutex, GridCoordinates& realEnd, mutex& realEndMutex,
								DataGrid<VoxelData>& slice, mutex& sliceMutex,
								const Surface& slicePlane,
								const Model& modelRef );

	/*!
	* @brief Get voxel indices for given Coordinates in local coordinate system
	* @param x x-coordinate
	* @param y y-coordinate
	* @param z	z-coordinate
	* @return Indices of voxels where Coordinates are located
	*/
	Index3D GetVoxelIndices( const Tuple3D locCoords ) const;

	/*!
	 * @brief Get data index from indices organized in three dimensions
	 * @param indices 3D indides
	 * @return One dimensional index
	*/
	size_t GetDataIndex( const Index3D indices ) const{ return number_of_voxel_3D_.x * number_of_voxel_3D_.y * indices.z + number_of_voxel_3D_.x * indices.y + indices.x; };

	/*!
	 * @brief Element access
	 * @param index index of voxel
	 * @return Reference to voxel data_
	*/
	VoxelData& operator()( const Index3D index );

};