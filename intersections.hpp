
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
	PrimitiveVector3 lineR = l.R().Components();
	PrimitiveVector3 lineO = l.O().Components();

	// Same system?
	if( l.R().GetCoordinateSystem() == s.R1().GetCoordinateSystem() ){
		surfaceO = s.O().Components();
		surfaceR1 = s.R1().Components();
		surfaceR2 = s.R2().Components();
	}
	else{
		surfaceO = s.O().Components( l.R() );
		surfaceR1 = s.R1().Components( l.R() );
		surfaceR2 = s.R2().Components( l.R() );
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