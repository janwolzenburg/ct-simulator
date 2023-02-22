#pragma once
/*********************************************************************
 * @file   propability.h
 * @brief  Classes for randomness and probability distributions
 *
 * @author Jan Wolzenburg
 * @date   Febuary 2023
 *********************************************************************/


 /*********************************************************************
   Includes
*********************************************************************/

#include <random>
#include <vector>
using std::vector;

#include "propability.fwd.h"
#include "generel.h"

/*********************************************************************
   Definitions
*********************************************************************/

constexpr size_t MAX_GENERATORS = 20;

allRandomNumberGenerators& ALL_GENERATORS( void );


class randomNumberGenerator{

	public:
	randomNumberGenerator( const int minValue, const int maxValue );

	int getRandom( void );


	private:
	std::default_random_engine generator;
	std::uniform_int_distribution<int> distribution;

};



class allRandomNumberGenerators{

	public:

	static allRandomNumberGenerators& getInstance(){
		static allRandomNumberGenerators instance;
		return instance;
	}

	randomNumberGenerator* addGenerator( const int minValue, const int maxValue );


	private:

	allRandomNumberGenerators( void );

	/*!
	 * @brief Deleted copy constructor
	*/
	allRandomNumberGenerators( const allRandomNumberGenerators& tree_ ) = delete;

	/*!
	 * @brief Deleted assignment operator
	*/
	allRandomNumberGenerators& operator=( const allRandomNumberGenerators& tree_ ) = delete;


	private:
	vector<randomNumberGenerator> allGenerators;

};



class propabilityDistribution{


	public:

	propabilityDistribution( const vector<v2> distribution_, const size_t maxNumberOfBins );

	//propabilityDistribution( void );

	void construct( const vector<v2> distribution_, const size_t maxNumberOfBins );

	double getRandom( void ) const;


	private:

	vector<v2> distribution;
	vector<double> uniformPropabilities;
	randomNumberGenerator* generator;

};