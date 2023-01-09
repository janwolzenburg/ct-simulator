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

	double getIntegral( void ) const;

	public:
	vector<v2> data;		/*!<2D point data*/


};