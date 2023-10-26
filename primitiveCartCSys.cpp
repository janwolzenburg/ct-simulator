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
#include "serialization.h"


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
	ex.normalise();
	ey.normalise();
	ez.normalise();

	// Vector have to be orthogonal to each other
	if( !iseqErr( ex * ey, 0 ) ||
		!iseqErr( ey * ez, 0 ) ||
		!iseqErr( ex * ez, 0 ) ) checkErr( MATH_ERR::INPUT, "Unit axis must be orthogonal to each other!" );
}

primitiveCartCSys::primitiveCartCSys( const vector<char>& binary_data, vector<char>::const_iterator& it ) : 
	origin( DeSerialize<primitiveVec3>( binary_data, it ) ),
	ex( DeSerialize<primitiveVec3>( binary_data, it ) ),
	ey( DeSerialize<primitiveVec3>( binary_data, it ) ),
	ez( DeSerialize<primitiveVec3>( binary_data, it ) )
{
	// Normalize vectors
	ex.normalise();
	ey.normalise();
	ez.normalise();

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


size_t primitiveCartCSys::Serialize( vector<char>& binary_data ) const{

	size_t num_bytes = 0;
	num_bytes += origin.Serialize( binary_data );
	num_bytes += ex.Serialize( binary_data );
	num_bytes += ey.Serialize( binary_data );
	num_bytes += ez.Serialize( binary_data );

	return num_bytes;
}