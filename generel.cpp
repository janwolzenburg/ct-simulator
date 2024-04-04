/*********************************************************************
 * @file   generel.cpp
 * @brief  Generel function implementation
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
 * Indices and vector implementation
*/


Index3D::Index3D( const vector<char>& binary_data, vector<char>::const_iterator& it ) : 
	x( DeSerializeBuildIn<size_t>( 0, binary_data, it ) ),
	y( DeSerializeBuildIn<size_t>( 0, binary_data, it ) ),
	z( DeSerializeBuildIn<size_t>( 0, binary_data, it ) )
{}

size_t Index3D::Serialize( vector<char>& binary_data ) const{

	size_t num_bytes = 0;
	num_bytes += SerializeBuildIn<size_t>( this->x, binary_data );
	num_bytes += SerializeBuildIn<size_t>( this->y, binary_data );
	num_bytes += SerializeBuildIn<size_t>( this->z, binary_data );

	return num_bytes;
}


Tuple3D::Tuple3D( const vector<char>& binary_data, vector<char>::const_iterator& it ):
	x( DeSerializeBuildIn<double>( 0., binary_data, it ) ),
	y( DeSerializeBuildIn<double>( 0., binary_data, it ) ),
	z( DeSerializeBuildIn<double>( 0., binary_data, it ) )
{}

size_t Tuple3D::Serialize( vector<char>& binary_data ) const{

	size_t num_bytes = 0;
	num_bytes += SerializeBuildIn<double>( this->x, binary_data );
	num_bytes += SerializeBuildIn<double>( this->y, binary_data );
	num_bytes += SerializeBuildIn<double>( this->z, binary_data );

	return num_bytes;
}


GridIndex::GridIndex( const vector<char>& binary_data, vector<char>::const_iterator& it ) :
	c( DeSerializeBuildIn<size_t>( 0, binary_data, it ) ),
	r( DeSerializeBuildIn<size_t>( 0, binary_data, it ) )
{}

size_t GridIndex::Serialize( vector<char>& binary_data ) const{

	size_t num_bytes = 0;
	num_bytes += SerializeBuildIn<size_t>( this->c, binary_data );
	num_bytes += SerializeBuildIn<size_t>( this->r, binary_data );

	return num_bytes;
}


GridCoordinates::GridCoordinates( const vector<char>& binary_data, vector<char>::const_iterator& it ) :
	c( DeSerializeBuildIn<double>( 0., binary_data, it ) ),
	r( DeSerializeBuildIn<double>( 0., binary_data, it ) )
{}

size_t GridCoordinates::Serialize( vector<char>& binary_data ) const{

	size_t num_bytes = 0;
	num_bytes += SerializeBuildIn<double>( this->c, binary_data );
	num_bytes += SerializeBuildIn<double>( this->r, binary_data );

	return num_bytes;
}


/*!
 * Ranges implementation
*/

NumberRange::NumberRange( const double start, const double end ) : start_( start ), end_( end )
{
	if( start_ >= end_ ){
		cerr << "class NumberRange: Start must be less than end!" << endl;
		start_ = 0.;
		end_ = 1.;
	}
}

NumberRange::NumberRange( const NaturalNumberRange& naturalRange ) :
	NumberRange{ static_cast<double>( naturalRange.start() ), static_cast<double>( naturalRange.end() ) }
{}

NumberRange::NumberRange( const vector<char>& binary_data, vector<char>::const_iterator& it ) :
	start_(	DeSerializeBuildIn<double>( 0., binary_data, it ) ),
	end_(	DeSerializeBuildIn<double>( 0., binary_data, it ) )
{}

double NumberRange::GetResolution( const size_t number ) const{
	if( number < 2 ) return 1;
	return GetDifference() / static_cast<double>( number - 1 );
}

size_t NumberRange::Serialize( vector<char>& binary_data ) const{
	size_t num_bytes = 0;

	num_bytes += SerializeBuildIn<double>( start_, binary_data );
	num_bytes += SerializeBuildIn<double>( end_, binary_data );

	return num_bytes;
}


NaturalNumberRange::NaturalNumberRange( const signed long long start, const signed long long end ) : start_( start ), end_( end ){
	if( start_ >= end_ ){
		cerr << "class NaturalNumberRange: Start must be less than end!" << endl;
		start_ = end_ - 1;
	}
}


template<>
double ToNum<double>( const string str ){
	if( str.length() == 0 ) return 0.;
	return std::stod( str );
}

template<>
int ToNum<int>( const string str ){
	if( str.length() == 0 ) return 0;
	return std::stoi( str );
}

template<>
unsigned int ToNum<unsigned int>( const string str ){
	if( str.length() == 0 ) return 0;
	return std::stoul( str );
}

template<>
long long ToNum<long long>( const string str ){
	if( str.length() == 0 ) return 0;
	return std::stoll( str );
}

template<>
size_t ToNum<size_t>( const string str ){
	if( str.length() == 0 ) return 0;
	return static_cast<size_t>( std::stoll( str ) );
}

bool UnsetFlag( bool& flag ){

	const bool previouValue = flag;

	flag = false;

	return previouValue;
}

vector<Tuple2D> ConvertToTuple( const VectorPair vector_pair ){

	size_t size = vector_pair.first.size();

	if( vector_pair.first.size() != vector_pair.second.size() ){
		cerr << "Vectors in VectorPair do not match in size!" << endl;
		size_t size = Min( vector_pair.first.size(), vector_pair.second.size() );
	}

	vector<Tuple2D> tuple_vector( size, Tuple2D{ 0., 0. } );

	for (size_t i = 0; i < size; i++) {
		tuple_vector.at( i ) = ( Tuple2D{ vector_pair.first.at(i), vector_pair.second.at(i) } );
	}

	return tuple_vector;
}