#pragma once
/*********************************************************************
 * @file   image.h
 * @brief
 *
 * @author Jan Wolzenburg
 * @date   March 2023
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/

#include "grid.h"


/*********************************************************************
   Definitions
*********************************************************************/

class image{

	public:

	image( const grid source ) : 
		width( source.Size().col ),
		height( source.Size().row ),
		numPixel( width * height ),
		data( numPixel, 0. ),
		imData( numPixel, 0. )
	{
		
		for( size_t c = 0; c < width; c++ ){
			for( size_t r = 0; r < height; r++ ){

				data.at( c + r * width ) = source.operator()(idx2CR{c, r});
			}
		}

		

	}


	private:
	size_t width;
	size_t height;
	size_t numPixel;


	vector<double> data;
	vector<char> imData;


};