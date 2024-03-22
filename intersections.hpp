
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
LineSurfaceIntersection<L, S>::LineSurfaceIntersection( void ) :
	intersection_point_{},
	intersection_exists_( false ),
	line_parameter_( 0. ),
	surface_parameter_1_( 0. ),
	surface_parameter_2_( 0. )
{}

template<class L, class S>
LineSurfaceIntersection<L, S>::LineSurfaceIntersection( const L& line, const S& surface ) :
	intersection_exists_( false )
{
	// primitve vectors without coordinate system context
	PrimitiveVector3 surface_origin, surface_direction_1, surface_direction_2;
	PrimitiveVector3 line_direction = line.direction().GetComponents();
	PrimitiveVector3 line_origin = line.origin().GetComponents();

	// same system?
	if( line.direction().GetCoordinateSystem() == 
		  surface.direction_1().GetCoordinateSystem() ){
		// simply get the components
		surface_origin = surface.origin().GetComponents();
		surface_direction_1 = surface.direction_1().GetComponents();
		surface_direction_2 = surface.direction_2().GetComponents();
	}
	else{
		// convert surface vectors to line's coordinate system
		surface_origin = surface.origin().GetComponents( line.direction() );
		surface_direction_1 = surface.direction_1().GetComponents( line.direction() );
		surface_direction_2 = surface.direction_2().GetComponents( line.direction() );
	}

	// create system of equations with three variables and populate
	// a*v_1 +  b*v_2 - c*v = u_line - u_surface
	SystemOfEquations system_of_equations( 3 );
	system_of_equations.PopulateColumn( surface_direction_1 );
	system_of_equations.PopulateColumn( surface_direction_2 );
	system_of_equations.PopulateColumn( -line_direction );
	system_of_equations.PopulateColumn( line_origin - surface_origin );

	// solve system
	SystemOfEquationsSolution solution = system_of_equations.Solve();

	// no solution found
	if( !solution.solution_found() ) return;
	
	// copy result
	intersection_exists_ = true;														// solution found
	line_parameter_ = solution.GetVariableValue( 2 );				// line parameter
	surface_parameter_1_ = solution.GetVariableValue( 0 );	// surface parameter A
	surface_parameter_2_ = solution.GetVariableValue( 1 );	// surface parameter B
	intersection_point_ = line.GetPoint( line_parameter_ );	// point of intersection

	// surface parameters outside allowed bounds of surface or line parameter invalid
	// uses late binding with virtual methods
	if( !surface.AreParametersInBounds( surface_parameter_1_, surface_parameter_2_ ) || 
			!line.IsParameterInBounds( line_parameter_ ) ) 
		intersection_exists_ = false;

};


template<class L, class S>
string LineSurfaceIntersection<L, S>::ToString( unsigned int newline_tabulators ) const{
	string str;
	string newLine = { '\n' };

	for( unsigned int i = 0; i < newline_tabulators; i++ ) newLine += '\t';

	str += "solution=" + intersection_exists_;
	str += newLine + intersection_point_.ToString();
	return str;
}
