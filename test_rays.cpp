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
  #include "cSysTree.h"
  #include "test_device.h"
  #include "test_model.h"

  #include "tube.h"

bool test_scattered_angle_propabilities( void ){

	double angleResolution = 2. / 360 * 2 * PI;
	range frequencyRange = range{ alFilterCutOffFrequency, e_As * 150000 / h_Js };
	size_t numFrequencies = 20;
	double frequencyResolution = frequencyRange.Resolution( numFrequencies );

	scatteredAngles anglePropabilites{ angleResolution, frequencyRange, frequencyResolution, uvec3{ v3{ 1, 0, 0 }, GLOBAL_CSYS() } };

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


bool test_ray_scattering(void){

	gantry testGantry = getTestGantry( idx2CR{ 70, 20 }, 3 );
	model mod = getTestModel( GLOBAL_CSYS(), 1 );

	ofstream ax1 = openAxis( path( "./test_ray_scattering.txt" ), true );

	addObject( ax1, "Gantry", testGantry, "r", GANTRY_SPECIFIERS::DETECTOR_SURFACES );
	addObject( ax1, "TestModel", mod, "g", 0.015 );

	vector<ray> rays = testGantry.getBeam();
	for( ray &r : rays ) r = r.convertTo( mod.CSys()); 


	size_t maxRadiationLoopsTest = 1;

	for( size_t currentLoop = 0; currentLoop < maxRadiationLoopsTest && rays.size() > 0; currentLoop++ ){


		const bool enableScattering = currentLoop < maxRadiationLoopsTest;	// No scattering in last iteration
		vector<ray> raysForNextIteration;								// Rays to process in the next iteration

		for( const ray r : rays ){

			const ray retRay = mod.rayTransmission( r, enableScattering, testGantry.rayScattering() );

			if( mod.pntInside( retRay.O() ) ) raysForNextIteration.push_back( retRay );

			double plotLength = (retRay.O() - r.O() ).Length();

			addSingleObject( ax1, "Ray", r, "b", plotLength );

		}

		// Copy rays to vector
		rays = raysForNextIteration;

	}


	for( ray r : rays ) addSingleObject( ax1, "Ray", r, "c", 200 );


	closeAxis( ax1 );

	return true;
}