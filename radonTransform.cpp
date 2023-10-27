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
#include "serialization.h"



/*********************************************************************
  Implementations
*********************************************************************/



/*!
 * radonCoords implementation
*/

radonCoords::radonCoords( const CoordinateSystem* const cSys, const Line l ) : 
	theta( 0 ), distance( 0 )
{
	// Project ray on XY plane
	const Line projectedLine = l.ProjectOnXYPlane( cSys );

	// Get perpendicualar to projected ray through coordinate system's origin_
	Vector3D lot = projectedLine.GetLot( cSys->GetOriginPoint() );


	// Ray intersects origin_
	if( IsNearlyEqualDistance( lot.length(), 0 ) ){
		// Lot vector only for GetAngle calculation
		lot = projectedLine.direction() ^ cSys->GetEz();
	}
	// No intersection -> distance from perpendicular
	else{
		// y component is zero
		if( IsNearlyEqualDistance( lot.Y(), 0 ) ){
			// x component is less than zero
			if( lot.X() < 0 ) distance = -lot.length();
			else distance = lot.length();
		}
		// y component is not zero
		else{
			// y component is less than zero
			if( lot.Y() < 0 ) distance = -lot.length();
			else  distance = lot.length();
		}
	}

	// Calculate radon GetAngle

	// y component is zero
	if( IsNearlyEqualDistance( lot.Y(), 0 ) ) theta = 0;
	// y component is greater than zero
	else if( lot.Y() > 0 ) theta = cSys->GetEx().GetAngle( lot );
	// y component is less than zero
	else theta = PI - cSys->GetEx().GetAngle( lot );

}


/*!
 * radonTransformed implementation
*/


const string radonTransformed::FILE_PREAMBLE{ "RADON_TRANSFORMED_FILE_PREAMBLE" };

radonTransformed::radonTransformed( void ) :
	grid<>()
{
	gridErrors = vector<vector<GridCoordinates>>( Size().c, vector<GridCoordinates>( Size().r, GridCoordinates{ INFINITY, INFINITY } ) );
}

radonTransformed::radonTransformed( const detectorRadonParameter detectorParameter ) :
	grid<>{		GridIndex{ detectorParameter.numberPoints.c - 1,
						detectorParameter.numberPoints.r }, 
				GridCoordinates{	0, 
						-( (double) ( detectorParameter.numberPoints.r - 1 ) * detectorParameter.resolution.r ) / 2. },
				detectorParameter.resolution,
				0 }
{
	gridErrors = vector<vector<GridCoordinates>>( Size().c, vector<GridCoordinates>( Size().r, GridCoordinates{ INFINITY, INFINITY }));
}

void radonTransformed::assignData( const radonPoint dataPoint ){

	GridCoordinates point{ dataPoint.coordinates.theta, dataPoint.coordinates.distance };
	GridIndex index = getIndex( point );
	GridCoordinates gridPoint{ getCoordinates( index )};

	GridCoordinates error{
		gridPoint.c - point.c,
		gridPoint.r - point.r
	};

	gridErrors.at( index.c ).at( index.r ) = error;
	this->setData( index, dataPoint.value );
}


size_t radonTransformed::Serialize( vector<char>& binary_data ) const{
	size_t num_bytes = 0;

	num_bytes += SerializeBuildIn( FILE_PREAMBLE, binary_data );
	num_bytes += grid<>::Serialize( binary_data );
	num_bytes += SerializeBuildIn( gridErrors, binary_data );
	return num_bytes;
}


radonTransformed::radonTransformed( const vector<char>& binary_data, vector<char>::const_iterator& it ) : 
	grid<>( binary_data, it ),
	gridErrors( DeSerialize< vector<vector<GridCoordinates>> >( binary_data, it ) )
{}