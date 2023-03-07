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

	/*!
	 * @brief Available filter types
	*/
	enum TYPE{
		constant,		/*!<No filter*/
		absolute,		/*!<Mathematically correct but unpractical filter*/
		ramLak,			/*!<Ramachandran and Lakshminarayanan*/
		sheppLogan		/*!<Shepp and Logan*/
	};


	/*!
	 * @brief Constructor
	 * @param pointsRange_ Range of whole numbers to calculate the filter values for
	 * @param samplingInterval_ Sampling interval
	 * @param type Filter type
	*/
	discreteFilter( const Zrange pointsRange_, const double samplingInterval_, const TYPE type );

	/*!
	 * @brief Get values of filter kernel
	 * @return Vector with values
	*/
	inline vector<double> Values( void ) const{ return values; };

	/*!
	 * @brief Get range of kernel
	 * @return Range
	*/
	inline Zrange Range( void ) const{ return pointsRange; };

	/*!
	 * @brief Get value of filter at index
	 * @param Zidx Signed index
	 * @return Value at index
	*/
	double operator()( const signed long long Zidx ) const;


	private:

	/*!
	 * @brief Get unsigned index corresponding to signed index
	 * @param Zidx 
	 * @return 
	*/
	size_t getIndex( const signed long long Zidx ) const;

	/*!
	 * @brief Get value at index
	 * @param idx Index
	 * @return Value
	*/
	double get( const size_t idx ) const;

	/*!
	 * @brief Set value at index
	 * @param idx Index
	 * @return Reference to value at index
	*/
	double& set( const size_t idx );


	private:

	TYPE type;					/*!<Type of filter*/
	Zrange pointsRange;			/*!<Range of discrete sampling points*/
	size_t numberPoints;		/*!<Amount of points*/
	double samplingInterval;	/*!<Sampling interval*/
	vector<double> values;		/*!<Values*/

 };