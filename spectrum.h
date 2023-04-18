#pragma once
/*********************************************************************
 * @file   spectrum.h
 * @brief  Class for x ray spectrum
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/



 /*********************************************************************
   Includes
*********************************************************************/

#include <vector>
using std::vector;
#include <functional>


#include "generel.h"
#include "voxel.h"


/*********************************************************************
   Definitions
*********************************************************************/

/*!
 * @brief Class for storing a spectrum
*/
class spectrum {

	public:

	/*!
	 * @brief Default constructor
	*/
	spectrum(void);

	/*!
	 * @brief Constructor
	 * @param X X values
	 * @param Y Y values
	*/
	spectrum(const vector<double> X, const vector<double> Y);

	/*!
	 * @brief Get scaled version of this spectrum
	 * @param factor Scaling factor
	 * @return Scaled spectrum
	*/
	spectrum getScaled( const double factor ) const;

	/*!
	 * @brief Scale this spectrum
	 * @param factor Factor
	*/
	void scale( const double factor );

	/*!
	 * @brief Get the intregral of spectrum
	 * @return Integral
	*/
	double getIntegral( void ) const;

	/*!
	 * @brief Get sum of all y values
	 * @return Sum of all y values
	*/
	double getSum( void ) const;

	/*!
	 * @brief Get mean x value
	 * @return Mean value of x by weightening by y value
	*/
	double getMean( void ) const;

	/*!
	 * @brief Get minimum x value
	 * @return Minimum x value
	*/
	double getMin( void ) const;

	/*!
	 * @brief Get maximum x value
	 * @return Maximum x value
	*/
	double getMax( void ) const;

	/*!
	 * @brief Modify spectrum
	 * @param modFunction Function taking reference to spectrum point to modify
	*/
	void modify( std::function<void( v2& )> modFunction );

	inline vector<v2> rawData( void ) const { return data; };

	inline double FrequencyResolution( void ) const { return frequencyResolution; };

	private:

	/*!
	 * @brief Update mean value
	*/
	void updateMean( void );


	private:

	vector<v2> data;		/*!<2D point data sorted by x value*/
	double frequencyResolution;
	double mean;			/*!<Mean Frequency of spectrum*/

};