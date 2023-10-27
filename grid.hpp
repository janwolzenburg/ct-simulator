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
#include <limits>

#include "vectorAlgorithm.h"
#include "grid.h"
#include "generelMath.h"
#include "voxel.h"
#include "serialization.h"


/*********************************************************************
  Implementations
*********************************************************************/

template<class D>
grid<D>::grid( void ) : 
	size_{ 3, 3 },
	start{ 0. ,0. },
	resolution{ 1., 1. }
{
	initialiseMinMaxValue();
	fillVectors( D() );
}

template<class D>
grid<D>::grid( const GridIndex size_, const GridCoordinates start_, const GridCoordinates resolution_, D defaultValue ) :
	size_( size_ ),
	resolution( resolution_ ),
	start( start_ )
{
	initialiseMinMaxValue();
	fillVectors( defaultValue );
}

template<class D>
grid<D>::grid( const NumberRange columnRange, const NumberRange rowRange, const GridCoordinates resolution_, D defaultValue ) :
	size_{ GridIndex{	(size_t) ( ( columnRange.end() - columnRange.start() ) / resolution_.c ) + 1,
					(size_t) ( ( rowRange.end() - rowRange.start() ) / resolution_.r ) + 1 } },
	start{ GridCoordinates{	columnRange.start(),
					rowRange.start() } },
	resolution{ GridCoordinates{ ( columnRange.end() - start.c ) / (double) ( size_.c - 1 ),
						( rowRange.end() - start.r ) / (double) ( size_.r - 1 ) } }
{
	initialiseMinMaxValue();
	fillVectors( defaultValue );
}

template<class D>
grid<D>::grid( const vector<char>& binary_data, vector<char>::const_iterator& it ) :
	size_( GridIndex( binary_data, it ) ),
	start( GridCoordinates( binary_data, it ) ),
	resolution( GridCoordinates( binary_data, it ) )
{

		fillVectors( D() );

		if constexpr( std::is_fundamental_v<D> ){
			minValue = DeSerializeBuildIn( D(), binary_data, it );
			maxValue = DeSerializeBuildIn( D(), binary_data, it );
		}
		else{

			minValue = D{ binary_data, it };
			maxValue = D{ binary_data, it };
		}

		for( vector<D>& column : data_ ){
			for( D& rowData : column ){
				if constexpr( std::is_fundamental_v<D> )
					rowData = DeSerializeBuildIn( D{}, binary_data, it );
				else
					rowData = D{ binary_data, it };
			}
		}
}

template<class D>
void grid<D>::fillVectors( const D defaultValue ){

	if( size_.c == 0 || size_.r == 0 ) return;

	resolution.c = ForcePositive( resolution.c );
	resolution.r = ForcePositive( resolution.r );


	// Fill axis
	columnPoints = CreateLinearSpace( start.c, start.c + (double) ( size_.c - 1 ) * resolution.c, size_.c );
	rowPoints = CreateLinearSpace( start.r, start.r + (double) ( size_.r - 1 ) * resolution.r, size_.r );

	// Create data_ structure
	data_ = vector<vector<D>>( size_.c, vector<D>( size_.r, defaultValue ) );
}

template<class D>
 bool grid<D>::checkIndex( const GridIndex index ) const{

	if( index.r >= size_.r || index.c >= size_.c ){
		std::cerr << "Invalid grid index!";
		return false;
	}
	return true;
}

template<class D>
D&  grid<D>::operator()( const GridIndex index ){
	// Check the index for validity
	if( !checkIndex( index ) ) return data_.at( 0 ).at( 0 );
	return data_.at( index.c ).at( index.r );
}

template<class D>
D grid<D>::operator()( const GridIndex index ) const{
	// Check the index for validity
	if( !checkIndex( index ) ) return data_.at( 0 ).at( 0 );
	return data_.at( index.c ).at( index.r );
}

template<class D>
GridIndex grid<D>::getIndex( const GridCoordinates coordinates ) const{

	GridIndex index{
		(size_t) floor( ( coordinates.c - start.c ) / resolution.c + 0.5 ),
		(size_t) floor( ( coordinates.r - start.r ) / resolution.r + 0.5 )
	};

	if( index.c >= size_.c ) index.c = size_.c - 1;
	if( index.r >= size_.r ) index.r = size_.r - 1;

	return index;
}

template<class D>
GridCoordinates grid<D>::getCoordinates( const GridIndex index ) const{

	if( !checkIndex( index ) ) return GridCoordinates{ columnPoints.at( 0 ), rowPoints.at( 0 ) };

	double column = columnPoints.at( index.c );
	double row = rowPoints.at( index.r );

	return GridCoordinates{ column, row };
}

template<class D>
void grid<D>::initialiseMinMaxValue( void ){

	if constexpr( std::is_arithmetic_v<D> ){
		minValue = (std::numeric_limits<D>::min)(); // Additional () needed to stop macro expansion of min() and max()!!!
		maxValue = (std::numeric_limits<D>::max)();
	}
	// Special for own VoxelData class
	else if( std::is_same_v<D, VoxelData> ){
		minValue = VoxelData{ INFINITY, 1. };
		maxValue = VoxelData{ -INFINITY, 1. };
	}
}

template<class D>
bool grid<D>::setData( const GridIndex index, const D newValue ){

	if( !checkIndex( index ) ) return false;

	if constexpr( std::is_arithmetic_v<D> ){
		if( newValue < minValue ) minValue = newValue;
		if( newValue > maxValue ) maxValue = newValue;;
	}
	// Special for own VoxelData class
	else if( std::is_same_v<D, VoxelData> ){
		if( newValue < minValue && !newValue.HasSpecificProperty( VoxelData::UNDEFINED ) ) minValue = newValue;
		if( newValue > maxValue && !newValue.HasSpecificProperty( VoxelData::UNDEFINED ) ) maxValue = newValue;
	}

	this->operator()( index ) = newValue;

	return true;

}

template<class D>
size_t grid<D>::Serialize( vector<char>& binary_data ) const{

	size_t num_bytes = 0;
	num_bytes += size_.Serialize( binary_data );
	num_bytes += start.Serialize( binary_data );
	num_bytes += resolution.Serialize( binary_data );

	if constexpr( std::is_fundamental_v<D> ){
		num_bytes += SerializeBuildIn( minValue, binary_data );
		num_bytes += SerializeBuildIn( maxValue, binary_data );
	}
	else{
		num_bytes += minValue.Serialize( binary_data );
		num_bytes += maxValue.Serialize( binary_data );
	}

	for( const vector<D>& column : data_ ){
		for( const D& rowData : column ){

			if constexpr( std::is_fundamental_v<D> )
				num_bytes += SerializeBuildIn( rowData, binary_data );
			else
				num_bytes += rowData.Serialize( binary_data );
		}
	}

	return num_bytes;
}