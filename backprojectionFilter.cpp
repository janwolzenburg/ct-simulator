/*********************************************************************
 * @file   filter.cpp
 * @brief  Implementations of filter class
 *
 * @author Jan Wolzenburg
 * @date   Febuary 2023
 *********************************************************************/


 /*********************************************************************
  Includes
*********************************************************************/

#include "generelMath.h"
#include "backprojectionFilter.h"


/*********************************************************************
   Implementations
*********************************************************************/

const std::map < BackprojectionFilter::TYPE, string> BackprojectionFilter::filter_types{
		{ constant,	"Constant" },
		{ ramLak,	"RamLak" },
		{ sheppLogan,"SheppLogan" },
};

const double BackprojectionFilter::significance_threshold = 1e-5;

BackprojectionFilter::TYPE BackprojectionFilter::GetType( const string searchString ){
	for( auto& [typeEnum, typeString] : BackprojectionFilter::filter_types ){
		if( typeString == searchString )
			return typeEnum;
	}

	return constant;
}

BackprojectionFilter::BackprojectionFilter( const NaturalNumberRange pointsRange_, const double samplingInterval_, const BackprojectionFilter::TYPE type_ ) :
	type_( type_ ),
	points_range_( pointsRange_ ),
	number_of_points_( static_cast<size_t>( points_range_.end() - points_range_.start() ) + 1 ), // N - 1 - (-N + 1) + 1 = 2N - 1
	sampling_interval_( samplingInterval_ ),
	values_( number_of_points_, 0. )
{

	// Iterate over all whole numbers in range
	for( signed long long n = points_range_.start(); n <= points_range_.end(); n++ ){

		double kernelValue = 0.;

		switch( type_ ){
			
			case BackprojectionFilter::constant:
			{
				if( n == 0 ) kernelValue = 1.;
				else kernelValue = 0.;

				break;
			}

			case BackprojectionFilter::ramLak:
			{
				// Conditions for filter calculation
				if( n == 0 )				kernelValue = 1. / ( 4. * pow( sampling_interval_, 2. ) );
				else if( IsEven( n ) )		kernelValue = 0.;
				else						kernelValue = -1. / ( pow( PI, 2. ) * pow( sampling_interval_, 2. ) * pow( (double) n, 2. ) );

				break;
			}

			case BackprojectionFilter::sheppLogan:
			{
				kernelValue = - 2. / ( PI_2 * pow( sampling_interval_, 2. ) ) / ( 4. * pow( (double) n, 2. ) - 1.  );

				break;
			}
		}

		this->SetValue( GetUnsignedIndex( n ), kernelValue );
	}

}

NaturalNumberRange BackprojectionFilter::GetRelevantRange( void ) const{

	NaturalNumberRange relevant( -1, 1 );

	for( signed long long int i = points_range_.start(); i < 0; i++ ){
		if( abs( this->operator()( i ) ) > significance_threshold ){
			relevant.start( i );
			break;
		}
	}

	for( signed long long int i = points_range_.end(); i > 0; i-- ){
		if( abs( this->operator()( i ) ) > significance_threshold ){
			relevant.end( i );
			break;
		}
	}

	return relevant;

}


VectorPair BackprojectionFilter::GetPlotValues( void ) const{

	VectorPair XY( vector<double>( number_of_points_ ), values_ );

	std::iota( XY.first.begin(), XY.first.end(), floor( (double) points_range_.start() ) );

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
