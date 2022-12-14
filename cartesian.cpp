/*********************************************************************
 * @file   cartesian.cpp
 * @brief  Implementations
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/


/*********************************************************************
	Includes
 *********************************************************************/
 #include <cmath>
 #include <string>
 using std::string;
 #include <format>

 #include "generel.h"
 #include "generelMath.h"
 #include "cartesian.h"
 #include "vec3D.h"
 #include "line.h"
 #include "surf.h"
 #include "equationSystem.h"


 /*********************************************************************
	Implementations
 *********************************************************************/


 cSysTree& CSYS_TREE( void ){
	 return cSysTree::getInstance();
 }
 
 const cartCSys* GLOBAL_CSYS( void ){
	 return CSYS_TREE().getGlobal();
 }

 const cartCSys* DUMMY_CSYS( void ){
	 return CSYS_TREE().getDummy();
 }



/*
	primitiveVec3 implementation
*/

primitiveVec3::primitiveVec3( v3 xyz ) : v3( xyz )
{}

primitiveVec3::primitiveVec3( const double x_, const double y_, const double z_ ) : primitiveVec3( v3{ x_, y_, z_ } )
{}

string primitiveVec3::toStr( [[maybe_unused]] const unsigned int newLineTabulators ) const{
	string str;
	char tempCharArr[ 64 ] = { 0 };
	snprintf( tempCharArr, 64, "(%.6f,%.6f,%.6f)", x, y, z );

	str += tempCharArr;
	return str;
};

bool primitiveVec3::operator== ( const primitiveVec3 v ) const{
	// Check equality for each component
	if( !iseqErr( x, v.x ) ) return false;
	if( !iseqErr( y, v.y ) ) return false;
	if( !iseqErr( z, v.z ) ) return false;
	return true;
};

bool primitiveVec3::operator!= ( const primitiveVec3 v ) const{
	return !(this->operator==( v ));
};

primitiveVec3 primitiveVec3::operator+ ( const primitiveVec3 v ) const{
	return primitiveVec3{x + v.x, y + v.y, z + v.z};
}

primitiveVec3 primitiveVec3::operator- ( const primitiveVec3 v ) const{
	return primitiveVec3{x - v.x, y - v.y, z - v.z};
}

primitiveVec3 primitiveVec3::operator- ( void ) const{
	return primitiveVec3{ -x, -y, -z};
}

primitiveVec3 primitiveVec3::operator* ( const double scalar ) const{
	return primitiveVec3{ scalar* x, scalar* y, scalar* z };
};

primitiveVec3 primitiveVec3::operator/ ( const double divisor ) const{
	if( divisor == 0 ) return *this;
	return primitiveVec3{x / divisor, y / divisor, z / divisor};
};

void primitiveVec3::scale( const double scalar ){
	x *= scalar;
	y *= scalar;
	z *= scalar;
};

mathObj::MATH_ERR primitiveVec3::normalize( void ){
	// Reset error flag
	errno = 0;

	// New length
	double len = sqrt( pow( x, 2 ) + pow( y, 2 ) + pow( z, 2 ) );

	// Check error flag from sqrt()
	if( errno != 0 ) return checkErr( MATH_ERR::GENERAL, "Error calculating square root!" );

	// Length must not by zero
	if( len == 0 ) return checkErr( MATH_ERR::OPERATION, "Normalization only possible with vector that has length!" );

	// Exit when length is already one
	if( iseqErr( len, 1 ) ) return MATH_ERR::OK;

	// Calculate scaling factor as reciprocal of length
	double lenRec = 1 / len;

	// Scale and return error code
	scale( lenRec );

	return MATH_ERR::OK;
}

void primitiveVec3::rotXM( const double sinPhi, const double cosPhi ){
	// Apply rotation matrix
	double x_ = x;
	double y_ = y * cosPhi - z * sinPhi;
	double z_ = y * sinPhi + z * cosPhi;

	x = x_; y = y_; z = z_;
};

void primitiveVec3::rotYM( const double sinPhi, const double cosPhi ){
	// Apply rotation matrix
	double x_ = x * cosPhi + z * sinPhi;
	double y_ = y;
	double z_ = z * cosPhi - x * sinPhi;

	x = x_; y = y_; z = z_;
};

void primitiveVec3::rotZM( const double sinPhi, const double cosPhi ){
	// Apply rotation matrix
	double x_ = x * cosPhi - y * sinPhi;
	double y_ = x * sinPhi + y * cosPhi;
	double z_ = z;

	x = x_; y = y_; z = z_;
};

