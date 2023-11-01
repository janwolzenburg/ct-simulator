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

#include "Fl_GridImage_Adjust.h"
#include "widgets.h"


/*********************************************************************
  Implementations
*********************************************************************/

Fl_GridImage_Adjust::Fl_GridImage_Adjust( int x, int y, int w, int h, const char* label ) :
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

void Fl_GridImage_Adjust::changeContrast( const NumberRange bounds ){

	lowerBound.value( ForceRange( bounds.start(), lowerBound.minimum(), lowerBound.maximum() ) );
	upperBound.value( ForceRange( bounds.end(), upperBound.minimum(), upperBound.maximum() ) );

}

void Fl_GridImage_Adjust::assignImage( const GrayscaleImage& img ){

	imgWidget.assignImage( img );

	if( !boundsSet )
		setSliderBoundsFromImage();

	imgWidget.originalImage.AdjustContrast( NumberRange( lowerBound.value(), upperBound.value() ) );
	imgWidget.updateScaled();

	this->show();


}

void Fl_GridImage_Adjust::assignImage( const DataGrid<VoxelData>& modGrid, const bool normalise ){

	imgWidget.assignImage( modGrid, normalise );

	if( !boundsSet )
		setSliderBoundsFromImage();

	imgWidget.originalImage.AdjustContrast( NumberRange( lowerBound.value(), upperBound.value() ) );
	imgWidget.updateScaled();

	this->show();

}

void Fl_GridImage_Adjust::setSliderBoundsFromImage( void ){

	lowerBound.bounds( imgWidget.originalImage.GetMinimum(), imgWidget.originalImage.GetMaximum() );
	upperBound.bounds( imgWidget.originalImage.GetMinimum(), imgWidget.originalImage.GetMaximum() );

	lowerBound.value( lowerBound.minimum() );
	upperBound.value( upperBound.maximum() );

	lowerBound.step( imgWidget.originalImage.GetMaximum() - imgWidget.originalImage.GetMinimum(), 200 );
	upperBound.step( imgWidget.originalImage.GetMaximum() - imgWidget.originalImage.GetMinimum(), 200 );

	boundsSet = true;
}

void Fl_GridImage_Adjust::setSliderBounds( const NumberRange newBound ){

	lowerBound.bounds( newBound.start(), newBound.end() );
	upperBound.bounds( newBound.start(), newBound.end() );

	lowerBound.value( newBound.start() );
	upperBound.value( newBound.end() );

	lowerBound.step( newBound.GetDifference() / 100. );
	upperBound.step( newBound.GetDifference() / 100. );

	changeContrast( newBound ); 

	boundsSet = true;
}

bool Fl_GridImage_Adjust::handleEvents( void ){

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

		imgWidget.originalImage.AdjustContrast( NumberRange( lowerBound.value(), upperBound.value() ) );
		imgWidget.updateScaled();

	}

	return updateImage;

}