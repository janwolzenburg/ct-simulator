#pragma once
/*********************************************************************
 * @file   filter.h
 * @brief  Class for a filter kernel
 *
 * @author Jan Wolzenburg
 * @date   Febuary 2023
 *********************************************************************/


 /*********************************************************************
   Includes
*********************************************************************/

#include "generel.h"



/*********************************************************************
  Definitions
*********************************************************************/

/*!
 * @brief Class for a 1D symmetric filter kernel
*/
class discreteFilter{
	
	public:
	
	double& operator()( const size_t idx );

	double operator()( const size_t idx ) const;

	discreteFilter( const size_t numberPoints_, const double samplingInterval_ );

	virtual void build( void );


	private:

	size_t numberPoints;		/*!<Amount of points*/
	double samplingInterval;	/*!<Sampling interval*/
	vector<double> values;		/*!<Values*/

 };


class ramLakFilter : private virtual discreteFilter{

	public:
	
	void build( void );

 };