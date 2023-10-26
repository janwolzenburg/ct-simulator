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

#include <numeric>
#include <map>
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
		ramLak,			/*!<Ramachandran and Lakshminarayanan*/
		sheppLogan		/*!<Shepp and Logan*/
	};

	static const std::map<TYPE, string> filterTypes;	/*!<Filter types with names*/
	static const double threshold;						/*!<Threshold to determine significant range*/
	
	/*!
	 * @brief Get filter type by string
	 * @param searchString Name of filter type
	 * @return Filter enumeration value
	*/
	static discreteFilter::TYPE getEnum( const string searchString );


	/*!
	 * @brief Constructor
	 * @param pointsRange_ Range of whole numbers to calculate the filter values for
	 * @param samplingInterval_ Sampling interval
	 * @param type Filter type
	*/
	discreteFilter( const NaturalNumberRange pointsRange_, const double samplingInterval_, const TYPE type );

	/*!
	 * @brief Default constructor
	 * @param  
	*/
	discreteFilter( void ) : 
		discreteFilter( NaturalNumberRange{ -5, 5 }, 1., constant ) {};

	/*!
	 * @brief Get values of filter kernel
	 * @return Vector with values
	*/
	vector<double> Values( void ) const{ return values; };

	/*!
	 * @brief Get the filter as points
	 * @return Vector with x,y points
	*/
	VectorPair PlotValues( void ) const;

	/*!
	 * @brief Get range of kernel
	 * @return Range
	*/
	NaturalNumberRange Range( void ) const{ return pointsRange; };

	/*!
	 * @brief Get the relevant range by threshold
	 * @return Range of indices where the filter values are significant
	*/
	NaturalNumberRange getRelevantRange( void ) const;

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
	NaturalNumberRange pointsRange;			/*!<Range of discrete sampling points*/
	size_t numberPoints;		/*!<Amount of points*/
	double samplingInterval;	/*!<Sampling interval*/
	vector<double> values;		/*!<Values*/

 };