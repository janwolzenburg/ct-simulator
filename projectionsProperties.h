#pragma once
/*********************************************************************
 * @file   radonProperties.h
 * @brief  classes radon transformation
 *
 * @author Jan Wolzenburg
 * @date   October 2023
 *********************************************************************/


 #include "generel.h"
 

/*!
 * @brief class for radon transformation properties
*/
class ProjectionsProperties{

	public:

	static const string FILE_PREAMBLE; /*!< string to prepend to file when storing as file*/


	public:
	/*!
	 * @brief constructor
	 * @param number_of_projections Amount of projections
	 * @param number_of_distances Amount of distances
	 * @param measuring_field_size Distance range
	*/
	ProjectionsProperties( const size_t number_of_projections, const size_t number_of_distances, 
							const double measuring_field_size );

	/*!
	 * @brief default constructor
	*/
	ProjectionsProperties( void ): ProjectionsProperties{ 59, 26, 500. }{};

	/*!
	 * @brief constructor from serialized data
	 * @param binary_data Reference to vector with binary data
	 * @param current_byte Iterator to start of data in vector
	*/
	ProjectionsProperties( const vector<char>& binary_data, vector<char>::const_iterator& current_byte );

	/*!
	 * @brief serialize this object
	 * @param binary_data Reference to vector where data will be appended
	*/
	size_t Serialize( vector<char>& binary_data ) const;

	/*!
	 * @brief get number of projections
	 * @return number of projections
	*/
	size_t number_of_projections( void ) const{ return number_of_projections_; };

	/*!
	 * @brief get number of distances
	 * @return number of distances
	*/
	size_t number_of_distances( void ) const{ return number_of_distances_; };

	/*!
	 * @brief get transformation size
	 * @return size of radon transformation
	*/
	GridIndex GetTransformationSize( void ) const{ return GridIndex{ number_of_projections_, number_of_distances_ }; };

	/*!
	 * @brief get size of measuring field
	 * @return size of measuering field
	*/
	double measuring_field_size( void ) const{ return measuring_field_size_; };

	/*!
	 * @brief get angle resolution
	 * @return angle resolution
	*/
	double angles_resolution( void ) const { return angles_resolution_; };

	/*!
	 * @brief get distance resolution
	 * @return Distance resolution
	*/
	double distances_resolution( void ) const { return distances_resolution_; };

	/*!
	 * @brief get transformation resolution
	 * @return transformation resolution
	*/
	GridCoordinates GetTransformationResolution( void ) const{ return GridCoordinates{ angles_resolution_, distances_resolution_ }; };

	/*!
	 * @brief get the number of frames to fill radon transformation completely
	 * @return the number of frames necessary to complete radon transformation. Is the amount of small rotations by the angle resolution of gantry
	*/
	size_t number_of_frames_to_fill( void ) const{ return number_of_frames_to_fill_; };


	private:

	size_t number_of_projections_;			/*!< Number of angles. Is the number of projections*/
	size_t number_of_distances_;			/*!< Number of distances. Is the number of point in one projection*/
	double measuring_field_size_;			/*!< measuring field in mm*/

	double angles_resolution_;				/*!< angle resolution*/
	double distances_resolution_;			/*!< distance resolution*/
	size_t number_of_frames_to_fill_;		/*!< amount of frames to fill sinogram*/

};
