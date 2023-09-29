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

#include <filesystem>
using std::filesystem::path;

#include <sstream>
#include <tuple>
#include <mutex>
using std::mutex;



 /*********************************************************************
	Definitions
 *********************************************************************/

/*!
 * @brief Amount of Threads used during radiation
*/
extern const size_t numThreads;	


/*!
 * @brief Pair of vectors
*/
typedef std::pair<vector<double>, vector<double>> vectorPair;


/*!
 * @brief Class for 2D indices
*/
class idx2{

	public:

	idx2( const size_t x_, const size_t y_ );

	idx2( void );


	public:

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

	idx3( const vector<char>& binData, vector<char>::const_iterator& it );

	size_t serialize( vector<char>& binData ) const;

	bool operator==( const idx3& second ) const;


	public:

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

	public:

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

	v3( const vector<char>& binData, vector<char>::const_iterator& it );

	size_t serialize( vector<char>& binData ) const;


	public:

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

	idx2CR( const vector<char>& binData, vector<char>::const_iterator& it );

	size_t serialize( vector<char>& binData ) const;


	public:

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

	v2CR( const vector<char>& binData, vector<char>::const_iterator& it );

	size_t serialize( vector<char>& binData ) const;

	double col;
	double row;
};


/*!
 * @brief Class for a range of whole numbers
*/
class Zrange{

	public:

	Zrange( const signed long long start_, const signed long long end_ );


	public:

	signed long long start;
	signed long long end;
};

/*!
 * @brief Class for a range of real numbers with start
*/
class range{

	public:

	range( const double start_, const double end_ );
	
	range( void );

	range( const Zrange naturalRange );

	range( const vector<char>& binData, vector<char>::const_iterator& it );

	size_t serialize( vector<char>& binData ) const;

	inline double Diff( void ) const { return end - start; };

	double Resolution( const size_t number ) const;


	public:

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
size_t serializeBuildIn( const T& val, vector<char>& binData );

/*!
 * @brief Serialize string
 * @param val String to serialize
 * @param binData Reference to vector to append to
 * @return Amount of bytes appended
*/
template<>
size_t serializeBuildIn<string>( const string& val, vector<char>& binData );

/*!
 * @brief Serialize 2D-vector of grid points
 * @param val Object to serialize
 * @param binData Reference to vector to append to
 * @return Amount of bytes appended
*/
template<>
size_t serializeBuildIn<vector<vector<v2CR>>>( const vector<vector<v2CR>>& val, vector<char>& binData );

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

template<>
size_t deSerializeBuildIn<string>( string& val, string defaultVal, const vector<char>& binData, vector<char>::const_iterator& it );

/*!
 * @brief Deserialize build in data type
 * @tparam T Type
 * @param defaultVal Default value 
 * @param binData Vector with binary data
 * @param it Iterator to start reading from. Will be advanced
 * @return Deserialized object
*/
template< typename T>
T deSerializeBuildIn( T defaultVal, const vector<char>& binData, vector<char>::const_iterator& it );

/*!
 * @brief Deserialize and return object
 * @tparam T Type
 * @param binData Binary data 
 * @param it Iterator
 * @return Deserialized object
*/
template< typename T>
T deSerialize( const vector<char>& binData, vector<char>::const_iterator& it );

template<>
vector<vector<v2CR>> deSerialize<vector<vector<v2CR>>>( const vector<char>& binData, vector<char>::const_iterator& it );


/*!
 * @brief Export serial data to file
 * @param fileName Filename
 * @param binData Vector with binary data
 * @return True at success
*/
bool exportSerialized( const string fileName, const vector<char>& binData );

bool exportSerialized( const path filePath, const vector<char>& binData );


/*!
 * @brief Import serial data from file
 * @param fileName Filename
 * @return Vector with data
*/
vector<char> importSerialized( const string fileName );

vector<char> importSerialized( const path filePath );

/*!
	* @brief Check if data in vector is from a valid file
	* @param binData Reference to vector with binary data
	* @param it Iterator to start of data in vector
	* @return True when preambles match
*/
bool validBinaryData( const string preamble, const vector<char>& binData, vector<char>::const_iterator& it );

/*!
 * @brief Convert number to string with given precision
 * @tparam T Type of number
 * @param value Value
 * @param precision Precision of converted number. Negative rounds number to significant digits
 * @return Number as string
*/
template<typename T>
string toString( T value, const int precision = 0 );

/*!
 * @brief Convert string to number
 * @tparam T Type of number
 * @param str String
 * @return Number corresponding to string
*/
template<typename T>
T toNum( const string str );

/*!
 * @brief Write to a variable used in multiple threads
 * @tparam T Type of variable
 * @param var Reference to variable
 * @param value Value to write
 * @param m Reference to mutex
 * @return Written value
*/
template<typename T>
T writeThreadVar( T& var, const T& value, mutex& m );

#include "generel.hpp"