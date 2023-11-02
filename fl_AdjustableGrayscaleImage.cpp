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
	image_widget_{ X( *this, 0. ), Y( *this, 0. ), W( *this, 1. ), H( *this, .9 ), "Image" },
	lower_bound_{ X( *this, 0.2 ), Y( *this, 0.91 ), W( *this, .6 ), H( *this, .04 ), "Low" },
	upper_bound_{ X( *this, 0.2 ), Y( *this, 0.96 ), W( *this, .6 ), H( *this, .04 ), "High" },
	bounds_set_( false ){
	
	Fl_Group::add( image_widget_ );
	Fl_Group::add( lower_bound_ );
	Fl_Group::add( upper_bound_ );

	lower_bound_.align( FL_ALIGN_LEFT );
	upper_bound_.align( FL_ALIGN_LEFT );

	lower_bound_.ValueToLabel( true );
	upper_bound_.ValueToLabel( true );

	this->hide();

}

void Fl_AdjustableGrayscaleImage::ChangeSliderValues( const NumberRange bounds ){

	lower_bound_.value( ForceRange( bounds.start(), lower_bound_.minimum(), lower_bound_.maximum() ) );
	upper_bound_.value( ForceRange( bounds.end(), upper_bound_.minimum(), upper_bound_.maximum() ) );

}

void Fl_AdjustableGrayscaleImage::AssignImage( const GrayscaleImage& img ){
	image_widget_.AssignImage( img );
	UpdateSliderBounds();

	image_widget_.AdjustContrast( NumberRange{ lower_bound_.value(), upper_bound_.value() } );
	this->show();
}

bool Fl_AdjustableGrayscaleImage::UpdateSliderBounds( void ){
	
	bool bounds_changed = false;

	if( !bounds_set_ ){
		SetSliderBoundsFromImage();
		bounds_changed = true;
	}

	if( lower_bound_.minimum() > image_widget_.GetMinimum() ){
		SetSliderBounds( NumberRange{ image_widget_.GetMinimum(), upper_bound_.maximum() } );
		lower_bound_.value( image_widget_.GetMinimum() );
		bounds_changed = true;
	}

	if( upper_bound_.maximum() < image_widget_.GetMaximum() ){
		SetSliderBounds( NumberRange{ lower_bound_.minimum(), image_widget_.GetMaximum() } );
		upper_bound_.value( image_widget_.GetMaximum() );
		bounds_changed = true;
	}

	return bounds_changed;
}

void Fl_AdjustableGrayscaleImage::AssignImage( const DataGrid<VoxelData>& modGrid ){//, const bool normalise ){

	image_widget_.AssignImage( modGrid, false );//, normalise );
	UpdateSliderBounds();
	image_widget_.AdjustContrast( NumberRange{ lower_bound_.value(), upper_bound_.value() } );
	
	this->show();
}

void Fl_AdjustableGrayscaleImage::SetSliderBoundsFromImage( void ){

	lower_bound_.bounds( image_widget_.GetMinimum(), image_widget_.GetMaximum() );
	upper_bound_.bounds( image_widget_.GetMinimum(), image_widget_.GetMaximum() );

	lower_bound_.step( image_widget_.GetMaximum() - image_widget_.GetMinimum(), 200 );
	upper_bound_.step( image_widget_.GetMaximum() - image_widget_.GetMinimum(), 200 );
	
	lower_bound_.value( lower_bound_.minimum() );
	upper_bound_.value( upper_bound_.maximum() );

	bounds_set_ = true;
}

void Fl_AdjustableGrayscaleImage::SetSliderBounds( const NumberRange newBound ){

	lower_bound_.bounds( newBound.start(), newBound.end() );
	upper_bound_.bounds( newBound.start(), newBound.end() );

	lower_bound_.value( newBound.start() );
	upper_bound_.value( newBound.end() );

	lower_bound_.step( newBound.GetDifference() / 100. );
	upper_bound_.step( newBound.GetDifference() / 100. );

	ChangeSliderValues( newBound ); 

	bounds_set_ = true;
}

bool Fl_AdjustableGrayscaleImage::HandleEvents( void ){

	bool updateImage = false;

	if( lower_bound_.ChangeFlag() ){
		if( lower_bound_.value() >= upper_bound_.value() ){
			lower_bound_.value( upper_bound_.value() - ( lower_bound_.maximum() - lower_bound_.minimum() ) / 200. );
		}

		updateImage = true;
	}

	if( upper_bound_.ChangeFlag() ){
		if( upper_bound_.value() <= lower_bound_.value() ){
			upper_bound_.value( lower_bound_.value() + ( upper_bound_.maximum() - upper_bound_.minimum() ) / 200. );
		}

		updateImage = true;
	}

	if( updateImage ){
		image_widget_.AdjustContrast( NumberRange{ lower_bound_.value(), upper_bound_.value() } );
	}

	return updateImage;

}