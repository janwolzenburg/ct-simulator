/*********************************************************************
 * @file   plotting.cpp
 * @brief  Implementations
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/


 /*********************************************************************
	 Includes
  *********************************************************************/

#include "plotting.h"



  /*********************************************************************
	 Implementations
  *********************************************************************/

ofstream openAxis( path fileName, const bool reset ){
	ofstream outFile;


	if( reset ) outFile.open( fileName, std::ios::out | std::ios::trunc );
	else outFile.open( fileName, std::ios::out | std::ios::app );

	return outFile;
}

void closeAxis( ofstream& axis ){
	axis.close();
}


template<>
string getObjectString<ray, double>(const ray r, const double length) {

	char tempCharArr[256];
	snprintf(tempCharArr, 256, "lin (%.12f,%.12f,%.12f;%.12f,%.12f,%.12f;%.12f)", r.O().gX(), r.O().gY(), r.O().gZ(), r.R().gX(), r.R().gY(), r.R().gZ(), length);

	return string{ tempCharArr };

}
template<>
void addObject<vector<ray>, double>(ofstream& axis, const string name, const vector<ray> rays, const string parameter, const double length) {

	for (auto r : rays) {
		addSingleObject(axis, name, r, parameter, length);
	}

}