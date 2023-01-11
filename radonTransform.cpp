/*********************************************************************
 * @file   radonTransform.cpp
 * @brief  Implementations
 *
 * @author Jan Wolzenburg
 * @date   January 2023
 *********************************************************************/



 /*********************************************************************
   Includes
*********************************************************************/

#include "vectorAlgorithm.h"
#include "radonTransform.h"
#include "generelMath.h"



/*********************************************************************
  Implementations
*********************************************************************/



radonCoords::radonCoords( const double theta_, const double distance_ ) : 
	theta( theta_ ), distance( distance_ )
{}

radonCoords::radonCoords( const cartCSys* const cSys, const line l )
{
	// Project ray on XY plane
	const line projectedLine = l.projectOnXYPlane( cSys );

	// Get perpendicualar to projected ray through coordinate system's origin
	vec3 lot = projectedLine.getLot( cSys->OPnt() );


	// Ray intersects origin
	if( iseqErr( lot.Length(), 0 ) ){
		// Lot vector only for angle calculation
		lot = projectedLine.R() ^ cSys->EzVec();
	}
	// No intersection -> distance from perpendicular
	else{
		// y component is zero
		if( iseqErr( lot.Y(), 0 ) ){
			// x component is less than zero
			if( lot.X() < 0 ) distance = -lot.Length();
			else distance = lot.Length();
		}
		// y component is not zero
		else{
			// y component is less than zero
			if( lot.Y() < 0 ) distance = -lot.Length();
			else  distance = lot.Length();
		}
	}

	// Calculate radon angle

	// y component is zero
	if( iseqErr( lot.Y(), 0 ) ) theta = 0;
	// y component is greater than zero
	else if( lot.Y() > 0 ) theta = cSys->ExVec().angle( lot );
	// y component is less than zero
	else theta = PI - cSys->ExVec().angle( lot );

}

radonCoords::radonCoords( void ) : radonCoords( 0, 0 )
{}


radonPoint::radonPoint( const radonCoords coordinates_, const double value_ ) :
	coordinates( coordinates_ ),
	value( value_ )
{}


grid::grid( const idx2RC size_, const v2RC start_, const v2RC resolution_, double defaultValue ) :
	size( size_ ),
	resolution( resolution_ ),
	start( start_ )
{
	// Force size and resolution to positive value 
	size.c = Fpos( size.c );
	size.r = Fpos( size.r );

	resolution.c = Fpos( resolution.c );
	resolution.r = Fpos( resolution.r );


	// Fill axis
	columnPoints =	linearSpace( start.c, start.c + (double) ( size.c - 1 ) * resolution.c, size.c );
	rowPoints =		linearSpace( start.r, start.r + (double) ( size.r - 1 ) * resolution.r, size.r );
	
	// Create data structure
	data = vector<vector<double>>( size.c, vector<double>( size.r, defaultValue ));
}

idx2RC grid::Size( void ) const{
	return size;
}

v2RC grid::Start( void ) const{
	return start;
}

v2RC grid::Resolution( void ) const{
	return resolution;
}

bool grid::checkIndex( const idx2RC index ) const{
	
	if( index.r >= size.r || index.c >= size.c ){
		std::cerr << "Invalid grid index!";
		return false;
	}
	return true;
}


double& grid::operator()( const idx2RC index ){
	// Check the index for validity
	if( !checkIndex( index ) ) return data.at( 0 ).at( 0 );
	return data.at( index.c ).at( index.r );
}

double grid::operator()( const idx2RC index ) const{
	// Check the index for validity
	if( !checkIndex( index ) ) return data.at( 0 ).at( 0 );
	return data.at( index.c ).at( index.r );
}

idx2RC grid::getIndex( const v2RC coordinates ) const{

	idx2RC index;

	index.c = (size_t) floor( ( coordinates.c - start.c ) / resolution.c + 0.5 );
	index.r = (size_t) floor( ( coordinates.r - start.r ) / resolution.r + 0.5 );

	if( index.c >= size.c ) index.c = size.c - 1;
	if( index.r >= size.r ) index.r = size.r - 1;

	return index;
}

double grid::operator()( const v2RC coordinates ) const{
	return this->operator()( getIndex( coordinates ) );
}

double& grid::operator()( const v2RC coordinates ){
	return this->operator()( getIndex( coordinates ) );
}



radonTransformed::radonTransformed( const range distanceRange, const v2RC resolution_ ) :
	dataGrid(	idx2RC{ (size_t) ( PI / resolution_.c ) + 1,
						(size_t) ( ( distanceRange.end - distanceRange.start ) / resolution_.r ) + 1 },
				v2RC{	0., 
						distanceRange.start },
				resolution_, 
				0. )
{

}

grid radonTransformed::Data( void ) const{
	return dataGrid;
}