/*********************************************************************
 * @file   grid.hpp
 * @brief  Implementations
 *
 * @author Jan Wolzenburg
 * @date   January 2023
 *********************************************************************/



 /*********************************************************************
   Includes
*********************************************************************/

#include <type_traits>

#include "vectorAlgorithm.h"
#include "grid.h"
#include "generelMath.h"



/*********************************************************************
  Implementations
*********************************************************************/

template<class D>
grid<D>::grid( void ) : 
	size( 3, 3 ),
	resolution( 1., 1. ),
	start( 0. ,0. )
{
	fillVectors( D{} );
}

template<class D>
grid<D>::grid( const idx2CR size_, const v2CR start_, const v2CR resolution_, D defaultValue ) :
	size( size_ ),
	resolution( resolution_ ),
	start( start_ ){
	fillVectors( defaultValue );
}

template<class D>
grid<D>::grid( const range columnRange, const range rowRange, const v2CR resolution_, D defaultValue ) :
	size( idx2CR{	(size_t) ( ( columnRange.end - columnRange.start ) / resolution_.col ) + 1,
					(size_t) ( ( rowRange.end - rowRange.start ) / resolution_.row ) + 1 } ),
	start( v2CR{	columnRange.start,
					rowRange.start } ),
	resolution( v2CR{ ( columnRange.end - start.col ) / (double) ( size.col - 1 ),
						( rowRange.end - start.row ) / (double) ( size.row - 1 ) } )

{
	fillVectors( defaultValue );
}

template<class D>
grid<D>::grid( const vector<char>& binData, vector<char>::const_iterator& it ) :
	size( idx2CR( binData, it ) ),
	start( v2CR( binData, it ) ),
	resolution( v2CR( binData, it ) )
	
	{

		fillVectors( D() );

		for( vector<D>& column : data ){
			for( D& rowData : column ){
				if constexpr( std::is_fundamental_v<D> )
					rowData = deSerializeBuildIn( D{}, binData, it );
				else
					rowData = D( binData, it );
			}
		}



}

template<class D>
void grid<D>::fillVectors( const D defaultValue ){

	// Force size and resolution to positive value 
	size.col = Fpos( size.col );
	size.row = Fpos( size.row );

	resolution.col = Fpos( resolution.col );
	resolution.row = Fpos( resolution.row );


	// Fill axis
	columnPoints = linearSpace( start.col, start.col + (double) ( size.col - 1 ) * resolution.col, size.col );
	rowPoints = linearSpace( start.row, start.row + (double) ( size.row - 1 ) * resolution.row, size.row );

	// Create data structure
	data = vector<vector<D>>( size.col, vector<D>( size.row, defaultValue ) );
}

template<class D>
 bool grid<D>::checkIndex( const idx2CR index ) const{

	if( index.row >= size.row || index.col >= size.col ){
		std::cerr << "Invalid grid index!";
		return false;
	}
	return true;
}


template<class D>
v2CR grid<D>::End( void ) const{

	return getCoordinates( idx2CR{ size.col - 1, size.row - 1 } );

}

template<class D>
D&  grid<D>::operator()( const idx2CR index ){
	// Check the index for validity
	if( !checkIndex( index ) ) return data.at( 0 ).at( 0 );
	return data.at( index.col ).at( index.row );
}

template<class D>
D grid<D>::operator()( const idx2CR index ) const{
	// Check the index for validity
	if( !checkIndex( index ) ) return data.at( 0 ).at( 0 );
	return data.at( index.col ).at( index.row );
}

template<class D>
idx2CR grid<D>::getIndex( const v2CR coordinates ) const{

	idx2CR index{
		(size_t) floor( ( coordinates.col - start.col ) / resolution.col + 0.5 ),
		(size_t) floor( ( coordinates.row - start.row ) / resolution.row + 0.5 )
	};

	if( index.col >= size.col ) index.col = size.col - 1;
	if( index.row >= size.row ) index.row = size.row - 1;

	return index;
}

template<class D>
D grid<D>::operator()( const v2CR coordinates ) const{
	return this->operator()( getIndex( coordinates ) );
}

template<class D>
D& grid<D>::operator()( const v2CR coordinates ){
	return this->operator()( getIndex( coordinates ) );
}

template<class D>
v2CR grid<D>::getCoordinates( const idx2CR index ) const{

	if( !checkIndex( index ) ) return v2CR{ columnPoints.at( 0 ), rowPoints.at( 0 ) };

	double column = columnPoints.at( index.col );
	double row = rowPoints.at( index.row );

	return v2CR{ column, row };
}


template<class D>
size_t grid<D>::serialize( vector<char>& binData ) const{

	size_t numBytes = 0;
	numBytes += size.serialize( binData );
	numBytes += start.serialize( binData );
	numBytes += resolution.serialize( binData );

	for( const vector<D>& column : data ){
		for( const D& rowData : column ){

			if constexpr( std::is_fundamental_v<D> )
				numBytes += serializeBuildIn( rowData, binData );
			else
				numBytes += rowData.serialize( binData );
		}
	}

	return numBytes;
}