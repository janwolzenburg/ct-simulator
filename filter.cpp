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

discreteFilter::discreteFilter( const size_t numberPoints_, const double samplingInterval_ ) : 
	numberPoints( Fmin1( numberPoints_ ) ),
	samplingInterval( samplingInterval_ ),
	values( numberPoints, 0. )
{

	build();

}

void discreteFilter::build( void ){

}

double& discreteFilter::operator()( const size_t idx ){
	if( idx > numberPoints - 1 ) return values.at( idx );
	return values.at( idx );
}

double discreteFilter::operator()( const size_t idx ) const{
	if( idx > numberPoints - 1 ) return values.at( idx );
	return values.at( idx );
}


void ramLakFilter::build( void ){
	
}