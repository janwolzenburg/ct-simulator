
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


	// Create system of equations with three variables
	eqnSys sys( 3 );
	sys.populateColumn( s.R1().XYZ( l.R() ) );
	sys.populateColumn( s.R2().XYZ( l.R() ) );
	sys.populateColumn( -( l.R().XYZ() ) );
	sys.populateColumn( l.R().XYZ() - s.O().XYZ( l.O() ) );

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