mathObj::MATH_ERR primitiveVec3::rotNM( const primitiveVec3 n, const double phi ){
	// Steps for rotation:
	// 1: rotate around z axis to tilt rot. axis into x-z plane
	// 2: rotate around y-axis to align rot. axis with z-axis
	// 3. rotate this primitiveVec3 by phi around z-axis
	// 4. Undo previous rotation steps 1 and 2 in reverse order

	// n must have direction
	if( n == primitiveVec3{ 0, 0, 0 } ) return checkErr( MATH_ERR::INPUT, "Rotation axis must have length!" );

	// Create copy and normalize
	primitiveVec3 nCpy{ n };
	nCpy.normalize();

	double d = sqrt( pow( nCpy.x, 2 ) + pow( nCpy.y, 2 ) );		// Length of the axis projection on x-y plane
	if( errno != 0 ) return checkErr( MATH_ERR::GENERAL, "Error calculation square root!" );		// Check error flag

	double sinThe = 0, cosThe = 1;								// Sine and cosine of angle Theta (angle between rot. axis projection onto x-y plane and x axis)

	// Avoid division by zero. d = 0 means rot. axis is parallel to z
	if( d > 0 ){
		sinThe = nCpy.y / d;		// Sine of the angle Theta
		cosThe = nCpy.x / d;		// Cosine of the angle Theta

		// Clockwise rotation of rotation axis and this vector around z-axis to align rotation axis to x-z plane
		nCpy.rotZM( -sinThe, cosThe );
		this->rotZM( -sinThe, cosThe );
	}

	// Gamma is the angle between the rotation axis (aligned to x-z plane) and the z-axis
	double sinGam = d;				// Rotation axis vector has been normalized - sine of Gamma is d / 1
	double cosGam = nCpy.z;			// Cosine is just the z-component of vector n_z / 1

	// Clockwise rotation of this vector around y-axis
	this->rotYM( -sinGam, cosGam );

	// The axis rotation vector is now aligned with the z-axis

	// Sine and cosine of angle to rotate around
	double sinPhi = sin( phi );
	double cosPhi = cos( phi );

	// Counter-clockwise z-axis rotation of this vector by Phi
	this->rotZM( sinPhi, cosPhi );

	// Counter-clockwise y-axis rotation of this vector by Gamma to reverse step 2
	this->rotYM( sinGam, cosGam );


	if( d > 0 ){
		// Counter-clockwise z-axis rotation of this vector by Theta to reverse step 1
		this->rotZM( sinThe, cosThe );
	}
	return MATH_ERR::OK;
};



/*
	primitiveCartCSys implementation
*/

primitiveCartCSys::primitiveCartCSys( const primitiveVec3 origin_, const primitiveVec3 ex_, const primitiveVec3 ey_, const primitiveVec3 ez_ )
	: origin( origin_ ),
	ex( ex_ ),
	ey( ey_ ),
	ez( ez_ )

{
	ex.normalize();
	ey.normalize();
	ez.normalize();

	// Vector have to be orthogonal to each other
	if( !iseqErr( ex.x * ey.x + ex.y * ey.y + ex.z * ey.z, 0 ) ||
		!iseqErr( ey.x * ez.x + ey.y * ez.y + ey.z * ez.z, 0 ) ||
		!iseqErr( ex.x * ez.x + ex.y * ez.y + ex.z * ez.z, 0 ) ) checkErr( MATH_ERR::INPUT, "Unit axis must be orthogonal to each other!" );
};

string primitiveCartCSys::toStr( const unsigned int newLineTabulators ) const{
	string str;
	string newLine = { '\n' };

	for( unsigned int i = 0; i < newLineTabulators; i++ ) newLine += '\t';

	str += newLine + "p=" + origin.toStr();
	str += newLine + "ex=" + ex.toStr() + newLine + "ey=" + ey.toStr() + newLine + "ez=" + ez.toStr();
	return str;
}

mathObj::MATH_ERR primitiveCartCSys::rotateM( const primitiveVec3 n, const double phi ){
	MATH_ERR tErr = MATH_ERR::OK;
	MATH_ERR errCode = MATH_ERR::OK;

	if( ( tErr = ex.rotNM( n, phi ) ) != MATH_ERR::OK ) errCode = tErr;
	if( ( tErr = ey.rotNM( n, phi ) ) != MATH_ERR::OK ) errCode = tErr;
	if( ( tErr = ez.rotNM( n, phi ) ) != MATH_ERR::OK ) errCode = tErr;

	return errCode;
}



