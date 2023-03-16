#pragma once
/*********************************************************************
 * @file   surf.h
 * @brief  Classes for surfaces
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/



 /*********************************************************************
	Includes
 *********************************************************************/
#include "generelMath.h"
#include "vec3D.h"



/*********************************************************************
   Definitions
 *********************************************************************/


/*!
 * @brief Class for unconfined surfaces
*/
class surf : virtual public mathObj{

	public:

	/*!
	 * @brief Constructor
	 * @param v1 First direction vector
	 * @param v2 Second direction vector. Must be orthogonal to first vector
	 * @param p O
	*/
	explicit surf( const uvec3  v1, const uvec3  v2, const pnt3  p );

	surf( const vector<char>& binData, vector<char>::const_iterator& it );

	/*!
	 * @brief Default constructor
	*/
	surf( void );

	/*!
	 * @brief Convert surface's data to string
	 * @param newLineTabulators Amount of tabulators to insert after each line break
	 * @return String with surface's data
	*/
	string toStr( const unsigned int newLineTabulators = 0 ) const override;

	/*!
	 * @brief Get origin of surface
	 * @return Origing
	*/
	inline pnt3  O( void ) const{ return o; };

	/*!
	 * @brief Get first direction vector
	 * @return r1 of surface
	*/
	inline uvec3  R1( void ) const{ return r1; };

	/*!
	 * @brief Get second direction vector
	 * @return r2 of surface
	*/
	inline uvec3  R2( void ) const{ return r2; };

	/*!
	 * @brief Get point on the surface based on parameters
	 * @param surfParaA Surface parameter a
	 * @param surfParaB Surface parameter b
	 * @return Point p = o + r1*a + r2*b
	*/
	pnt3  getPnt( const double surfParaA, const double surfParaB ) const;

	/*!
	 * @brief Get surface normal by cross product
	 * @return Surface normal
	*/
	uvec3  Normal( void ) const;

	/*!
	 * @brief Convert surface to different coordinate system
	 * @param cSys_ System to convert to
	 * @return Converted surface
	*/
	surf convertTo( const cartCSys* const cSys_ ) const;

	/*!
	 * @brief Checks if parameters are inside surface bounds
	 * @param a Surface parameter a
	 * @param b Surface parameter b
	 * @return Always true for unconfined surfaces
	*/
	inline bool parasInBounds( const double a, const double b ) const { return true; };

	size_t serialize( vector<char>& binData ) const;

	protected:
	uvec3  r1;					/*!<First direction vector*/
	uvec3  r2;					/*!<Second direction vector*/
	pnt3  o;						/*!<O*/
};



/*!
 * @brief Confined surface with limited parameters creating a rectangle
*/
class surfLim : public surf{

	public:

	/*!
	 * @brief Constructor
	 * @param v1 First direction vector
	 * @param v2 Second direction vector. Must be orthogonal to first vector
	 * @param p O
	 * @param aMin Lower limit for parameter a
	 * @param aMax Upper limit for parameter a
	 * @param bMin Lower limit for parameter b
	 * @param bMax Upper limit for parameter b
	*/
	explicit surfLim( const uvec3  v1, const uvec3  v2, const pnt3  p,
					  const double aMin, const double aMax,
					  const double bMin, const double bMax );

	/*!
	 * @brief Constructor
	 * @param s Base surface
	 * @param aMin Lower limit for parameter a
	 * @param aMax Upper limit for parameter a
	 * @param bMin Lower limit for parameter b
	 * @param bMax Upper limit for parameter b
	*/
	explicit surfLim( const surf s,
					  const double aMin, const double aMax,
					  const double bMin, const double bMax );

	/*!
	 * @brief Default constructor
	*/
	surfLim( void );

	surfLim( const vector<char>& binData, vector<char>::const_iterator& it );

	/*!
	 * @brief Convert surface's data to string
	 * @param newLineTabulators Amount of tabulators to insert after each line break
	 * @return String with surface's data
	*/
	std::string toStr( const unsigned int newLineTabulators = 0 ) const override;

	/*!
	 * @brief Get minimum of parameter a
	 * @return Value of aMin
	*/
	inline double AMin( void ) const{ return pAMin; };

	/*!
	 * @brief Get maximum of parameter a
	 * @return Value of aMax
	*/
	inline double AMax( void ) const{ return pAMax; };

	/*!
	 * @brief Get minimum of parameter b
	 * @return Value of bMin
	*/
	inline double BMin( void ) const{ return pBMin; };

	/*!
	 * @brief Get maximum of parameter b
	 * @return Value of bMax
	*/
	inline double BMax( void ) const{ return pBMax; };

	/*!
	 * @brief Convert surface to different coordinate system
	 * @param cSys_ System to convert to
	 * @return Converted surface
	*/
	surfLim convertTo( const cartCSys* const cSys_ ) const;

	/*!
	 * @brief Checks if parameters are inside surface bounds
	 * @param a Surface parameter a
	 * @param b Surface parameter b
	 * @return True when parameters are inside surface bounds
	*/
	bool parasInBounds( const double a, const double b ) const;

	/*!
	 * @brief Get center point of limited surface
	 * @return The center point
	*/
	pnt3  getCenter( void ) const;

	/*!
	 * @brief Get the surfaces normal as line through its center
	 * @return Normal line
	*/
	line  NormalLine( void ) const;


	size_t serialize( vector<char>& binData ) const;

	private:

	double pAMin;			/*!<Lower limit for parameter a*/
	double pAMax;			/*!<Upper limit for parameter a*/
	double pBMin;			/*!<Lower limit for parameter b*/
	double pBMax;			/*!<Upper limit for parameter b*/
};
