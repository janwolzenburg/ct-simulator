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

#include <filesystem>
using std::filesystem::path;

#include <iostream>
using std::cerr; using std::endl; using std::cout;

#include <utility>
 using std::pair;
#include <mutex>
using std::mutex;

#include <chrono>
using namespace std::chrono;
static microseconds start_tic;
#define tic start_tic = duration_cast< microseconds >( system_clock::now().time_since_epoch() )
#define toc ( duration_cast< microseconds >( system_clock::now().time_since_epoch() ) - start_tic ).count()


 /*********************************************************************
	Definitions
 *********************************************************************/

 #define TRANSMISSION_TRACKING


/*!
 * @brief Pair of vectors
*/
typedef pair<vector<double>, vector<double>> VectorPair;


/*!
 * @brief 2D indices
*/
struct Index2D{
	size_t x = 0;
	size_t y = 0;
 };


 /*!
 * @brief Primitive 2D vector
*/
struct Tuple2D{
	double x = 0.;
	double y = 0.;
};


 /*!
 * @brief Class for 3D indices
*/
class Index3D{

	public:

	Index3D( const size_t x_, const size_t y_, const size_t z_ ) : x( x_ ), y( y_ ), z( z_ ) {};

	Index3D( void ) : Index3D{ 0, 0, 0 }{};

	Index3D( const vector<char>& binary_data, vector<char>::const_iterator& current_byte );

	size_t Serialize( vector<char>& binary_data ) const;

	bool operator==( const Index3D& operand ) const{ return x == operand.x && y == operand.y && z == operand.y; };


	size_t x;
	size_t y;
	size_t z;
};


/*!
* @brief Class for 3D vector
*/
class Tuple3D{

	public:

	Tuple3D( const double x_, const double y_, const double z_ ) : x( x_ ), y( y_ ), z( z_ ){};

	Tuple3D( void ) : Tuple3D{ 0., 0., 0. } {};

	Tuple3D( const vector<char>& binary_data, vector<char>::const_iterator& current_byte );

	size_t Serialize( vector<char>& binary_data ) const;


	double x;
	double y;
	double z;
};


/*!
 * @brief Class for indicies to data organized in row/column structure
*/
class GridIndex{

	public:

	GridIndex( const size_t column, const size_t row ) : c( column ), r( row ) {};

	GridIndex( void ) : GridIndex{ 0, 0 } {};

	GridIndex( const vector<char>& binary_data, vector<char>::const_iterator& current_byte );

	size_t Serialize( vector<char>& binary_data ) const;


	size_t c;	/*!< Column*/
	size_t r;	/*!< Row*/
};


/*!
 * @brief Class for  data organized in row/column structure
*/
class GridCoordinates {

	public:

	GridCoordinates( const double column, const double row ) : c( column ), r( row ) {};

	GridCoordinates( void ) : GridCoordinates{ 0., 0. } {};

	GridCoordinates( const vector<char>& binary_data, vector<char>::const_iterator& current_byte );

	size_t Serialize( vector<char>& binary_data ) const;


	double c;
	double r;
};


/*!
 * @brief Class for a range of whole numbers
*/
class NaturalNumberRange{

	public:

	NaturalNumberRange( const signed long long start, const signed long long end );

	signed long long start( void ) const{ return start_; };

	signed long long end( void ) const{ return end_; };

	void start( const signed long long newStart ){ start_ = ( newStart < end_ ) ? newStart : end_ - 1; };

	void end( const signed long long newEnd ){ end_ = ( newEnd > start_ ) ? newEnd : start_ + 1; };


	private:

	signed long long start_;
	signed long long end_;
};


/*!
 * @brief Class for a range of real numbers with start
*/
class NumberRange{

	public:
	
	NumberRange( const double start, const double end );

	NumberRange( void ) : NumberRange{ 0., 1. }{};

	NumberRange( const NaturalNumberRange naturalRange );

	NumberRange( const vector<char>& binary_data, vector<char>::const_iterator& current_byte );

	size_t Serialize( vector<char>& binary_data ) const;
	
	double start( void ) const{ return start_; };

	double end( void ) const{ return end_; };

	void start( const double newStart ){ start_ = ( newStart < end_ ) ? newStart : end_ - 1.; };

	void end( const double newEnd ){ end_ = ( newEnd > start_ ) ? newEnd : start_ + 1.; };

	double GetDifference( void ) const{ return end_ - start_; };

	/*!
	 * @brief Get the difference between two elements when range is divided equally spaced
	 * @param number_of_elements Amount of elements in range
	 * @return ( end_ - start_ ) / ( number_of_elements - 1 )
	*/
	double GetResolution( const size_t number_of_elements ) const;


	private:
	
	double start_;
	double end_;
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
constexpr typename std::underlying_type_t<T> ToUnderlying( T var );

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
 * @param left_operand Minuend
 * @param right_operand Subtrahend
 * @return GetDifference val1 - val2 cast to its their underlying type
*/
template <typename T>
typename std::enable_if_t<std::is_enum_v<T>, T> operator-( const T left_operand, const T right_operand );

/*!
 * @brief Convert number_of_pixel to string with given precision
 * @tparam T Type of number_of_pixel
 * @param value Value
 * @param precision Precision of converted number_of_pixel. Negative rounds number_of_pixel to significant digits
 * @return Number as string
*/
template<typename T>
string ToString( T value, const int precision = 0 );

/*!
 * @brief Convert string to number_of_pixel
 * @tparam T Type of number_of_pixel
 * @param str String
 * @return Number corresponding to string
*/
template<typename T>
T ToNum( const string str );

/*!
 * @brief Write to a variable used in multiple threads
 * @tparam T Type of variable
 * @param var Reference to variable
 * @param value Value to write
 * @param var_mutex Reference to mutex
 * @return Written value
*/
template<typename T>
T WriteThreadVar( T& var, const T& value, mutex& var_mutex );

/*!
 * @brief Set flag to false but return previous state of flag
 * @param flag Flag to set to false
 * @return Flag value before its reset
*/
bool UnsetFlag( bool& flag );

/*!
 * @brief Convert a vector pair to a vector uf 2D-Tuples
 * @param vector_pair Vector pair
 * @return Vector with 2D-Tuples
*/
vector<Tuple2D> ConvertToTuple( const VectorPair vector_pair );

#include "generel.hpp"