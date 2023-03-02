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

#include "generel.h"
#include "voxel.h"


/*********************************************************************
   Definitions
*********************************************************************/

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

	double getMean( void ) const;

	double getMinFrequency( void ) const;
	double getMaxFrequency( void ) const;

	void attenuate( const voxData& voxelData, const double distance ){


		for( v2& v : data ){

			const double k = voxelData.attenuationAt( v.x );

			v.y *= exp( -k * distance );

		}

	}

	private:

	void updateMean( void );

	private:
	vector<v2> data;		/*!<2D point data*/
	double mean;			/*!<Mean Frequency of spectrum*/

};