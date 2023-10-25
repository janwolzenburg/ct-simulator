/*********************************************************************
 * @file   coordinates.cpp
 * @brief  Implementations
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/



 /*********************************************************************
	Includes
 *********************************************************************/
 #include <format>

#include "coordinates.h"
#include "equationSystem.h"


 /*********************************************************************
	Implementations
 *********************************************************************/


/*
	coordinates implementation
*/

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

	return coordinates{ locCoords, this->cSys };
}

coordinates coordinates::operator- ( const coordinates coords ) const{
	// Add converted coordinates' components to this componentes
	primitiveVec3 locCoords = this->primitiveVec3::operator-( coords.convertTo( this->cSys ) );

	return coordinates{ locCoords, this->cSys };
}

coordinates coordinates::operator- ( void ) const{
	return coordinates{ this->primitiveVec3::operator-(), this->cSys };
}

coordinates coordinates::operator* ( const double scalar ) const{
	primitiveVec3 scaledCoords = this->primitiveVec3::operator*( scalar );

	return coordinates{ scaledCoords, this->cSys };
}

coordinates coordinates::operator/ ( const double divisor ) const{
	return coordinates{ this->primitiveVec3::operator/( divisor ), this->cSys };
}

coordinates coordinates::convertTo( const cartCSys* const target_cSys ) const{
	
	coordinates tempC{ v3{ x, y, z }, this->cSys };

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
	primitiveVec3 coordComps{ cSys->Primitive().O() + cSys->Primitive().Ex() * x + cSys->Primitive().Ey() * y + cSys->Primitive().Ez() * z };
	coordinates parentCoords{ coordComps, cSys->Parent() };

	return parentCoords;
}

coordinates coordinates::toChildcSys( const cartCSys* const child_cSys ) const{
	// Error when child's parent system is not this system
	if( !this->sameSystem( child_cSys->Parent() ) ) checkErr( MATH_ERR::INPUT, "Parent of child system is not this system!" );

	eqnSys tEqnSys( 3 );		// System of equation to solve for x,y and z in local coordinate system

	// Poulate columns of system of equations
	tEqnSys.populateColumn( child_cSys->Primitive().Ex() );
	tEqnSys.populateColumn( child_cSys->Primitive().Ey() );
	tEqnSys.populateColumn( child_cSys->Primitive().Ez() );

	tEqnSys.populateColumn( (primitiveVec3) *this - child_cSys->Primitive().O() );

	// Solve
	eqnSysSolution tEqnSysSol = tEqnSys.solve();

	// System solution are new coordinates
	return coordinates{ v3{ tEqnSysSol.getVar( 0 ), tEqnSysSol.getVar( 1 ), tEqnSysSol.getVar( 2 ) }, child_cSys };
}
