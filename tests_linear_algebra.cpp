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
 #include "coordinateSystem.h"
 #include "coordinateSystemTree.h"
 #include "vector3D.h"
 #include "tests_linear_algebra.h"



 /*********************************************************************
	Implementations
 *********************************************************************/



 bool test_linear_algebra( void ){

	 CoordinateSystem* sys1 = CoordinateSystems().AddSystem( PrimitiveVector3{ Tuple3D{ 1, 1, 1 }},
													   PrimitiveVector3{ Tuple3D{ 1, 0, 0 }}, 
													   PrimitiveVector3{ Tuple3D{ 0, 1, 0 }}, 
													   PrimitiveVector3{ Tuple3D{ 0, 0, 1 }}, 
													   "Sys 1");
	 Vector3D vect1{ Tuple3D{ 2, 2, 2 }, sys1 };
	 Vector3D vect2 = vect1.ConvertTo( GlobalSystem() );
	 if( vect2 != Vector3D{ Tuple3D{ 2, 2, 2 }, GlobalSystem() } ) return false;


	 CoordinateSystem* sys2 = CoordinateSystems().AddSystem( PrimitiveVector3{ Tuple3D{ 0, 0, 0 }},
													   PrimitiveVector3{ Tuple3D{ -1, 0, 0 }},
													   PrimitiveVector3{ Tuple3D{ 0, 1, 0 }},
													   PrimitiveVector3{ Tuple3D{ 0, 0, 1 }},
													   "Sys 2" );
	 Vector3D vect3{ Tuple3D{ 2, 2, 2 }, sys2 };
	 Vector3D vect4 = vect3.ConvertTo( GlobalSystem() );
	 if( vect4 != Vector3D{ Tuple3D{ -2, 2, 2 }, GlobalSystem() } ) return false;


	 CoordinateSystem* sys3 = CoordinateSystems().AddSystem( PrimitiveVector3{ Tuple3D{ 1, 1, 1 }},
													   PrimitiveVector3{ Tuple3D{ 1, 0, 0 }},
													   PrimitiveVector3{ Tuple3D{ 0, 1, 0 }},
													   PrimitiveVector3{ Tuple3D{ 0, 0, 1 }},
													   sys2,  "Sys 3" );
	Vector3D vect6{ Tuple3D{ 1, 0, 0 }, sys3 };
	Vector3D vect7 = vect6.ConvertTo( sys1 );
	if( vect7 != Vector3D{ Tuple3D{ -1, 0, 0 }, sys1 } ) return false;

	Point3D p1{ Tuple3D{ 1, 0, 0 }, sys3 };
	Point3D p2 = p1.ConvertTo( sys1 );
	if( p2 != Point3D{ Tuple3D{ -3, 0, 0 }, sys1 } ) return false;

	UnitVector3D uv1{ Tuple3D{ 5, 2, 2}, sys3 };
	if( !IsNearlyEqualDistance( uv1.length(), 1 ) ) return false;

	Vector3D rotVec1 = vect6.RotateConstant( Vector3D{ Tuple3D{ 0, 1, 0 }, vect6.GetCoordinateSystem() }, PI/2);
	if( rotVec1 != Vector3D{ Tuple3D{ 0, 0, -1 }, vect6.GetCoordinateSystem() } ) return false;




	CoordinateSystem* cSys1 = GlobalSystem()->AddCoordinateSystem(	PrimitiveVector3{ Tuple3D{ 1, 1, 1 } },
												PrimitiveVector3{ Tuple3D{ 0, 1, 0 } },
												PrimitiveVector3{ Tuple3D{ 0, 0, 1 } },
												PrimitiveVector3{ Tuple3D{ 1, 0, 0 } },
												"System 1" );

	CoordinateSystem* cSys11 = cSys1->AddCoordinateSystem(	PrimitiveVector3{ Tuple3D{ 2, 2, 2 } },
										PrimitiveVector3{ Tuple3D{ sqrt( 2 )/2, sqrt( 2 ) / 2, 0 } },
										PrimitiveVector3{ Tuple3D{ -sqrt( 2 ) / 2, sqrt( 2 ) / 2, 0 } },
										PrimitiveVector3{ Tuple3D{ 0, 0, 1 } },
										   "System 1.1" );

	CoordinateSystem* cSys2 = GlobalSystem()->AddCoordinateSystem( PrimitiveVector3{ Tuple3D{ -1, -1, -1 } },
											  PrimitiveVector3{ Tuple3D{ 0, 0, 1 } },
											  PrimitiveVector3{ Tuple3D{ 0, 1, 0 } },
											  PrimitiveVector3{ Tuple3D{ 1, 0, 0 } },
											  "System 2" );
	Point3D P1{ Tuple3D{ 3, 3, 3 }, cSys11 };
	Point3D P2 = P1.ConvertTo( cSys1 );
	Point3D P3 = P1.ConvertTo( GlobalSystem() );
	Point3D P4 = P1.ConvertTo( cSys2 );

	Point3D expectedP4{ Tuple3D{ 4+3*sqrt(2), 4, 7 }, cSys2 };
	if( P4 != expectedP4 ) return false;

	 return true;
 }