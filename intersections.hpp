
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
 #include "equationSystem.h"



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
	PrimitiveVector3 lineR = l.R().GetComponents();
	PrimitiveVector3 lineO = l.O().GetComponents();

	// Same system?
	if( l.R().GetCoordinateSystem() == s.R1().GetCoordinateSystem() ){
		surfaceO = s.O().GetComponents();
		surfaceR1 = s.R1().GetComponents();
		surfaceR2 = s.R2().GetComponents();
	}
	else{
		surfaceO = s.O().GetComponents( l.R() );
		surfaceR1 = s.R1().GetComponents( l.R() );
		surfaceR2 = s.R2().GetComponents( l.R() );
	}

	// Create system of equations with three variables
	eqnSys sys( 3 );
	sys.populateColumn( surfaceR1 );
	sys.populateColumn( surfaceR2 );
	sys.populateColumn( -lineR );
	sys.populateColumn( lineO - surfaceO );

	// Solve system
	eqnSysSolution sysSol = sys.solve();

	// No solution found
	if( !sysSol.Success() ){
		return;
	}

	// Copy result
	result.hasSolution = true;						// Solution found
	result.linePara = sysSol.getVar( 2 );			// Line parameter

	result.surfParaA = sysSol.getVar( 0 );			// Surface parameter A
	result.surfParaB = sysSol.getVar( 1 );			// Surface parameter B

	result.intersectionPoint = l.getPnt( result.linePara );	// Point of intersection


	// Surface parameters outside allowed bounds of surface
	if( !s.parasInBounds( result.surfParaA, result.surfParaB ) ) result.hasSolution = false;
	if( !l.paraInBounds( result.linePara ) ) result.hasSolution = false;

};