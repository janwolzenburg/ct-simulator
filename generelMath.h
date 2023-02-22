#pragma once

/*********************************************************************
 * @file   generelMath.h
 * @brief  Mathematical constants and objects
 * 
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/



/*********************************************************************
	Includes
 *********************************************************************/

#include <string>
using std::string;
#include <iostream>



/*********************************************************************
	Definitions
 *********************************************************************/

constexpr double EPSILON = 1E-9;								/*!< Error for equality determination of floating point numbers in mm */
constexpr double PI = 3.1415926535897932384626433832795;		/*!< PI */
constexpr double c_mPers = 299792456;							// Speed of light in m/s
constexpr double e_As = 1.6021917E-19;							// Electron charge in As 
constexpr double h_Js = 6.626196E-34;							// Planck's constant

constexpr double m_0c2_eV = 0.511E6;							// Compton Wavelength in eV

constexpr double J_PER_eV = e_As;	
constexpr double m_0c2_J = m_0c2_eV * J_PER_eV;

inline std::ostream& mathErrOut = std::cerr;					/*!< Outstream for errors */
inline std::ostream& stdOut = std::cout;						/*!< Standard out stream*/



/*********************************************************************
	Functions
 *********************************************************************/

/*!
 * @brief Compares to values for their equality with tolerance
 * @param a Value 1
 * @param b Value 2
 * @param tolerance Maximum difference between a and b for them to be considered equal
 * @return True when the difference is less than tolerance
*/
bool iseqErr( const double a, const double b, const double tolerance );

/*!
 * @brief Compares to values for their equality with tolerance
 * @param a Value 1
 * @param b Value 2
 * @return True when the difference is less than EPSILON
*/
bool iseqErr( const double a, const double b );


/*!
 * @brief Checks if number is even
 * @tparam T Integer type
 * @param integer Value
 * @return True when integer is even
*/
template <typename T>
typename std::enable_if_t<std::is_arithmetic_v<T>, bool>
isEven( const T integer );

/*!
 * @brief Return the minimum of two values
 * @tparam T Arithmetic type
 * @param a Value a
 * @param b Value b
 * @return Minimum value
*/
template <typename T>
typename std::enable_if_t<std::is_arithmetic_v<T>, T>
min( const T a, const T b );

/*!
 * @brief Return the maximum of two values
 * @tparam T Arithmetic type
 * @param a Value a
 * @param b Value b
 * @return Maximum value
*/
template <typename T>
typename std::enable_if_t<std::is_arithmetic_v<T>, T> 
max( const T a, const T b );

/*!
 * @brief Force a maximum value
 * @tparam T Arithmetic type
 * @param value Value to force 
 * @param maximum upper bound of value
 * @return value when value is smaller than maximum. Maximum otherwise
*/
template <typename T>
typename std::enable_if_t<std::is_arithmetic_v<T>, T>
Fmax( const T value, const T maximum );

/*!
 * @brief Force a minimum value of one
 * @tparam T Arithmetic type
 * @param a Value a
 * @return a when a is greater than one. 1 otherwise
*/
template <typename T>
typename std::enable_if_t<std::is_arithmetic_v<T>, T> 
Fmin1( T a );

/*!
 * @brief Force a minimum value
 * @tparam T Arithmetic type
 * @param a Value a
 * @param minValue minimum value
 * @return a or minValue when a is less than minValue
*/
template <typename T>
typename std::enable_if_t<std::is_arithmetic_v<T>, T>
Fmin( T a, T minValue );

/*!
 * @brief Force value to be positive
 * @tparam T Arithmetic type
 * @param a Value a
 * @return a when a is greater than 0. 1 otherwise
*/
template <typename T>
typename std::enable_if_t<std::is_arithmetic_v<T>, T> 
Fpos( T a );

template <typename T>
typename std::enable_if_t<std::is_integral_v<T>, T>
FOdd( T a );


/*********************************************************************
	Classes
 *********************************************************************/

/*!
 * @brief Abstract class for mathematical objects
 * @details Class with function prototypes to convert object's data to strings, dumping the data and outputting error messages
*/
class mathObj{
	public:

	/*!
	 * @brief Enumeration of error codes
	*/
	enum class MATH_ERR{
		OK,				/*!<  No error */
		GENERAL,		/*!<  Abstract math error */
		OPERATION,		/*!<  Error during mathematical operation */
		BOUNDS,			/*!<  Out of bounds error */
		INPUT			/*!<  Invalid input in function */
	};

	/*!
	 * @brief Destructor is virtual
	*/
	virtual ~mathObj(){};

	/*!
	 * @brief Convert object's data to string
	 * @param newLineTabulators Amount of tabulators to insert after each line break
	 * @return String with object's data
	 * @details Virtual for object specific implementation
	*/
	virtual string toStr( [[maybe_unused]] const unsigned int newLineTabulators = 0 ) const = 0;

	/*!
	 * @brief Dump object's type and data to output stream
	 * @param output Output stream to dump to
	 * @param objName Name of the object to include in the dump
	*/
	void dump( std::ostream& output, const string objName ) const;

	/*!
	 * @brief Convert last error to string<
	 * @param code Error code
	 * @return String describing error
	*/
	string errStr( const MATH_ERR code ) const;

	/*!
	 * @brief Outputs error string to stream when it's not MATH_ERR::OK
	 * @param code Error code
	 * @return Same as code
	*/
	MATH_ERR checkErr( const MATH_ERR code ) const;

	/*!
	 * @brief Outputs error string to stream when it's not MATH_ERR::OK
	 * @param code Error code
	 * @param message Message to output on mathErrOut
	 * @return Same as code
	*/
	MATH_ERR checkErr( const MATH_ERR code, const string message ) const;
};

#include "generelMath.hpp"