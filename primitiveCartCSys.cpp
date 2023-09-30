/*********************************************************************
 * @file   primitiveCartCSys.cpp
 * @brief  Implementations
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/



/*********************************************************************
	Includes
 *********************************************************************/
#include "primitiveCartCSys.h"



 /*********************************************************************
	Implementations
 *********************************************************************/


/*
	primitiveCartCSys implementation
*/

primitiveCartCSys::primitiveCartCSys( const primitiveVec3 origin_, const primitiveVec3 ex_, const primitiveVec3 ey_, const primitiveVec3 ez_ )
	: origin( origin_ ),
	ex( ex_ ),
	ey( ey_ ),
	ez( ez_ )
{
	// Normalize vectors
	ex.normalize();
	ey.normalize();
	ez.normalize();

	// Vector have to be orthogonal to each other
	if( !iseqErr( ex * ey, 0 ) ||
		!iseqErr( ey * ez, 0 ) ||
		!iseqErr( ex * ez, 0 ) ) checkErr( MATH_ERR::INPUT, "Unit axis must be orthogonal to each other!" );
}

primitiveCartCSys::primitiveCartCSys( const vector<char>& binData, vector<char>::const_iterator& it ) : 
	origin( deSerialize<primitiveVec3>( binData, it ) ),
	ex( deSerialize<primitiveVec3>( binData, it ) ),
	ey( deSerialize<primitiveVec3>( binData, it ) ),
	ez( deSerialize<primitiveVec3>( binData, it ) )
{
	// Normalize vectors
	ex.normalize();
	ey.normalize();
	ez.normalize();

	// Vector have to be orthogonal to each other
	if( !iseqErr( ex * ey, 0 ) ||
		!iseqErr( ey * ez, 0 ) ||
		!iseqErr( ex * ez, 0 ) ) checkErr( MATH_ERR::INPUT, "Unit axis must be orthogonal to each other!" );
}

string primitiveCartCSys::toStr( const unsigned int newLineTabulators ) const{
	string str;
	string newLine = { '\n' };

	for( unsigned int i = 0; i < newLineTabulators; i++ ) newLine += '\t';

	str += newLine + "p=" + origin.toStr();
	str += newLine + "ex=" + ex.toStr() + newLine + "ey=" + ey.toStr() + newLine + "ez=" + ez.toStr();
	return str;
}

mathObj::MATH_ERR primitiveCartCSys::rotateM( const primitiveVec3 n, const double phi ){
	MATH_ERR tErr = MATH_ERR::OK;
	MATH_ERR errCode = MATH_ERR::OK;

	if( ( tErr = ex.rotNM( n, phi ) ) != MATH_ERR::OK ) errCode = tErr;
	if( ( tErr = ey.rotNM( n, phi ) ) != MATH_ERR::OK ) errCode = tErr;
	if( ( tErr = ez.rotNM( n, phi ) ) != MATH_ERR::OK ) errCode = tErr;

	return errCode;
}


size_t primitiveCartCSys::serialize( vector<char>& binData ) const{

	size_t numBytes = 0;
	numBytes += origin.serialize( binData );
	numBytes += ex.serialize( binData );
	numBytes += ey.serialize( binData );
	numBytes += ez.serialize( binData );

	return numBytes;
}