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
#include "cartesian.h"
#include "voxel.h"
#include "rays.h"
#include "scattering.h"
#include "grid.h"
#include "tomography.fwd.h"



  /*********************************************************************
	Definitions
 *********************************************************************/


/*!
 * @brief Class for 3D-Models
*/
class model : public mathObj{
	
	public:

	static const string FILE_PREAMBLE;		/*!<String at the beginning of file with exported model*/


	public:

	/*!
	 * @brief Constructor
	 * @param cSys_ Position and orientation of model in global system
	 * @param numVox3D_	Amount of voxels in each spacial dimension
	 * @param voxSize3D_ Spacial size of voxels
	 * @param name_ Name of model
	*/
	model( cartCSys* const cSys_, const Index3D numVox3D_, const Tuple3D voxSize3D_, const string name_ = "Default model name" );

	/*!
	 * @brief Constructor from serialized data
	 * @details Before calling this constructor check with static method validModelData( binbData, it ) whether the data is from model file
	 * @param binary_data Reference to vector with binary data
	 * @param it Iterator to start of data in vector
	*/
	model( const vector<char>& binary_data, vector<char>::const_iterator& it );

	/*!
	 * @brief Default constructor
	*/
	model( void ) : model( DUMMY_CSYS(), Index3D{ 1, 1, 1 }, Tuple3D{ 1, 1, 1 } ){};

	/*!
	 * @brief Convert model's data to string
	 * @param newLineTabulators Amount of tabulators to insert after each line break
	 * @return String with model's data
	*/
	string toStr( const unsigned int newLineTabulators = 0 ) const override;

	/*!
	 * @brief Get number of voxels
	 * @return Voxel amount
	*/
	Index3D NumVox( void ) const{ return numVox3D; };

	/*!
	 * @brief Get size of model
	 * @return Model size
	*/
	Tuple3D ModSize( void ) const{ return size3D; };

	/*!
	 * @brief Get size of voxel
	 * @return Voxel size
	*/
	Tuple3D VoxSize( void ) const{ return voxSize3D; };

	/*!
	 * @brief Get coordinate system of model
	 * @return coordinate system of base voxel
	*/
	cartCSys* CSys( void ) const{ return cSys; };

	/*!
	 * @brief Get range of attenuation in model
	 * @return Range of attenuation
	*/
	NumberRange attenuationRange( void ) const{ return NumberRange{ attenuationMin, attenuationMax }; };

	/*!
	 * @brief Get model name
	 * @return Name
	*/
	string Name( void ) const{ return name; };

	/*!
	 * @brief Get the longest edge
	 * @return Length of longest edge
	*/
	double LongestSide( void ) const{ return Max( Max( size3D.x, size3D.y ), size3D.z ); };

	/*!
	 * @brief Get voxel describing model boundaries
	 * @return Voxel as complete model
	*/
	vox Vox( void ) const;

	/*!
	 * @brief Check whether indices are defined in model
	 * @param indices Indices to check
	 * @return True when there exists a voxel with the indices within model
	*/
	bool checkIndices( const Index3D indices ) const;

	/*!
	 * @brief Checks if local voxel coordinates are defined in model
	 * @param voxCoords Coordinates to check
	 * @return True if coordinates are defined in model
	*/
	bool validCoords( const Tuple3D voxCoords ) const{
		return voxCoords.x >= 0 && voxCoords.y >= 0 && voxCoords.z >= 0 &&
			voxCoords.x < size3D.x && voxCoords.y < size3D.y && voxCoords.z < size3D.z; };

	/*!
	 * @brief Checks if local point is inside model
	 * @param localPnt Point to check
	 * @return True when point is inside model
	*/
	bool pntInside( const pnt3 localPnt ) const;

	/*!
	 * @brief Get voxel indices for given coordinates in local coordinate system
	 * @param voxpnt Point in coordinate system of model
	 * @return Indices of voxels where coordinates are located
	*/
	Index3D getVoxelIndices( const pnt3 voxpnt ) const;

	/*!
	 * @brief Set voxel data
	 * @param newData Data to set
	 * @param indices Indices of target voxel
	 * @return True when indices are valid
	*/
	bool setVoxelData( const voxData newData, const Index3D indices );

	/*!
	 * @brief Set voxel's special properties
	 * @param property Data to set
	 * @param indices Indices of target voxel
	 * @return True when indices are valid
	*/
	bool setVoxelProperty( const voxData::specialProperty property, const Index3D indices );

