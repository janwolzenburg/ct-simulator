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
 
 #include "generelMath.h"
 #include "cartesian.h"
 #include "voxel.h"


  /*********************************************************************
	Definitions
 *********************************************************************/


constexpr double rayStepSize = 10e-6;				/*!<Stepsize during ray iteration in ray direction vector's unit*/
constexpr double muAir = 0.00001883552;				/*!<Absorption air in 1 / mm	for 120keV*/
constexpr double muWater = 0.01611970000;			/*!<Absorption Water in 1 / mm for 120keV*/

/*!
 * @brief Class for 3D-Models
*/
class model : virtual public mathObj{
	// Public functions
	public:

	/*!
	 * @brief Constructor
	 * @param cSys_ Position and orientation of model in global system
	 * @param numVox3D_	Amount of voxels in each spacial dimension
	 * @param voxSize3D_ Spacial size of voxels
	*/
	model( cartCSys* const cSys_, const idx3 numVox3D_, const v3 voxSize3D_ );

	/*!
	 * @brief Constructor
	 * @param file Path to file with 3D-model
	*/
	model( path file );

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
	std::string toStr( const unsigned int newLineTabulators = 0 ) const override;

	/*!
	 * @brief Assignment oeprator
	 * @param mod Model to assign
	 * @return Reference to this
	*/
	model& operator=( const model& mod );

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

	/*!
	 * @brief Element read
	 * @param x x-index of voxel
	 * @param y y-index of voxel
	 * @param z z-index of voxel
	 * @return Voxel data
	*/
	voxData operator() ( const size_t x, const size_t y, const size_t z ) const;

	/*!
	 * @brief Element read
	 * @param indices Indices of voxel
	 * @return Voxel data
	*/
	voxData operator() ( const idx3 indices ) const;

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
	vector<ray> rayTransmission( const ray tRay, const bool enableScattering ) const;

	/*!
	 * @brief Export model to file
	 * @param file Path to file to store model
	 * @return True if succeeded
	*/
	bool exportToFile( const path file ) const;

	/*!
	 * @brief Crop model
	 * @param minCoords Start coordinates of new model
	 * @param maxCoords	End coordinate of new model
	 * @return True if succeeded
	*/
	bool crop( const v3 minCoords, const v3 maxCoords );


	private:
	idx3 numVox3D;								/*!<Amount of voxels in each dimension*/
	v3 voxSize3D;								/*!<Voxelsize in each dimension in mm*/
	v3 size3D;									/*!<Size of complete model in mm*/
	size_t numVox;								/*!<Absolute amount of voxels in model*/
	voxData* parameter;							/*!<Voxel data. Access with ROWS*COLS*dep + COLS*row + col*/
	bool importSuccess;							/*!<Model construction from file succeeded*/
	cartCSys* cSys;								/*!<Coordinate system*/


	private:

	/*!
	* @brief Get voxel indices for given coordinates in local coordinate system
	* @param x x-coordinate
	* @param y y-coordinate
	* @param z	z-coordinate
	* @return Indices of voxels where coordinates are located
	*/
	idx3 getVoxelIndices( const v3 locCoords ) const;
};



/*!*/
model getTestModel( const cartCSys* const parent );