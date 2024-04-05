#pragma once

/*********************************************************************
 * @file   generelMath.h
 * @brief  mathematical constants and objects
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

constexpr double PI =   3.1415926535897932384626433832795;		/*!< pi */
constexpr double PI_2 = 9.8696044010893586188344909998761;		/*!< pi ^ 2*/
constexpr double c_mPers = 299792456;													/*!< speed of light in m/s */
constexpr double e_As = 1.6021917E-19;												/*!< electron charge in As */
constexpr double J_Per_eV = e_As;															/*!< joules in one eV */
constexpr double h_Js = 6.626196E-34;													/*!< planck's constant */
constexpr double h_eVs = 4.1357E-15;													/*!< planck's constant */
constexpr double h_c_eVmm = c_mPers * h_eVs * 1000.;					/*!< product of h and c in eV*mm*/

constexpr double efficiancy_constant_PerV = 1.1E-9;						/*!< efficiancy constant for roentgen power */

constexpr double me_c2_eV = 0.511e6;													/*!< compton Wavelength in eV */
constexpr double me_c2_J = me_c2_eV * J_Per_eV;								/*!< compton wavelenth in Joule */
constexpr double per_me_c2_eV = 1 / me_c2_eV;									/*!< reciprocal of compton wavelength*/

constexpr double permitivity_As_Per_Vm = 8.8541878128e-12;		/*!< epsilon0: Permitivity of vaccum in As/Vm */
constexpr double r_e_mm = 2.8179403262e-12;										/*!< electron radius in mm*/
constexpr double reduced_energy_divisor_eV = 510998.95;				/*!< divisor in reduced energy calculation m_e * c^2*/

constexpr double reference_energy_for_mu_eV = 100000.;										/*!< reference energy for stored coefficients*/
constexpr double absorption_water_Per_mm = 3e-2 * 0.997 / 10.;						/*!< absorption Water in 1 / mm for 100keV*/
constexpr double mu_air = 0.;																							/*!< absorption coefficient of air*/
constexpr double absorption_bone_Per_mm = absorption_water_Per_mm * 1.33;	/*!< absorption coefficient of bone tissue*/
constexpr double absorption_lung_Per_mm = absorption_water_Per_mm * 0.8;	/*!< absorption coefficient of lung tissue*/
constexpr double absorption_titan_Per_mm = 0.0135;												/*!< absorption coefficient of titan*/

constexpr double attenuation_water_Per_mm = 0.153 / 10;										/*!< attenuation coefficient of water*/

constexpr double change_energy_for_constant_mu = 52e3;	/*!< energy when absorption becomes constant*/

constexpr double avogadro_1Permol = 6.0221367e23;				/*!< avogardo's constant in 1/mol*/

constexpr double electron_density_water_1Permm3 =				/*!< electron density or Conversion factor from cross section to absorption coefficiant for water in 1 / mm^3*/
				 10. *		// electrons per molecule
				 .998	*		// g/cm^3
				 avogadro_1Permol /
				 18. * 1e-3; // mass of one mol cm^3/mm^3


inline std::ostream& math_error_out = std::cerr;					/*!< outstream for errors */
inline std::ostream& standard_out = std::cout;						/*!< standard out stream */



/*********************************************************************
	Functions
 *********************************************************************/

/*!
 * @brief modes for numerical comparison
 */
enum ComparisonMode{
	Absolute,
	Relative
};

/*!
 * @brief compares to values for their equality with tolerance
 * @param a value 1
 * @param b value 2
 * @param tolerance maximum difference between a and b for them to be considered equal. Absolute or relative depending on next argument
 * @param mode comparison mode: tolerance is absolute difference or relative difference
 * @return true when the difference is less than tolerance
*/
bool IsNearlyEqual( const double a, const double b, const double tolerance, const ComparisonMode mode );

/*!
 * @brief compares to values for their equality with tolerance
 * @param a value 1
 * @param b value 2
 * @return true when the difference is less than EPSILON
*/
bool IsNearlyEqualDistance( const double a, const double b );

/*!
 * @brief get the relative deviation between two values
 * @tparam T 
 * @param a 
 * @param b 
 * @return relative deviation
*/
template <typename T>
double RelativeDeviation( const T a, const T b );

/*!
 * @brief checks if number is even
 * @tparam T integer type
 * @param integer value
 * @return true when integer is even
*/
template <typename T>
typename std::enable_if_t<std::is_arithmetic_v<T>, bool>
IsEven( const T integer );