/*
	cSysTree implementation
*/

string cSysTree::toStr( const unsigned int newLineTabulators ) const{
	string str;
	string newLine = { '\n' };

	for( unsigned int i = 0; i < newLineTabulators; i++ ) newLine += '\t';

	for( size_t idx = 0; idx < numSystems; idx++ ){
		str += newLine + systems[ idx ].toStr( 1 );
	}

	return str;
}

cSysTree::cSysTree( void ){
	cartCSys dummySys{ primitiveVec3{ 0, 0, 0 }, primitiveVec3{ 1, 0, 0 }, primitiveVec3{ 0, 1, 0 }, primitiveVec3{ 0, 0, 1 }, nullptr, "Dummy system" };
	cartCSys globalSys{ primitiveVec3{ 0, 0, 0 }, primitiveVec3{ 1, 0, 0 }, primitiveVec3{ 0, 1, 0 }, primitiveVec3{ 0, 0, 1 }, nullptr, "Global system" };
	
	systems = new cartCSys[ MAX_CSYS_IN_TREE ];

	systems[ 0 ] = dummySys;
	systems[ 1 ] = globalSys;
	numSystems = 2;
}

cartCSys* cSysTree::addCSys( const primitiveVec3 origin_, const primitiveVec3 ex_, const primitiveVec3 ey_, const primitiveVec3 ez_, const cartCSys* parent_, const string name_ ){
	// Is the given parent valid in this tree
	if( !validTreeElement( parent_ ) ){
		checkErr( MATH_ERR::INPUT, "Parent is not part of tree!" );
		parent_ = &( systems[ 1 ] );		// Set parent to global
	}

	// Add new system to tree
	cartCSys newSys{ origin_, ex_, ey_, ez_, parent_, name_ };
	systems[ numSystems++ ] = newSys;

	return &systems[ numSystems - 1 ];
};

cartCSys* cSysTree::addCSys( const primitiveVec3 origin_, const primitiveVec3 ex_, const primitiveVec3 ey_, const primitiveVec3 ez_, const string name_ ){
	return addCSys( origin_, ex_, ey_, ez_, GLOBAL_CSYS(), name_ );
}

const cartCSys* cSysTree::getDummy( void ) {
	return &systems[0];
}

const cartCSys* cSysTree::getGlobal( void ){
	return &systems[ 1 ];
}

bool cSysTree::validTreeElement( const cartCSys* const element ) const{
	for( size_t idx = 0; idx < numSystems; idx++ ){
		if( &( systems[ idx ] ) == element ) return true;
	}

	return false;
}



/*
	cartCSys implementation
*/

cartCSys::cartCSys( const primitiveVec3 origin_, const primitiveVec3 ex_, const primitiveVec3 ey_, const primitiveVec3 ez_,
					const cartCSys* const parent_, const string name_ )
	: primitiveCartCSys( origin_, ex_, ey_, ez_ ),
	parent( parent_ ),
	name( name_ ){};

cartCSys::cartCSys( void )
	: cartCSys( primitiveVec3{ 0, 0, 0 }, primitiveVec3{ 1, 0, 0 }, primitiveVec3{ 0, 1, 0 }, primitiveVec3{ 0, 0, 1 }, nullptr, "Uninitialised system" ){};

string cartCSys::toStr( const unsigned int newLineTabulators ) const{
	string str;
	string newLine = { '\n' };

	for( unsigned int i = 0; i < newLineTabulators; i++ ) newLine += '\t';

	str += newLine + name;
	str += newLine + primitiveCartCSys::toStr();
	str += newLine + "Parent cSys:" + std::format( "{:#X}", (size_t) parent );
	return str;
}

cartCSys* cartCSys::createCopy( const string newName ) const{
	return CSYS_TREE().addCSys( origin, ex, ey, ez, parent, newName );
}

cartCSys* cartCSys::addCSys( const primitiveVec3 origin_, const primitiveVec3 ex_, const primitiveVec3 ey_, const primitiveVec3 ez_, const string name_ ) const{
	return CSYS_TREE().addCSys( origin_, ex_, ey_, ez_, this, name_ );
}

