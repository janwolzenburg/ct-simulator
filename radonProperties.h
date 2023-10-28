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
 * @brief Class for detector parameters
*/
class radonProperties{

	public:

	static const string FILE_PREAMBLE;


	public:
	/*!
	 * @brief Constructor
	 * @param numberPoints_ Amount of point in radon transformed
	 * @param distanceRange Distance range
	*/
	radonProperties( const GridIndex numberPoints_, const double distanceRange_ );

	/*!
	 * @brief Default constructor
	*/
	radonProperties( void ): radonProperties{ GridIndex{ 32, 16 }, 500. } {};

	/*!
	 * @brief Constructor from serialized data_
	 * @param binary_data Reference to vector with binary data_
	 * @param it Iterator to start of data_ in vector
	*/
	radonProperties( const vector<char>& binary_data, vector<char>::const_iterator& it );

	/*!
		* @brief Serialize this object
		* @param binary_data Reference to vector where data_ will be appended
	*/
	size_t Serialize( vector<char>& binary_data ) const;


	public:

	double distanceRange;			/*!<Measure field in mm*/
	GridIndex numberPoints;			/*!<Number of points on the axis*/
	GridCoordinates resolution;				/*!GetResolution of the axis*/
	size_t framesToFillSinogram;	/*!<Amount of frames to fill sinogram*/
};
