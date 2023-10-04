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

const std::map < discreteFilter::TYPE, string> discreteFilter::filterTypes{
		{ constant,	"Constant" },
		{ absolute,	"Absolute" },
		{ ramLak,	"RamLak" },
		{ sheppLogan,"SheppLogan" },
};

const double discreteFilter::threshold = 1e-4;

const discreteFilter::TYPE discreteFilter::getEnum( const string searchString ){
	for( auto& [typeEnum, typeString] : discreteFilter::filterTypes ){

		if( typeString == searchString ){

			return typeEnum;

		}
	}

	return constant;

}

discreteFilter::discreteFilter( const Zrange pointsRange_, const double samplingInterval_, const discreteFilter::TYPE type_ ) :
	type( type_ ),
	pointsRange( pointsRange_ ),
	numberPoints( pointsRange.end - pointsRange.start + 1 ), // N - 1 - (-N + 1) + 1 = 2N - 1
	samplingInterval( samplingInterval_ ),
	values( numberPoints, 0. )
{

	// Iterate over all whole numbers in range
	for( signed long long n = pointsRange.start; n <= pointsRange.end; n++ ){

		double kernelValue = 0.;

		switch( type ){
			
			case discreteFilter::constant:
			{
				if( n == 0 ) kernelValue = 1.;
				else kernelValue = 0.;

				break;
			}

			case discreteFilter::absolute:
			{
				// Constant to be able to approximate inverse FT of abs( w )
				constexpr double e = 1;									

				const double e2 = pow( e, 2. );								// Epsilon squared
				const double c2 = pow( 2. * PI * samplingInterval, 2. );	// Constant for calculation
				const double k2 = pow( (double) n, 2. );					// Double version of n^2

				// Kernel value
				const double h = ( e2 -  c2 * k2 ) / pow( e2 + c2 * k2, 2. );

				kernelValue = h;

				break;
			}

			case discreteFilter::ramLak:
			{
				// Conditions for filter calculation
				if( n == 0 )				kernelValue = 1. / ( 4. * pow( samplingInterval, 2. ) );
				else if( isEven( n ) )		kernelValue = 0.;
				else						kernelValue = -1. / ( pow( PI, 2. ) * pow( samplingInterval, 2. ) * pow( (double) n, 2. ) );

				break;
			}

			case discreteFilter::sheppLogan:
			{
				kernelValue = - 2. / ( PI_2 * pow( samplingInterval, 2. ) ) / ( 4. * pow( (double) n, 2. ) - 1.  );

				break;
			}
		}

		this->set( getIndex( n ) ) = kernelValue;
	}

}

Zrange discreteFilter::getRelevantRange( void ) const{

	Zrange relevant( -1, 1 );

	for( signed long long int i = pointsRange.start; i < 0; i++ ){
		if( abs( this->operator()( i ) ) > threshold ){
			relevant.start = i;
			break;
		}
	}

	for( signed long long int i = pointsRange.end; i > 0; i-- ){
		if( abs( this->operator()( i ) ) > threshold ){
			relevant.end = i;
			break;
		}
	}

	return relevant;

}


vectorPair discreteFilter::PlotValues( void ) const{

	vectorPair XY( vector<double>( numberPoints ), values );

	std::iota( XY.first.begin(), XY.first.end(), floor( (double) pointsRange.start ) );

	return XY;
}

size_t discreteFilter::getIndex( const signed long long Zidx ) const{
	if( Zidx < pointsRange.start ) return 0;
	if( Zidx > pointsRange.end ) return numberPoints - 1;

	return Zidx - pointsRange.start;
}

double& discreteFilter::set( const size_t idx ){
	if( idx > numberPoints - 1 ) return values.at( idx );
	return values.at( idx );
}

double discreteFilter::get( const size_t idx ) const{
	if( idx > numberPoints - 1 ) return values.at( idx );
	return values.at( idx );
}

double discreteFilter::operator()( const signed long long Zidx ) const{
	return this->get( getIndex( Zidx ) );
}
