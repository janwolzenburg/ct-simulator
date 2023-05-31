/*********************************************************************
 * @file   radonTransform.cpp
 * @brief  Implementations
 *
 * @author Jan Wolzenburg
 * @date   January 2023
 *********************************************************************/



 /*********************************************************************
   Includes
*********************************************************************/

#include "vectorAlgorithm.h"
#include "radonTransform.h"
#include "generelMath.h"



/*********************************************************************
  Implementations
*********************************************************************/



/*!
 * radonCoords implementation
*/

radonCoords::radonCoords( const double theta_, const double distance_ ) : 
	theta( theta_ ), 
	distance( distance_ )
{}

radonCoords::radonCoords( const cartCSys* const cSys, const line l ) : 
	theta( 0 ), 
	distance( 0 )
{
	// Project ray on XY plane
	const line projectedLine = l.projectOnXYPlane( cSys );

	// Get perpendicualar to projected ray through coordinate system's origin
	vec3 lot = projectedLine.getLot( cSys->OPnt() );


	// Ray intersects origin
	if( iseqErr( lot.Length(), 0 ) ){
		// Lot vector only for angle calculation
		lot = projectedLine.R() ^ cSys->EzVec();
	}
	// No intersection -> distance from perpendicular
	else{
		// y component is zero
		if( iseqErr( lot.Y(), 0 ) ){
			// x component is less than zero
			if( lot.X() < 0 ) distance = -lot.Length();
			else distance = lot.Length();
		}
		// y component is not zero
		else{
			// y component is less than zero
			if( lot.Y() < 0 ) distance = -lot.Length();
			else  distance = lot.Length();
		}
	}

	// Calculate radon angle

	// y component is zero
	if( iseqErr( lot.Y(), 0 ) ) theta = 0;
	// y component is greater than zero
	else if( lot.Y() > 0 ) theta = cSys->ExVec().angle( lot );
	// y component is less than zero
	else theta = PI - cSys->ExVec().angle( lot );

}


radonCoords::radonCoords( void ) : radonCoords( 0, 0 )
{}


radonPoint::radonPoint( const radonCoords coordinates_, const double value_ ) :
	coordinates( coordinates_ ),
	value( value_ )
{}



/*!
 * radonTransformed implementation
*/


const string radonTransformed::FILE_PREAMBLE{ "RADON_TRANSFORMED_FILE_PREAMBLE" };

radonTransformed::radonTransformed( void ) :
	grid()
{
	gridErrors = vector<vector<v2CR>>( Size().col, vector<v2CR>( Size().row, v2CR{ INFINITY, INFINITY } ) );
}

radonTransformed::radonTransformed( const detectorRadonParameter detectorParameter ) :
	grid{		idx2CR{ detectorParameter.numberPoints.col - 1,
						detectorParameter.numberPoints.row }, 
				v2CR{	0, 
						-( (double) ( detectorParameter.numberPoints.row - 1 ) * detectorParameter.resolution.row ) / 2. },
				detectorParameter.resolution,
				0 }
{
	gridErrors = vector<vector<v2CR>>( Size().col, vector<v2CR>( Size().row, v2CR{ INFINITY, INFINITY }));
}


grid radonTransformed::Data( void ) const{
	return (grid) *this;
}

void radonTransformed::assignData( const idx2CR index, const double value ){
	this->operator()( index ) = value;
}

void radonTransformed::assignData( const radonPoint data ){

	v2CR point{ data.coordinates.theta, data.coordinates.distance };
	idx2CR index = getIndex( point );
	v2CR gridPoint{ getCoordinates( index )};

	v2CR error{
		gridPoint.col - point.col,
		gridPoint.row - point.row
	};

	gridErrors.at( index.col ).at( index.row ) = error;
	this->operator()( index ) = data.value;
}


size_t radonTransformed::serialize( vector<char>& binData ) const{
	size_t numBytes = 0;

	numBytes += serializeBuildIn( FILE_PREAMBLE, binData );
	numBytes += grid::serialize( binData );
	numBytes += serializeBuildIn( gridErrors, binData );
	return numBytes;
}


radonTransformed::radonTransformed( const vector<char>& binData, vector<char>::const_iterator& it ) : 
	grid{ binData, it },
	gridErrors( deSerialize< vector<vector<v2CR>> >( binData, it ) )
{
	
}