	/*!
	 * @brief Access voxel data
	 * @param x x-index of voxel
	 * @param y y-index of voxel
	 * @param z z-index of voxel
	 * @return Const reference to voxel data
	*/
	const voxData& getVoxelData( const size_t x, const size_t y, const size_t z ) const{ return this->operator()( x, y, z); };

	/*!
	 * @brief Access voxel data
	 * @param indices inidices of voxel
	 * @return Const reference to voxel data
	*/
	const voxData& getVoxelData( const Index3D indices ) const{ return getVoxelData( indices.x, indices.y, indices.z ); };

	/*!
	 * @brief Access voxel data
	 * @param p Point in model
	 * @return Const reference to voxel data
	*/
	const voxData& getVoxelData( const pnt3 p ) const{ return getVoxelData( getVoxelIndices( p ) ); };

	/*!
	 * @brief Get voxel instance for given indices
	 * @param indices Indices of voxel
	 * @return Instance of voxel
	*/
	vox getVoxel( const Index3D indices ) const;

	/*!
	 * @brief Get voxel in model
	 * @param point Point in model
	 * @return Voxel at point
	*/
	vox getVoxel( const pnt3 point ) const{ return getVoxel( getVoxelIndices( point ) ); };

	/*!
	 * @brief Calculate ray transmission through model
	 * @param r Transmitting ray in model's coordiante system
	 * @param enableScattering Flag indicating if ray can be scattered
	 * @return Vector of ray exiting model or created in model
	*/
	ray rayTransmission( const ray tRay, const tomographyParameter& tomoParameter, const rayScattering& scatteringProperties ) const;

	/*!
	 * @brief Crop model
	 * @param minCoords Start coordinates of new model
	 * @param maxCoords	End coordinate of new model
	 * @return True if succeeded
	*/
	bool crop( const Tuple3D minCoords, const Tuple3D maxCoords );

	/*!
	 * @brief Serialize this object
	 * @param binary_data Reference to vector where data will be appended
	*/
	size_t Serialize( vector<char>& binary_data ) const;

	/*!
	 * @brief Get slice through model
	 * @param sliceLocation Where to slice
	 * @param resolution Resolution of grid
	 * @return Grid with slice
	*/
	grid<voxData> getSlice( const surf sliceLocation, const double resolution ) const; 

	/*!
	 * @brief Alter special properties in the specified sphere
	 * @param property Property to add
	 * @param center Center of sphere
	 * @param radius Radius of sphere
	*/
	void addSpecialSphere( const voxData::specialProperty property, const pnt3 center, const double radius );


	private:

	Index3D numVox3D;								/*!<Amount of voxels in each dimension*/
	Tuple3D voxSize3D;								/*!<Voxelsize in each dimension in mm*/
	Tuple3D size3D;									/*!<Size of complete model in mm*/
	size_t numVox;								/*!<Absolute amount of voxels in model*/
	vector<voxData> parameter;					/*!<Voxel data. Access with ROWS*COLS*dep + COLS*row + col*/
	cartCSys* cSys;								/*!<Coordinate system*/
	double attenuationMin;						/*!<Minimum attenuation in model*/
	double attenuationMax;						/*!<Maximum attenuation in model*/
	string name;								/*!<Model name*/


	private:

	/*!
	 * @brief Function for multi threaded model slicing
	*/
	static void sliceThreadFunction(	size_t& xIdx, mutex& currentXMutex, size_t& yIdx, mutex& currentYMutex,
										GridCoordinates& realStart, mutex& realStartMutex, GridCoordinates& realEnd, mutex& realEndMutex,
										grid<voxData>& slice, mutex& sliceMutex,
										const surf& slicePlane,
										const model& modelRef );

	/*!
	* @brief Get voxel indices for given coordinates in local coordinate system
	* @param x x-coordinate
	* @param y y-coordinate
	* @param z	z-coordinate
	* @return Indices of voxels where coordinates are located
	*/
	Index3D getVoxelIndices( const Tuple3D locCoords ) const;

	/*!
	 * @brief Element access
	 * @param x x-index of voxel
	 * @param y y-index of voxel
	 * @param z z-index of voxel
	 * @return Const reference to voxel data
	*/
	const voxData& operator()( const size_t x, const size_t y, const size_t z ) const;

	/*!
	 * @brief Element access
	 * @param x x-index of voxel
	 * @param y y-index of voxel
	 * @param z z-index of voxel
	 * @return Reference to voxel data
	*/
	voxData& operator() ( const size_t x, const size_t y, const size_t z );

	/*!
	 * @brief Element access
	 * @param indices Indices of voxel
	 * @return Reference to voxel data
	*/
	voxData& operator() ( const Index3D indices ){ return ( *this )( indices.x, indices.y, indices.z ); };
};