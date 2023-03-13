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

class greyImage{

	public:

	greyImage( void ) :
		width( 0 ),
		height( 0 ),
		numPixel( width* height ),
		data( numPixel, 0. ),
		imData( numPixel, 0 )
	{

	}

	greyImage( const grid source ) : 
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

	greyImage( const vector<char>& binData, vector<char>::const_iterator& it ) : 
		width( deSerializeBuildIn( (size_t) 1, binData, it ) ),
		height( deSerializeBuildIn( (size_t) 1, binData, it ) ),
		numPixel( width* height ),
		data( numPixel, 0. ),
		imData( numPixel, 0 )
	{
		
		for( size_t i = 0; i < numPixel; i++ ){
			data.at( i ) = deSerializeBuildIn( 0., binData, it );
		}

		if( data.size() == 0 ) return;

		const double maxVal = Max( data );
		const double minVal = Min( data );

		for( size_t i = 0; i < numPixel; i++ ){
			imData.at( i ) = (unsigned char) ( ( ( data.at( i ) - minVal ) / ( maxVal - minVal ) ) * 255. );
		}

	};

	inline size_t Width( void ) const{ return width; };
	inline size_t Heigth( void ) const{ return height; };
	vector<unsigned char> getImage( void ) const{ return imData; };

	/*!
	 * @brief Serialize this object
	 * @param binData Reference to vector where data will be appended
	*/
	size_t serialize( vector<char>& binData ) const{
		
		size_t numBytes = 0;
		numBytes += serializeBuildIn( width, binData );
		numBytes += serializeBuildIn( height, binData );

		for( size_t i = 0; i < numPixel; i++ ){
			numBytes += serializeBuildIn( data.at( i ), binData );
		}

		return numBytes;
	};


	private:
	size_t width;
	size_t height;
	size_t numPixel;


	vector<double> data;
	vector<unsigned char> imData;


};