#pragma once
/*********************************************************************
 * @file   tube.h
 * @brief  Class for roentgen tube
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/


 #include <vector>
 using std::vector;

 #include "vectorAlgotrithm.h"
 #include "rays.h"
 

 constexpr double c_mPers = 299792456;					// Speed of light in m/s
 constexpr double e_As = 1.6021917E-19;					// Electron charge in As 
 constexpr double h_Js = 6.626196E-34;					// Planck's constant

 constexpr double k_1PerV = 1.1E-9;						// k constant for roentgen power
 constexpr double alFilterCutOffFrequency = 16000. * e_As / h_Js;	// Cut of energy/frequency in roentgen spectrum by 2mm aluminium filter
 constexpr double alFilterGradiantFactor = -3.5;		// Quotient of filter gradient to bremsstrahlung gradient


 class pnt2{
	 public:
	 double x = 0;
	 double y = 0;
};
 

class spectrum{

	public:

	spectrum(void) {

	}

	spectrum( const vector<double> X, const vector<double> Y )
	{
		if( X.size() != Y.size() ){
			data = vector<pnt2>{ pnt2{0, 0} };
			return;
		}

		for (size_t i = 0; i < X.size(); i++) {
			data.push_back( pnt2{ X.at( i ), Y.at( i ) } );
		}
	
	};

	public:
	vector<pnt2> data;


 };


class tube{

	public:

	tube( const double anodeVoltage_, const double anodeCurrent_, const size_t anodeAtomicNumber_ ) :
		anodeVoltage_V( Fpos( anodeVoltage_ ) ),
		anodeCurrent_A( Fpos( anodeCurrent_ ) ),
		anodeAtomicNumber( Fpos( anodeAtomicNumber_ ) ),
		totalPower_W( k_1PerV * anodeAtomicNumber * anodeCurrent_A * pow( anodeVoltage_V, 2 ) ),
		maxRadiationFrequency_Hz( e_As * anodeVoltage_V / h_Js )
	{

		// Frequencies
		vector<double> frequencies = linearSpace( alFilterCutOffFrequency, maxRadiationFrequency_Hz, numPointsInSpectrum );
		
		// Values
		vector<double> spectralPower( frequencies.size(), 0. );

		
		
		// Frequency to which the filter dominates spectral behavious
		double changeFrequency = frequencies.front() + ( frequencies.back() - frequencies.front() ) / ( 1. - alFilterGradiantFactor );

		// Fill value vector
		for (auto freqIt = frequencies.begin(); freqIt < frequencies.end(); freqIt++) {
			size_t curIdx = freqIt - frequencies.begin();	// Current index

			double bremsGradient = -1;											// Gradient of brems spectrum
			double filterGradient = alFilterGradiantFactor * bremsGradient;		// Gradient of filter spectrum

			// Filter dominates
			if (*freqIt < changeFrequency) {
				spectralPower.at( curIdx ) = ( *freqIt - frequencies.front() ) * filterGradient;
				continue;
			}

			// Bremsspectrum dominates
			spectralPower.at(curIdx) = ( frequencies.back() - *freqIt ) * ( - bremsGradient );
		}


		// Calculate correction factor for spectrum for its values to sum up to totalPower
		double currentSum = sum( spectralPower );
		double correctionFactor = totalPower_W / currentSum;

		// Correct values for sums to match
		scale( spectralPower, correctionFactor );

		// Write frequency and power values to spectrum
		xRay_spectrum = spectrum{ frequencies, spectralPower };
	
	};



	private:

	const size_t numPointsInSpectrum = 20;

	double anodeVoltage_V;
	double anodeCurrent_A;
	size_t anodeAtomicNumber;

	double totalPower_W;
	double maxRadiationFrequency_Hz;



	spectrum xRay_spectrum;

 };