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
#include "dataGrid.h"
#include "generelMath.h"
#include "voxel.h"
#include "serialization.h"


/*********************************************************************
  Implementations
*********************************************************************/

template<class D>
DataGrid<D>::DataGrid( void ) : 
	size_{ 3, 3 },
	start_{ 0. ,0. },
	resolution_{ 1., 1. }
{
	InitialiseMinAndMaxValue();
	FillVectors( D() );
}

template<class D>
DataGrid<D>::DataGrid( const GridIndex size, const GridCoordinates start, const GridCoordinates resolution, D defaultValue ) :
	size_( size ),
	resolution_( resolution ),
	start_( start )
{
	InitialiseMinAndMaxValue();
	FillVectors( defaultValue );
}

template<class D>
DataGrid<D>::DataGrid( const NumberRange columnRange, const NumberRange rowRange, const GridCoordinates resolution_, D defaultValue ) :
	size_{ GridIndex{	(size_t) ( ( columnRange.end() - columnRange.start() ) / resolution_.c ) + 1,
					(size_t) ( ( rowRange.end() - rowRange.start() ) / resolution_.r ) + 1 } },
	start_{ GridCoordinates{	columnRange.start(),
					rowRange.start() } },
	resolution_{ GridCoordinates{ ( columnRange.end() - start_.c ) / (double) ( size_.c - 1 ),
						( rowRange.end() - start_.r ) / (double) ( size_.r - 1 ) } }
{
	InitialiseMinAndMaxValue();
	FillVectors( defaultValue );
}

template<class D>
DataGrid<D>::DataGrid( const vector<char>& binary_data, vector<char>::const_iterator& it ) :
	size_( GridIndex( binary_data, it ) ),
	start_( GridCoordinates( binary_data, it ) ),
	resolution_( GridCoordinates( binary_data, it ) )
{

		FillVectors( D() );

		if constexpr( std::is_fundamental_v<D> ){
			min_value_ = DeSerializeBuildIn( D(), binary_data, it );
			max_value_ = DeSerializeBuildIn( D(), binary_data, it );
		}
		else{

			min_value_ = D{ binary_data, it };
			max_value_ = D{ binary_data, it };
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
void DataGrid<D>::FillVectors( const D defaultValue ){

	if( size_.c == 0 || size_.r == 0 ) return;

	resolution_.c = ForcePositive( resolution_.c );
	resolution_.r = ForcePositive( resolution_.r );


	// Fill axis
	column_points_ = CreateLinearSpace( start_.c, start_.c + (double) ( size_.c - 1 ) * resolution_.c, size_.c );
	row_points_ = CreateLinearSpace( start_.r, start_.r + (double) ( size_.r - 1 ) * resolution_.r, size_.r );

	// Create data structure
	data_ = vector<vector<D>>( size_.c, vector<D>( size_.r, defaultValue ) );
}

template<class D>
 bool DataGrid<D>::IsIndexValid( const GridIndex index ) const{

	if( index.r >= size_.r || index.c >= size_.c ){
		std::cerr << "Invalid grid index!";
		return false;
	}
	return true;
}

template<class D>
D&  DataGrid<D>::operator()( const GridIndex index ){
	// Check the index for validity
	if( !IsIndexValid( index ) ) return data_.at( 0 ).at( 0 );
	return data_.at( index.c ).at( index.r );
}

template<class D>
D DataGrid<D>::operator()( const GridIndex index ) const{
	// Check the index for validity
	if( !IsIndexValid( index ) ) return data_.at( 0 ).at( 0 );
	return data_.at( index.c ).at( index.r );
}

template<class D>
GridIndex DataGrid<D>::GetIndex( const GridCoordinates coordinates ) const{

	GridIndex index{
		(size_t) floor( ( coordinates.c - start_.c ) / resolution_.c + 0.5 ),
		(size_t) floor( ( coordinates.r - start_.r ) / resolution_.r + 0.5 )
	};

	if( index.c >= size_.c ) index.c = size_.c - 1;
	if( index.r >= size_.r ) index.r = size_.r - 1;

	return index;
}

template<class D>
GridCoordinates DataGrid<D>::GetCoordinates( const GridIndex index ) const{

	if( !IsIndexValid( index ) ) return GridCoordinates{ column_points_.at( 0 ), row_points_.at( 0 ) };

	double column = column_points_.at( index.c );
	double row = row_points_.at( index.r );

	return GridCoordinates{ column, row };
}

template<class D>
void DataGrid<D>::InitialiseMinAndMaxValue( void ){

	if constexpr( std::is_arithmetic_v<D> ){
		min_value_ = (std::numeric_limits<D>::max)(); // Additional () needed to stop macro expansion of min() and max()!!!
		max_value_ = (std::numeric_limits<D>::min)();
	}
	// Special for own VoxelData class
	else if( std::is_same_v<D, VoxelData> ){
		min_value_ = VoxelData{ INFINITY, 1., VoxelData::Undefined };
		max_value_ = VoxelData{ -INFINITY, 1., VoxelData::Undefined };
	}
}

template<class D>
bool DataGrid<D>::SetData( const GridIndex index, const D newValue ){

	if( !IsIndexValid( index ) ) return false;

	if constexpr( std::is_arithmetic_v<D> ){
		if( newValue < min_value_ ) min_value_ = newValue;
		if( newValue > max_value_ ) max_value_ = newValue;
	}
	// Special for own VoxelData class
	else if( std::is_same_v<D, VoxelData> ){
		if( newValue < min_value_ && !newValue.HasSpecificProperty( VoxelData::Undefined ) ) min_value_ = newValue;
		if( newValue > max_value_ && !newValue.HasSpecificProperty( VoxelData::Undefined ) ) max_value_ = newValue;
	}

	this->operator()( index ) = newValue;

	return true;

}

template<class D>
size_t DataGrid<D>::Serialize( vector<char>& binary_data ) const{

	size_t num_bytes = 0;
	num_bytes += size_.Serialize( binary_data );
	num_bytes += start_.Serialize( binary_data );
	num_bytes += resolution_.Serialize( binary_data );

	if constexpr( std::is_fundamental_v<D> ){
		num_bytes += SerializeBuildIn( min_value_, binary_data );
		num_bytes += SerializeBuildIn( max_value_, binary_data );
	}
	else{
		num_bytes += min_value_.Serialize( binary_data );
		num_bytes += max_value_.Serialize( binary_data );
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