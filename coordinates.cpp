/*********************************************************************
 * @file   Coordinates.cpp
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
	Coordinates implementation
*/

string Coordinates::ToString( const unsigned int newline_tabulators ) const{
	string str;
	string newLine = { '\n' };

	for( unsigned int i = 0; i < newline_tabulators; i++ ) newLine += '\t';

	char tempCharArr[ 64 ] = { 0 };
	snprintf( tempCharArr, 64, "(%.6f,%.6f,%.6f)", x, y, z );

	str += newLine + tempCharArr;
	str += newLine + "Parent coordinate_system_:" + std::format( "{:#X}", (size_t) coordinate_system_ );

	return str;
}

bool Coordinates::operator== ( const Coordinates coords ) const{
	if( this->IsSameSystem( coords ) ) return this->PrimitiveVector3::operator==( coords );

	// Convert both Coordinates to global system
	Coordinates globalCoords_1 = this->convertTo( GlobalSystem() );
	Coordinates globalCoords_2 = coords.convertTo( GlobalSystem() );

	// Compare components
	return globalCoords_1.PrimitiveVector3::operator==( globalCoords_2 );
};

Coordinates Coordinates::operator+ ( const Coordinates coords ) const{
	// Add converted Coordinates' components to this componentes
	PrimitiveVector3 locCoords = this->PrimitiveVector3::operator+( coords.convertTo( this->coordinate_system_ ) );

	return Coordinates{ locCoords, this->coordinate_system_ };
}

Coordinates Coordinates::operator- ( const Coordinates coords ) const{
	// Add converted Coordinates' components to this componentes
	PrimitiveVector3 locCoords = this->PrimitiveVector3::operator-( coords.convertTo( this->coordinate_system_ ) );

	return Coordinates{ locCoords, this->coordinate_system_ };
}

Coordinates Coordinates::operator- ( void ) const{
	return Coordinates{ this->PrimitiveVector3::operator-(), this->coordinate_system_ };
}

Coordinates Coordinates::operator* ( const double scalar ) const{
	PrimitiveVector3 scaledCoords = this->PrimitiveVector3::operator*( scalar );

	return Coordinates{ scaledCoords, this->coordinate_system_ };
}

Coordinates Coordinates::operator/ ( const double divisor ) const{
	return Coordinates{ this->PrimitiveVector3::operator/( divisor ), this->coordinate_system_ };
}

Coordinates Coordinates::convertTo( const CoordinateSystem* const target_coordinate_system ) const{
	
	Coordinates tempC{ Tuple3D{ x, y, z }, this->coordinate_system_ };

	if( this->IsSameSystem( target_coordinate_system ) ) return tempC;

	// Loop until Coordinates are in context of global system
	while( !tempC.coordinate_system_->IsGlobal() ){
		tempC = tempC.ConvertToParentSystem();
	}


	// Target system is not global system
	if( !target_coordinate_system->IsGlobal() ){
		// Find path from global system to target system
		vector<const CoordinateSystem*> path = target_coordinate_system->GetPathFromGlobal();

		// Target system is not the global system
		if( path.size() > 0 ){
			// Iterate each coordinate system in path
			for( vector<const CoordinateSystem*>::const_iterator cur_coordinate_systemIt = path.begin(); cur_coordinate_systemIt < path.end(); cur_coordinate_systemIt++ ){
				tempC = tempC.ConvertToChildSystem( *cur_coordinate_systemIt );
			}
		}

		tempC = tempC.ConvertToChildSystem( target_coordinate_system );
	}
	return tempC;
}

Coordinates Coordinates::ConvertToParentSystem( void ) const{
	// Return this Coordinates if this' coordinate system is the global system
	if( this->coordinate_system_->IsGlobal() ) return *this;

	// Values of Coordinates in parent_ coordinate system
	PrimitiveVector3 coordComps{ coordinate_system_->Primitive().Origin() + coordinate_system_->Primitive().UnitX() * x + coordinate_system_->Primitive().UnitY() * y + coordinate_system_->Primitive().UnitZ() * z };
	Coordinates parentCoords{ coordComps, coordinate_system_->Parent() };

	return parentCoords;
}

Coordinates Coordinates::ConvertToChildSystem( const CoordinateSystem* const child_coordinate_system ) const{
	// Error when child's parent_ system is not this system
	if( !this->IsSameSystem( child_coordinate_system->Parent() ) ) CheckForAndOutputError( MathError::Input, "Parent of child system is not this system!" );

	eqnSys tEqnSys( 3 );		// System of equation to solve for x,y and z in local coordinate system

	// Poulate columns of system of equations
	tEqnSys.populateColumn( child_coordinate_system->Primitive().UnitX() );
	tEqnSys.populateColumn( child_coordinate_system->Primitive().UnitY() );
	tEqnSys.populateColumn( child_coordinate_system->Primitive().UnitZ() );

	tEqnSys.populateColumn( (PrimitiveVector3) *this - child_coordinate_system->Primitive().Origin() );

	// Solve
	eqnSysSolution tEqnSysSol = tEqnSys.solve();

	// System solution are new Coordinates
	return Coordinates{ Tuple3D{ tEqnSysSol.getVar( 0 ), tEqnSysSol.getVar( 1 ), tEqnSysSol.getVar( 2 ) }, child_coordinate_system };
}
