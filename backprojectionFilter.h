#pragma once
/*********************************************************************
 * @file   backprojectionFilter.h
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
class BackprojectionFilter{
	
	public:

	/*!
	 * @brief Available filter types
	*/
	enum TYPE{
		constant,		/*!< No filter*/
		ramLak,			/*!< Ramachandran and Lakshminarayanan*/
		sheppLogan		/*!< Shepp and Logan*/
	};

	static const std::map<TYPE, string> filter_types;			/*!< Filter types with names*/
	static constexpr double significance_percentage = 1e-2;			/*!< Threshold to determine significant range*/
	
	/*!
	 * @brief Get filter type by string
	 * @param search_string Name of filter type
	 * @return Filter enumeration value
	*/
	static BackprojectionFilter::TYPE GetType( const string search_string );

	/*!
	 * @brief Constructor
	 * @param points_range Range of whole numbers to calculate the filter values for
	 * @param sampling_interval Sampling interval
	 * @param filter_type Filter type
	*/
	BackprojectionFilter( const NaturalNumberRange points_range, const double sampling_interval, const TYPE filter_type );

	/*!
	 * @brief Default constructor
	*/
	BackprojectionFilter( void ) : 
		BackprojectionFilter( NaturalNumberRange{ -5, 5 }, 1., constant ) {};

	/*!
	 * @brief Get values of filter kernel
	 * @return Vector with values
	*/
	vector<double> values( void ) const{ return values_; };

	/*!
	 * @brief Get filter type
	 * @return Filter type
	*/
	TYPE type( void ) const{ return type_; };

	/*!
	 * @brief Get range of kernel
	 * @return Range
	*/
	NaturalNumberRange points_range( void ) const{ return points_range_; };

	/*!
	 * @brief Get the filter as points
	 * @return Vector with x,y points
	*/
	VectorPair GetPlotValues( void ) const;

	/*!
	 * @brief Get the relevant range by threshold
	 * @return Range of indices where the filter values are significant
	*/
	NaturalNumberRange GetRelevantRange( void ) const;

	/*!
	 * @brief Get value of filter at index
	 * @param signed_index Signed index
	 * @return Value at index
	*/
	double operator()( const signed long long signed_index ) const{ 
		return this->GetValue( GetUnsignedIndex( signed_index ) ); }

	/*!
	 * @brief Normalise to make sum of values equal to one
	*/
	void Normalise( void );


	private:

	TYPE type_;							/*!< Type of filter*/
	NaturalNumberRange points_range_;	/*!< Range of discrete sampling points*/
	size_t number_of_points_;			/*!< Amount of points*/
	double sampling_interval_;			/*!< Sampling interval*/
	vector<double> values_;				/*!< Values*/


	/*!
	 * @brief Get unsigned index corresponding to signed index
	 * @param signed_index signed index
	 * @return Unsigned index
	*/
	size_t GetUnsignedIndex( const signed long long signed_index ) const;

	/*!
	 * @brief Get value at index
	 * @param index Index
	 * @return Value
	*/
	double GetValue( const size_t index ) const;

	/*!
	 * @brief Set value at index
	 * @param index Index
	 * @return Reference to value at index
	*/
	bool SetValue( const size_t index, const double new_value );


 };