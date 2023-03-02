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
using std::cerr; using std::endl; using std::cout;



 /*********************************************************************
	Definitions
 *********************************************************************/

/*!
 * @brief Class for 2D indices
*/
class idx2{

	public:

	idx2( const size_t x_, const size_t y_ );
	idx2( void );

	size_t x;
	size_t y;
 };

 /*!
 * @brief Class for 3D indices
*/
class idx3{

	public:

	idx3( const size_t x_, const size_t y_, const size_t z_ );
	idx3( void );

	size_t x;
	size_t y;
	size_t z;
};

/*!
 * @brief Class for 2D vector
*/
class v2{

	public:

	v2( const double x_, const double y_ );
	v2( void );

	double x;
	double y;
};

/*!
* @brief Class for 3D vector
*/
class v3{

	public:

	v3( const double x_, const double y_, const double z_ );
	v3( void );

	double x;
	double y;
	double z;
};

/*!
 * @brief Class for indicies to data organized in row/column structure
*/
class idx2CR{

	public:

	idx2CR( const size_t col_, const size_t row_ );
	idx2CR( void );

	/*!
	 * @brief Constructor from serialized data
	 * @param binData Reference to vector with binary data
	 * @param it Iterator to start of data in vector
	*/
	idx2CR( const vector<char>& binData, vector<char>::const_iterator& it );

	/*!
	 * @brief Serialize this object
	 * @param binData Reference to vector where data will be appended
	*/
	size_t serialize( vector<char>& binData ) const;

	size_t col;
	size_t row;
};

/*!
 * @brief Class for  data organized in row/column structure
*/
class v2CR {

	public:

	v2CR( const double c_, const double r_ );
	v2CR( void );

	/*!
	 * @brief Constructor from serialized data
	 * @param binData Reference to vector with binary data
	 * @param it Iterator to start of data in vector
	*/
	v2CR( const vector<char>& binData, vector<char>::const_iterator& it );

	/*!
	 * @brief Serialize this object
	 * @param binData Reference to vector where data will be appended
	*/
	size_t serialize( vector<char>& binData ) const;

	double col;
	double row;
};

/*!
 * @brief Class for a range of real numbers with start
*/
class range{

	public:

	range( const double start_, const double end_ );
	range( void );

	/*!
	 * @brief Get the size of the interval
	 * @return Difference of end and start
	 * @details Diff = end - start
	*/
	inline double Diff( void ) const { return end - start; };

	/*!
	 * @brief Get the resolution of the range was split in given amount of discrete values
	 * @param number Amount of discrete values
	 * @return Resolution of interval when slit in given amount
	 * @detail Resolution = (end - start) / (number - 1)
	*/
	double Resolution( const size_t number ) const;


	double start;
	double end;
};

/*!
 * @brief Class for a range of whole numbers
*/
class Zrange{
	public:
	Zrange( const signed long long start_, const signed long long end_ );
	Zrange( void );

	signed long long start;
	signed long long end;
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
 * @param binData Vector with binary data
 * @param it Iterator to start reading from. Will be advanced
 * @return Amount of bytes read
*/
template< typename T >
size_t deSerializeBuildIn( T& val, T defaultVal, const vector<char>& binData, vector<char>::const_iterator& it );

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