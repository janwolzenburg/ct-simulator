#pragma once
/*********************************************************************
 * @file   Surface.h
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
class Surface : public MathematicalObject{

	public:

	/*!
	 * @brief Constructor
	 * @param direction_1 First direction vector
	 * @param direction_2 Second direction vector. Must be orthogonal to first vector
	 * @param origin Origin
	*/
	explicit Surface( const UnitVector3D direction_1, const UnitVector3D  direction_2, const Point3D  origin );

	/*!
	 * @brief Construct from binary data_
	 * @param binary_data Data vector
	 * @param current_byte Iterator
	 * @param coordinate_system System to assign surface to
	*/
	Surface( const vector<char>& binary_data, vector<char>::const_iterator& current_byte, CoordinateSystem* coordinate_system );

	/*!
	 * @brief Default constructor
	*/
	Surface( void );

	/*!
	 * @brief Convert surface's data_ to string
	 * @param newline_tabulators Amount of tabulators to insert after each Line break
	 * @return String with surface's data_
	*/
	string ToString( const unsigned int newline_tabulators = 0 ) const override;

	/*!
	 * @brief Serialize this object
	 * @param binary_data Reference to vector where data_ will be appended
	*/
	size_t Serialize( vector<char>& binary_data ) const;

	/*!
	 * @brief Get origin_ of surface
	 * @return Origing
	*/
	Point3D origin( void ) const{ return origin_; };

	/*!
	 * @brief Get first direction vector
	 * @return direction_1_ of surface
	*/
	UnitVector3D  direction_1( void ) const{ return direction_1_; };

	/*!
	 * @brief Get second direction vector
	 * @return direction_2_ of surface
	*/
	UnitVector3D  direction_2( void ) const{ return direction_2_; };

	/*!
	 * @brief Get point on the surface based on parameters
	 * @param surface_parameter_1 Surface parameter a
	 * @param surface_parameter_2 Surface parameter b
	 * @return Point p = origin_ + direction_1_*a + direction_2_*b
	*/
	Point3D GetPoint( const double surface_parameter_1, const double surface_parameter_2 ) const{ return  origin_ + ( direction_1_ * surface_parameter_1 + direction_2_ * surface_parameter_2 ); };

	/*!
	 * @brief Get surface normal by cross product
	 * @return Surface normal
	*/
	UnitVector3D GetNormal( void ) const{ return direction_1_ ^ direction_2_; };

	/*!
	 * @brief Convert surface to different coordinate system
	 * @param coordinate_system System to convert to
	 * @return Converted surface
	*/
	Surface ConvertTo( const CoordinateSystem* const coordinate_system ) const;

	/*!
	 * @brief Checks if parameters are inside surface bounds
	 * @param a Surface parameter a
	 * @param b Surface parameter b
	 * @return Always true for unconfined surfaces
	*/
	virtual bool AreParametersInBounds( [[maybe_unused]] const double parameter_1, [[maybe_unused]] const double parameter_2 ) const { return true; };


	protected:
	UnitVector3D  direction_1_;			/*!<First direction vector*/
	UnitVector3D  direction_2_;			/*!<Second direction vector*/
	Point3D  origin_;					/*!<Origin*/
};



/*!
 * @brief Confined surface with limited parameters creating a rectangle
*/
class BoundedSurface : public Surface{

	public:

	/*!
	 * @brief Constructor
	 * @param direction_1 First direction vector
	 * @param direction_2 Second direction vector. Must be orthogonal to first vector
	 * @param origin Origin
	 * @param direction_1_range Limits for parameter a
	 * @param direction_2_range Limits for parameter b
	*/
	explicit BoundedSurface( const UnitVector3D  direction_1, const UnitVector3D  direction_2, const Point3D  origin,
							 const NumberRange direction_1_range, const NumberRange direction_2_range );

	/*!
	 * @brief Constructor
	 * @param direction_1 First direction vector
	 * @param direction_2 Second direction vector. Must be orthogonal to first vector
	 * @param origin Origin
	 * @param parameter_1_min Lower bound for parameter 1
	 * @param parameter_1_max Upper bound for parameter 1
	 * @param parameter_2_min Lower bound for parameter 2
	 * @param parameter_2_max Upper bound for parameter 2
	*/
	explicit BoundedSurface( const UnitVector3D  direction_1, const UnitVector3D  direction_2, const Point3D  origin,
							 const double parameter_1_min, const double parameter_1_max,
							 const double parameter_2_min, const double parameter_2_max );

