/*********************************************************************
 * @file   spectrum.cpp
 * @brief  Implementations of spectrum class
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/



 /*********************************************************************
   Includes
*********************************************************************/

#include <algorithm>
using std::sort;
using std::for_each;

#include <numeric>
#include "spectrum.h"
#include "generelMath.h"


/*********************************************************************
   Implementations
*********************************************************************/

EnergySpectrum::EnergySpectrum( const VectorPair energy_quantaties ) :
	data_( ConvertToTuple( energy_quantaties ) ),
	mean_energy_valid_( false )
{

	// Sort data by x value
	sort( data_.begin(), data_.end(), []( const Tuple2D& d1, const Tuple2D& d2) { return d1.x < d2.x; } );

	energy_resolution_ = data_.at( 1 ).x - data_.at( 0 ).x;

	// Check consistency of energy
	for( auto dataIt = data_.cbegin() + 1; dataIt < data_.cend(); dataIt++ ){

		double diff = dataIt->x - ( dataIt - 1 )->x;

		if( !IsNearlyEqual( energy_resolution_, diff, .005, ComparisonMode::Relative ) ){
			cerr << "Spectrum must have energies equally spaced!" << endl;
		}
	}

	UpdateMeanEnergy();
}

double EnergySpectrum::mean_energy( void ){

	if( !mean_energy_valid_ ){
		UpdateMeanEnergy();
	}
	
	return mean_energy_;
}

void EnergySpectrum::Scale( const double factor ){

	for_each( data_.begin(), data_.end(), [ & ] ( Tuple2D& v ) { v.y *= factor; } );
	UpdateMeanEnergy();
}

EnergySpectrum EnergySpectrum::GetScaled( const double factor ) const {

	EnergySpectrum scaledSpectrum{ *this };
	scaledSpectrum.Scale( factor );

	return scaledSpectrum;

}

double EnergySpectrum::GetTotal( void ) const{

	return std::accumulate( data_.cbegin(), data_.cend(), 0., [] ( const double& currentSum, const Tuple2D& currentValue ) { return currentSum + currentValue.y; });

}

void EnergySpectrum::UpdateMeanEnergy( void ){

	// Get the sum of products. In principle an "expected value"
	const double expectedValue = std::accumulate( data_.cbegin(), data_.cend(), 0., [] ( const double& currentSum, const Tuple2D& currentValue ){ return currentSum + currentValue.x * currentValue.y; } );

	mean_energy_ = expectedValue / GetTotal();	
	mean_energy_valid_ = true;
}

void EnergySpectrum::Modify( std::function<void( Tuple2D& )> modFunction ){
	for( Tuple2D& v : data_ ){
		modFunction( v );
	}
}

void EnergySpectrum::Attenuate( const VoxelData& voxel_data, const double distance ){
	
	double k;

	for( auto& data_point: data_ ){
		k = voxel_data.GetAttenuationAtReferenceEnergy();//( data_point.x );
		data_point.y *= exp( -k * distance );
	}
}