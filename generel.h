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


 /*********************************************************************
	definitions
 *********************************************************************/

 //#define VERIFY

 #ifdef VERIFY
 #define TRANSMISSION_TRACKING
 #endif
 
 // global variable to store a single thread id
 static std::thread::id first_thread_id;

 // maximum size of vectors -> used to check data validity when deserializing
 static constexpr size_t maximum_vector_size = 1 << 28;


/*!
 * @brief pair of vectors
*/
typedef pair<vector<double>, vector<double>> VectorPair;


/*!
 * @brief 2D indices
*/
struct Index2D{
	size_t x = 0;		/*!< x index*/
	size_t y = 0;		/*!< y index*/
 };


 /*!
 * @brief primitive 2D vector
*/
struct Tuple2D{
	double x = 0.;	/*!< x value*/
	double y = 0.;	/*!< y value*/
};


 /*!
 * @brief class for 3D indices
*/
class Index3D{

	public:

	/*!
	 * @brief constructor
	 * @param x_ x value
	 * @param y_ y value
	 * @param z_ z value
	 */
	Index3D( const size_t x_, const size_t y_, const size_t z_ ) : x( x_ ), y( y_ ), z( z_ ) {};

	/*!
	 * @brief default constructor 
	 */
	Index3D( void ) : Index3D{ 0, 0, 0 }{};

	/*!
	 * @brief construct from serialsized data
	 * @param binary_data data
	 * @param current_byte iterator pointing to next byte
	 */
	Index3D( const vector<char>& binary_data, vector<char>::const_iterator& current_byte );

	/*!
	 * @brief serialize this instance
	 * @param binary_data data to write to
	 * @return bytes written
	 */
	size_t Serialize( vector<char>& binary_data ) const;

	/*!
	 * @brief check for equality
	 * @param operand second instance
	 * @return true when equal
	 */
	bool operator==( const Index3D& operand ) const{ return x == operand.x && y == operand.y && z == operand.y; };


	size_t x;	/*!< x index*/
	size_t y;	/*!< y index*/
	size_t z;	/*!< z index*/
};


/*!
* @brief class to store three double values with x,y and z coordinate
*/
class Tuple3D{

	public:

	/*!
	 * @brief constructor
	 * @param x_ x value
	 * @param y_ y value
	 * @param z_ z value
	 */
	Tuple3D( const double x_, const double y_, const double z_ ) : x( x_ ), y( y_ ), z( z_ ){};
	
	/*!
	 * @brief default constructor
	 */
	Tuple3D( void ) : Tuple3D{ 0., 0., 0. } {};

	/*!
	 * @brief construct from serialsized data
	 * @param binary_data data
	 * @param current_byte iterator pointing to next byte
	 */
	Tuple3D( const vector<char>& binary_data, vector<char>::const_iterator& current_byte );
	
	/*!
	 * @brief serialize this instance
	 * @param binary_data data to write to
	 * @return bytes written
	 */
	size_t Serialize( vector<char>& binary_data ) const;


	double x;	/*!< x value*/
	double y;	/*!< y value*/
	double z;	/*!< z value*/
};


/*!
 * @brief class for indicies to data organized in row/column structure
 */
class GridIndex{

	public:

	GridIndex( const size_t column, const size_t row ) : c( column ), r( row ) {};
	
	/*!
	 * @brief default constructor
	 */
	GridIndex( void ) : GridIndex{ 0, 0 } {};

	/*!
	 * @brief construct from serialsized data
	 * @param binary_data data
	 * @param current_byte iterator pointing to next byte
	 */
	GridIndex( const vector<char>& binary_data, vector<char>::const_iterator& current_byte );

	/*!
	 * @brief serialize this instance
	 * @param binary_data data to write to
	 * @return bytes written
	 */
	size_t Serialize( vector<char>& binary_data ) const;


	size_t c;	/*!< Column*/
	size_t r;	/*!< Row*/
};


/*!
 * @brief class for  data organized in row/column structure
 */
