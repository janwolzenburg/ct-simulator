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
#include "vector3D.h"



/*********************************************************************
   Definitions
 *********************************************************************/


/*!
 * @brief Class for unconfined surfaces
*/
class surf : public MathematicalObject{

	public:

	/*!
	 * @brief Constructor
	 * @param v1 First direction vector
	 * @param v2 Second direction vector. Must be orthogonal to first vector
	 * @param p Origin
	*/
	explicit surf( const UnitVector3D  v1, const UnitVector3D  v2, const Point3D  p );

	/*!
	 * @brief Construct from binary data
	 * @param binary_data Data vector
	 * @param it Iterator
	 * @param coordinate_system System to assign surface to
	*/
	surf( const vector<char>& binary_data, vector<char>::const_iterator& it, CoordinateSystem* coordinate_system );

	/*!
	 * @brief Default constructor
	*/
	surf( void );

	/*!
	 * @brief Convert surface's data to string
	 * @param newline_tabulators Amount of tabulators to insert after each line break
	 * @return String with surface's data
	*/
	string ToString( const unsigned int newline_tabulators = 0 ) const override;

	/*!
	 * @brief Get origin_ of surface
	 * @return Origing
	*/
	Point3D  O( void ) const{ return o; };

	/*!
	 * @brief Get first direction vector
	 * @return r1 of surface
	*/
	UnitVector3D  R1( void ) const{ return r1; };

	/*!
	 * @brief Get second direction vector
	 * @return r2 of surface
	*/
	UnitVector3D  R2( void ) const{ return r2; };

	/*!
	 * @brief Get point on the surface based on parameters
	 * @param surfParaA Surface parameter a
	 * @param surfParaB Surface parameter b
	 * @return Point p = o + r1*a + r2*b
	*/
	Point3D  getPnt( const double surfParaA, const double surfParaB ) const{ return  o + ( r1 * surfParaA + r2 * surfParaB ); };

	/*!
	 * @brief Get surface normal by cross product
	 * @return Surface normal
	*/
	UnitVector3D  Normal( void ) const{ return r1 ^ r2; };

	/*!
	 * @brief Convert surface to different coordinate system
	 * @param coordinate_system System to convert to
	 * @return Converted surface
	*/
	surf convertTo( const CoordinateSystem* const coordinate_system ) const;

	/*!
	 * @brief Checks if parameters are inside surface bounds
	 * @param a Surface parameter a
	 * @param b Surface parameter b
	 * @return Always true for unconfined surfaces
	*/
	virtual bool parasInBounds( [[maybe_unused]] const double a, [[maybe_unused]] const double b ) const { return true; };

	/*!
	 * @brief Serialize this object
	 * @param binary_data Reference to vector where data will be appended
	*/
	size_t Serialize( vector<char>& binary_data ) const;


	protected:
	UnitVector3D  r1;					/*!<First direction vector*/
	UnitVector3D  r2;					/*!<Second direction vector*/
	Point3D  o;						/*!<Origin*/
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
	 * @param p Origin
	 * @param aMin Lower limit for parameter a
	 * @param aMax Upper limit for parameter a
	 * @param bMin Lower limit for parameter b
	 * @param bMax Upper limit for parameter b
	*/
	explicit surfLim( const UnitVector3D  v1, const UnitVector3D  v2, const Point3D  p,
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
	surfLim( void ) : surfLim( surf{}, 0, 1, 0, 1 ) {};

	/*!
	 * @brief Construct from binary data
	 * @param binary_data Data vector
	 * @param it Iterator
	 * @param coordinate_system System to assign surface to
	*/
	surfLim( const vector<char>& binary_data, vector<char>::const_iterator& it, CoordinateSystem* coordinate_system );

	/*!
	 * @brief Convert surface's data to string
	 * @param newline_tabulators Amount of tabulators to insert after each line break
	 * @return String with surface's data
	*/
	std::string ToString( const unsigned int newline_tabulators = 0 ) const override;

	/*!
	 * @brief Get minimum of parameter a
	 * @return Value of aMin
	*/
	double AMin( void ) const{ return pAMin; };

	/*!
	 * @brief Get maximum of parameter a
	 * @return Value of aMax
	*/
	double AMax( void ) const{ return pAMax; };

	/*!
	 * @brief Get minimum of parameter b
	 * @return Value of bMin
	*/
	double BMin( void ) const{ return pBMin; };

	/*!
	 * @brief Get maximum of parameter b
	 * @return Value of bMax
	*/
	double BMax( void ) const{ return pBMax; };

	/*!
	 * @brief Convert surface to different coordinate system
	 * @param coordinate_system System to convert to
	 * @return Converted surface
	*/
	surfLim convertTo( const CoordinateSystem* const coordinate_system ) const{ return surfLim{ this->surf::convertTo( coordinate_system ), pAMin, pAMax, pBMin, pBMax }; };

	/*!
	 * @brief Checks if parameters are inside surface bounds
	 * @param a Surface parameter a
	 * @param b Surface parameter b
	 * @return True when parameters are inside surface bounds
	*/
	bool parasInBounds( const double a, const double b ) const override{ return pAMin <= a && a <= pAMax && pBMin <= b && b <= pBMax; };

	/*!
	 * @brief Get center point of limited surface
	 * @return The center point
	*/
	Point3D  getCenter( void ) const{ return this->getPnt( ( pAMax + pAMin ) / 2, ( pBMax + pBMin ) / 2 ); };

	/*!
	 * @brief Get the surfaces normal as line through its center
	 * @return Normal line
	*/
	line  NormalLine( void ) const;

	/*!
	 * @brief Serialize this object
	 * @param binary_data Reference to vector where data will be appended
	*/
	size_t Serialize( vector<char>& binary_data ) const;

	private:

	double pAMin;			/*!<Lower limit for parameter a*/
	double pAMax;			/*!<Upper limit for parameter a*/
	double pBMin;			/*!<Lower limit for parameter b*/
	double pBMax;			/*!<Upper limit for parameter b*/
};
