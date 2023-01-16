#pragma once
/*********************************************************************
 * @file   generel.h
 * @brief  Generel definitions and useful functions
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/



  /*********************************************************************
	Includes
 *********************************************************************/
#include <type_traits>
#include <vector>
using std::vector;

#include <string>
using std::string;
using std::to_string;

#include <iostream>
using std::cerr; using std::endl;


 /*********************************************************************
	Definitions
 *********************************************************************/

 // TODO: Split into definition and implementation - and comment

/*!
 * @brief Class for 2D indices
*/
class idx2{
	public:
	size_t x = 0;
	size_t y = 0;
 };

 /*!
 * @brief Class for 3D indices
*/
class idx3{
	public:
	size_t x = 0;
	size_t y = 0;
	size_t z = 0;
};

/*!
 * @brief Class for 2D vector
*/
class v2{
	public:
	v2( const double x_, const double y_ ) : x( x_ ), y( y_ ){};
	v2( void ) : v2( 0, 0 ){};
	double x = 0;
	double y = 0;
};

/*!
* @brief Class for 3D vector
*/
class v3{
	public:
	v3( const double x_, const double y_, const double z_ ) : x( x_ ), y( y_ ), z( z_ ){};
	v3( void ) : v3( 0, 0, 0 ){};
	double x = 0;
	double y = 0;
	double z = 0;
};


class idx2CR{
	public:
	idx2CR( const size_t c_, const size_t r_ ) : c( c_ ), r( r_ ){};
	idx2CR( void ) : idx2CR( 0, 0 ){};
	size_t c = 0;
	size_t r = 0;
};

/*!
 * @brief Class for table
*/
class v2CR {
	public:
	v2CR(const double c_, const double r_) : c(c_), r(r_) {};
	v2CR(void) : v2CR(0, 0) {};
	double c = 0;
	double r = 0;
};

class range{
	public:
	range( const double start_, const double end_ ) : start( start_ ), end( end_ ){
		if( start >= end ){ 
			cerr << "Start must be less than end!" << endl;
			start = end - 1.;
		}
	};
	range( void ) : range( 0., 1. ){};
	double start;
	double end;
};

/*********************************************************************
	Functions
 *********************************************************************/

/*!
 * @brief Converts variable to underlying type
 * @tparam T Type of var
 * @param var Variable
 * @return Value of var cast to underlying type
*/
template <typename T>
constexpr typename std::underlying_type_t<T> toUnderlying( T var );

/*!
 * @brief Postincrement operator for enumeratable variable
 * @tparam T Type of var
 * @param var Reference to variable
 * @return Incremented variable
*/
template <typename T>
typename std::enable_if_t<std::is_enum_v<T>, T> operator++( T& var );

/*!
 * @brief Substraction operator for enum classes. Assumes value 1 is greater than value 2
 * @tparam T Type of values
 * @param val1 Value 1
 * @param val2 Value 2
 * @return Difference val1 - val2 cast to its their underlying type
*/
template <typename T>
typename std::enable_if_t<std::is_enum_v<T>, T> operator-( const T val1, const T val2 );

/*!
 * @brief Serialize build in datatype
 * @tparam T Type of variable
 * @param val Value
 * @param binData Vector to append binary data
 * @return Amount of bytes appended
*/
template< typename T >
size_t serializeBuildIn( const T val, vector<char>& binData );

/*!
 * @brief Deserialize build in data type
 * @tparam T Expected type
 * @param val Reference to write value to
 * @param it Iterator to start reading from. Will be advanced
 * @return Amount of bytes read
*/
template< typename T >
size_t deSerializeBuildIn( T& val, vector<char>::const_iterator& it );

/*!
 * @brief Export serial data to file
 * @param fileName Filename
 * @param binData Vector with binary data
 * @return True at success
*/
bool exportSerialized( const string fileName, const vector<char> binData );

/*!
 * @brief Import serial data from file
 * @param fileName Filename
 * @return Vector with data
*/
vector<char> importSerialized( const string fileName );

#include "generel.hpp"