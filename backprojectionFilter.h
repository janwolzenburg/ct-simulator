#pragma once
/*********************************************************************
 * @file   backprojectionFilter.h
 * @brief  class for a filter kernel
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
 * @brief class for a 1D symmetric filter kernel
*/
class BackprojectionFilter{
	
	public:

	/*!
	 * @brief available filter types
	*/
	enum TYPE{
		ramLak,			/*!< ramachandran and Lakshminarayanan*/
		sheppLogan,		/*!< shepp and Logan*/
		constant,		/*!< no filter*/
	};

	static const std::map<TYPE, string> filter_types;				/*!< filter types with names*/
	static constexpr double significance_percentage = 1e-3;			/*!< threshold to determine significant range*/
	
	/*!
	 * @brief get filter type by string
	 * @param search_string Name of filter type
	 * @return filter enumeration value
	*/
	static BackprojectionFilter::TYPE GetType( const string search_string );

	/*!
	 * @brief constructor
	 * @param points_range Range of whole numbers to calculate the filter values for
	 * @param sampling_interval Sampling interval
	 * @param filter_type Filter type
	*/
	BackprojectionFilter( const NaturalNumberRange points_range, const double sampling_interval, const TYPE filter_type );

	/*!
	 * @brief default constructor
	*/
	BackprojectionFilter( void ) : 
		BackprojectionFilter( NaturalNumberRange{ -5, 5 }, 1., ramLak ) {};

	/*!
	 * @brief get values of filter kernel
	 * @return vector with values
	*/
	vector<double> values( void ) const{ return values_; };

	/*!
	 * @brief get filter type
	 * @return filter type
	*/
	TYPE type( void ) const{ return type_; };

	/*!
	 * @brief get range of kernel
	 * @return range
	*/
	NaturalNumberRange points_range( void ) const{ return points_range_; };

	/*!
	 * @brief get the filter as points
	 * @return vector with x,y points
	*/
	VectorPair GetPlotValues( void ) const;

	/*!
	 * @brief get the relevant range by threshold
	 * @return range of indices where the filter values are significant
	*/
	NaturalNumberRange GetRelevantRange( void ) const;

	/*!
	 * @brief get value of filter at index
	 * @param signed_index Signed index
	 * @return value at index
	*/
	double operator()( const signed long long signed_index ) const{ 
		return this->GetValue( GetUnsignedIndex( signed_index ) ); }

	/*!
	 * @brief normalise to make sum of values equal to one
	*/
	void Normalise( void );


	private:

	TYPE type_;							/*!< type of filter*/
	NaturalNumberRange points_range_;	/*!< range of discrete sampling points*/
	size_t number_of_points_;			/*!< amount of points*/
	double sampling_interval_;			/*!< sampling interval*/
	vector<double> values_;				/*!< values*/


	/*!
	 * @brief get unsigned index corresponding to signed index
	 * @param signed_index signed index
	 * @return Unsigned index
	*/
	size_t GetUnsignedIndex( const signed long long signed_index ) const;

	/*!
	 * @brief get value at index
	 * @param index Index
	 * @return value
	*/
	double GetValue( const size_t index ) const;

	/*!
	 * @brief set value at index
	 * @param index Index
	 * @return reference to value at index
	*/
	bool SetValue( const size_t index, const double new_value );


 };