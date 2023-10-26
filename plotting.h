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

enum GANTRY_SPECIFIERS{
	ORIGIN =			0x01 << 0,
	BEAMS =				0x01 << 1,
	DETECTOR_SURFACES = 0x01 << 2,
	DETECTOR_NORMALS =	0x01 << 3
 };


ofstream openAxis( path file_name, const bool reset );

void closeAxis( ofstream& axis );

template<class C>
string getObjectString( const C object );

template<class C, typename T>
string getObjectString( const C object, const T additional );

template<class C>
void addSingleObject( ofstream& axis, const string name, const C object );

template<class C>
void addSingleObject( ofstream& axis, const string name, const C object, const string parameter );

template<class C, typename T>
void addSingleObject( ofstream& axis, const string name, const C object, const string parameter, const T additional );

template<class C, typename T>
void addObject(ofstream& axis, const string name, const C object, const string parameter, const T additional);


#include "plotting.hpp"