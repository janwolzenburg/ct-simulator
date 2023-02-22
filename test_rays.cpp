#pragma once
/*********************************************************************
 * @file   test_rays.cpp
 * @brief  Implementations
 *
 * @author Jan Wolzenburg
 * @date   Febuary 2023
 *********************************************************************/


 /*********************************************************************
	 Includes
  *********************************************************************/

  #include "test_rays.h"
  #include "scattering.h"
  #include "vectorAlgorithm.h"
  #include "plotting.h"

bool test_scattered_angle_propabilities( void ){

	double angleResolution = 2. / 360 * 2 * PI;
	range frequencyRange = range{ alFilterCutOffFrequency, e_As * 150000 / h_Js };
	size_t numFrequencies = 20;
	double frequencyResolution = frequencyRange.Resolution( numFrequencies );

	scatteredAngles anglePropabilites{ angleResolution, frequencyRange, frequencyResolution };

	const double testFrequency = e_As * 100000. / h_Js;
	vector<v2> distribution = anglePropabilites.getDistribution( testFrequency );

	vector<v2> experimentalDistribution = distribution;
	for( v2& currentValue : experimentalDistribution ) currentValue.y = 0;

	double angleStart = experimentalDistribution.front().x;

	for( int i = 0; i < 100000; i++ ){
		
		double angle = anglePropabilites.getRandomAngle( testFrequency );
		
		size_t angleIndex = Fmax( (size_t) floor( ( angle - angleStart ) / angleResolution + 0.5 ), experimentalDistribution.size() );

		( experimentalDistribution.at( angleIndex ).y )++;

	}


	ofstream ax = openAxis( path( "./test_scattered_angle_propabilities_given.txt" ), true );
	addSingleObject( ax, "Given", distribution, "theta;p;Dots" );
	closeAxis( ax );

	ofstream ax1 = openAxis( path( "./test_scattered_angle_propabilities_experimental.txt" ), true );
	addSingleObject( ax1, "Experimental", experimentalDistribution, "theta;N;Dots" );
	closeAxis( ax1 );

	return true;

}