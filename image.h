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
#include "vectorAlgorithm.h"

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
		imData( numPixel, 0 )
	{
		
		for( size_t c = 0; c < width; c++ ){
			for( size_t r = 0; r < height; r++ ){
				data.at( c + r * width ) = source.operator()(idx2CR{c, r});
			}
		}

		const double maxVal = Max( data );
		const double minVal = Min( data );
		
		for( size_t i = 0; i < numPixel; i++ ){
			imData.at( i ) = (unsigned char) ( ( ( data.at( i ) - minVal ) / ( maxVal - minVal ) ) * 255. );
		}

	}

	image( const image& sImg) = delete;
	image& operator=( const image& sImg ) = delete;

	inline size_t Width( void ) const{ return width; };
	inline size_t Heigth( void ) const{ return height; };
	vector<unsigned char> getImage( void ) const{ return imData; };
	inline const unsigned char* getImDataPtr( void ){ return imData.data(); };


	private:
	size_t width;
	size_t height;
	size_t numPixel;


	vector<double> data;
	vector<unsigned char> imData;


};