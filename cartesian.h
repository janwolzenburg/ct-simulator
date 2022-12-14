#pragma once
/*********************************************************************
 * @file   cartesian.h
 * @brief  Classes for cartesian coordinate systems
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/



 /*********************************************************************
	Includes
 *********************************************************************/
 #include <type_traits>
 #include<string>
 using std::string;

#include "generel.h"
#include "generelMath.h"
#include "cartesian.fwd.h"
#include "vec3D.fwd.h"
#include "line.fwd.h"
#include "surf.fwd.h"



 /*********************************************************************
	Definitions
 *********************************************************************/

 // Maximum amount of Coordinate systems in tree
 constexpr size_t MAX_CSYS_IN_TREE = 32;

/*!
 * @brief Get singleton instance of cartesian system tree
 * @return Reference to tree
*/
 cSysTree& CSYS_TREE( void );

/*!
 * @brief Get global coordinate system
 * @return Pointer to global system
*/
const cartCSys* GLOBAL_CSYS( void );

/*!
 * @brief Get dummy system
 * @return Pointer to dummy system
*/
const cartCSys* DUMMY_CSYS( void );


/*!
 * @brief Class for a primitve 3D Vector with some operations
*/
class primitiveVec3 : public v3, virtual public mathObj{

	public:

	/*!
	 * @brief Constructor
	 * @param x_ x value
	 * @param y_ y value
	 * @param z_ z value
	*/
	primitiveVec3( const v3 xyz );

	primitiveVec3( const double x_, const double y_, const double z_ );

	/*!
	 * @brief Convert data to string
	 * @param newLineTabulators
	 * @return String with data
	*/
	std::string toStr( [[maybe_unused]] const unsigned int newLineTabulators = 0 ) const override;

	/*!
	 * @brief Comparison operator
	 * @param v Vector to compare with
	 * @return True when all components between this and second vector are almost equal
	*/
	bool operator== ( const primitiveVec3 v ) const;

	/*!
	 * @brief Negated comparison operator
	 * @param v Vector to compare with
	 * @return False when all components between this and second vector are almost equal
	*/
	inline bool operator!= ( const primitiveVec3 v ) const;

	/*!
	 * @brief Add second vector to this
	 * @param v Vector to add
	 * @return Sum of this and v
	*/
	inline primitiveVec3 operator+ ( const primitiveVec3 v ) const;

	/*!
	 * @brief Substract second vector to this
	 * @param v Vector to substract
	 * @return Difference of this and v
	*/
	inline primitiveVec3 operator- ( const primitiveVec3 v ) const;

	/*!
	 * @brief Negation operator
	 * @return Negated vector
	*/
	inline primitiveVec3 operator- ( void ) const;

	/*!
	 * @brief Scale this vector
	 * @param scalar Factor to scale with
	 * @return Scaled vector
	*/
	inline primitiveVec3 operator* ( const double scalar ) const;

	/*!
	 * @brief Divide this vector
	 * @param divisor Value to divide by
	 * @return Divided vector
	*/
	primitiveVec3 operator/ ( const double divisor ) const;

	/*!
	 * @brief Scale this vector
	 * @param scalar Factor to scale by
	*/
	void scale( const double scalar );

	/*!
	 * @brief Normalize values of components
	 * @return Error code
	*/
	MATH_ERR normalize( void );

	/*!
	 * @brief Add value to x component
	 * @param x_ Value to add
	*/
	inline void addXM( const double x_ ){ x += x_; };

	/*!
	 * @brief Add value to y component
	 * @param y_ Value to add
	*/
	inline void addYM( const double y_ ){ y += y_; };

	/*!
	 * @brief Add value to z component
	 * @param z_ Value to add
	*/
	inline void addZM( const double z_ ){ z += z_; };

	/*!
	 * @brief Rotate the point of all three components around x-axis of coordinate system
	 * @param sinPhi Sine of the angle
	 * @param cosPhi Cosine of the angle
	*/
	void rotXM( const double sinPhi, const double cosPhi );

	/*!
	 * @brief Rotate the point of all three components around y-axis of coordinate system
	 * @param sinPhi Sine of the angle
	 * @param cosPhi Cosine of the angle
	*/
	void rotYM( const double sinPhi, const double cosPhi );

	/*!
	 * @brief Rotate the point of all three components around z-axis of coordinate system
	 * @param sinPhi Sine of the angle
	 * @param cosPhi Cosine of the angle
	*/
	void rotZM( const double sinPhi, const double cosPhi );