vector<const cartCSys*> cartCSys::getPathFromGlobal( void ) const{
	vector<const cartCSys*> path;

	if( this->isGlobal() ) return path;

	const cartCSys* cur_cSys = this->parent;	// Start with parent of this system

	// Loop while current coordinate system is not the global system
	while( !cur_cSys->isGlobal() ){
		path.push_back( cur_cSys );
		cur_cSys = cur_cSys->parent;
	}

	// Reverse order to get path of coordinate systems traverse
	std::reverse( path.begin(), path.end() );

	return path;
};


pnt3 cartCSys::OPnt( void ) const{
	return pnt3{ v3{0, 0, 0}, this };
};

pnt3 cartCSys::OPntPrnt( void ) const{
	if( this->isGlobal() ) return pnt3{ origin, this };
	return pnt3{ origin, parent };
};

uvec3 cartCSys::ExVec( void ) const{
	return uvec3{ v3{1, 0, 0},  this };
};

uvec3 cartCSys::EyVec( void ) const{
	return uvec3{ v3{0, 1, 0},   this };
};

uvec3 cartCSys::EzVec( void ) const{
	return uvec3{ v3{0, 0, 1},  this };
};

line cartCSys::xAxis( void ) const{
	const cartCSys* parent_ptr = parent;
	if( this->isGlobal() ) parent_ptr = GLOBAL_CSYS();

	return line{ vec3{ex, parent_ptr},  pnt3{origin, parent_ptr} };
};

line cartCSys::yAxis( void ) const{
	const cartCSys* parent_ptr = parent;
	if( this->isGlobal() ) parent_ptr = GLOBAL_CSYS();

	return line{ vec3{ey, parent_ptr},  pnt3{origin, parent_ptr} };
};

line cartCSys::zAxis( void ) const{
	const cartCSys* parent_ptr = parent;
	if( this->isGlobal() ) parent_ptr = GLOBAL_CSYS();

	return line{ vec3{ez, parent_ptr},  pnt3{origin, parent_ptr} };
};

surf cartCSys::xyPlane( void ) const{
	const cartCSys* parent_ptr = parent;
	if( this->isGlobal() ) parent_ptr = GLOBAL_CSYS();

	return surf{ vec3{ex, parent_ptr}, vec3{ey, parent_ptr}, pnt3{origin, parent_ptr } };
}

surf cartCSys::yzPlane( void ) const{
	const cartCSys* parent_ptr = parent;
	if( this->isGlobal() ) parent_ptr = GLOBAL_CSYS();

	return surf{ vec3{ey, parent_ptr}, vec3{ez, parent_ptr}, pnt3{origin, parent_ptr } };
}

surf cartCSys::xzPlane( void ) const{
	const cartCSys* parent_ptr = parent;
	if( this->isGlobal() ) parent_ptr = GLOBAL_CSYS();

	return surf{ vec3{ex, parent_ptr}, vec3{ez, parent_ptr}, pnt3{origin, parent_ptr } };
}

mathObj::MATH_ERR cartCSys::translateM( const vec3 dV ){
	if( this->isGlobal() ){
		return checkErr( MATH_ERR::OPERATION, "Global coordinate system cannot be translated!" );
	}

	primitiveCartCSys::translateM( dV.XYZ( parent ) );

	return MATH_ERR::OK;
}

mathObj::MATH_ERR cartCSys::rotateM( const uvec3 n, const double phi ){
	if( this->isGlobal() ){
		return checkErr( MATH_ERR::OPERATION, "Global coordinate system cannot be rotated!" );
	}

	return primitiveCartCSys::rotateM( n.XYZ( parent ), phi );
}

mathObj::MATH_ERR cartCSys::rotateM( const line l, const double phi ){
	MATH_ERR tErr = MATH_ERR::OK;
	MATH_ERR errCode = MATH_ERR::OK;

	// Rotate coordinate system's unit vectors
	if( ( tErr = rotateM( l.R(), phi ) ) != MATH_ERR::OK ) errCode = tErr;

	// Move rotation center to origin of rotation axis
	translateM( -l.O() );

	// Rotate position vector of origin around rotation axis
	if( ( tErr = origin.rotNM( l.R().XYZ( parent ), phi ) ) != MATH_ERR::OK ) errCode = tErr;

	// Translate back
	translateM( l.O() );

	return errCode;
}



/*
	coordinates implementation
*/

coordinates::coordinates( const v3 vec3_, const cartCSys* const cSys_ )
	: primitiveVec3( vec3_ ),
	  cSys( cSys_ ){}


