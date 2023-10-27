#pragma once
/*********************************************************************
 * @file   spectrum.h
 * @brief  Class for x Ray spectrum
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/



 /*********************************************************************
   Includes
*********************************************************************/

#include <functional>
#include "generel.h"


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
	spectrum(void) :
		energyResolution( 1. ),
		mean( 0. )
	{};

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
	void Scale( const double factor );

	/*!
	 * @brief Get sum of all y values
	 * @return Sum of all y values
	*/
	double getSum( void ) const;

	/*!
	 * @brief Get mean x value
	 * @return Mean value of x by weightening by y value
	*/
	double getMean( void ) const{ return mean; };

	/*!
	 * @brief Get minimum x value
	 * @return Minimum x value
	*/
	double getMin( void ) const{ return data_.front().x; };

	/*!
	 * @brief Get maximum x value
	 * @return Maximum x value
	*/
	double getMax( void ) const{ return data_.back().x; };

	/*!
	 * @brief Modify spectrum
	 * @param modFunction Function taking reference to spectrum point to modify
	*/
	void modify( std::function<void( Tuple2D& )> modFunction );

	/*!
	 * @brief Get raw data_
	 * @return Vector of points
	*/
	vector<Tuple2D> rawData( void ) const { return data_; };

	/*!
	 * @brief Get the energy resolution
	 * @return Energy resolution
	*/
	double EnergyResolution( void ) const { return energyResolution; };


	private:

	/*!
	 * @brief Update mean value
	*/
	void updateMean( void );

	vector<Tuple2D> data_;			/*!<2D point data_ sorted by x value*/
	double energyResolution;	/*!<Reolution of energies in spectrum*/
	double mean;				/*!<Mean Frequency of spectrum*/

};