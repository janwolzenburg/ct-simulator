#pragma once
/*********************************************************************
 * @file   detectorProperties_.h
 * @brief  detector parameter classes
 *
 * @author Jan Wolzenburg
 * @date   January 2023
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/

#include "generel.h"
#include "generelMath.h"
#include "projections.fwd.h"
#include "serialization.h"
#include "simulation.h"


/*********************************************************************
   Definitions
*********************************************************************/

/*!
 * @brief class for physical detector properties
*/
struct PhysicalDetectorProperties{

	static const string FILE_PREAMBLE; /*!< string to prepend to file when storing as file*/
	
	/*!
	 * @brief default constructor
	*/
	PhysicalDetectorProperties( void );

	/*!
	 * @brief constructor
	 * @param row_width width of one detector row
	 * @param detector_focus_distance distance from detector pixel array to its focus
	 * @param has_anti_scattering_structure enable anti scattering structure for pixel
	 * @param max_angle_allowed_by_structure maximum angle between ray and pixel normal
	*/
	PhysicalDetectorProperties( const double row_width, const double detector_focus_distance, const bool has_anti_scattering_structure = false, 
								const double max_angle_allowed_by_structure = default_max_ray_angle_allowed_by_structure );

	/*!
	 * @brief constructor from serialized data
	 * @param binary_data reference to vector with binary data
	 * @param current_byte iterator to start of data in vector
	*/
	PhysicalDetectorProperties( const vector<char>& binary_data, vector<char>::const_iterator& current_byte );

	/*!
	 * @brief serialize this object
	 * @param binary_data reference to vector where data will be appended
	 * @return written bytes
	*/
	size_t Serialize( vector<char>& binary_data ) const;


	double row_width;												/*!< row width. The length of one the pixel side which is connected to the next pixel inside one row*/
	double detector_focus_distance;					/*!< distance from the detector array to the focus*/
	bool has_anti_scattering_structure;			/*!< flag for anti scatter structure*/
	double max_angle_allowed_by_structure;	/*!< maximum angle between pixel normal and Ray if structure is enabled*/
};


/*!
 * @brief class for detector properties derived from radon and physical properties
*/
class DetectorProperties{

	public:

	/*!
	 * @brief constructor
	 * @param projections_properties radon properties
	 * @param physical_properties physical detector properties
	*/
	DetectorProperties( const ProjectionsProperties projections_properties, const PhysicalDetectorProperties physical_properties );


	GridIndex number_of_pixel;					/*!< amount of pixel in each dimension*/
	double row_width;										/*!< size of one pixel in column direction*/
	double arc_angle;										/*!< angle between outer normals*/
	double detector_focus_distance;			/*!< distance of focus and detector pixel*/

	bool has_anti_scattering_structure;			/*!< flag for anti scatter structure*/
	double max_angle_allowed_by_structure;	/*!< maximum angle between pixel normal and Ray*/
};