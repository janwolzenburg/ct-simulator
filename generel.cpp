/*********************************************************************
 * @file   generel.cpp
 * @brief  generel function implementation
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/


/*********************************************************************
	Includes
 *********************************************************************/

#include "generel.h"
#include "generelMath.h"
#include "serialization.h"


/*********************************************************************
	Implementations
 *********************************************************************/


/*!
 * indices and vector implementation
*/


Index3D::Index3D( const vector<char>& binary_data, vector<char>::const_iterator& current_byte ) : 
	x( DeSerializeBuildIn<size_t>( 0, binary_data, current_byte ) ),
	y( DeSerializeBuildIn<size_t>( 0, binary_data, current_byte ) ),
	z( DeSerializeBuildIn<size_t>( 0, binary_data, current_byte ) )
{}

size_t Index3D::Serialize( vector<char>& binary_data ) const{

	size_t number_of_bytes = 0;
	number_of_bytes += SerializeBuildIn<size_t>( this->x, binary_data );
	number_of_bytes += SerializeBuildIn<size_t>( this->y, binary_data );
	number_of_bytes += SerializeBuildIn<size_t>( this->z, binary_data );

	return number_of_bytes;
}


Tuple3D::Tuple3D( const vector<char>& binary_data, vector<char>::const_iterator& current_byte ):
	x( DeSerializeBuildIn<double>( 0., binary_data, current_byte ) ),
	y( DeSerializeBuildIn<double>( 0., binary_data, current_byte ) ),
	z( DeSerializeBuildIn<double>( 0., binary_data, current_byte ) )
{}

size_t Tuple3D::Serialize( vector<char>& binary_data ) const{

	size_t number_of_bytes = 0;
	number_of_bytes += SerializeBuildIn<double>( this->x, binary_data );
	number_of_bytes += SerializeBuildIn<double>( this->y, binary_data );
	number_of_bytes += SerializeBuildIn<double>( this->z, binary_data );

	return number_of_bytes;
}


GridIndex::GridIndex( const vector<char>& binary_data, vector<char>::const_iterator& current_byte ) :
	c( DeSerializeBuildIn<size_t>( 0, binary_data, current_byte ) ),
	r( DeSerializeBuildIn<size_t>( 0, binary_data, current_byte ) )
{}

size_t GridIndex::Serialize( vector<char>& binary_data ) const{

	size_t number_of_bytes = 0;
	number_of_bytes += SerializeBuildIn<size_t>( this->c, binary_data );
	number_of_bytes += SerializeBuildIn<size_t>( this->r, binary_data );

	return number_of_bytes;
}


GridCoordinates::GridCoordinates( const vector<char>& binary_data, vector<char>::const_iterator& current_byte ) :
	c( DeSerializeBuildIn<double>( 0., binary_data, current_byte ) ),
	r( DeSerializeBuildIn<double>( 0., binary_data, current_byte ) )
{}

size_t GridCoordinates::Serialize( vector<char>& binary_data ) const{

	size_t number_of_bytes = 0;
	number_of_bytes += SerializeBuildIn<double>( this->c, binary_data );
	number_of_bytes += SerializeBuildIn<double>( this->r, binary_data );

	return number_of_bytes;
}


/*!
 * ranges implementation
*/

NumberRange::NumberRange( const double start, const double end ) : start_( start ), end_( end )
{
	if( start_ >= end_ ){
		cerr << "class NumberRange: Start must be less than end!" << endl;
		start_ = 0.;
		end_ = 1.;
	}
}

NumberRange::NumberRange( const NaturalNumberRange& natural_number_range ) :
	NumberRange{ static_cast<double>( natural_number_range.start() ), static_cast<double>( natural_number_range.end() ) }
{}

NumberRange::NumberRange( const vector<char>& binary_data, vector<char>::const_iterator& current_byte ) :
	start_(	DeSerializeBuildIn<double>( 0., binary_data, current_byte ) ),
	end_(	DeSerializeBuildIn<double>( 0., binary_data, current_byte ) )
{}

double NumberRange::GetResolution( const size_t number_of_elements ) const{
	if( number_of_elements < 2 ) return 1;
	return GetDifference() / static_cast<double>( number_of_elements - 1 );
}

size_t NumberRange::Serialize( vector<char>& binary_data ) const{
	size_t number_of_bytes = 0;

	number_of_bytes += SerializeBuildIn<double>( start_, binary_data );
	number_of_bytes += SerializeBuildIn<double>( end_, binary_data );

	return number_of_bytes;
}


NaturalNumberRange::NaturalNumberRange( const signed long long start, const signed long long end ) : start_( start ), end_( end ){
	if( start_ >= end_ ){
		cerr << "class NaturalNumberRange: Start must be less than end!" << endl;
		start_ = end_ - 1;
	}
}


template<>
double ConvertToNumber<double>( const string string_to_convert ){
	if( string_to_convert.length() == 0 ) return 0.;
	return std::stod( string_to_convert );
}

template<>
int ConvertToNumber<int>( const string string_to_convert ){
	if( string_to_convert.length() == 0 ) return 0;
	return std::stoi( string_to_convert );
}

template<>
unsigned int ConvertToNumber<unsigned int>( const string string_to_convert ){
	if( string_to_convert.length() == 0 ) return 0;
	return std::stoul( string_to_convert );
}

template<>
unsigned long int ConvertToNumber<unsigned long int>( const string string_to_convert ){
	if( string_to_convert.length() == 0 ) return 0;
	return std::stoul( string_to_convert );
}

template<>
long long ConvertToNumber<long long>( const string string_to_convert ){
	if( string_to_convert.length() == 0 ) return 0;
	return std::stoll( string_to_convert );
}

template<>
unsigned long long int ConvertToNumber<unsigned long long int>( const string string_to_convert ){
	if( string_to_convert.length() == 0 ) return 0;
	return static_cast<unsigned long long int>( std::stoll( string_to_convert ) );
}


bool UnsetFlag( bool& flag ){

	const bool previous_value = flag;

	flag = false;

	return previous_value;
}

vector<Tuple2D> ConvertToTuple( const VectorPair vector_pair ){

	size_t size = vector_pair.first.size();

	if( vector_pair.first.size() != vector_pair.second.size() ){
		cerr << "vectors in VectorPair do not match in size!" << endl;
		size = Min( vector_pair.first.size(), vector_pair.second.size() );
	}

	vector<Tuple2D> tuple_vector( size, Tuple2D{ 0., 0. } );

	for (size_t i = 0; i < size; i++) {
		tuple_vector.at( i ) = ( Tuple2D{ vector_pair.first.at(i), vector_pair.second.at(i) } );
	}

	return tuple_vector;
}