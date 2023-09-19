
#include "grid.h"

template<>
void grid<double>::initializeMinMaxValue( void ){

	minValue = INFINITY;
	maxValue = -INFINITY;

}


template<>
void grid<voxData>::initializeMinMaxValue( void ){

	minValue = voxData( INFINITY, 1. );
	maxValue = voxData( -INFINITY, 1. );

}