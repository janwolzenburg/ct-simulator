
/*********************************************************************
 * @file   intersections.hpp
 * @brief  Template implementations
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/

 /*********************************************************************
	Includes
 *********************************************************************/

 #include "intersections.h"
 #include "systemOfEquations.h"



 /*********************************************************************
	Implementations
 *********************************************************************/

/*
	linSurfIntersection
*/

template<class L, class S>
LineSurfaceIntersection<L, S>::LineSurfaceIntersection( void ) :
	intersection_point_{},
	intersection_exists_( false ),
	line_parameter_( 0. ),
	surface_parameter_1_( 0. ),
	surface_parameter_2_( 0. )
{}

template<class L, class S>
LineSurfaceIntersection<L, S>::LineSurfaceIntersection( const L line, const S surface ) :
	intersection_exists_( false )
{
	PrimitiveVector3 surfaceO;
	PrimitiveVector3 surfaceR1;
	PrimitiveVector3 surfaceR2;
	PrimitiveVector3 lineR = line.direction().GetComponents();
	PrimitiveVector3 lineO = line.origin().GetComponents();

	// Same system?
	if( line.direction().GetCoordinateSystem() == surface.direction_1().GetCoordinateSystem() ){
		surfaceO = surface.origin().GetComponents();
		surfaceR1 = surface.direction_1().GetComponents();
		surfaceR2 = surface.direction_2().GetComponents();
	}
	else{
		surfaceO = surface.origin().GetComponents( line.direction() );
		surfaceR1 = surface.direction_1().GetComponents( line.direction() );
		surfaceR2 = surface.direction_2().GetComponents( line.direction() );
	}

	// Create system of equations with three variables
	SystemOfEquations sys( 3 );
	sys.PopulateColumn( surfaceR1 );
	sys.PopulateColumn( surfaceR2 );
	sys.PopulateColumn( -lineR );
	sys.PopulateColumn( lineO - surfaceO );

	// Solve system
	SystemOfEquationsSolution sysSol = sys.Solve();

	// No solution found
	if( !sysSol.solution_found() ){
		return;
	}

	// Copy result
	intersection_exists_ = true;							// Solution found
	line_parameter_ = sysSol.GetVariableValue( 2 );			// Line parameter

	surface_parameter_1_ = sysSol.GetVariableValue( 0 );	// Surface parameter A
	surface_parameter_2_ = sysSol.GetVariableValue( 1 );	// Surface parameter B

	intersection_point_ = line.GetPoint( line_parameter_ );	// Point of intersection


	// Surface parameters outside allowed bounds of surface
	if( !surface.AreParametersInBounds( surface_parameter_1_, surface_parameter_2_ ) ) intersection_exists_ = false;
	if( !line.IsParameterInBounds( line_parameter_ ) ) intersection_exists_ = false;

};


template<class L, class S>
string LineSurfaceIntersection<L, S>::ToString( unsigned int newline_tabulators ) const{
	string str;
	string newLine = { '\n' };

	for( unsigned int i = 0; i < newline_tabulators; i++ ) newLine += '\t';

	str += "solution=" + intersection_exists_;
	str += newLine + intersection_point_.ToString();
	return str;
}
