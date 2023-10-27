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
  #include "coordinateSystemTree.h"
  #include "test_device.h"
  #include "test_model.h"
  #include "tomography.h"
  #include "tube.h"
#include "simulation.h"

bool test_scattered_angle_propabilities( void ){

	double angleResolution = 2. / 360 * 2 * PI;
	NumberRange energyRange = NumberRange{ al_filter_cut_off_energy_eV, 150000. };
	size_t numEnergies = 20;
	//double energyResolution = energyRange.GetResolution( numEnergies );

	rayScattering anglePropabilites{ (size_t) (2.*PI / angleResolution), energyRange, numEnergies, UnitVector3D{Tuple3D{1., 0., 0.}, GlobalSystem()}};

	const double testEnergy = 100000.;
	vector<Tuple2D> distribution = anglePropabilites.getDistribution( testEnergy );

	vector<Tuple2D> experimentalDistribution = distribution;
	for( Tuple2D& currentValue : experimentalDistribution ) currentValue.y = 0;

	double angleStart = experimentalDistribution.front().x;

	for( int i = 0; i < 100000; i++ ){
		
		double angle = anglePropabilites.getRandomAngle( testEnergy );
		
		size_t angleIndex = ForceToMax( (size_t) floor( ( angle - angleStart ) / angleResolution + 0.5 ), experimentalDistribution.size() );

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

	gantry testGantry = getTestGantry( GridIndex{ 70, 20 }, 1000 );
	model mod = getTestModel( GlobalSystem(), 1 );

	ofstream ax1 = openAxis( path( "./test_ray_scattering.txt" ), true );

	addObject( ax1, "Gantry", testGantry, "r", GANTRY_SPECIFIERS::ORIGIN | GANTRY_SPECIFIERS::DETECTOR_SURFACES );
	//addObject( ax1, "TestModel", mod, "g", 0.015 );

	vector<ray> rays = testGantry.getBeam( 1. );
	for( ray &r : rays ) r = r.convertTo( mod.CSys()); 


	size_t maxRadiationLoopsTest = 10;
	vector<double> angles;

	vector<Tuple2D> anglesHistogram;

	vector<double> discreteAngles;
	for( double a = 0; a <= PI; a += PI / 1080 ){
		discreteAngles.push_back( a );
		anglesHistogram.emplace_back( a, 0.);
	}

	tomographyParameter tomoParameter{1., true, 16, .05, 5e-2};

	for( size_t currentLoop = 0; currentLoop < maxRadiationLoopsTest && rays.size() > 0; currentLoop++ ){


		//const bool enableScattering = currentLoop < maxRadiationLoopsTest - 1;	// No scattering in last iteration
		vector<ray> raysForNextIteration;								// Rays to process in the next iteration

		for( const ray r : rays ){

			const ray retRay = mod.rayTransmission( r, tomoParameter, testGantry.RayScattering() );

			double plotLength = ( retRay.origin() - r.origin() ).length();

			if( mod.pntInside( retRay.origin() ) ){
				addSingleObject( ax1, "Ray", r, "m", plotLength );
				angles.push_back( r.direction().GetAngle( retRay.direction() ) );
				raysForNextIteration.push_back( retRay );

			} 
			else if( currentLoop > 0 ){
				addSingleObject( ax1, "Ray", r, "m", plotLength );
			}

		
			
		}

		// Copy rays to vector
		rays = raysForNextIteration;

	}


	for( ray r : rays ){
		RayVoxelIntersection res{ mod.Vox(), r };

		addSingleObject( ax1, "Ray", r, "m", ( r.origin() - res.exit_.intersection_point_ ).length() );
	}

	closeAxis( ax1 );


	for( auto current : angles ){

		size_t idx = GetClosestElementIndex( discreteAngles, current );
		if( idx >= anglesHistogram.size() ) idx = anglesHistogram.size() - 1;
		anglesHistogram.at( idx ).y++;
	}

	ofstream ax2 = openAxis( path( "./test_ray_scattering_angles.txt" ), true );

	addSingleObject( ax2, "Angles", anglesHistogram, "Angles;Num;Dots");

	closeAxis( ax2 );
	return true;
}