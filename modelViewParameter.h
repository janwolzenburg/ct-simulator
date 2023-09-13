#pragma once


#include <string>
using std::string;

#include <vector>
using std::vector;

#include "generel.h"


class modelViewParameter{

	public:

	static const string FILE_PREAMBLE;

	modelViewParameter( void );

	modelViewParameter( const vector<char>& binData, vector<char>::const_iterator& it );

	size_t serialize( vector<char>& binData ) const;


	public:

	range viewContrast;

};