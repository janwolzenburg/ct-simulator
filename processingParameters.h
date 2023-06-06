#pragma once


#include "generel.h"

#include "filter.h"

class processingParameter{

	public:

	static const string FILE_PREAMBLE;

	processingParameter( void );

	processingParameter( const vector<char>& binData, vector<char>::const_iterator& it );

	size_t serialize( vector<char>& binData ) const;


	public:

	range projectionsContrast;

	discreteFilter::TYPE filterType;

	range filteredProjectionsContrast;

	range reconstrucedImageContrast;

};