	/*!
	 * @brief Rotate point that these three components form around a vector the components of n form
	 * @param n Rotation axis
	 * @param phi Angle
	 * @return Error code
	*/
	MATH_ERR rotNM( const primitiveVec3 n, const double phi );

};


/*!
 * @brief Class describing a right-handed cartesian coordinate system
*/
class primitiveCartCSys : virtual public mathObj{
	
	protected:

	/*!
	 * @brief Constructor
	 * @param origin_ O of coordinate system
	 * @param ex_ x-axis
	 * @param ey_ y-axis
	 * @param ez_ z-axis
	 * @details Axes must be orthogonal to each other
	*/
	primitiveCartCSys( const primitiveVec3 origin_, const primitiveVec3 ex_, const primitiveVec3 ey_, const primitiveVec3 ez_ );

	/*!
	 * @brief Convert coordinate system's data to string
	 * @return String with coordinate system's data
	*/
	string toStr( const unsigned int newLineTabulators = 0 ) const override;

	/*!
	 * @brief Set origin
	 * @param newOrigin New origin
	 * @return New origin
	*/
	inline primitiveVec3 O( const primitiveVec3 newOrigin ){ return origin = newOrigin; };

	/*!
	 * @brief Translate coordinate system
	 * @param c Translation in x, y and z direction
	*/
	inline void translateM( const primitiveVec3 coords ){ origin = origin + coords; };

	/*!
	 * @brief Rotate coordinate system
	 * @param n Rotation axis
	 * @param phi Rotation angle
	*/
	MATH_ERR rotateM( const primitiveVec3 n, const double phi );


	protected:
	primitiveVec3 origin;		/*!< Origin point */
	primitiveVec3 ex;			/*!< x-axis unit vector */
	primitiveVec3 ey;			/*!< y-axis unit vector */
	primitiveVec3 ez;			/*!< z-axis unit vector */
};


/*!
 * @brief Class to store related coordinate systems. Singleton pattern
*/
class cSysTree : virtual public mathObj{

	public:

	/*!
	 * @brief Get single instance
	 * @return Reference to instance
	*/
	static cSysTree& getInstance(){
		static cSysTree instance; 
		return instance;
	}

	/*!
	 * @brief Convert coordinate tree's data
	 * @return String with tree's data
	*/
	string toStr( const unsigned int newLineTabulators = 0 ) const override;

	/*!
	 * @brief Add system to tree
	 * @param origin_ O of coordinate system
	 * @param ex_ x-axis
	 * @param ey_ y-axis
	 * @param ez_ z-axis
	 * @param parent_ Pointer to parent system
	 * @param name_ Name of the system
	*/
	cartCSys* addCSys( const primitiveVec3 origin_, const primitiveVec3 ex_, const primitiveVec3 ey_, const primitiveVec3 ez_, const cartCSys* parent_, const string name_ );

	/*!
	 * @brief Add system to tree with global system as parent
	 * @param origin_ O of coordinate system
	 * @param ex_ x-axis
	 * @param ey_ y-axis
	 * @param ez_ z-axis
	 * @param name_ Name of the system
	*/
	cartCSys* addCSys( const primitiveVec3 origin_, const primitiveVec3 ex_, const primitiveVec3 ey_, const primitiveVec3 ez_, const string name_ );

	/*!
	 * @brief Get dummy system
	 * @return Pointer to dummy
	*/
	const cartCSys* getDummy( void );

	/*!
	 * @brief Get global system
	 * @return Pointer to global system
	*/
	const cartCSys* getGlobal( void );

	/*!
	 * @brief Check if pointed to system is valid
	 * @param element System to check
	 * @return True when system is part of this tree
	*/
	bool validTreeElement( const cartCSys* const element ) const;


	private:
	/*!
	 * @brief Default Constructor
	 * @details Initializes all systems in array to default cartCSys
	*/
	cSysTree( void );

	/*!
	 * @brief Deleted copy constructor
	*/
	cSysTree( const cSysTree& tree_ ) = delete;

	/*!
	 * @brief Deleted assignment operator
	*/
	cSysTree& operator=( const cSysTree& tree_ ) = delete;
	
	private:
	size_t numSystems;						  /*!< Number of systems in tree*/
	cartCSys* systems;		/*!< Array with all systems*/
};


