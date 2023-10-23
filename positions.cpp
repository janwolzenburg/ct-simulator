/******************************************************************
* @file   positions.cpp
* @brief  Implementations
*
* @author Jan Wolzenburg
* @date   March 2023
* ********************************************************************/



/*********************************************************************
  Includes
*********************************************************************/

#include "positions.h"

#include "generelMath.h"


/*********************************************************************
  Implementations
*********************************************************************/



/*
	relPosition implementation
*/

relPosition::relPosition( double x_, double y_, double w_, double h_ ) :
	w( Fmax( Fmin( w_, 0. ), 1. ) ),
	h( Fmax( Fmin( h_, 0. ), 1. ) ),
	x( Fmax( Fmin( x_, 0. ), 1. - w ) ),
	y( Fmax( Fmin( y_, 0. ), 1. - h ) ){}


relPosition_Hor::relPosition_Hor( double y_, double w_, double h_ ) :
	relPosition{ ( 1. - w_ ) / 2., y_, w_, h_ }{}


relPosition_Ver::relPosition_Ver( double x_, double w_, double h_ ) :
	relPosition{ x_, ( 1. - h_ ) / 2., w_, h_ }{}


relPosition_VerHor::relPosition_VerHor( double w_, double h_ ) :
	relPosition{ ( 1. - w_ ) / 2., ( 1. - h_ ) / 2., w_, h_ }{}