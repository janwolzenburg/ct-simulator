/*********************************************************************
 * @file   operations.cpp
 * @brief  Implementations
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/


/*********************************************************************
	Includes
 *********************************************************************/
#include <string>
using std::string;

 #include "vec3D.h"
 #include "line.h"
 #include "surf.h"
 #include"equationSystem.h"
 #include "operations.h"



/*********************************************************************
   Implementations
*********************************************************************/


/*
	linSurf_Intersection_Result implementation
*/

linSurf_Intersection_Result::linSurf_Intersection_Result( void )
	: hasSolution( false ),
	linPara( 0 ), surfParaA( 0 ), surfParaB( 0 ),
	isectPnt( pnt3{} ){}

string linSurf_Intersection_Result::toStr( unsigned int newLineTabulators ) const{
	string str;
	string newLine = { '\n' };

	for( unsigned int i = 0; i < newLineTabulators; i++ ) newLine += '\t';

	str += "solution=" + hasSolution;
	str += newLine + isectPnt.toStr();
	return str;
}



/*
	linSurfIntersection implementation
*/

linSurfIntersection::linSurfIntersection( const line l_, const surf s_ ) :
	l( l_ ),
	s( s_ )
{

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

}
