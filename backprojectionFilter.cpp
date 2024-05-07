/*********************************************************************
 * @file   backprojectionFilter.cpp
 *
 * @author Jan Wolzenburg
 * @date   Febuary 2023
 *********************************************************************/


 /*********************************************************************
  Includes
*********************************************************************/

#include <cmath>
#include "generelMath.h"
#include "backprojectionFilter.h"
#include "vectorAlgorithm.h"

/*********************************************************************
   Implementations
*********************************************************************/

const std::map < BackprojectionFilter::TYPE, string> BackprojectionFilter::filter_types{
		{ ramLak,	"RamLak" },
		{ sheppLogan,"SheppLogan" },
		{ constant,	"Constant" }
};


BackprojectionFilter::TYPE BackprojectionFilter::GetType( const string filter_type_to_search ){
	for( auto& [filter_type_enumeration, filter_type_string] : BackprojectionFilter::filter_types ){
		if( filter_type_string == filter_type_to_search )
			return filter_type_enumeration;
	}

	return constant;
}

BackprojectionFilter::BackprojectionFilter( const NaturalNumberRange points_range, const double sampling_interval, const BackprojectionFilter::TYPE filter_type ) :
	type_( filter_type ),
	points_range_( points_range ),
	number_of_points_( static_cast<size_t>( points_range_.end() - points_range_.start() ) + 1 ), // n - 1 - (-N + 1) + 1 = 2N - 1
	sampling_interval_( sampling_interval ),
	values_( number_of_points_, 0. )
{
	// iterate over all whole numbers in range
	for( signed long long n = points_range_.start(); n <= points_range_.end(); n++ ){

		double kernel_value = 0.;

		switch( filter_type ){
			
			case BackprojectionFilter::constant:
			{
				if( n == 0 ) kernel_value = 1.;
				else kernel_value = 0.;

				break;
			}

			case BackprojectionFilter::ramLak:
			{
				// conditions for filter calculation
				if( n == 0 )						kernel_value = 1. / ( 4. * pow( sampling_interval_, 2. ) );
				else if( IsEven( n ) )	kernel_value = 0.;
				else										kernel_value = -1. / ( pow( PI, 2. ) * pow( sampling_interval_, 2. ) * pow( static_cast<double>( n ), 2. ) );

				break;
			}

			case BackprojectionFilter::sheppLogan:
			{
				kernel_value = - 2. / ( PI_2 * pow( sampling_interval_, 2. ) ) / ( 4. * pow( static_cast<double>( n ), 2. ) - 1.  );

				break;
			}
		}

		this->SetValue( GetUnsignedIndex( n ), kernel_value );
	}

}

NaturalNumberRange BackprojectionFilter::GetRelevantRange( void ) const{

	NaturalNumberRange relevant{ - 1, 1 };

	const double most_significant_value = Max<double>( std::abs(  GetMaxElement( values_ ) ), std::abs( GetMinElement( values_ ) ) );
	const double significance_threshold = most_significant_value * significance_percentage;

	for( signed long long int i = points_range_.start(); i < 0; i++ ){
		if( std::abs( this->operator()( i ) ) > significance_threshold ){
			relevant.start( i );
			break;
		}
	}

	for( signed long long int i = points_range_.end(); i > 0; i-- ){
		if( std::abs( this->operator()( i ) ) > significance_threshold ){
			relevant.end( i );
			break;
		}
	}

	return relevant;

}


VectorPair BackprojectionFilter::GetPlotValues( void ) const{

	VectorPair XY( vector<double>( number_of_points_ ), values_ );

	std::iota( XY.first.begin(), XY.first.end(), floor( static_cast<double>( points_range_.start() ) ) );

	return XY;
}

size_t BackprojectionFilter::GetUnsignedIndex( const signed long long Zidx ) const{
	if( Zidx < points_range_.start() ) return 0;
	if( Zidx > points_range_.end() ) return number_of_points_ - 1;

	return static_cast<size_t>( Zidx - points_range_.start() );
}

bool BackprojectionFilter::SetValue( const size_t idx, const double new_value ){
	if( idx > number_of_points_ - 1 ) return false;
	
	values_.at( idx ) = new_value;
	
	return true;
}

double BackprojectionFilter::GetValue( const size_t idx ) const{
	if( idx > number_of_points_ - 1 ) return values_.at( idx );
	return values_.at( idx );
}

void BackprojectionFilter::Normalise( void ){
	
	const double current_sum = Sum( values_ );
	if( current_sum == 0. ) return;

	for( auto& value : values_ )
		value /= current_sum;

}