#pragma once
/*********************************************************************
 * @file   intersections.h
 * @brief  Classes for operations on mathematical objects
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/
#include <type_traits>

#include "generelMath.h"
#include "vector3D.h"
#include "line.h"
#include "surf.h"
#include "rays.h"
#include "voxel.h"


 /*********************************************************************
	Definitions
 *********************************************************************/

 /*!
  * @brief Class for an intersection result of two lines
 */
 class lineLine_Intersection_Result : public MathematicalObject{
 
	 public:

	 /*!
	  * @brief Default constructor
	 */
	 lineLine_Intersection_Result( void ) : 
		intersectionPoint( Point3D{} ),
		hasSolution( false ),
		lineParameter1( 0 ), lineParameter2( 0 )
	{};

	 /*!
	  * @brief Convert result's data to string
	  * @return String with result's data
	 */
	 string ToString( const unsigned int newline_tabulators = 0 ) const override;


	 public:

	 Point3D intersectionPoint;		/*!<Point of intersection*/
	 bool hasSolution;			/*!Flag whether the lines intersect*/
	 double lineParameter1;		/*!Parameter of first line at intersection*/
	 double lineParameter2;		/*!Parameter of second line at intersection*/


 };


 class lineLine_Intersection{

	 public:

	 /*!
	  * @brief Constructor
	  * @param l1_ Object derived from class line
	  * @param l2_ Object derived from class surf
	 */
	 lineLine_Intersection( const line l1_, const line l2_ );


	 public:

	 line l1;									/*!<First Line*/
	 line l2;									/*!<Second Line*/
	 lineLine_Intersection_Result result;		/*!<Calculated result*/

 };



/*!
 * @brief Class describing the intersection result of a line and surface
*/
class linSurf_Intersection_Result : virtual public MathematicalObject{

	public:

	/*!
	 * @brief Default constructor
	*/
	linSurf_Intersection_Result( void ) :
		intersectionPoint( Point3D{} ),
		hasSolution( false ),
		linePara( 0 ), surfParaA( 0 ), surfParaB( 0 )
	{};

	/*!
	 * @brief Convert result's data to string
	 * @return String with result's data
	*/
	string ToString( const unsigned int newline_tabulators = 0 ) const override;


	public:

	Point3D intersectionPoint;					/*!< Point of intersection */
	bool hasSolution;						/*!< Line intersects surface */
	double linePara;						/*!< Line parameter where intersection occurs */
	double surfParaA;						/*!< Surface parameter a where intersection occurs */
	double surfParaB;						/*!< Surface parameter b where intersection occurs */

};



/*!
 * @brief Class for calculation the intersection of a line and a surface
*/
template<class L, class S>
class linSurfIntersection {

	// Assert base classes
	static_assert( std::is_base_of_v< surf, S > );
	static_assert( std::is_base_of_v< line, L > );


	public:

	/*!
	 * @brief Constructor
	 * @param l_ Object derived from class line
	 * @param s_ Object derived from class surf
	*/
	linSurfIntersection( const L l_, const S s_ );


	public:
	L l;									/*!<Line*/
	S s;									/*!<Surface*/
	linSurf_Intersection_Result result;		/*!<Calculated result*/
	

};



/*!
 * @brief Class describing iuntersection of one voxel face with ray
*/
class rayVox_Intersection_Result : public linSurf_Intersection_Result{

	public:

	/*!
	 * @brief Constructor
	 * @param res_ Instance of parent_ class
	*/
	rayVox_Intersection_Result( const linSurf_Intersection_Result res_ ) :
		linSurf_Intersection_Result( res_ ),
		face( FACE_ID::INVALID )
	{};

	/*!
	 * @brief Default constructor
	*/
	rayVox_Intersection_Result( void ) : face( FACE_ID::INVALID ) {};


	public:

	FACE_ID face;	/*!<ID of face for which the result is valid*/
};



/*!
 * @brief Class for calculation the intersection of a line and a surface
*/
class rayVoxelIntersection {

	public:

	/*!
	 * @brief Constructor
	 * @param v_ Voxel
	 * @param r_ Ray
	*/
	rayVoxelIntersection( const vox v_, const ray r_ );


	public:

	vox v;										/*!<Voxel*/
	ray r;										/*!<Ray*/
	rayVox_Intersection_Result entrance;		/*!<Entrance*/
	rayVox_Intersection_Result exit;			/*!Exit*/

};

#include "intersections.hpp"