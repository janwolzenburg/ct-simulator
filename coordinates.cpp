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
 #include <stdint.h>

#include "coordinates.h"
#include "systemOfEquations.h"


 /*********************************************************************
	Implementations
 *********************************************************************/


/*
	Coordinates implementation
*/

string Coordinates::ConvertToString( const unsigned int newline_tabulators ) const{
	string str;
	string newLine = { '\n' };

	for( unsigned int i = 0; i < newline_tabulators; i++ ) newLine += '\t';

	char tempory_coordinatesharArr[ 64 ] = { 0 };
	snprintf( tempory_coordinatesharArr, 64, "(%.6f,%.6f,%.6f)", x, y, z );

	str += newLine + tempory_coordinatesharArr;
	str += newLine + "parent coordinate_system_:" + std::format( "{:#X}", reinterpret_cast<std::uintptr_t>( coordinate_system_ ) );

	return str;
}

bool Coordinates::operator== ( const Coordinates coordinates ) const{
	if( this->HasSameSystem( coordinates ) ) return this->Primitivevector3::operator==( coordinates );

	// convert both Coordinates to global system
	Coordinates globalCoords_1 = this->ConvertTo( GetGlobalSystem() );
	Coordinates globalCoords_2 = coordinates.ConvertTo( GetGlobalSystem() );

	// compare components
	return globalCoords_1.Primitivevector3::operator==( globalCoords_2 );
};

Coordinates Coordinates::operator+ ( const Coordinates coordinates ) const{
	// add converted Coordinates' components to this componentes
	Primitivevector3 locCoords = this->Primitivevector3::operator+( coordinates.ConvertTo( this->coordinate_system_ ) );

	return Coordinates{ locCoords, this->coordinate_system_ };
}

Coordinates Coordinates::operator- ( const Coordinates coordinates ) const{
	// add converted coordinates' components to this componentes
	Primitivevector3 locCoords = this->Primitivevector3::operator-( coordinates.ConvertTo( this->coordinate_system_ ) );

	return Coordinates{ locCoords, this->coordinate_system_ };
}

Coordinates Coordinates::operator- ( void ) const{
	return Coordinates{ this->Primitivevector3::operator-(), this->coordinate_system_ };
}

Coordinates Coordinates::operator* ( const double scalar ) const{
	Primitivevector3 scaledCoords = this->Primitivevector3::operator*( scalar );

	return Coordinates{ scaledCoords, this->coordinate_system_ };
}

Coordinates Coordinates::operator/ ( const double divisor ) const{
	return Coordinates{ this->Primitivevector3::operator/( divisor ), this->coordinate_system_ };
}

Coordinates Coordinates::ConvertTo( const CoordinateSystem* 
																		const target_coordinate_system ) const{
	
	Coordinates tempory_coordinates{ Tuple3D{ x, y, z }, this->coordinate_system_ };

	if( this->HasSameSystem( target_coordinate_system ) ) return tempory_coordinates;

	// loop until coordinates are in context of global system
	while( !tempory_coordinates.coordinate_system_->IsGlobal() ){
		tempory_coordinates = tempory_coordinates.ConvertToParentSystem(); }

	// target system is not global system
	if( !target_coordinate_system->IsGlobal() ){
		// find path from global system to target system
		vector<const CoordinateSystem*> path = target_coordinate_system->GetPathFromGlobal();

		// target system is not the global system
		if( path.size() > 0 ){
			// iterate each coordinate system in path
			for( auto cur_coordinate_system_it = path.cbegin(); 
								cur_coordinate_system_it < path.cend(); cur_coordinate_system_it++ ){
				tempory_coordinates = 
					tempory_coordinates.ConvertToChildSystem( *cur_coordinate_system_it );
			}
		}

		tempory_coordinates = 
			tempory_coordinates.ConvertToChildSystem( target_coordinate_system );
	}
	return tempory_coordinates;
}

Coordinates Coordinates::ConvertToParentSystem( void ) const{
	// return this Coordinates if this' coordinate system is the global system
	if( this->coordinate_system_->IsGlobal() ) return *this;

	// values of Coordinates in parent_ coordinate system
	Primitivevector3 coordComps{ coordinate_system_->GetPrimitive().origin() + coordinate_system_->GetPrimitive().ex() * x + coordinate_system_->GetPrimitive().ey() * y + coordinate_system_->GetPrimitive().ez() * z };
	Coordinates parentCoords{ coordComps, coordinate_system_->parent() };

	return parentCoords;
}

Coordinates Coordinates::ConvertToChildSystem( const CoordinateSystem* const child_coordinate_system ) const{
	// error when child's parent_ system is not this system
	if( !this->HasSameSystem( child_coordinate_system->parent() ) ) CheckForAndOutputError( MathError::Input, "parent of child system is not this system!" );

	SystemOfEquations tEqnSys( 3 );		// system of equation to Solve for x,y and z in local coordinate system

	// poulate columns of system of equations
	tEqnSys.PopulateColumn( child_coordinate_system->GetPrimitive().ex() );
	tEqnSys.PopulateColumn( child_coordinate_system->GetPrimitive().ey() );
	tEqnSys.PopulateColumn( child_coordinate_system->GetPrimitive().ez() );

	tEqnSys.PopulateColumn( (Primitivevector3) *this - child_coordinate_system->GetPrimitive().origin() );

	// solve
	SystemOfEquationsSolution tEqnSysSol = tEqnSys.Solve();

	// system solution are new Coordinates
	return Coordinates{ Tuple3D{ tEqnSysSol.GetVariableValue( 0 ), tEqnSysSol.GetVariableValue( 1 ), tEqnSysSol.GetVariableValue( 2 ) }, child_coordinate_system };
}
