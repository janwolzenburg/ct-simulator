
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
linSurfIntersection<L, S>::linSurfIntersection( const L l_, const S s_ ) :
	l( l_ ),
	s( s_ )
{

	PrimitiveVector3 surfaceO;
	PrimitiveVector3 surfaceR1;
	PrimitiveVector3 surfaceR2;
	PrimitiveVector3 lineR = l.direction().GetComponents();
	PrimitiveVector3 lineO = l.origin().GetComponents();

	// Same system?
	if( l.direction().GetCoordinateSystem() == s.direction_1().GetCoordinateSystem() ){
		surfaceO = s.origin().GetComponents();
		surfaceR1 = s.direction_1().GetComponents();
		surfaceR2 = s.direction_2().GetComponents();
	}
	else{
		surfaceO = s.origin().GetComponents( l.direction() );
		surfaceR1 = s.direction_1().GetComponents( l.direction() );
		surfaceR2 = s.direction_2().GetComponents( l.direction() );
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
	result.hasSolution = true;						// Solution found
	result.linePara = sysSol.GetVariableValue( 2 );			// Line parameter

	result.surfParaA = sysSol.GetVariableValue( 0 );			// Surface parameter A
	result.surfParaB = sysSol.GetVariableValue( 1 );			// Surface parameter B

	result.intersectionPoint = l.GetPoint( result.linePara );	// Point of intersection


	// Surface parameters outside allowed bounds of surface
	if( !s.AreParametersInBounds( result.surfParaA, result.surfParaB ) ) result.hasSolution = false;
	if( !l.IsParameterInBounds( result.linePara ) ) result.hasSolution = false;

};