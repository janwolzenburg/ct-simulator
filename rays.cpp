/*********************************************************************
 * @file   rays.cpp
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

#include "rays.h"
#include "cSysTree.h"

/*********************************************************************
   Implementations
*********************************************************************/


/*
	ray implementation
*/

ray::ray( const vec3 v_, const pnt3 p_, const rayProperties properties_ )
	: line( v_, p_ ),
	properties( properties_ ){};

ray::ray( const line line_, const rayProperties  properties_ )
	: line( line_ ),
	properties( properties_ ){};

ray::ray( void ) :
	ray(vec3{ v3{ 1, 0, 0 }, DUMMY_CSYS()}, pnt3{v3{ 0, 0, 0}, DUMMY_CSYS() }, rayProperties{})
{

}

ray ray::convertTo( const cartCSys* const target ) const{
	return ray{ this->line::convertTo( target ), properties };
};

double ray::getPara( const pnt3 p, bool* const success ) const{
	double t = line::getPara( p, success );
	*success = *success && ( t >= 0 );
	return t;
}

ray ray::projectOnXYPlane( const cartCSys* const cSys ) const{
	return ray{ this->line::projectOnXYPlane( cSys ), this->properties };
}

void ray::updateProperties( const voxData data, const double distance ){

	// Equally scale powerspectrum
	properties.powerSpectrum.scale( exp( -data.k * distance ) );

}

bool ray::paraInBounds(const double para) const { return para >= 0; };