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
#include <filesystem>
using std::filesystem::path;
 #include <utility>
 using std::pair;
#include <algorithm>


 #include "generelMath.h"
 #include "cartesian.h"
 #include "voxel.h"
 #include "scattering.h"
 #include "grid.h"



  /*********************************************************************
	Definitions
 *********************************************************************/


/*!
 * @brief Class for 3D-Models
*/
class model : virtual public mathObj{
	
	public:

	static const string FILE_PREAMBLE;


	public:

	/*!
	 * @brief Constructor
	 * @param cSys_ Position and orientation of model in global system
	 * @param numVox3D_	Amount of voxels in each spacial dimension
	 * @param voxSize3D_ Spacial size of voxels
	 * @param name_ Name of model
	*/
	model( cartCSys* const cSys_, const idx3 numVox3D_, const v3 voxSize3D_, const string name_ = "Default model name" );

	/*!
	 * @brief Constructor from serialized data
	 * @details Before calling this constructor check with static method validModelData( binbData, it ) whether the data is from model file
	 * @param binData Reference to vector with binary data
	 * @param it Iterator to start of data in vector
	*/
	model( const vector<char>& binData, vector<char>::const_iterator& it );

	/*!
	 * @brief Copy constructor
	 * @param mod Model to copy from
	*/
	model( const model& mod );

	/*!
	 * @brief Default constructor
	*/
	model( void );

	/*!
	 * @brief Destructor
	*/
	~model();

	/*!
	 * @brief Convert model's data to string
	 * @param newLineTabulators Amount of tabulators to insert after each line break
	 * @return String with model's data
	*/
	string toStr( const unsigned int newLineTabulators = 0 ) const override;

	/*!
	 * @brief Assignment oeprator
	 * @param mod Model to assign
	 * @return Reference to this
	*/
	model& operator=( const model& mod );

	bool setData( const voxData newData, const idx3 indices );

	const voxData& getVoxelDataC( const size_t x, const size_t y, const size_t z ) const;

	const voxData& getVoxelDataC( const idx3 indices ) const;

	const voxData& getVoxelDataC( const pnt3 p ) const;

	/*!
	 * @brief Element read
	 * @param x x-index of voxel
	 * @param y y-index of voxel
	 * @param z z-index of voxel
	 * @return Voxel data
	*/
	voxData getVoxelData( const size_t x, const size_t y, const size_t z ) const;

	/*!
	 * @brief Element read
	 * @param indices Indices of voxel
	 * @return Voxel data
	*/
	voxData getVoxelData( const idx3 indices ) const;

	/*!
	 * @brief Get number of voxels
	 * @return Voxel amount
	*/
	inline idx3 NumVox( void ) const{ return numVox3D; };

	/*!
	 * @brief Get size of model
	 * @return Model size
	*/
	inline v3 ModSize( void ) const{ return size3D; };

	double LongestSide( void ) const{ return Max( Max(size3D.x, size3D.y), size3D.z ); };

	/*!
	 * @brief Get size of voxel
	 * @return Voxel size
	*/
	inline v3 VoxSize( void ) const{ return voxSize3D; };

	/*!
	 * @brief Get coordinate system of model
	 * @return coordinate system of base voxel
	*/
	inline cartCSys* CSys( void ) const{ return cSys; };

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
	bool checkIndices( const idx3 indices ) const;

	/*!
	 * @brief Checks if local voxel coordinates are defined in model
	 * @param voxCoords Coordinates to check
	 * @return True if coordinates are defined in model
	*/
	bool validCoords( const v3 voxCoords ) const;

	/*!
	 * @brief Checks if point is defined in model
	 * @param point Point to check
	 * @return True if coordinates are defined in model
	*/
	bool validCoords( const pnt3 point ) const;

	/*!
	 * @brief Get voxel indices for given coordinates in local coordinate system
	 * @param voxpnt Point in coordinate system of model
	 * @return Indices of voxels where coordinates are located
	*/
	idx3 getVoxelIndices( const pnt3 voxpnt ) const;

	/*!
	 * @brief Get voxel instance for given indices
	 * @param indices Indices of voxel
	 * @return Instance of voxel
	*/
	vox getVoxel( const idx3 indices ) const;

	/*!
	 * @brief Get voxel in model
	 * @param point Point in model
	 * @return Voxel at point
	*/
	vox getVoxel( const pnt3 point ) const;

	/*!
	 * @brief Checks if local point is inside model
	 * @param localPnt Point to check
	 * @return True when point is inside model
	*/
	bool pntInside( const pnt3 localPnt ) const;

	/*!
	 * @brief Calculate ray transmission through model
	 * @param r Transmitting ray in model's coordiante system
	 * @param enableScattering Flag indicating if ray can be scattered
	 * @return Vector of ray exiting model or created in model
	*/
	ray rayTransmission( const ray tRay, const bool enableScattering, const rayScattering& scatteringProperties ) const;

	/*!
	 * @brief Crop model
	 * @param minCoords Start coordinates of new model
	 * @param maxCoords	End coordinate of new model
	 * @return True if succeeded
	*/
	bool crop( const v3 minCoords, const v3 maxCoords );

	/*!
	 * @brief Serialize this object
	 * @param binData Reference to vector where data will be appended
	*/
	size_t serialize( vector<char>& binData ) const;

	/*!
	 * @brief Get slice through model
	 * @param sliceLocation Where to slice
	 * @param resolution Resolution of grid
	 * @return Grid with slice
	*/
	grid<voxData> getSlice( const surf sliceLocation, const double resolution ) const; 

	inline range attenuationRange( void ) const{ return range( attenuationMin, attenuationMax ); };

	inline string Name( void ) const{ return name; };

	void addSpecialSphere( const voxData::specialProperty property, const pnt3 center, const double radius );

	private:

	idx3 numVox3D;								/*!<Amount of voxels in each dimension*/
	v3 voxSize3D;								/*!<Voxelsize in each dimension in mm*/
	v3 size3D;									/*!<Size of complete model in mm*/
	size_t numVox;								/*!<Absolute amount of voxels in model*/
	voxData* parameter;							/*!<Voxel data. Access with ROWS*COLS*dep + COLS*row + col*/
	cartCSys* cSys;								/*!<Coordinate system*/
	double attenuationMin;
	double attenuationMax;
	string name;								/*!<Model name*/

	private:

	/*!
	* @brief Get voxel indices for given coordinates in local coordinate system
	* @param x x-coordinate
	* @param y y-coordinate
	* @param z	z-coordinate
	* @return Indices of voxels where coordinates are located
	*/
	idx3 getVoxelIndices( const v3 locCoords ) const;

	/*!
	 * @brief Element assignment
	 * @param x x-index of voxel
	 * @param y y-index of voxel
	 * @param z z-index of voxel
	 * @return Reference to voxel data
	*/
	voxData& operator() ( const size_t x, const size_t y, const size_t z );

	/*!
	 * @brief Element assignment
	 * @param indices Indices of voxel
	 * @return Reference to voxel data
	*/
	voxData& operator() ( const idx3 indices );

};