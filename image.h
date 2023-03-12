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
		data( new double[numPixel] ),
		imData( new char[numPixel] )
	{
		
		for( size_t c = 0; c < width; c++ ){
			for( size_t r = 0; r < height; r++ ){

				data[ c + r * width ] = source.operator()(idx2CR{c, r});
			}
		}

		

	}

	image( const image& sImg ) : 
		width( sImg.width ),
		height( sImg.height ),
		numPixel( width* height ),
		data( new double[numPixel] ),
		imData( new char[numPixel] )
	{
		

		memcpy( data, sImg.data, numPixel * sizeof( double ) );
		memcpy( imData, sImg.imData, numPixel * sizeof( char ) );
	};

	image& operator()( const image& sImg ){
		width = sImg.width;
		height = sImg.height;

		numPixel = width * height;
		double* tempData = new double[numPixel];
		char* tempImData = new char[numPixel];

		delete[] data;
		delete[] imData;

		data = tempData;
		memcpy( data, sImg.data, numPixel * sizeof( double ) );

		imData = tempImData;
		memcpy( imData, sImg.imData, numPixel * sizeof( char ) );

		return *this;
	}

	~image( void ){ delete[] data; };


	private:
	size_t width;
	size_t height;
	size_t numPixel;

	double* data;
	char* imData;


};