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
 #include "equationSystem.h"
 #include "intersections.h"



/*********************************************************************
   Implementations
*********************************************************************/

/*
lineLine_Intersection_Result::lineLine_Intersection_Result( void )
	: hasSolution( false ),
	lineParameter1( 0 ), lineParameter2( 0 ),
	intersectionPoint( pnt3{} ){}
*/

string lineLine_Intersection_Result::toStr( unsigned int newLineTabulators ) const{
	string str;
	string newLine = { '\n' };

	for( unsigned int i = 0; i < newLineTabulators; i++ ) newLine += '\t';

	str += "solution=" + hasSolution;
	str += newLine + intersectionPoint.toStr();
	return str;
}


lineLine_Intersection::lineLine_Intersection( const line l1_, const line l2_ ) :
	l1( l1_ ),
	l2( l2_ )
{


	// Create system of equations with two variables
	eqnSys sys( 2 );

	v3 column0 = l1.R().XYZ();
	v3 column1 = -l2.R().XYZ( l1.R() );
	v3 column2 = l2.O().XYZ( l1.O() ) - l1.O().XYZ();

	sys.populateColumn( v2{ column0.x, column0.y } );
	sys.populateColumn( v2{ column1.x, column1.y } );
	sys.populateColumn( v2{ column2.x, column2.y } );

	// Solve system
	eqnSysSolution sysSol = sys.solve();

	// No solution found
	if( !sysSol.Success() || 
		!iseqErr( sysSol.getVar( 0 ) * column0.z + sysSol.getVar( 1 ) * column1.z, column0.z ) ){
		return;
	}


	// Copy result
	result.hasSolution = true;						// Solution found
	result.lineParameter1 = sysSol.getVar( 0 );			// Surface parameter A
	result.lineParameter2 = sysSol.getVar( 1 );			// Surface parameter B

	result.intersectionPoint = l1.getPnt( result.lineParameter1 );	// Point of intersection

};


/*
	linSurf_Intersection_Result
*/
/*
linSurf_Intersection_Result::linSurf_Intersection_Result( void )
	: hasSolution( false ),
	linePara( 0 ), surfParaA( 0 ), surfParaB( 0 ),
	intersectionPoint( pnt3{} ){}
*/

string linSurf_Intersection_Result::toStr( unsigned int newLineTabulators ) const{
	string str;
	string newLine = { '\n' };

	for( unsigned int i = 0; i < newLineTabulators; i++ ) newLine += '\t';

	str += "solution=" + hasSolution;
	str += newLine + intersectionPoint.toStr();
	return str;
}



/*
	rayVox_Intersection_Result
*/

/*
rayVox_Intersection_Result::rayVox_Intersection_Result( const linSurf_Intersection_Result res_ ) :
	linSurf_Intersection_Result( res_ ),
	face( FACE_ID::INVALID )
{

}

rayVox_Intersection_Result::rayVox_Intersection_Result( void ) :
	//linSurf_Intersection_Result( linSurf_Intersection_Result{} ),
	face( FACE_ID::INVALID )
{}
*/


/*
	rayVoxelIntersection
*/

rayVoxelIntersection::rayVoxelIntersection( const vox v_, const ray r_ ) :
	v( v_ ),
	r( r_ )
{

	// Components of ray trajectory in voxel coordinate system
	v3 comps = r.R().XYZ( v.O() );
	bool facePossible = true;

	// Find Entrance

	// Ray origin inside voxel
	if(  v.contains( r.O() ) ){
		entrance.hasSolution = true;
		entrance.linePara = 0;
		entrance.intersectionPoint = r.O();
		entrance.face = FACE_ID::INVALID;
	}
	else{

		// Iterate all faces
		for( FACE_ID i = FACE_ID::BEGIN; i < FACE_ID::END; ++i ){
			facePossible = true;

			// Check if face can be an entrance face of the tRay
			switch( i ){
				case FACE_ID::YZ_Xp:
				if( comps.x >= 0 ) facePossible = false; break;
				case FACE_ID::YZ_Xm:
				if( comps.x <= 0 ) facePossible = false; break;

				case FACE_ID::XZ_Yp:
				if( comps.y >= 0 ) facePossible = false; break;
				case FACE_ID::XZ_Ym:
				if( comps.y <= 0 ) facePossible = false; break;

				case FACE_ID::XY_Zp:
				if( comps.z >= 0 ) facePossible = false; break;
				case FACE_ID::XY_Zm:
				if( comps.z <= 0 ) facePossible = false; break;

				default: break;
			}

			if( facePossible ){
				// Check  if tRay intersects current face
				entrance = linSurfIntersection{ r, v.getFace( i ) }.result;
				if( entrance.hasSolution ){
					entrance.face = i; break;
				}
			}
		}
	}


	// Find exit

	for( FACE_ID i = FACE_ID::BEGIN; i < FACE_ID::END; ++i ){
		facePossible = true;

		// Check if face can be an exit face of the tRay
		switch( i ){
			case FACE_ID::YZ_Xp:
			if( comps.x <= 0 ) facePossible = false; break;
			case FACE_ID::YZ_Xm:
			if( comps.x >= 0 ) facePossible = false; break;

			case FACE_ID::XZ_Yp:
			if( comps.y <= 0 ) facePossible = false; break;
			case FACE_ID::XZ_Ym:
			if( comps.y >= 0 ) facePossible = false; break;

			case FACE_ID::XY_Zp:
			if( comps.z <= 0 ) facePossible = false; break;
			case FACE_ID::XY_Zm:
			if( comps.z >= 0 ) facePossible = false; break;

			default: break;
		}

		if( facePossible ){
			// Check  if tRay intersects current face
			exit = linSurfIntersection{ r, v.getFace( i ) }.result;
			if( exit.hasSolution ){
				exit.face = i; break;
			}
		}
	}
}