/*!
 * @brief Class for coordinate system with context in another system
*/
class cartCSys : private primitiveCartCSys {
	friend class cSysTree;

	// Private functions
	private:

	/*!
	 * @brief Constructor
	 * @param origin_ O of coordinate system
	 * @param ex_ x-axis
	 * @param ey_ y-axis
	 * @param ez_ z-axis
	 * @param parent_ Pointer to parent system
	 * @param name_ Name of the system
	*/
	cartCSys( const primitiveVec3 origin_, const primitiveVec3 ex_, const primitiveVec3 ey_, const primitiveVec3 ez_,
			  const cartCSys* const parent_, const std::string name_ );

	/*!
	 * @brief Default constructor
	*/
	cartCSys( void );


	// Public functions
	public:

	/*!
	 * @brief Convert coordinate system's data to string
	 * @return String with coordinate system's data
	*/
	string toStr( const unsigned int newLineTabulators = 0 ) const override;

	/*!
	 * @brief Get pointer to this system's parent
	 * @return Pointer to parent
	*/
	inline const cartCSys* Parent( void ) const{ return parent; };

	/*!
	 * @brief Checks if this system is a global system
	 * @return True when this system is the global system in tree
	*/
	inline bool isGlobal( void ) const{ return parent == nullptr; };

	/*!
	 * @brief Create copy of this system
	 * @param newName Name of new system
	 * @return Pointer to new coordiante system with same baseCartSystem and parent as this
	*/
	cartCSys* createCopy( const string newName ) const;

	/*!
	 * @brief Add coordinate system to this system's tree
	 * @param origin_ O of coordinate system
	 * @param ex_ x-axis
	 * @param ey_ y-axis
	 * @param ez_ z-axis
	 * @param name_ Name of the system
	 * @return Pointer to created system with this system as parent
	*/
	cartCSys* addCSys( const primitiveVec3 origin_, const primitiveVec3 ex_, const primitiveVec3 ey_, const primitiveVec3 ez_, const std::string name_ ) const;

	/*!
	 * @brief Get path from global system to this system
	 * @return Vector with pointers to intermediate coordinate systems excluding global and this system
	*/
	vector<const cartCSys*> getPathFromGlobal( void ) const;

	/*!
	 * @brief Get origin of this coordinate system in context of parent
	 * @return Origin componenets
	*/
	inline primitiveVec3 O( void ) const{ return origin; };

	/*!
	 * @brief Set origin
	 * @param newOrigin New origin
	 * @return New origin
	*/
	inline primitiveVec3 O( primitiveVec3 newOrigin ){ return primitiveCartCSys::O( newOrigin ); };

	/*!
	 * @brief Get x-axis of this coordinate system in context of parent
	 * @return x-axis values
	*/
	inline primitiveVec3 Ex( void ) const{ return ex; };

	/*!
	 * @brief Get y-axis of this coordinate system in context of parent
	 * @return y-axis values
	*/
	inline primitiveVec3 Ey( void ) const{ return ey; };

	/*!
	 * @brief Get z-axis of this coordinate system in context of parent
	 * @return z-axis values
	*/
	inline primitiveVec3 Ez( void ) const{ return ez; };

	/*!
	 * @brief Get origin point in this system's context
	 * @return Point of system's origin
	*/
	pnt3 OPnt( void ) const;

	/*!
	 * @brief Get origin point in this parent's context
	 * @return Point of system's origin
	*/
	pnt3 OPntPrnt( void ) const;

	/*!
	 * @brief Get unit vector of x-axis in this system's context
	 * @return x-axis unit vector
	*/
	uvec3 ExVec( void ) const;

	/*!
	 * @brief Get unit vector of y-axis in this system's context
	 * @return y-axis unit vector
	*/
	uvec3 EyVec( void ) const;

	/*!
	 * @brief Get unit vector of z-axis in this system's context
	 * @return z-axis unit vector
	*/
	uvec3 EzVec( void ) const;

	/*!
	 * @brief Get x-axis in parent coordinate system
	 * @return The x-axis as a line
	*/
	line xAxis( void ) const;

	/*!
	 * @brief Get y-axis in parent coordinate system
	 * @return The y-axis as a line
	*/
	line yAxis( void ) const;

	/*!
	 * @brief Get z-axis in parent coordinate system
	 * @return The z-axis as a line
	*/
	line zAxis( void ) const;

