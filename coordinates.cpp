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
	string instance_string;
	string new_line = { '\n' };

	for( unsigned int i = 0; i < newline_tabulators; i++ ) new_line += '\t';

	char tempory_character_array[ 64 ] = { 0 };
	snprintf( tempory_character_array, 64, "(%.6f,%.6f,%.6f)", x, y, z );

	instance_string += new_line + tempory_character_array;
	instance_string += new_line + "parent coordinate_system_:" + std::format( "{:#X}", reinterpret_cast<std::uintptr_t>( coordinate_system_ ) );

	return instance_string;
}

bool Coordinates::operator== ( const Coordinates coordinates_to_compare ) const{
	
	if( this->HasSameSystem( coordinates_to_compare ) ) return this->PrimitiveVector3::operator==( coordinates_to_compare );

	// convert both coordinates to global system
	Coordinates system_1_global = this->ConvertTo( GetGlobalSystem() );
	Coordinates system_2_global = coordinates_to_compare.ConvertTo( GetGlobalSystem() );

	// compare components
	return system_1_global.PrimitiveVector3::operator==( system_2_global );
};

Coordinates Coordinates::operator+ ( const Coordinates summand ) const{
	// add converted coordinates' components to this componentes
	PrimitiveVector3 local_coordinates = this->PrimitiveVector3::operator+( summand.ConvertTo( this->coordinate_system_ ) );

	return Coordinates{ local_coordinates, this->coordinate_system_ };
}

Coordinates Coordinates::operator- ( const Coordinates subtrahend ) const{
	// add converted coordinates' components to this componentes
	PrimitiveVector3 local_coordinates = this->PrimitiveVector3::operator-( subtrahend.ConvertTo( this->coordinate_system_ ) );

	return Coordinates{ local_coordinates, this->coordinate_system_ };
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
		vector<const CoordinateSystem*> path_from_global = target_coordinate_system->GetPathFromGlobal();

		// target system is not the global system
		if( path_from_global.size() > 0 ){
			// iterate each coordinate system in path
			for( auto current_system = path_from_global.cbegin(); 
								current_system < path_from_global.cend(); current_system++ ){
				tempory_coordinates = 
					tempory_coordinates.ConvertToChildSystem( *current_system );
			}
		}

		tempory_coordinates = 
			tempory_coordinates.ConvertToChildSystem( target_coordinate_system );
	}
	return tempory_coordinates;
}

Coordinates Coordinates::ConvertToParentSystem( void ) const{
	// return this coordinates if this' coordinate system is the global system
	if( this->coordinate_system_->IsGlobal() ) return *this;

	// values of coordinates in parent coordinate system
	PrimitiveVector3 components_in_parent_system{ coordinate_system_->GetPrimitive().origin() + coordinate_system_->GetPrimitive().ex() * x + coordinate_system_->GetPrimitive().ey() * y + coordinate_system_->GetPrimitive().ez() * z };
	Coordinates coordinate_in_parent_system{ components_in_parent_system, coordinate_system_->parent() };

	return coordinate_in_parent_system;
}

Coordinates Coordinates::ConvertToChildSystem( const CoordinateSystem* const child_coordinate_system ) const{
	// error when child's parent system is not this system
	if( !this->HasSameSystem( child_coordinate_system->parent() ) ) CheckForAndOutputError( MathError::Input, "parent of child system is not this system!" );

	SystemOfEquations equation_system( 3 );		// system of equation to solve for x,y and z in local coordinate system

	// poulate columns of system of equations
	equation_system.PopulateColumn( child_coordinate_system->GetPrimitive().ex() );
	equation_system.PopulateColumn( child_coordinate_system->GetPrimitive().ey() );
	equation_system.PopulateColumn( child_coordinate_system->GetPrimitive().ez() );

	equation_system.PopulateColumn( PrimitiveVector3{ x, y, z } - child_coordinate_system->GetPrimitive().origin() );

	// solve
	const SystemOfEquationsSolution equation_system_solution = equation_system.Solve();

	// system solution are new coordinates
	return Coordinates{ Tuple3D{ equation_system_solution.GetVariableValue( 0 ), equation_system_solution.GetVariableValue( 1 ), equation_system_solution.GetVariableValue( 2 ) }, child_coordinate_system };
}
