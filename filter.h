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

	vector<double> Values( void ) const;

	Zrange Range( void ) const;

	double operator()( const signed long long Zidx ) const;

	discreteFilter( const Zrange pointsRange_, const double samplingInterval_ );

	protected:

	size_t getIndex( const signed long long Zidx ) const;

	double get( const size_t idx ) const;
	double& set( const size_t idx );
	//double& operator()( const signed long long Zidx );

	virtual void build( void );


	protected:

	Zrange pointsRange;			/*!<Range of discrete sampling points*/
	size_t numberPoints;		
	double samplingInterval;	/*!<Sampling interval*/
	vector<double> values;		/*!<Values*/

 };


class ramLakFilter : public discreteFilter{

	using discreteFilter::discreteFilter;

	private:
	
	void build( void );

 };

class sheppLoganFilter : private virtual discreteFilter{

	private:

	void build( void );

};