class GridCoordinates {

	public:

	/*!
	 * @brief constructor
	 * @param column column coordinate
	 * @param row	row coordinate
	 */
	GridCoordinates( const double column, const double row ) : c( column ), r( row ) {};
	
	/*!
	 * @brief default constructor
	 */
	GridCoordinates( void ) : GridCoordinates{ 0., 0. } {};

	/*!
	 * @brief construct from serialsized data
	 * @param binary_data data
	 * @param current_byte iterator pointing to next byte
	 */
	GridCoordinates( const vector<char>& binary_data, vector<char>::const_iterator& current_byte );

	/*!
	 * @brief serialize this instance
	 * @param binary_data data to write to
	 * @return bytes written
	 */
	size_t Serialize( vector<char>& binary_data ) const;


	double c;	/*!< column coordinate*/
	double r; /*!< row coordinate*/
};


/*!
 * @brief class for a range of whole numbers
*/
class NaturalNumberRange{

	public:

	/*!
	 * @brief constructor
	 * @param start start of range
	 * @param end end of range
	 */
	NaturalNumberRange( const signed long long start, const signed long long end );

	/*!
	 * @brief get the start
	 * @return start of range
	 */
	signed long long start( void ) const{ return start_; };

	/*!
	 * @brief get the end
	 * @return end of range
	 */
	signed long long end( void ) const{ return end_; };

	/*!
	 * @brief set new start
	 * @param new_start new start
	 */
	void start( const signed long long new_start ){ start_ = ( new_start < end_ ) ? new_start : end_ - 1; };

	/*!
	 * @brief set new end
	 * @param new_start new end
	 */
	void end( const signed long long new_end ){ end_ = ( new_end > start_ ) ? new_end : start_ + 1; };


	private:

	signed long long start_;	/*!< start of range*/
	signed long long end_;		/*!< end of range*/
};


/*!
 * @brief class for a range of real numbers 
*/
class NumberRange{

	public:
	
	/*!
	 * @brief constructor
	 * @param start start of range
	 * @param end end of range
	 */
	NumberRange( const double start, const double end );
	
	/*!
	 * @brief default constructor
	 */
	NumberRange( void ) : NumberRange{ 0., 1. }{};

	/*!
	 * @brief converting constructor
	 * @param natural_number_range natural number range
	 */
	NumberRange( const NaturalNumberRange& natural_number_range );

	/*!
	 * @brief construct from serialsized data
	 * @param binary_data data
	 * @param current_byte iterator pointing to next byte
	 */
	NumberRange( const vector<char>& binary_data, vector<char>::const_iterator& current_byte );

	/*!
	 * @brief serialize this instance
	 * @param binary_data data to write to
	 * @return bytes written
	 */
	size_t Serialize( vector<char>& binary_data ) const;
	
	/*!
	 * @brief get the start
	 * @return start of range
	 */
	signed long long start( void ) const{ return start_; };

	/*!
	 * @brief get the end
	 * @return end of range
	 */
	signed long long end( void ) const{ return end_; };

	/*!
	 * @brief set new start
	 * @param new_start new start
	 */
	void start( const signed long long new_start ){ start_ = ( new_start < end_ ) ? new_start : end_ - 1.; };

	/*!
	 * @brief set new end
	 * @param new_start new end
	 */
	void end( const signed long long new_end ){ end_ = ( new_end > start_ ) ? new_end : start_ + 1.; };

	/*!
	 * @brief get the size of the range
	 * @return difference of end to start
	 */
	double GetDifference( void ) const{ return end_ - start_; };

	/*!
	 * @brief Get the difference between two elements when range is divided equally spaced
	 * @param number_of_elements Amount of elements in range
	 * @return ( end_ - start_ ) / ( number_of_elements - 1 )
	*/
	double GetResolution( const size_t number_of_elements ) const;


	private:
	
	double start_;	/*!< start of range*/
	double end_;		/*!< end of range*/
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