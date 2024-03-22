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
#include "energySpectrum.h"
#include "rayScattering.h"
#include "tomography.fwd.h"



  /*********************************************************************
	Definitions
 *********************************************************************/
#ifdef TRANSMISSION_TRACKING

class RayTrace{
	
	public:

	struct TracingStep{
		bool inside_model;
		double distance;
		VoxelData data;
		Point3D entrance;
		Point3D exit;
		double current_simple_intensity;
		double current_intensity;
		EnergySpectrum spectrum;
	};

	public:
	
	vector<TracingStep> tracing_steps;
};

#endif

/*!
 * @brief Class for Ray properties_
*/
class RayProperties{
	
	friend class Ray;
	friend class DetectorPixel;

	public:


	/*!
	 * @brief Constructor
	 * @param spectrum Ray spectrum
	 * @param expected_pixel_index Expected index of detector pixel this ray will hit
	*/
	RayProperties( const EnergySpectrum spectrum, const size_t expected_pixel_index = 0, const bool definitely_hits_expected_pixel = false ) :
		energy_spectrum_( spectrum ), voxel_hits_( 0 ), initial_power_( energy_spectrum_.GetTotalPower() ), expected_detector_pixel_index_( expected_pixel_index ),
		simple_intensity_( 1. ), definitely_hits_expected_pixel_( definitely_hits_expected_pixel )
		#ifdef TRANSMISSION_TRACKING
		,only_scattering_spectrum( energy_spectrum_ )
		,only_absorption_spectrum( energy_spectrum_ )
		#endif
		{};

	/*!
	 * @brief Default constructor
	*/
	RayProperties( void ) :
		energy_spectrum_( EnergySpectrum{} ), voxel_hits_( 0 ), initial_power_( energy_spectrum_.GetTotalPower() ), expected_detector_pixel_index_( 0 ), simple_intensity_( 1. ),
		definitely_hits_expected_pixel_( false )
		#ifdef TRANSMISSION_TRACKING
		,only_scattering_spectrum( energy_spectrum_ )
		,only_absorption_spectrum( energy_spectrum_ )
		#endif
	{};
	
	/*!
	 * @brief Get copy of energy spectrum
	 * @return Energy spectrum
	*/
	EnergySpectrum energy_spectrum( void ) const{ return energy_spectrum_; };

	/*!
	 * @brief Get intensity at start
	 * @return Intensity at start
	*/
	double start_intensity( void ) const{ return initial_power_; };

	/*!
	 * @brief Get index of the pixel in detector-pixel vector this ray is likely to hit
	 * @return Index of detector pixel
	*/
	size_t expected_detector_pixel_index( void ) const{ return expected_detector_pixel_index_; };

	/*!
	 * @brief Get whether this ray's expected pixel index is definitely
	 * @return True when the stored pixel will definitely be hit
	 */
	bool definitely_hits_expected_pixel( void ) const{ return definitely_hits_expected_pixel_; };

	/*!
	 * @brief Get the intensity if ray is attenuated after lampert beer's model without energy dependence. Start value is 1.
	 * @return Intensity.
	*/
	double simple_intensity( void ) const{ return simple_intensity_; };

	/*!
	 * @brief Scale specturm linearly
	 * @param factor Factor
	*/
	void ScaleSpectrumEvenly( const double factor ){ energy_spectrum_.ScaleEvenly( factor ); };

	/*!
	 * @brief Attenuate spectrum according to distance in given voxel
	 * @param voxel_data Data of voxel
	 * @param distance_traveled Distance traversed in Voxel
	*/
	void AttenuateSpectrum( const VoxelData& voxel_data, const double distance_traveled );


	private:

	EnergySpectrum energy_spectrum_;		/*!< Energy spectrum*/
	size_t voxel_hits_;						/*!< Counter for voxels hit during transmission*/
	double initial_power_;					/*!< Intensity before radiation in watts*/
	size_t expected_detector_pixel_index_;	/*!< Index of detector pixel the ray is likely to hit*/
	double simple_intensity_;				/*!< Current "simple" intensity. According to lambert beer's equation J = J * exp( -l * mu ) */
	bool definitely_hits_expected_pixel_;	/*!< Flag to indicate that this ray definitely hits the expected ray*/

	#ifdef TRANSMISSION_TRACKING
	public:
	RayTrace ray_tracing;
	EnergySpectrum only_scattering_spectrum;
	EnergySpectrum only_absorption_spectrum;

	#endif
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
	void ScaleSpectrumEvenly( const double factor ){ 
		properties_.energy_spectrum_.ScaleEvenly( factor );
		properties_.simple_intensity_ *= factor;
	};

	/*!
	 * @brief Increment the voxel hit count
	*/
	void IncrementHitCounter( void ){ properties_.voxel_hits_++; };

	/*!
	 * @brief Reset hit counter 
	*/
	void ResetHitCounter( void ){ properties_.voxel_hits_ = 0; };
	
	/*!
	 * @brief Set origin
	 * @param new_origin 
	*/
	void SetOrigin( const Point3D new_origin );

	/*!
	 * @brief Set direction
	 * @param new_direction 
	*/
	void SetDirection( const UnitVector3D new_direction );

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
	 * @return Array with flag for voxel faces. True when possible
	*/
	array<bool, ToUnderlying( Voxel::Face::End )> GetPossibleVoxelExits( void ) const;

	/*!
	 * @brief Scatter this ray
	 * @param scattering_information Scattering properties 
	 * @param voxel_data Voxel data
	 * @param distance_traveled_mm Distance traveled in voxel
	 * @param tomography_properties Properties of tomogrpahy
	 * @param newOrigin Point where sattering occured
	 * @return Vector with scattered rays
	*/
	vector<Ray> Scatter( RayScattering& scattering_information,  mutex& scattering_properties_mutex, const VoxelData& voxel_data, const double distance_traveled_mm, const TomographyProperties& tomography_properties, const Point3D& newOrigin);

	/*!
	 * @brief Set expected pixel index
	 * @param pixel_index The pixel index
	 * @param definitely_hits Indication whether given pixel is definitely hit
	 */
	void SetExpectedPixelIndex( const size_t pixel_index, const bool definitely_hits );


	private:

	RayProperties properties_;			/*!< Properties of Ray*/


	#ifdef TRANSMISSION_TRACKING
	public:
	RayTrace& ray_tracing( void ){ return properties_.ray_tracing; };
	#endif
};