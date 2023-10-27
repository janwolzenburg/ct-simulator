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

 #include "systemOfEquations.h"
 #include "intersections.h"



/*********************************************************************
   Implementations
*********************************************************************/

string lineLine_Intersection_Result::ToString( unsigned int newline_tabulators ) const{
	string str;
	string newLine = { '\n' };

	for( unsigned int i = 0; i < newline_tabulators; i++ ) newLine += '\t';

	str += "solution=" + hasSolution;
	str += newLine + intersectionPoint.ToString();
	return str;
}


lineLine_Intersection::lineLine_Intersection( const Line l1_, const Line l2_ ) :
	l1( l1_ ),
	l2( l2_ )
{


	// Create system of equations with two variables
	SystemOfEquations sys( 2 );

	Tuple3D column0 = l1.direction().GetComponents();
	Tuple3D column1 = -l2.direction().GetComponents( l1.direction() );
	Tuple3D column2 = l2.origin().GetComponents( l1.origin() ) - l1.origin().GetComponents();

	sys.PopulateColumn( Tuple2D{ column0.x, column0.y } );
	sys.PopulateColumn( Tuple2D{ column1.x, column1.y } );
	sys.PopulateColumn( Tuple2D{ column2.x, column2.y } );

	// Solve system
	SystemOfEquationsSolution sysSol = sys.Solve();

	// No solution found
	if( !sysSol.solution_found() || 
		!IsNearlyEqualDistance( sysSol.GetVariableValue( 0 ) * column0.z + sysSol.GetVariableValue( 1 ) * column1.z, column0.z ) ){
		return;
	}


	// Copy result
	result.hasSolution = true;						// Solution found
	result.lineParameter1 = sysSol.GetVariableValue( 0 );			// Surface parameter A
	result.lineParameter2 = sysSol.GetVariableValue( 1 );			// Surface parameter B

	result.intersectionPoint = l1.GetPoint( result.lineParameter1 );	// Point of intersection

};


string linSurf_Intersection_Result::ToString( unsigned int newline_tabulators ) const{
	string str;
	string newLine = { '\n' };

	for( unsigned int i = 0; i < newline_tabulators; i++ ) newLine += '\t';

	str += "solution=" + hasSolution;
	str += newLine + intersectionPoint.ToString();
	return str;
}


/*
	rayVoxelIntersection
*/

rayVoxelIntersection::rayVoxelIntersection( const vox v_, const ray r_ ) :
	v( v_ ),
	r( r_ )
{

	// Components of ray trajectory in voxel coordinate system
	Tuple3D comps = r.direction().GetComponents( v.O() );
	bool facePossible = true;

	// Find Entrance

	// Ray origin_ inside voxel
	if(  v.contains( r.origin() ) ){
		entrance.hasSolution = true;
		entrance.linePara = 0;
		entrance.intersectionPoint = r.origin();
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