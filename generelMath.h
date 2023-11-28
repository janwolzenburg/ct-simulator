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
#include <type_traits>


/*********************************************************************
	Definitions
 *********************************************************************/

constexpr double PI =   3.1415926535897932384626433832795;		/*!< PI */
constexpr double PI_2 = 9.8696044010893586188344909998761;		/*!< PI ^ 2*/
constexpr double c_mPers = 299792456;							/*!< Speed of light in m/s */
constexpr double e_As = 1.6021917E-19;							/*!< Electron charge in As */
constexpr double J_Per_eV = e_As;								/*!< Joules in one eV */
constexpr double h_Js = 6.626196E-34;							/*!< Planck's constant */
constexpr double h_eVs = 4.1357E-15;							/*!< Planck's constant */
constexpr double h_c_eVmm = c_mPers * h_eVs * 1000.;			/*!< Product of h and c in eV*mm*/

constexpr double me_c2_eV = 0.511e6;							/*!< Compton Wavelength in eV */
constexpr double me_c2_J = me_c2_eV * J_Per_eV;					/*!< Compton wavelenth in Joule */
constexpr double Per_me_c2_eV = 1 / me_c2_eV;					

constexpr double permitivity_As_Per_Vm = 8.8541878128e-12;		/*!< epsilon0: Permitivity of vaccum in As/Vm */
constexpr double r_e_mm = 2.8179403262e-12;						/*!< Electron radius in mm*/
constexpr double reduced_energy_divisor_eV = 510998.95;			/*!< Divisor in reduced energy calculation m_e * c^2*/

constexpr double reference_energy_for_mu_eV = 100000.;
constexpr double mu_water = 3e-3 * 0.997 / 10.;					/*!< Absorption Water in 1 / mm for 100keV*/
constexpr double mu_air = 0.;
constexpr double mu_bone = mu_water * 1.33;
constexpr double mu_lung = mu_water * 0.8;
constexpr double mu_iron = 0.156;

constexpr double change_energy_for_constant_mu = 49e3;

constexpr double avogadro_1Permol = 6.0221367e23;				/*!< Avogardo's constant in 1/mol*/

constexpr double electron_density_water_1Permm3 =				/*!< Electron density or Conversion factor from cross section to absorption coefficiant for water in 1 / mm^3*/
				 10. *		// Electrons per molecule
				 .998	*	// g/cm^3
				 avogadro_1Permol /
				 18. * 1e-3; // mass of one mol cm^3/mm^3

constexpr size_t bins_per_energy = 16;		/*!< When scattering: Into how many bins should the photons with the same energy be divided into*/

inline std::ostream& math_error_out = std::cerr;					/*!< Outstream for errors */
inline std::ostream& standard_out = std::cout;						/*!< Standard out stream */



/*********************************************************************
	Functions
 *********************************************************************/

enum ComparisonMode{
	Absolute,
	Relative
};

/*!
 * @brief Compares to values for their equality with tolerance
 * @param a Value 1
 * @param b Value 2
 * @param tolerance Maximum difference between a and b for them to be considered equal. Absolute or relative depending on next argument
 * @param mode Comparison mode: tolerance is absolute difference or relative difference
 * @return True when the difference is less than tolerance
*/
bool IsNearlyEqual( const double a, const double b, const double tolerance, const ComparisonMode mode );

/*!
 * @brief Compares to values for their equality with tolerance
 * @param a Value 1
 * @param b Value 2
 * @return True when the difference is less than EPSILON
*/
bool IsNearlyEqualDistance( const double a, const double b );

/*!
 * @brief Get the relative deviation between two values
 * @tparam T 
 * @param a 
 * @param b 
 * @return 
*/
template <typename T>
double RelativeDeviation( const T a, const T b );

/*!
 * @brief Checks if number_of_pixel is even
 * @tparam T Integer type
 * @param integer Value
 * @return True when integer is even
*/
template <typename T>
typename std::enable_if_t<std::is_arithmetic_v<T>, bool>
IsEven( const T integer );

/*!
 * @brief Return the minimum of two values
 * @tparam T Arithmetic type
 * @param a Value a
 * @param b Value b
 * @return Minimum value
*/
template <typename T>
typename std::enable_if_t<std::is_arithmetic_v<T>, T>
Min( const T a, const T b );

