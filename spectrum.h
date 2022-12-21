/*********************************************************************
 * @file   spectrum.h
 * @brief  Class for x ray spectrum
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/



 /*********************************************************************
   Includes
*********************************************************************/

#include <vector>
using std::vector;

#include "generel.h"



/*********************************************************************
   Definitions
*********************************************************************/

class spectrum {

public:

	spectrum(void) {

	}

	spectrum(const vector<double> X, const vector<double> Y)
	{
		if (X.size() != Y.size()) {
			data = vector<v2>{ v2{0, 0} };
			return;
		}

		for (size_t i = 0; i < X.size(); i++) {
			data.push_back(v2{ X.at(i), Y.at(i) });
		}

	};

public:
	vector<v2> data;


};