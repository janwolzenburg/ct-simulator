#pragma once
/*********************************************************************
 * @file   radonProperties.h
 * @brief  Classes radon transformation
 *
 * @author Jan Wolzenburg
 * @date   October 2023
 *********************************************************************/


 #include "generel.h"
 

/*!
 * @brief Class for radon transformation properties
*/
class ProjectionsProperties{

	public:

	static const string FILE_PREAMBLE; /*!< String to prepend to file when storing as file*/


	public:
	/*!
	 * @brief Constructor
	 * @param number_of_projections Amount of projections
	 * @param number_of_distances Amount of distances
	 * @param measuring_field_size Distance range
	*/
	ProjectionsProperties( const size_t number_of_projections, const size_t number_of_distances, const double measuring_field_size );

	/*!
	 * @brief Default constructor
	*/
	ProjectionsProperties( void ): ProjectionsProperties{ 59, 26, 500. } {};

	/*!
	 * @brief Constructor from serialized data
	 * @param binary_data Reference to vector with binary data
	 * @param current_byte Iterator to start of data in vector
	*/
	ProjectionsProperties( const vector<char>& binary_data, vector<char>::const_iterator& current_byte );

	/*!
	 * @brief Serialize this object
	 * @param binary_data Reference to vector where data will be appended
	*/
	size_t Serialize( vector<char>& binary_data ) const;

	/*!
	 * @brief Get number of projections
	 * @return Number of projections
	*/
	size_t number_of_projections( void ) const{ return number_of_projections_; };

	/*!
	 * @brief Get number of distances
	 * @return Number of distances
	*/
	size_t number_of_distances( void ) const{ return number_of_distances_; };

	/*!
	 * @brief Get transformation size
	 * @return Size of radon transformation
	*/
	GridIndex GetTransformationSize( void ) const{ return GridIndex{ number_of_projections_, number_of_distances_ }; };

	/*!
	 * @brief Get size of measuring field
	 * @return Size of measuering field
	*/
	double measuring_field_size( void ) const{ return measuring_field_size_; };

	/*!
	 * @brief Get angle resolution
	 * @return Angle resolution
	*/
	double angles_resolution( void ) const { return angles_resolution_; };

	/*!
	 * @brief Get distance resolution
	 * @return Distance resolution
	*/
	double distances_resolution( void ) const { return distances_resolution_; };

	/*!
	 * @brief Get transformation resolution
	 * @return Transformation resolution
	*/
	GridCoordinates GetTransformationResolution( void ) const{ return GridCoordinates{ angles_resolution_, distances_resolution_ }; };

	/*!
	 * @brief Get the number of frames to fill radon transformation completely
	 * @return The number of frames necessary to complete radon transformation. Is the amount of small rotations by the angle resolution of gantry
	*/
	size_t number_of_frames_to_fill( void ) const{ return number_of_frames_to_fill_; };

	/*!
	 * @brief Get mean energy of tube used for this projections
	 * @return Mean energy in eV
	*/
	double tube_mean_energy( void ) const{ return tube_mean_energy_; };
	
	/*!
	 * @brief Set mean energy of tube used for this projections
	 * @param New mean energy in eV
	 * @return Mean energy in eV
	*/
	double tube_mean_energy( const double energy ){ return tube_mean_energy_ = energy ; };


	private:

	size_t number_of_projections_;			/*!< Number of angles. Is the number of projections*/
	size_t number_of_distances_;			/*!< Number of distances. Is the number of point in one projection*/
	double measuring_field_size_;			/*!< Measuring field in mm*/

	double angles_resolution_;				/*!< Angle resolution*/
	double distances_resolution_;			/*!< Distance resolution*/
	size_t number_of_frames_to_fill_;		/*!< Amount of frames to fill sinogram*/

	double tube_mean_energy_;				/*!< Mean energy of tube*/
};
