
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


template<>
bool grid<double>::setData( const idx2CR index, const double newValue ){

	if( !checkIndex( index ) ) return false;

	if( newValue < minValue ) minValue = newValue;
	if( newValue > maxValue ) maxValue = newValue;

	this->operator()( index ) = newValue;

	return true;

}

template<>
bool grid<voxData>::setData( const idx2CR index, const voxData newValue ){

	if( !checkIndex( index ) ) return false;

	if( newValue < minValue && !newValue.hasSpecificProperty( voxData::UNDEFINED ) ) minValue = newValue;
	if( newValue > maxValue && !newValue.hasSpecificProperty( voxData::UNDEFINED ) ) maxValue = newValue;

	this->operator()( index ) = newValue;

	return true;

}