string coordinates::toStr( const unsigned int newLineTabulators ) const{
	string str;
	string newLine = { '\n' };

	for( unsigned int i = 0; i < newLineTabulators; i++ ) newLine += '\t';

	char tempCharArr[ 64 ] = { 0 };
	snprintf( tempCharArr, 64, "(%.6f,%.6f,%.6f)", x, y, z );

	str += newLine + tempCharArr;
	str += newLine + "Parent cSys:" + std::format( "{:#X}", (size_t) cSys );

	return str;
}

bool coordinates::operator== ( const coordinates coords ) const{
	if( this->sameSystem( coords ) ) return this->primitiveVec3::operator==( coords );

	// Convert both coordinates to global system
	coordinates globalCoords_1 = this->convertTo( GLOBAL_CSYS() );
	coordinates globalCoords_2 = coords.convertTo( GLOBAL_CSYS() );

	// Compare components
	return globalCoords_1.primitiveVec3::operator==( globalCoords_2 );
};

coordinates coordinates::operator+ ( const coordinates coords ) const{
	// Add converted coordinates' components to this componentes
	primitiveVec3 locCoords = this->primitiveVec3::operator+( coords.convertTo( this->cSys ) );

	return coordinates( locCoords, this->cSys );
}

coordinates coordinates::operator- ( const coordinates coords ) const{
	// Add converted coordinates' components to this componentes
	primitiveVec3 locCoords = this->primitiveVec3::operator-( coords.convertTo( this->cSys ) );

	return coordinates( locCoords, this->cSys );
}

coordinates coordinates::operator- ( void ) const{
	return coordinates{ this->primitiveVec3::operator-(), cSys };
}

coordinates coordinates::operator* ( const double scalar ) const{
	primitiveVec3 scaledCoords = this->primitiveVec3::operator*( scalar );

	return coordinates( scaledCoords, this->cSys );
}

coordinates coordinates::operator/ ( const double divisor ) const{
	return coordinates( this->primitiveVec3::operator/( divisor ), this->cSys );
}

coordinates coordinates::convertTo( const cartCSys* const target_cSys ) const{
	coordinates tempC = *this;

	if( this->sameSystem( target_cSys ) ) return tempC;

	// Loop until coordinates are in context of global system
	while( !tempC.cSys->isGlobal() ){
		tempC = tempC.toParentcSys();
	}


	// Target system is not global system
	if( !target_cSys->isGlobal() ){
		// Find path from global system to target system
		vector<const cartCSys*> path = target_cSys->getPathFromGlobal();

		// Target system is not the global system
		if( path.size() > 0 ){
			// Iterate each coordinate system in path
			for( vector<const cartCSys*>::const_iterator cur_cSys_It = path.begin(); cur_cSys_It < path.end(); cur_cSys_It++ ){
				tempC = tempC.toChildcSys( *cur_cSys_It );
			}
		}

		tempC = tempC.toChildcSys( target_cSys );
	}
	return tempC;
}

coordinates coordinates::toParentcSys( void ) const{
	// Return this coordinates if this' coordinate system is the global system
	if( this->cSys->isGlobal() ) return *this;

	// Values of coordinates in parent coordinate system
	primitiveVec3 coordComps{ cSys->O() + cSys->Ex() * x + cSys->Ey() * y + cSys->Ez() * z };
	coordinates parentCoords{ coordComps, cSys->Parent() };

	return parentCoords;
}

coordinates coordinates::toChildcSys( const cartCSys* const child_cSys ) const{
	// Error when child's parent system is not this system
	if( !this->sameSystem( child_cSys->Parent() ) ) checkErr( MATH_ERR::INPUT, "Parent of child system is not this system!" );

	eqnSys tEqnSys( 3 );		// System of equation to solve for x,y and z in local coordinate system

	// Poulate columns of system of equations
	tEqnSys.populateColumn( child_cSys->Ex() );
	tEqnSys.populateColumn( child_cSys->Ey() );
	tEqnSys.populateColumn( child_cSys->Ez() );

	tEqnSys.populateColumn( this->XYZ() - child_cSys->O() );

	// Solve
	eqnSysSolution tEqnSysSol = tEqnSys.solve();

	// System solution are new coordinates
	return coordinates{ v3{ tEqnSysSol.getVar( 0 ), tEqnSysSol.getVar( 1 ), tEqnSysSol.getVar( 2 ) }, child_cSys };
}
