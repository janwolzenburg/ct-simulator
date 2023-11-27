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
#include "rayScattering.h"

/*********************************************************************
   Implementations
*********************************************************************/

EnergySpectrum::EnergySpectrum( const VectorPair energy_quantaties ) :
	photonflow_per_energy_( ConvertToTuple( energy_quantaties ) ),
	mean_energy_valid_( false )
{
	if( photonflow_per_energy_.size() == 0 ) return;

	// Sort data by x value
	sort( photonflow_per_energy_.begin(), photonflow_per_energy_.end(), []( const Tuple2D& d1, const Tuple2D& d2) { return d1.x < d2.x; } );

	if( photonflow_per_energy_.size() == 1 ){
		mean_energy_ = photonflow_per_energy_.front().x;
		return;
	}

	energy_resolution_ = photonflow_per_energy_.at( 1 ).x - photonflow_per_energy_.at( 0 ).x;

	// Check consistency of energy
	for( auto dataIt = photonflow_per_energy_.cbegin() + 1; dataIt < photonflow_per_energy_.cend(); dataIt++ ){

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

void EnergySpectrum::ScaleEvenly( const double factor ){

	for_each( photonflow_per_energy_.begin(), photonflow_per_energy_.end(), [ & ] ( Tuple2D& v ) { v.y *= factor; } );
	mean_energy_valid_ = false;
}

EnergySpectrum EnergySpectrum::GetEvenlyScaled( const double factor ) const {

	EnergySpectrum scaledSpectrum{ *this };
	scaledSpectrum.ScaleEvenly( factor );

	return scaledSpectrum;

}


double EnergySpectrum::GetSum( void ) const{
	return std::accumulate( photonflow_per_energy_.cbegin(), photonflow_per_energy_.cend(), 0., [] ( const double& currentSum, const Tuple2D& currentValue ) { return currentSum + currentValue.y; });
}

double EnergySpectrum::GetTotalPowerIn_eVPerSecond( void ) const{
	return std::accumulate( photonflow_per_energy_.cbegin(), photonflow_per_energy_.cend(), 0., [] ( const double& currentSum, const Tuple2D& currentValue ){ return currentSum + currentValue.x * currentValue.y; } );;

}

void EnergySpectrum::UpdateMeanEnergy( void ){

	if( photonflow_per_energy_.size() == 0 ){
		mean_energy_ = 0.;	
		mean_energy_valid_ = true;
		return;
	}
	
	if( photonflow_per_energy_.size() == 1 ){
		mean_energy_ = photonflow_per_energy_.front().x;
		mean_energy_valid_ = true;
		return;
	}

	// Get the sum of products. In principle an "expected value"
	const double expectedValue = GetTotalPowerIn_eVPerSecond();

	mean_energy_ = expectedValue / GetSum();	
	mean_energy_valid_ = true;
}

void EnergySpectrum::Modify( std::function<void( Tuple2D& )> modFunction ){
	for( Tuple2D& v : photonflow_per_energy_ ){
		modFunction( v );
	}

	mean_energy_valid_ = false;
}

void EnergySpectrum::GetAbsorped( const VoxelData& voxel_data, const double distance ){
	
	double k;

	for( auto& data_point: photonflow_per_energy_ ){
		k = voxel_data.GetAbsorptionAtEnergy( data_point.x );
		data_point.y *= exp( -k * distance );
	}

	mean_energy_valid_ = false;
}


void EnergySpectrum::ScaleEnergy( double energy, const double factor ){

	if( photonflow_per_energy_.size() == 0 ) return;
	if( photonflow_per_energy_.size() == 1 ){
		photonflow_per_energy_.front().y *= factor;
		return;
	}

	energy = ForceRange( energy, photonflow_per_energy_.front().x, photonflow_per_energy_.back().x );

	size_t energy_index = static_cast<size_t>( floor( ( energy - photonflow_per_energy_.front().x ) / energy_resolution_ + 0.5 ) );

	if( energy_index >= photonflow_per_energy_.size() ) energy_index = photonflow_per_energy_.size() - 1;
	photonflow_per_energy_.at( energy_index ).y *= factor;

}