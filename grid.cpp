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
	size( idx2CR{ (size_t) ( ( columnRange.end - columnRange.start ) / resolution_.col ) + 1,
						(size_t) ( ( rowRange.end - rowRange.start ) / resolution_.row ) + 1 } ),
	start( v2CR{ columnRange.start,
						rowRange.start } ),
	resolution( v2CR{ ( columnRange.end - start.col ) / (double) ( size.col - 1 ),
						( rowRange.end - start.row ) / (double) ( size.row - 1 ) } )

{
	fillVectors( defaultValue );
}

void grid::fillVectors( const double defaultValue ){

	// Force size and resolution to positive value 
	size.col = Fpos( size.col );
	size.row = Fpos( size.row );

	resolution.col = Fpos( resolution.col );
	resolution.row = Fpos( resolution.row );


	// Fill axis
	columnPoints = linearSpace( start.col, start.col + (double) ( size.col - 1 ) * resolution.col, size.col );
	rowPoints = linearSpace( start.row, start.row + (double) ( size.row - 1 ) * resolution.row, size.row );

	// Create data structure
	data = vector<vector<double>>( size.col, vector<double>( size.row, defaultValue ) );
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

	if( index.row >= size.row || index.col >= size.col ){
		std::cerr << "Invalid grid index!";
		return false;
	}
	return true;
}


double& grid::operator()( const idx2CR index ){
	// Check the index for validity
	if( !checkIndex( index ) ) return data.at( 0 ).at( 0 );
	return data.at( index.col ).at( index.row );
}

double grid::operator()( const idx2CR index ) const{
	// Check the index for validity
	if( !checkIndex( index ) ) return data.at( 0 ).at( 0 );
	return data.at( index.col ).at( index.row );
}

idx2CR grid::getIndex( const v2CR coordinates ) const{

	idx2CR index;

	index.col = (size_t) floor( ( coordinates.col - start.col ) / resolution.col + 0.5 );
	index.row = (size_t) floor( ( coordinates.row - start.row ) / resolution.row + 0.5 );

	if( index.col >= size.col ) index.col = size.col - 1;
	if( index.row >= size.row ) index.row = size.row - 1;

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

	double column = columnPoints.at( index.col );
	double row = rowPoints.at( index.row );

	return v2CR{ column, row };
}
