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
#include "energySpectrum.h"
#include "generelMath.h"
#include "rayScattering.h"


/*********************************************************************
   Implementations
*********************************************************************/

EnergySpectrum::EnergySpectrum( const VectorPair& energy_quantaties ) :
	EnergySpectrum{ ConvertToTuple( energy_quantaties ) }
{
	
}


EnergySpectrum::EnergySpectrum( const vector<Tuple2D>& energy_quantaties ) :
	photonflow_per_energy_( energy_quantaties ),
	mean_energy_valid_( false ){

	if( photonflow_per_energy_.size() == 0 ) return;

	// sort data by x value
	sort( photonflow_per_energy_.begin(), photonflow_per_energy_.end(), []( const Tuple2D& d1, const Tuple2D& d2) { return d1.x < d2.x; } );

	if( photonflow_per_energy_.size() == 1 ){
		mean_energy_ = photonflow_per_energy_.front().x;
		mean_energy_valid_ = true;
		return;
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

	// get the sum of products. In principle an "expected value"
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
	
	double coefficient;

	for( auto& photonflow: photonflow_per_energy_ ){
		coefficient = voxel_data.GetAbsorptionAtEnergy( photonflow.x );
		photonflow.y *= exp( -coefficient * distance );
	}

	mean_energy_valid_ = false;
}



size_t EnergySpectrum::GetEnergyIndex( double energy_to_search ) const{
	
	double min_difference = INFINITY;
	size_t energy_index = 0, current_energy_index = 0;

	for( const auto& [energy, photonfloy] : photonflow_per_energy_ ){
		const double difference = abs( energy - energy_to_search );
		if( difference < min_difference ){
			min_difference = difference;
			energy_index = current_energy_index;
		}
		current_energy_index++;
	}

	return energy_index;
}

void EnergySpectrum::ScaleEnergy( double energy, const double factor ){

	ScaleEnergy( GetEnergyIndex( energy ), factor );

}

void EnergySpectrum::ScaleEnergy( const size_t energy_index, const double factor ){
	
	if( photonflow_per_energy_.size() == 0 ) return;
	if( photonflow_per_energy_.size() == 1 ){
		photonflow_per_energy_.front().y *= factor;
		return;
	}


	photonflow_per_energy_.at( energy_index ).y *= factor;
}

double EnergySpectrum::GetEnergy( const size_t index ) const{
	if( index > photonflow_per_energy_.size() - 1 ) return 0.;
	
	return photonflow_per_energy_.at( index ).x;

}

double EnergySpectrum::GetPhotonflow( const double energy ) const{
	return photonflow_per_energy_.at( GetEnergyIndex( energy ) ).y;
}