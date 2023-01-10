/*********************************************************************
 * @file   tests_linear_algebra.cpp
 * @brief  Implementation
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/
 #include <iostream>
 using std::cerr; using std::cout; using std::endl;

 #include "generelMath.h"
 #include "cartesian.h"
 #include "cSysTree.h"
 #include "vec3D.h"
 #include "tests_linear_algebra.h"



 /*********************************************************************
	Implementations
 *********************************************************************/



 bool test_linear_algebra( void ){

	 cartCSys* sys1 = CSYS_TREE().addCSys( primitiveVec3{ v3{ 1, 1, 1 }},
													   primitiveVec3{ v3{ 1, 0, 0 }}, 
													   primitiveVec3{ v3{ 0, 1, 0 }}, 
													   primitiveVec3{ v3{ 0, 0, 1 }}, 
													   "Sys 1");
	 vec3 vect1{ v3{ 2, 2, 2 }, sys1 };
	 vec3 vect2 = vect1.convertTo( GLOBAL_CSYS() );
	 if( vect2 != vec3{ v3{ 2, 2, 2 }, GLOBAL_CSYS() } ) return false;


	 cartCSys* sys2 = CSYS_TREE().addCSys( primitiveVec3{ v3{ 0, 0, 0 }},
													   primitiveVec3{ v3{ -1, 0, 0 }},
													   primitiveVec3{ v3{ 0, 1, 0 }},
													   primitiveVec3{ v3{ 0, 0, 1 }},
													   "Sys 2" );
	 vec3 vect3{ v3{ 2, 2, 2 }, sys2 };
	 vec3 vect4 = vect3.convertTo( GLOBAL_CSYS() );
	 if( vect4 != vec3{ v3{ -2, 2, 2 }, GLOBAL_CSYS() } ) return false;


	 cartCSys* sys3 = CSYS_TREE().addCSys( primitiveVec3{ v3{ 1, 1, 1 }},
													   primitiveVec3{ v3{ 1, 0, 0 }},
													   primitiveVec3{ v3{ 0, 1, 0 }},
													   primitiveVec3{ v3{ 0, 0, 1 }},
													   sys2,  "Sys 3" );
	vec3 vect6{ v3{ 1, 0, 0 }, sys3 };
	vec3 vect7 = vect6.convertTo( sys1 );
	if( vect7 != vec3{ v3{ -1, 0, 0 }, sys1 } ) return false;

	pnt3 p1{ v3{ 1, 0, 0 }, sys3 };
	pnt3 p2 = p1.convertTo( sys1 );
	if( p2 != pnt3{ v3{ -3, 0, 0 }, sys1 } ) return false;

	uvec3 uv1{ v3{ 5, 2, 2}, sys3 };
	if( !iseqErr( uv1.Length(), 1 ) ) return false;

	vec3 rotVec1 = vect6.rotN( vec3{ v3{ 0, 1, 0 }, vect6.CSys() }, PI/2);
	if( rotVec1 != vec3{ v3{ 0, 0, -1 }, vect6.CSys() } ) return false;




	cartCSys* cSys1 = GLOBAL_CSYS()->addCSys(	primitiveVec3{ v3{ 1, 1, 1 } },
												primitiveVec3{ v3{ 0, 1, 0 } },
												primitiveVec3{ v3{ 0, 0, 1 } },
												primitiveVec3{ v3{ 1, 0, 0 } },
												"System 1" );

	cartCSys* cSys11 = cSys1->addCSys(	primitiveVec3{ v3{ 2, 2, 2 } },
										primitiveVec3{ v3{ sqrt( 2 )/2, sqrt( 2 ) / 2, 0 } },
										primitiveVec3{ v3{ -sqrt( 2 ) / 2, sqrt( 2 ) / 2, 0 } },
										primitiveVec3{ v3{ 0, 0, 1 } },
										   "System 1.1" );

	cartCSys* cSys2 = GLOBAL_CSYS()->addCSys( primitiveVec3{ v3{ -1, -1, -1 } },
											  primitiveVec3{ v3{ 0, 0, 1 } },
											  primitiveVec3{ v3{ 0, 1, 0 } },
											  primitiveVec3{ v3{ 1, 0, 0 } },
											  "System 2" );
	pnt3 P1{ v3{ 3, 3, 3 }, cSys11 };
	pnt3 P2 = P1.convertTo( cSys1 );
	pnt3 P3 = P1.convertTo( GLOBAL_CSYS() );
	pnt3 P4 = P1.convertTo( cSys2 );

	pnt3 expectedP4{ v3{ 4+3*sqrt(2), 4, 7 }, cSys2 };
	if( P4 != expectedP4 ) return false;

	 return true;
 }