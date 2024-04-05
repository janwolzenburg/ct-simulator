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
	resolution_{ 1., 1. },
	min_value_(D()),
	max_value_(D())
{
	InitialiseMinAndMaxValue();
	Fillvectors( D() );
}

template<class D>
DataGrid<D>::DataGrid( const GridIndex size, const GridCoordinates start, const GridCoordinates resolution, D defaultValue ) :
	size_( size ),
	resolution_( resolution ),
	start_( start ),
	min_value_(D()),
	max_value_(D())
{
	InitialiseMinAndMaxValue();
	Fillvectors( defaultValue );
}

template<class D>
DataGrid<D>::DataGrid( const NumberRange columnRange, const NumberRange rowRange, const GridCoordinates resolution_, D defaultValue ) :
	size_{ GridIndex{	static_cast<size_t>( ( columnRange.end() - columnRange.start() ) / resolution_.c ) + 1,
						static_cast<size_t>( ( rowRange.end() - rowRange.start() ) / resolution_.r ) + 1 } },
	start_{ GridCoordinates{	columnRange.start(),
								rowRange.start() } },
	resolution_{ GridCoordinates{ ( columnRange.end() - start_.c )	/ static_cast<double>( size_.c - 1 ),
								  ( rowRange.end() - start_.r )		/ static_cast<double>( size_.r - 1 ) } },
	min_value_(D()),
	max_value_(D())
{
	InitialiseMinAndMaxValue();
	Fillvectors( defaultValue );
}

template<class D>
DataGrid<D>::DataGrid( const vector<char>& binary_data, vector<char>::const_iterator& current_byte ) :
	size_( GridIndex{ binary_data, current_byte } ),
	start_( GridCoordinates{ binary_data, current_byte } ),
	resolution_( GridCoordinates{ binary_data, current_byte } ),
	min_value_(D()),
	max_value_(D())
{

	if( size_.c > maximum_vector_size || size_.r > maximum_vector_size ){
		size_ = { 0, 0 };return;
	}

	Fillvectors( D() );

	if constexpr( std::is_fundamental_v<D> ){
		min_value_ = DeSerializeBuildIn<D>( D{}, binary_data, current_byte );
		max_value_ = DeSerializeBuildIn<D>( D{}, binary_data, current_byte );
	}
	else{

		min_value_ = D{ binary_data, current_byte };
		max_value_ = D{ binary_data, current_byte };
	}

	for( vector<D>& column : data_ ){
		for( D& rowData : column ){
			if constexpr( std::is_fundamental_v<D> )
				rowData = DeSerializeBuildIn<D>( D{}, binary_data, current_byte );
			else
				rowData = D{ binary_data, current_byte };
		}
	}
}

template<class D>
void DataGrid<D>::Fillvectors( const D defaultValue ){

	if( size_.c == 0 || size_.r == 0 ) return;

	resolution_.c = ForcePositive( resolution_.c );
	resolution_.r = ForcePositive( resolution_.r );

	// fill axis
	column_points_ = CreateLinearSpace( start_.c, start_.c + static_cast<double>( size_.c - 1 ) * resolution_.c, size_.c );
	row_points_ = CreateLinearSpace( start_.r, start_.r + static_cast<double>( size_.r - 1 ) * resolution_.r, size_.r );

	// create data structure
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
	// check the index for validity
	if( !IsIndexValid( index ) ) return data_.at( 0 ).at( 0 );
	return data_.at( index.c ).at( index.r );
}

template<class D>
D DataGrid<D>::operator()( const GridIndex index ) const{
	// check the index for validity
	if( !IsIndexValid( index ) ) return data_.at( 0 ).at( 0 );
	return data_.at( index.c ).at( index.r );
}

template<class D>
GridIndex DataGrid<D>::GetIndex( const GridCoordinates coordinates ) const{

	GridIndex index{
		static_cast<size_t>( floor( ( coordinates.c - start_.c ) / resolution_.c + 0.5 ) ),
		static_cast<size_t>( floor( ( coordinates.r - start_.r ) / resolution_.r + 0.5 ) )
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
DataGrid<double> DataGrid<D>::GetDoubleGrid( void ) const{

	DataGrid<double> double_grid{ size_, start_, resolution_, 0. };

	for( size_t column_index = 0; column_index < size_.c; column_index++ ){
		for( size_t row_index = 0; row_index < size_.r; row_index++ ){

			GridIndex grid_index{ column_index, row_index };

			if constexpr( std::is_arithmetic_v<D> ){
				double_grid.SetData( grid_index, 
									 static_cast<D>( this->GetData( grid_index ) ) );
			}
			else if constexpr( std::is_same_v<D, VoxelData> ){
			
				double_grid.SetData( grid_index, 
									 this->GetData( grid_index ).GetAbsorptionAtEnergy( reference_energy_for_mu_eV ) );
			
			}
		}
	}

	return double_grid;

}

template<class D>
void DataGrid<D>::InitialiseMinAndMaxValue( void ){

	if constexpr( std::is_arithmetic_v<D> ){
		min_value_ = (std::numeric_limits<D>::max)(); // additional () needed to stop macro expansion of min() and max()!!!
		max_value_ = (std::numeric_limits<D>::min)();
	}
	// special for own VoxelData class
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
	// special for own VoxelData class
	else if( std::is_same_v<D, VoxelData> ){
		if( newValue < min_value_ && !newValue.HasSpecificProperty( VoxelData::Undefined ) ) min_value_ = newValue;
		if( newValue > max_value_ && !newValue.HasSpecificProperty( VoxelData::Undefined ) ) max_value_ = newValue;
	}

	this->operator()( index ) = newValue;

	return true;

}

template<class D>
size_t DataGrid<D>::Serialize( vector<char>& binary_data ) const{

	size_t number_of_bytes = 0;
	number_of_bytes += size_.Serialize( binary_data );
	number_of_bytes += start_.Serialize( binary_data );
	number_of_bytes += resolution_.Serialize( binary_data );

	if constexpr( std::is_fundamental_v<D> ){
		number_of_bytes += SerializeBuildIn<D>( min_value_, binary_data );
		number_of_bytes += SerializeBuildIn<D>( max_value_, binary_data );
	}
	else{
		number_of_bytes += min_value_.Serialize( binary_data );
		number_of_bytes += max_value_.Serialize( binary_data );
	}

	for( const vector<D>& column : data_ ){
		for( const D& rowData : column ){

			if constexpr( std::is_fundamental_v<D> )
				number_of_bytes += SerializeBuildIn<D>( rowData, binary_data );
			else
				number_of_bytes += rowData.Serialize( binary_data );
		}
	}

	return number_of_bytes;
}