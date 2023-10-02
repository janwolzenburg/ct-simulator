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
		absolute,		/*!<Mathematically correct but unpractical filter*/
		ramLak,			/*!<Ramachandran and Lakshminarayanan*/
		sheppLogan		/*!<Shepp and Logan*/
	};

	static const std::map<TYPE, string> filterTypes;

	static const double threshold;

	/*!
	 * @brief Constructor
	 * @param pointsRange_ Range of whole numbers to calculate the filter values for
	 * @param samplingInterval_ Sampling interval
	 * @param type Filter type
	*/
	discreteFilter( const Zrange pointsRange_, const double samplingInterval_, const TYPE type );

	discreteFilter( void ) : 
		discreteFilter( Zrange( -5, 5 ), 1., constant )
	{}

	const static discreteFilter::TYPE getEnum( const string searchString ){
		for( auto& [typeEnum, typeString] : discreteFilter::filterTypes ){

			if( typeString == searchString ){

				return typeEnum;

			}
		}

		return constant;

	}

	/*!
	 * @brief Get values of filter kernel
	 * @return Vector with values
	*/
	vector<double> Values( void ) const{ return values; };

	vectorPair PlotValues( void ) const{
		
		vectorPair XY( vector<double>( numberPoints ), values );
		
		std::iota( XY.first.begin(), XY.first.end(), floor( (double) pointsRange.start ) );
		
		return XY;
	}

	/*!
	 * @brief Get range of kernel
	 * @return Range
	*/
	Zrange Range( void ) const{ return pointsRange; };

	Zrange getRelevantRange( void ) const;

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