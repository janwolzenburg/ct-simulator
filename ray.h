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
 * @brief Class for Ray properties_
*/
class RayProperties{
	
	friend class Ray;
	friend class DetectorPixel;

	public:

	/*!
	 * @brief Constructor
	 * @param spectrum_ Ray spectrum
	*/
	RayProperties( const EnergySpectrum spectrum_ ) :
		energy_spectrum_( spectrum_ ), voxel_hits_( 0 ) {};

	/*!
	 * @brief Default constructor
	*/
	RayProperties( void ) :
		energy_spectrum_( EnergySpectrum{} ), voxel_hits_( 0 ) {};
	
	/*!
	 * @brief Get copy of energy spectrum
	 * @return Energy spectrum
	*/
	EnergySpectrum energy_spectrum( void ) const{ return energy_spectrum_; };

	/*!
	 * @brief Attenuate spectrum according to distance in given voxel
	 * @param voxel_data Data of voxel
	 * @param distance_traveled Distance traversed in Voxel
	*/
	void AttenuateSpectrum( const VoxelData& voxel_data, const double distance_traveled );


	private:

	EnergySpectrum energy_spectrum_;		/*!<Energy spectrum*/
	size_t voxel_hits_;				/*!<Counter for voxels hit during transmission*/

};


/*!
 * @brief Class for rays
*/
class Ray : public Line{

	public:

	/*!
	 * @brief Constructor
	 * @param v_ Trajectory
	 * @param p_ Origin
	 * @param intensity_ Intensity
	*/
	explicit Ray( const Vector3D v_, const Point3D p_, const RayProperties properties_ ) : 
		Line{ v_, p_ }, properties_{ properties_ }{};

	/*!
	 * @brief Constructor
	 * @param line_ Line
	 * @param intensity_ Intensity
	*/
	explicit Ray( const Line line_, const RayProperties  properties_ ) :
	Line{ line_ }, properties_{ properties_ }{};

	/*!
	 * @brief Default constructor
	*/
	Ray( void ) : Ray{ Line{}, RayProperties{} }{};

	/*!
	 * @brief Get intensity
	 * @return Intensity
	*/
	RayProperties properties( void ) const{ return properties_; };

	/*!
	 * @brief Get voxel hits
	 * @return Amount of voxel the Ray has hit
	*/
	size_t voxel_hits( void ) const { return properties_.voxel_hits_; };

	/*!
	 * @brief Scale specturm linearly
	 * @param factor Factor
	*/
	void ScaleSpectrum( const double factor ){ properties_.energy_spectrum_.Scale( factor ); };

	/*!
	 * @brief Get the mean frequency of spectrum
	 * @return Mean frequency
	*/
	double GetMeanFrequencyOfSpectrum( void ) const{ return properties_.energy_spectrum_.mean_energy(); };

	/*!
	 * @brief Increment the voxel hit count
	*/
	void IncrementHitCounter( void ){ properties_.voxel_hits_++; };

	/*!
	 * @brief Reset hit counter 
	*/
	void ResetHitCounter( void ){ properties_.voxel_hits_ = 0; };
	
	/*!
	 * @brief Update Ray properties_ passing through voxel for specific distance
	 * @param voxel_properties Voxel properties
	 * @param distance_traveled Distance the Ray is inside voxel
	*/
	void UpdateProperties( const VoxelData& voxel_properties, const double distance_traveled );

	/*!
	 * @brief Convert Ray components to different coordinate system
	 * @param target_coordinate_system Target system
	 * @return Ray in target system
	*/
	Ray ConvertTo( const CoordinateSystem* const target_coordinate_system ) const;

	/*!
	 * @brief Get Ray parameter corresponding to point
	 * @param point_on_ray Point on Ray
	 * @param solution_found Is set to true when the given point lies on the Ray. False if not
	 * @return Ray parameter
	*/
	double GetLineParameter( const Point3D point_on_ray, bool* const solution_found ) const;

	/*!
	 * @brief Project Ray on XY plane of coordinate system
	 * @param coordinate_system System to project on
	 * @return Projected Ray
	*/
	Ray ProjectOnXYPlane( const CoordinateSystem* const coordinate_system ) const;

	/*!
	 * @brief Checks if parameter is greater than one
	 * @param parameter Parameter
	 * @return True when parameter is valid
	*/
	bool IsParameterInBounds( const double voxel_data_ ) const override{ return voxel_data_ >= 0; };

	/*!
	 * @brief Get the faces, which are aligned with the coordinate system of the Ray, through which the Ray could exit_
	 * @return Vector with possible face IDs
	*/
	vector<Voxel::Face> GetPossibleVoxelExits( void ) const;


	private:

	RayProperties properties_;			/*!<Properties of Ray*/
};