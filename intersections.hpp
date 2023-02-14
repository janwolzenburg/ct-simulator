
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
	s( s_ ){

	primitiveVec3 surfaceO;
	primitiveVec3 surfaceR1;
	primitiveVec3 surfaceR2;
	primitiveVec3 lineR = l.R().XYZ();
	primitiveVec3 lineO = l.O().XYZ();

	// Same system?
	if( l.R().CSys() == s.R1().CSys() ){
		surfaceO = s.O().XYZ();
		surfaceR1 = s.R1().XYZ();
		surfaceR2 = s.R2().XYZ();
	}
	else{
		surfaceO = s.O().XYZ( l.R() );
		surfaceR1 = s.R1().XYZ( l.R() );
		surfaceR2 = s.R2().XYZ( l.R() );
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
	result.linPara = sysSol.getVar( 2 );			// Line parameter

	result.surfParaA = sysSol.getVar( 0 );			// Surface parameter A
	result.surfParaB = sysSol.getVar( 1 );			// Surface parameter B

	result.isectPnt = l.getPnt( result.linPara );	// Point of intersection


	// Surface parameters outside allowed bounds of surface
	if( !s.parasInBounds( result.surfParaA, result.surfParaB ) ) result.hasSolution = false;
	if( !l.paraInBounds( result.linPara ) ) result.hasSolution = false;

};