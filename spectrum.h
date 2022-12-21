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

	spectrum(void);

	spectrum(const vector<double> X, const vector<double> Y);

	spectrum getScaled( const double factor ) const;


public:
	vector<v2> data;


};