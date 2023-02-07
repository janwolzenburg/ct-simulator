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
#include "filter.h"



/*********************************************************************
   Implementations
*********************************************************************/

discreteFilter::discreteFilter( const Zrange pointsRange_, const double samplingInterval_ ) :
	pointsRange( pointsRange_ ),
	numberPoints( pointsRange.end - pointsRange.start + 1 ),
	samplingInterval( samplingInterval_ ),
	values( numberPoints, 0. )
{
	build();
}

void discreteFilter::build( void ){

}


size_t discreteFilter::getIndex( const signed long long Zidx ) const{
	if( Zidx < pointsRange.start ) return 0;
	if( Zidx > pointsRange.end ) return numberPoints - 1;

	return Zidx - pointsRange.start;
}

double& discreteFilter::operator()( const size_t idx ){
	if( idx > numberPoints - 1 ) return values.at( idx );
	return values.at( idx );
}

double discreteFilter::operator()( const size_t idx ) const{
	if( idx > numberPoints - 1 ) return values.at( idx );
	return values.at( idx );
}


double& discreteFilter::operator()( const signed long long Zidx ){
	return this->operator()( getIndex( Zidx ) );
}

double discreteFilter::operator()( const signed long long Zidx ) const{
	return this->operator()( getIndex( Zidx ) );
}


void ramLakFilter::build( void ){
	
	// Iterate over all whole numbers in range
	for( signed long long n = pointsRange.start; n <= pointsRange.end; n++ ){

		// Conditions for filter calculation
		if( n == 0 )				this->operator()( n ) = 1. / ( 4. * pow( samplingInterval, 2. ) );
		else if( isEven( n ) )		this->operator()( n ) = 0.;
		else						this->operator()( n ) = 1 / ( pow( PI, 2. ) * pow( samplingInterval, 2. ) * pow( (double) n, 2. ) );
												

	}

}

void sheppLoganFilter::build( void ){

}