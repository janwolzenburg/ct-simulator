#pragma once
/*********************************************************************
 * @file   physical_detector_properties_.h
 * @brief  Detector parameter classes
 *
 * @author Jan Wolzenburg
 * @date   January 2023
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/

#include "generel.h"
#include "generelMath.h"
#include "radonTransformationProperties.h"
#include "serialization.h"
#include "simulation.h"


/*********************************************************************
   Definitions
*********************************************************************/

/*!
 * @brief Class for physical detector properties
*/
struct PhysicalDetectorProperties{

	static const string FILE_PREAMBLE;
	
	/*!
	 * @brief Default constructor
	*/
	PhysicalDetectorProperties( void );

	/*!
	 * @brief Constructor
	 * @param row_width Width of one detector row
	 * @param detector_focus_distance Distance from detector pixel array to its focus
	 * @param has_anti_scattering_structure Enable anti scattering structure for pixel
	 * @param max_ray_angle_allowed_by_structure 
	*/
	PhysicalDetectorProperties( const double row_width, const double detector_focus_distance, const bool has_anti_scattering_structure = false, 
								const double max_ray_angle_allowed_by_structure = default_max_ray_angle_allowed_by_structure );

	/*!
	 * @brief Constructor from serialized data
	 * @param binary_data Reference to vector with binary data
	 * @param current_byte Iterator to start of data in vector
	*/
	PhysicalDetectorProperties( const vector<char>& binary_data, vector<char>::const_iterator& current_byte );

	/*!
	* @brief Serialize this object
	* @param binary_data Reference to vector where data will be appended
	*/
	size_t Serialize( vector<char>& binary_data ) const;


	double row_width;								/*!<Row width. The length of one the pixel side which is connected to the next pixel inside one row*/
	double detector_focus_distance;					/*!<Distance from the detector array to the focus*/
	bool has_anti_scattering_structure;				/*!<Flag for anti scatter structure*/
	double max_ray_angle_allowed_by_structure;		/*!<Maximum angle between pixel normal and Ray if structure is enabled*/
};


/*!
 * @brief Class for detector properties derived from radon and physical properties
*/
class DetectorProperties{

	public:

	/*!
	 * @brief Constructor
	 * @param radon_properties Radon properties
	 * @param physical_properties Physical detector properties
	*/
	DetectorProperties( const RadonTransformationProperties radon_properties, const PhysicalDetectorProperties physical_properties );


	GridIndex number_of_pixel;					/*!<Amount of pixel in each dimension*/
	double row_width;							/*!<Size of one pixel in column direction*/
	double arc_angle;							/*!<Angle between outer normals*/
	double detector_focus_distance;				/*!<Distance of focus and detector pixel*/

	bool has_anti_scattering_structure;			/*!<Flag for anti scatter structure*/
	double max_ray_angle_allowed_by_structure;	/*!<Maximum angle between pixel normal and Ray*/
};