/*!
 * @brief Return the maximum of two values
 * @tparam T Arithmetic type
 * @param a Value a
 * @param b Value b
 * @return Maximum value
*/
template <typename T>
typename std::enable_if_t<std::is_arithmetic_v<T>, T> 
Max( const T a, const T b );

/*!
 * @brief Force a maximum value
 * @tparam T Arithmetic type
 * @param value Value to force 
 * @param maximum upper bound of value
 * @return value when value is smaller than maximum. Maximum otherwise
*/
template <typename T>
typename std::enable_if_t<std::is_arithmetic_v<T>, T>
ForceToMax( const T value, const T maximum );

/*!
 * @brief Force a minimum value
 * @tparam T Arithmetic type
 * @param value Value
 * @param  m minimum value
 * @return a or minValue when a is less than minValue
*/
template <typename T>
typename std::enable_if_t<std::is_arithmetic_v<T>, T>
ForceToMin( T value, T minimum );

/*!
 * @brief Force a minimum value of one
 * @tparam T Arithmetic type
 * @param a Value a
 * @return a when a is greater than one. 1 otherwise
*/
template <typename T>
typename std::enable_if_t<std::is_arithmetic_v<T>, T> 
ForceToMin1( T a );

/*!
 * @brief Force value to be positive
 * @tparam T Arithmetic type
 * @param a Value a
 * @return a when a is greater than 0. 1 otherwise
*/
template <typename T>
typename std::enable_if_t<std::is_arithmetic_v<T>, T> 
ForcePositive( T a );

/*!
 * @brief Force value to an odd value
 * @tparam T Integral type
 * @param a Value
 * @return a when add. a + 1 when even
*/
template <typename T>
typename std::enable_if_t<std::is_integral_v<T>, T>
ForceOdd( T a );

/*!
 * @brief Force value to an even value
 * @tparam T Integral type
 * @param a Value
 * @return a when add. a + 1 when even
*/
template <typename T>
typename std::enable_if_t<std::is_integral_v<T>, T>
Forceeven( T a );

/*!
 * @brief Force value to a range
 * @tparam T Arithmetic type
 * @param value Value
 * @param minimum Minimum value
 * @param maximum Maximum value
 * @return Value constraint to range
*/
template <typename T>
typename std::enable_if_t<std::is_arithmetic_v<T>, T>
ForceRange( const T value, const T minimum, const T maximum );


/*********************************************************************
	Classes
 *********************************************************************/

/*!
 * @brief Abstract class for mathematical objects
 * @details Class with function prototypes to convert object's data to strings, dumping the data and outputting error messages
*/
class MathematicalObject{
	public:

	/*!
	 * @brief Enumeration of error codes
	*/
	enum class MathError{
		Ok,				/*!<  No error */
		General,		/*!<  Abstract math error */
		Operation,		/*!<  Error during mathematical operation */
		Bounds,			/*!<  Out of bounds error */
		Input			/*!<  Invalid input in function */
	};

	/*!
	 * @brief Convert object's data to string
	 * @param newline_tabulators Amount of tabulators to insert after each Line break
	 * @return String with object's data_
	 * @details Virtual for object specific implementation
	*/
	virtual string ToString( [[maybe_unused]] const unsigned int newline_tabulators = 0 ) const = 0;

	/*!
	 * @brief Dump object's type and data to output stream
	 * @param output Output stream to dump to
	 * @param objName Name of the object to include in the dump
	*/
	void Dump( std::ostream& output, const string objName ) const;

	/*!
	 * @brief Convert last error to string<
	 * @param code Error code
	 * @return String describing error
	*/
	string GetErrorString( const MathError code ) const;

	/*!
	 * @brief Outputs error string to stream when it's not MATH_ERR::OK
	 * @param code Error code
	 * @return Same as code
	*/
	MathError CheckForAndOutputError( const MathError code ) const;

	/*!
	 * @brief Outputs error string to stream when it's not MATH_ERR::OK
	 * @param code Error code
	 * @param message Message to output on mathErrOut
	 * @return Same as code
	*/
	MathError CheckForAndOutputError( const MathError code, const string message ) const;
};

#include "generelMath.hpp"