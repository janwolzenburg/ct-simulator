#pragma once

#include <filesystem>
using std::filesystem::path;

#include <string>
using std::string;

constexpr size_t number_of_projections = 14;
constexpr size_t number_of_pixel = 10;
constexpr double measurefield_size = 400;

path GetPath( const string filename ){
	
	#ifdef WIN32
		#define folder ".\\verification\\"
	#else
		#define folder  "./verification/"
	#endif // WIN32
	
	return path{ folder } / filename;

}