	/*!
	 * @brief Constructor
	 * @param s Base surface
	 * @param direction_1_range Limits for parameter a
	 * @param direction_2_range Limits for parameter b
	*/
	explicit BoundedSurface( const Surface surface,
							 const NumberRange direction_1_range,  const NumberRange direction_2_range );

	/*!
	 * @brief Constructor
	 * @param s Base surface
	 * @param parameter_1_min Lower bound for parameter 1
	 * @param parameter_1_max Upper bound for parameter 1
	 * @param parameter_2_min Lower bound for parameter 2
	 * @param parameter_2_max Upper bound for parameter 2
	*/
	explicit BoundedSurface( const Surface surface,
							 const double parameter_1_min, const double parameter_1_max,
							 const double parameter_2_min, const double parameter_2_max );

	/*!
	 * @brief Default constructor
	*/
	BoundedSurface( void ) : BoundedSurface( Surface{}, NumberRange{ 0., 1. }, NumberRange{ 0., 1. } ){};

	/*!
	 * @brief Construct from binary data_
	 * @param binary_data Data vector
	 * @param current_byte Iterator
	 * @param coordinate_system System to assign surface to
	*/
	BoundedSurface( const vector<char>& binary_data, vector<char>::const_iterator& current_byte, CoordinateSystem* coordinate_system );

	/*!
	 * @brief Convert surface's data_ to string
	 * @param newline_tabulators Amount of tabulators to insert after each Line break
	 * @return String with surface's data_
	*/
	std::string ToString( const unsigned int newline_tabulators = 0 ) const override;

	/*!
	 * @brief Serialize this object
	 * @param binary_data Reference to vector where data_ will be appended
	*/
	size_t Serialize( vector<char>& binary_data ) const;

	/*!
	 * @brief Get minimum of parameter a
	 * @return Value of aMin
	*/
	double parameter_1_min( void ) const{ return parameter_1_range_.start(); };

	/*!
	 * @brief Get maximum of parameter a
	 * @return Value of aMax
	*/
	double parameter_1_max( void ) const{ return parameter_1_range_.end(); };

	/*!
	 * @brief Get minimum of parameter b
	 * @return Value of bMin
	*/
	double parameter_2_min( void ) const{ return parameter_2_range_.start(); };

	/*!
	 * @brief Get maximum of parameter b
	 * @return Value of bMax
	*/
	double parameter_2_max( void ) const{ return parameter_2_range_.end(); };

	/*!
	 * @brief Convert surface to different coordinate system
	 * @param coordinate_system System to convert to
	 * @return Converted surface
	*/
	BoundedSurface ConvertTo( const CoordinateSystem* const coordinate_system ) const{ return BoundedSurface{ this->Surface::ConvertTo( coordinate_system ), parameter_1_range_, parameter_2_range_ }; };

	/*!
	 * @brief Checks if parameters are inside surface bounds
	 * @param parameter_1 Surface parameter a
	 * @param parameter_2 Surface parameter b
	 * @return True when parameters are inside surface bounds
	*/
	bool AreParametersInBounds( const double parameter_1, const double parameter_2 ) const override{ 
		return parameter_1_min() <= parameter_1 && parameter_1 <= parameter_1_max() && parameter_2_min() <= parameter_2 && parameter_2 <= parameter_2_max();  };

	/*!
	 * @brief Get center point of limited surface
	 * @return The center point
	*/
	Point3D  GetCenter( void ) const{ return this->GetPoint( ( parameter_1_max() + parameter_1_min() ) / 2, ( parameter_2_max() + parameter_2_min() ) / 2); };

	/*!
	 * @brief Get the surfaces normal as Line through its center
	 * @return GetCenterNormal Line
	*/
	Line  GetCenterNormal( void ) const;

	
	private:
	
	NumberRange parameter_1_range_;		/*!<Bounds for parameter 1*/
	NumberRange parameter_2_range_;		/*!<Bounds for parameter 2*/
	
};
