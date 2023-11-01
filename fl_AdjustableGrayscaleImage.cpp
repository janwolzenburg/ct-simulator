/******************************************************************
* @file   Fl_Grid_Image.cpp
* @brief  Implementations
*
* @author Jan Wolzenburg
* @date   October 2023
* ********************************************************************/


/*********************************************************************
  Includes
*********************************************************************/

#include "fl_AdjustableGrayscaleImage.h"
#include "widgets.h"


/*********************************************************************
  Implementations
*********************************************************************/

Fl_AdjustableGrayscaleImage::Fl_AdjustableGrayscaleImage( int x, int y, int w, int h, const char* label ) :
	Fl_Group{ x, y, w, h, label },
	imgWidget{ X( *this, 0. ), Y( *this, 0. ), W( *this, 1. ), H( *this, .9 ), "Image" },
	lowerBound{ X( *this, 0.2 ), Y( *this, 0.91 ), W( *this, .6 ), H( *this, .04 ), "Low" },
	upperBound{ X( *this, 0.2 ), Y( *this, 0.96 ), W( *this, .6 ), H( *this, .04 ), "High" },
	boundsSet( false ){
	
	
	Fl_Group::add( imgWidget );
	Fl_Group::add( lowerBound );
	Fl_Group::add( upperBound );

	lowerBound.align( FL_ALIGN_LEFT );
	upperBound.align( FL_ALIGN_LEFT );

	lowerBound.ValueToLabel( true );
	upperBound.ValueToLabel( true );

	this->hide();

}

void Fl_AdjustableGrayscaleImage::changeContrast( const NumberRange bounds ){

	lowerBound.value( ForceRange( bounds.start(), lowerBound.minimum(), lowerBound.maximum() ) );
	upperBound.value( ForceRange( bounds.end(), upperBound.minimum(), upperBound.maximum() ) );

}

void Fl_AdjustableGrayscaleImage::assignImage( const GrayscaleImage& img ){

	imgWidget.AssignImage( img );

	if( !boundsSet )
		setSliderBoundsFromImage();

	imgWidget.AdjustContrast( NumberRange( lowerBound.value(), upperBound.value() ) );


	this->show();


}

void Fl_AdjustableGrayscaleImage::assignImage( const DataGrid<VoxelData>& modGrid, const bool normalise ){

	imgWidget.AssignImage( modGrid, normalise );

	if( !boundsSet )
		setSliderBoundsFromImage();
		
	imgWidget.AdjustContrast( NumberRange( lowerBound.value(), upperBound.value() ) );


	this->show();

}

void Fl_AdjustableGrayscaleImage::setSliderBoundsFromImage( void ){

	lowerBound.bounds( imgWidget.GetMinimum(), imgWidget.GetMaximum() );
	upperBound.bounds( imgWidget.GetMinimum(), imgWidget.GetMaximum() );

	lowerBound.value( lowerBound.minimum() );
	upperBound.value( upperBound.maximum() );

	lowerBound.step( imgWidget.GetMaximum() - imgWidget.GetMinimum(), 200 );
	upperBound.step( imgWidget.GetMaximum() - imgWidget.GetMinimum(), 200 );

	boundsSet = true;
}

void Fl_AdjustableGrayscaleImage::setSliderBounds( const NumberRange newBound ){

	lowerBound.bounds( newBound.start(), newBound.end() );
	upperBound.bounds( newBound.start(), newBound.end() );

	lowerBound.value( newBound.start() );
	upperBound.value( newBound.end() );

	lowerBound.step( newBound.GetDifference() / 100. );
	upperBound.step( newBound.GetDifference() / 100. );

	changeContrast( newBound ); 

	boundsSet = true;
}

bool Fl_AdjustableGrayscaleImage::handleEvents( void ){

	bool updateImage = false;

	if( lowerBound.ChangeFlag() ){
		if( lowerBound.value() >= upperBound.value() ){
			lowerBound.value( upperBound.value() - ( lowerBound.maximum() - lowerBound.minimum() ) / 200. );
		}

		updateImage = true;
	}

	if( upperBound.ChangeFlag() ){
		if( upperBound.value() <= lowerBound.value() ){
			upperBound.value( lowerBound.value() + ( upperBound.maximum() - upperBound.minimum() ) / 200. );
		}

		updateImage = true;
	}

	if( updateImage ){
		imgWidget.AdjustContrast( NumberRange( lowerBound.value(), upperBound.value() ) );
	}

	return updateImage;

}