/*!
 * @brief return the minimum of two values
 * @tparam T arithmetic type
 * @param a value a
 * @param b value b
 * @return minimum value
*/
template <typename T>
typename std::enable_if_t<std::is_arithmetic_v<T>, T>
Min( const T a, const T b );

/*!
 * @brief return the maximum of two values
 * @tparam T arithmetic type
 * @param a value a
 * @param b value b
 * @return maximum value
*/
template <typename T>
typename std::enable_if_t<std::is_arithmetic_v<T>, T> 
Max( const T a, const T b );

/*!
 * @brief force a maximum value
 * @tparam T arithmetic type
 * @param value value to force 
 * @param maximum upper bound of value
 * @return value when value is smaller than maximum. maximum otherwise
*/
template <typename T>
typename std::enable_if_t<std::is_arithmetic_v<T>, T>
ForceToMax( const T value, const T maximum );

/*!
 * @brief force a minimum value
 * @tparam T arithmetic type
 * @param value value
 * @param  minimum minimum value
 * @return a or minimum when a is less than minimum
*/
template <typename T>
typename std::enable_if_t<std::is_arithmetic_v<T>, T>
ForceToMin( T value, T minimum );

/*!
 * @brief force a minimum value of one
 * @tparam T arithmetic type
 * @param a value a
 * @return a when a is greater than one. 1 otherwise
*/
template <typename T>
typename std::enable_if_t<std::is_arithmetic_v<T>, T> 
ForceToMin1( T a );

/*!
 * @brief force value to be positive
 * @tparam T arithmetic type
 * @param a value a
 * @return a when a is greater than 0. 1 otherwise
*/
template <typename T>
typename std::enable_if_t<std::is_arithmetic_v<T>, T> 
ForcePositive( T a );

/*!
 * @brief force value to an odd value
 * @tparam T integral type
 * @param a value
 * @return a when add. a + 1 when even
*/
template <typename T>
typename std::enable_if_t<std::is_integral_v<T>, T>
ForceOdd( T a );

/*!
 * @brief force value to an even value
 * @tparam T integral type
 * @param a value
 * @return a when add. a + 1 when even
*/
template <typename T>
typename std::enable_if_t<std::is_integral_v<T>, T>
ForceEven( T a );

/*!
 * @brief force value to a range
 * @tparam T arithmetic type
 * @param value value
 * @param minimum minimum value
 * @param maximum maximum value
 * @return value constraint to range
*/
template <typename T>
typename std::enable_if_t<std::is_arithmetic_v<T>, T>
ForceRange( const T value, const T minimum, const T maximum );


/*********************************************************************
	classes
 *********************************************************************/

/*!
 * @brief abstract class for mathematical objects
 * @details class with function prototypes to convert object's data to strings, dumping the data and outputting error messages
*/
class MathematicalObject{

	public:

	/*!
	 * @brief enumeration of error codes
	*/
	enum class MathError{
		Ok,					/*!< no error */
		General,		/*!< abstract math error */
		Operation,	/*!< error during mathematical operation */
		Bounds,			/*!< out of bounds error */
		Input				/*!< invalid input in function */
	};

	/*!
	 * @brief convert object's data to string
	 * @param newline_tabulators amount of tabulators to insert after each line break
	 * @return string with object's data
	 * @details virtual for object specific implementation
	*/
	virtual string ConvertToString( [[maybe_unused]] const unsigned int newline_tabulators = 0 ) const = 0;

	/*!
	 * @brief dump object's type and data to output stream
	 * @param output output stream to dump to
	 * @param object_name name of the object to include in the dump
	*/
	void Dump( std::ostream& output, const string object_name ) const;

	/*!
	 * @brief convert last error to string
	 * @param code error code
	 * @return string describing error
	*/
	string GetErrorString( const MathError code ) const;

	/*!
	 * @brief outputs error string to stream when it's not MATH_ERR::OK
	 * @param code error code
	 * @return same as code
	*/
	MathError CheckForAndOutputError( const MathError code ) const;

	/*!
	 * @brief outputs error string to stream when it's not MATH_ERR::OK
	 * @param code error code
	 * @param message message to output on math_error_out
	 * @return same as code
	*/
	MathError CheckForAndOutputError( const MathError code, const string message ) const;
};

#include "generelMath.hpp"