	/*!
	 * @brief Get the xy-plane in parent coordinate system
	 * @return xy-plane as surface
	*/
	surf xyPlane( void ) const;

	/*!
	 * @brief Get the yz-plane in parent coordinate system
	 * @return yz-plane as surface
	*/
	surf yzPlane( void ) const;

	/*!
	 * @brief Get the xz-plane in parent coordinate system
	 * @return xz-plane as surface
	*/
	surf xzPlane( void ) const;

	/*!
	 * @brief Translate coordiante system
	 * @param x Translation in x-direction
	 * @param y Translation in y-direction
	 * @param z Translation in z-direction
	*/
	//void translateM( const double x, const double y, const double z );

	/*!
	 * @brief Translate coordinate system
	 * @param dV Translation vector
	*/
	MATH_ERR translateM( const vec3 dV );

	/*!
	 * @brief Rotate coordinate system around vector
	 * @param n Rotation axis
	 * @param phi Angle
	 * @return Error code
	*/
	MATH_ERR rotateM( const uvec3 n, const double phi );

	/*!
	 * @brief Rotate coordinate system
	 * @param l Rotation axis
	 * @param phi Angle
	 * @return Error code
	*/
	MATH_ERR rotateM( const line l, const double phi );


	private:
	const cartCSys* parent;		/*!<Pointer to parent system*/
	string name;			/*!<Name of system*/
};





/*!
 * @brief Class for coordinates with coordinate system with context
*/
class coordinates : protected primitiveVec3{

	protected:
	const cartCSys* cSys;						/*!< Pointer to coordinate system*/


	public:

	/*!
	 * @brief Constructor initializing coordinate system to global system
	 * @param vec3_ Values
	*/
	coordinates( const v3 vec3_, const cartCSys* const cSys_ );

	/*!
	 * @brief Convert coordinate's data to string
	 * @return String with coordinate's data
	*/
	string toStr( const unsigned int newLineTabulators = 0 ) const override;

	/*!
	 * @brief Comparison operator
	 * @param coords Coordinates to compare with
	 * @return True when all components are the same in global coordinate system
	*/
	bool operator== ( const coordinates coords ) const;

	/*!
	 * @brief Addition operator
	 * @param coords Coordinates to add
	 * @return Coordinates with sum of components in coordinate system of first operand
	*/
	coordinates operator+ ( const coordinates coords ) const;

	/*!
	 * @brief Substraction operator
	 * @param coords Coordinates to add
	 * @return Coordinates with difference of components in coordinate system of first operand
	*/
	coordinates operator- ( const coordinates coords ) const;

	/*!
	 * @brief Negation operator
	 * @return Negated coordinates
	*/
	coordinates operator- ( void ) const;

	/*!
	 * @brief Scale coordinates
	 * @param scalar Factor to scale by
	 * @return Scaled coordinates
	*/
	coordinates operator* ( const double scalar ) const;

	/*!
	 * @brief Divide coordinates
	 * @param divisor Value to divide by
	 * @return Divided coordinates
	*/
	coordinates operator/ ( const double divisor ) const;

	/*!
	 * @brief Check if coordinate system is this coordinate's system
	 * @param cSys_ System to check
	 * @return True when given system is this coordinate's system
	*/
	inline bool sameSystem( const cartCSys* const cSys_ ) const{ return this->cSys == cSys_; };

	/*!
	 * @brief Check if two coordinates have the same coordiante system
	 * @param c Second set of coordinates
	 * @return True when both coordinates have the same coordiante system
	*/
	inline bool sameSystem( const coordinates c ) const{ return this->sameSystem( c.cSys ); };

	/*!
	 * @brief Convert coordinates to a different coordinate system
	 * @param target_cSys System to convert to
	 * @return Coordinates in target system
	*/
	coordinates convertTo( const cartCSys* const target_cSys ) const;

	/*!
	 * @brief Get components of coordinates
	 * @return Componenets
	*/
	inline primitiveVec3 XYZ( void ) const{ return primitiveVec3{ x, y, z }; };


	private:

	/*!
		* @brief Convert coordinate values to parent coordinate system
		* @return Coordinates in parent's system
	*/
	coordinates toParentcSys( void ) const;

	/*!
		* @brief Convert coordiante values to child coordinate system
		* @param child_cSys
		* @return Coordinates in child system
	*/
	coordinates toChildcSys( const cartCSys* const child_cSys ) const;
};