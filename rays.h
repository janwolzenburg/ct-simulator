#pragma once
/*********************************************************************
 * @file   rays.h
 * @brief  Classes for rays
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/
 


 /*********************************************************************
	Includes
 *********************************************************************/

 #include "line.h"
 #include "voxel.h"
 #include "spectrum.h"



  /*********************************************************************
	Definitions
 *********************************************************************/

/*!
 * @brief Class for ray properties
*/
class rayProperties{
	
	friend class ray;
	friend class pixel;

	public:

	/*!
	 * @brief Constructor
	 * @param spectrum_ Ray spectrum
	*/
	rayProperties( const spectrum spectrum_ ) :
		energySpectrum( spectrum_ ),
		voxHits( 0 )
	{};

	/*!
	 * @brief Default constructor
	*/
	rayProperties( void ) :
		energySpectrum( spectrum{} ),
		voxHits( 0 )
	{};
	
	/*!
	 * @brief Get copy of energy spectrum
	 * @return Energy spectrum
	*/
	spectrum EnergySpectrum( void ) const{ return energySpectrum; };

	/*!
	 * @brief Attenuate spectrum according to distance in given voxel
	 * @param voxelData Data of voxel
	 * @param distance Distance traversed in Voxel
	*/
	void attenuateSpectrum( const voxData& voxelData, const double distance );


	private:

	spectrum energySpectrum;	/*!<Energy spectrum*/
	size_t voxHits;				/*!<Counter for voxels hit during transmission*/

};



/*!
 * @brief Class for rays
*/
class ray : public Line{

	public:

	/*!
	 * @brief Constructor
	 * @param v_ Trajectory
	 * @param p_ Origin
	 * @param intensity_ Intensity
	*/
	explicit ray( const Vector3D v_, const Point3D p_, const rayProperties properties_ ) : 
		Line{ v_, p_ }, properties{ properties_ }{};

	/*!
	 * @brief Constructor
	 * @param line_ Line
	 * @param intensity_ Intensity
	*/
	explicit ray( const Line line_, const rayProperties  properties_ ) :
	Line{ line_ }, properties{ properties_ }{};

	/*!
	 * @brief Default constructor
	*/
	ray( void ) : ray{ Line{}, rayProperties{} }{};

	/*!
	 * @brief Get intensity
	 * @return Intensity
	*/
	rayProperties Properties( void ) const{ return properties; };

	/*!
	 * @brief Convert ray components to different coordinate system
	 * @param target Target system
	 * @return Ray in target system
	*/
	ray convertTo( const CoordinateSystem* const target ) const;

	/*!
	 * @brief Get ray parameter corresponding to point
	 * @param p Point on ray
	 * @param solution_found_ Is set to true when the given point lies on the ray. False if not
	 * @return Ray parameter
	*/
	double getPara( const Point3D p, bool* const solution_found_ ) const;

	/*!
	 * @brief Update ray properties passing through voxel for specific distance
	 * @param data Voxel properties
	 * @param distance Distance the ray is inside voxel
	*/
	void updateProperties( const voxData& data, const double distance );

	/*!
	 * @brief Project ray on XY plane of coordinate system
	 * @param coordinate_system_ System to project on
	 * @return Projected ray
	*/
	ray projectOnXYPlane( const CoordinateSystem* const cSys ) const;

	/*!
	 * @brief Checks if parameter is greater than one
	 * @param para Parameter
	 * @return True when parameter is valid
	*/
	bool IsParameterInBounds( const double para ) const override{ return para >= 0; };

	/*!
	 * @brief Get the faces, which are aligned with the coordinate system of the ray, through which the ray could exit_
	 * @return Vector with possible face IDs
	*/
	vector<FACE_ID> getPossibleVoxelExits( void ) const;

	/*!
	 * @brief Scale specturm linearly
	 * @param factor Factor
	*/
	void scaleSpectrum( const double factor ){ properties.energySpectrum.Scale( factor ); };

	/*!
	 * @brief Get the mean frequency of spectrum
	 * @return Mean frequency
	*/
	double getMeanFrequency( void ) const{ return properties.energySpectrum.getMean(); };

	/*!
	 * @brief Increment the voxel hit count
	*/
	void incrementHitCounter( void ){ properties.voxHits++; };

	/*!
	 * @brief Reset hit counter 
	*/
	void resetHitCounter( void ){ properties.voxHits = 0; };

	/*!
	 * @brief Get voxel hits
	 * @return Amount of voxel the ray has hit
	*/
	size_t VoxelHits( void ) const { return properties.voxHits; };


	private:

	rayProperties properties;			/*!<Properties of ray*/
};