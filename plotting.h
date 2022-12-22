#pragma once
/*********************************************************************
 * @file   plotting.h
 * @brief  MATLAB plotting 
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/



 /*********************************************************************
	Includes
 *********************************************************************/
 #include <string>
 using std::string;

#include <filesystem>
 using std::filesystem::path;

#include <fstream>
using std::ofstream;


 /*********************************************************************
	Definitions
 *********************************************************************/


ofstream openAxis( path fileName, const bool reset );

void closeAxis( ofstream& axis );

template<class C, typename T>
string getObjectString( const C object, const T additional );

template<class C, typename T>
void addObject( ofstream axis, const string name, const C object, const string parameter, const T additional ){

	axis << name;
	axis << ": " << getObjString( object, additional ) << ' ' << parameter << '\n';

}