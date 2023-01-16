/*********************************************************************
 * @file   grid.cpp
 * @brief  Implementations
 *
 * @author Jan Wolzenburg
 * @date   January 2023
 *********************************************************************/



 /*********************************************************************
   Includes
*********************************************************************/

#include "vectorAlgorithm.h"
#include "grid.h"
#include "generelMath.h"



/*********************************************************************
  Implementations
*********************************************************************/

grid::grid( const idx2CR size_, const v2CR start_, const v2CR resolution_, double defaultValue ) :
	size( size_ ),
	resolution( resolution_ ),
	start( start_ ){
	fillVectors( defaultValue );
}

grid::grid( const range columnRange, const range rowRange, const v2CR resolution_, double defaultValue ) :
	size( idx2CR{ (size_t) ( ( columnRange.end - columnRange.start ) / resolution_.c ) + 1,
						(size_t) ( ( rowRange.end - rowRange.start ) / resolution_.r ) + 1 } ),
	start( v2CR{ columnRange.start,
						rowRange.start } ),
	resolution( v2CR{ ( columnRange.end - start.c ) / (double) ( size.c - 1 ),
						( rowRange.end - start.r ) / (double) ( size.r - 1 ) } )

{
	fillVectors( defaultValue );
}

void grid::fillVectors( const double defaultValue ){

	// Force size and resolution to positive value 
	size.c = Fpos( size.c );
	size.r = Fpos( size.r );

	resolution.c = Fpos( resolution.c );
	resolution.r = Fpos( resolution.r );


	// Fill axis
	columnPoints = linearSpace( start.c, start.c + (double) ( size.c - 1 ) * resolution.c, size.c );
	rowPoints = linearSpace( start.r, start.r + (double) ( size.r - 1 ) * resolution.r, size.r );

	// Create data structure
	data = vector<vector<double>>( size.c, vector<double>( size.r, defaultValue ) );
}

idx2CR grid::Size( void ) const{
	return size;
}

v2CR grid::Start( void ) const{
	return start;
}

v2CR grid::Resolution( void ) const{
	return resolution;
}

bool grid::checkIndex( const idx2CR index ) const{

	if( index.r >= size.r || index.c >= size.c ){
		std::cerr << "Invalid grid index!";
		return false;
	}
	return true;
}


double& grid::operator()( const idx2CR index ){
	// Check the index for validity
	if( !checkIndex( index ) ) return data.at( 0 ).at( 0 );
	return data.at( index.c ).at( index.r );
}

double grid::operator()( const idx2CR index ) const{
	// Check the index for validity
	if( !checkIndex( index ) ) return data.at( 0 ).at( 0 );
	return data.at( index.c ).at( index.r );
}

idx2CR grid::getIndex( const v2CR coordinates ) const{

	idx2CR index;

	index.c = (size_t) floor( ( coordinates.c - start.c ) / resolution.c + 0.5 );
	index.r = (size_t) floor( ( coordinates.r - start.r ) / resolution.r + 0.5 );

	if( index.c >= size.c ) index.c = size.c - 1;
	if( index.r >= size.r ) index.r = size.r - 1;

	return index;
}

double grid::operator()( const v2CR coordinates ) const{
	return this->operator()( getIndex( coordinates ) );
}

double& grid::operator()( const v2CR coordinates ){
	return this->operator()( getIndex( coordinates ) );
}

v2CR grid::getCoordinates( const idx2CR index ) const{

	if( !checkIndex( index ) ) return v2CR{ columnPoints.at( 0 ), rowPoints.at( 0 ) };

	double column = columnPoints.at( index.c );
	double row = rowPoints.at( index.r );

	return v2CR{ column, row };
}
