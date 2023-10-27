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
	num_bytes += SerializeBuildIn( this->x, binary_data );
	num_bytes += SerializeBuildIn( this->y, binary_data );
	num_bytes += SerializeBuildIn( this->z, binary_data );

	return num_bytes;
}


Tuple3D::Tuple3D( const vector<char>& binary_data, vector<char>::const_iterator& it ):
	x( DeSerializeBuildIn<double>( 0., binary_data, it ) ),
	y( DeSerializeBuildIn<double>( 0., binary_data, it ) ),
	z( DeSerializeBuildIn<double>( 0., binary_data, it ) )
{}

size_t Tuple3D::Serialize( vector<char>& binary_data ) const{

	size_t num_bytes = 0;
	num_bytes += SerializeBuildIn( this->x, binary_data );
	num_bytes += SerializeBuildIn( this->y, binary_data );
	num_bytes += SerializeBuildIn( this->z, binary_data );

	return num_bytes;
}


GridIndex::GridIndex( const vector<char>& binary_data, vector<char>::const_iterator& it ) :
	c( DeSerializeBuildIn<size_t>( 0, binary_data, it ) ),
	r( DeSerializeBuildIn<size_t>( 0, binary_data, it ) )
{}

size_t GridIndex::Serialize( vector<char>& binary_data ) const{

	size_t num_bytes = 0;
	num_bytes += SerializeBuildIn( this->c, binary_data );
	num_bytes += SerializeBuildIn( this->r, binary_data );

	return num_bytes;
}


GridCoordinates::GridCoordinates( const vector<char>& binary_data, vector<char>::const_iterator& it ) :
	c( DeSerializeBuildIn<double>( 0., binary_data, it ) ),
	r( DeSerializeBuildIn<double>( 0., binary_data, it ) )
{}

size_t GridCoordinates::Serialize( vector<char>& binary_data ) const{

	size_t num_bytes = 0;
	num_bytes += SerializeBuildIn( this->c, binary_data );
	num_bytes += SerializeBuildIn( this->r, binary_data );

	return num_bytes;
}


/*!
 * Ranges implementation
*/

NumberRange::NumberRange( const double start, const double end ) : start_( start ), end_( end )
{
	if( start_ >= end_ ){
		cerr << "class NumberRange: Start must be less than end!" << endl;
		start_ = end_ - 1.;
	}
}

NumberRange::NumberRange( const NaturalNumberRange naturalRange ) :
	NumberRange{ (double) naturalRange.start(), (double) naturalRange.end() }
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

	num_bytes += SerializeBuildIn( start_, binary_data );
	num_bytes += SerializeBuildIn( end_, binary_data );

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
	return std::stod( str );
}

template<>
int ToNum<int>( const string str ){
	return std::stoi( str );
}

template<>
long long ToNum<long long>( const string str ){
	return std::stoll( str );
}

template<>
size_t ToNum<size_t>( const string str ){

	return static_cast<size_t>( std::stoll( str ) );

}

bool UnsetFlag( bool& flag ){

	const bool previouValue = flag;

	flag = false;

	return previouValue;
}