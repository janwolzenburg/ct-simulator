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
	image_widget_{ X( *this, 0. ), Y( *this, 0. ), W( *this, 1. ), H( *this, .8 ), "Image" },
	lower_bound_{ X( *this, 0.1 ), Y( *this, 0.82 ), W( *this, .8 ), H( *this, .08 ), "Low" },
	upper_bound_{ X( *this, 0.1 ), Y( *this, 0.92 ), W( *this, .8 ), H( *this, .08 ), "High" },
	
	common_factor_text_{ X( *this, 0.9 ), Y( *this, 0.82 ), W( *this, .1 ), H( *this, .18 ), "" },
	common_power_( 0 ),
	bounds_set_( false ),
	contrast_changed_( false )
{
	
	Fl_Group::add( image_widget_ );
	Fl_Group::add( lower_bound_ );
	Fl_Group::add( upper_bound_ );
	Fl_Group::add( common_factor_text_ );

	lower_bound_.type( FL_HOR_NICE_SLIDER  );
	upper_bound_.type( FL_HOR_NICE_SLIDER  );

	lower_bound_.textsize( 10 );
	upper_bound_.textsize( 10 );

	lower_bound_.align( FL_ALIGN_LEFT );
	upper_bound_.align( FL_ALIGN_LEFT );
	
	lower_bound_.precision( 0 );
	upper_bound_.precision( 0 );

	lower_bound_.callback( HandleValueChange, this );
	upper_bound_.callback( HandleValueChange, this );

	common_factor_text_.type( FL_NO_BOX );

	this->hide();

}

void Fl_AdjustableGrayscaleImage::ChangeSliderValues( const NumberRange bounds ){

	lower_bound_.value( ForceRange( bounds.start() * pow( 10., common_power_ ) , lower_bound_.minimum(), lower_bound_.maximum() ) );
	upper_bound_.value( ForceRange( bounds.end()  * pow( 10., common_power_ ), upper_bound_.minimum(), upper_bound_.maximum() ) );
	contrast_changed_ = true;
}

void Fl_AdjustableGrayscaleImage::AssignImage( const GrayscaleImage& img ){
	image_widget_.AssignImage( img );
	UpdateSliderBounds();
	image_widget_.AdjustContrast( NumberRange{ lower_bound_.value() * pow( 10., -common_power_ ), upper_bound_.value() * pow( 10., -common_power_ ) } );
	this->show();
}

void Fl_AdjustableGrayscaleImage::AssignImage( const DataGrid<VoxelData>& modGrid ){//, const bool normalise ){

	image_widget_.AssignImage( modGrid, false );
	UpdateSliderBounds();
	image_widget_.AdjustContrast( NumberRange{ lower_bound_.value() * pow( 10., -common_power_ ), upper_bound_.value() * pow( 10., -common_power_ ) } );
	
	this->show();
}

void Fl_AdjustableGrayscaleImage::UpdateSliderBounds( void ){
	
	const bool bounds_were_set = bounds_set_;

	double previous_low = lower_bound_.value();
	double previous_upper = upper_bound_.value();

	SetSliderBoundsFromImage();

	if( bounds_were_set )
		ChangeSliderValues( NumberRange{ previous_low * pow( 10., -common_power_ ), previous_upper * pow( 10., -common_power_ ) } );
	
}



void Fl_AdjustableGrayscaleImage::SetSliderBoundsFromImage( void ){
	SetSliderBounds( NumberRange{ image_widget_.GetMinimum(), image_widget_.GetMaximum() } );
}

void Fl_AdjustableGrayscaleImage::SetSliderBounds( const NumberRange newBound ){

	size_t digits_to_display = 4;

	int tenth_power_low = 0;
	double low = abs( newBound.start() );

	if( !IsNearlyEqual( low, 0., 1e-9, ComparisonMode::Absolute ) ){
		if( low < pow( 10., digits_to_display ) ){
			for(; ( low *= 10. ) < pow( 10., digits_to_display ); tenth_power_low++ );
		}
		else{
			for(; ( low /= 10. ) > pow( 10., digits_to_display ); tenth_power_low-- );
		}
	}
	else{
		tenth_power_low = 4;
	}

	int tenth_power_high = 0;
	double high = abs( newBound.end() );
	if( !IsNearlyEqual( high, 0., 1e-9, ComparisonMode::Absolute ) ){
		if( high < pow( 10., digits_to_display ) ){
			for(; ( high *= 10. ) < pow( 10., digits_to_display ); tenth_power_high++ );
		}
		else{
			for(; ( high /= 10. ) > pow( 10., digits_to_display ); tenth_power_high-- );
		}
	}
	else{
		tenth_power_high = 4;
	}

	common_power_ = Min( tenth_power_low, tenth_power_high );

	common_factor_text_.copy_label( string{ "x10^" + to_string( -common_power_ ) }.c_str() );


	NumberRange scaledBounds = NumberRange{ newBound.start() * pow (10., common_power_ ),
											newBound.end() *   pow (10., common_power_ ) };

	lower_bound_.step( 1 );
	upper_bound_.step( 1 );

	lower_bound_.bounds( scaledBounds.start(), scaledBounds.end() - upper_bound_.step() );
	upper_bound_.bounds( scaledBounds.start() - upper_bound_.step(), scaledBounds.end() );

	lower_bound_.value( scaledBounds.start() );
	upper_bound_.value( scaledBounds.end() );


	contrast_changed_ = true;
	bounds_set_ = true;
}


void Fl_AdjustableGrayscaleImage::HandleValueChange( Fl_Widget* widgetPtr, void* image_widget){

	Fl_AdjustableGrayscaleImage* widget = static_cast<Fl_AdjustableGrayscaleImage*>( image_widget );

	if( widget->lower_bound_.value() >= widget->upper_bound_.value() ){
		if( widget->lower_bound_.value() > widget->lower_bound_.minimum() )
			widget->lower_bound_.value( widget->upper_bound_.value() - widget->lower_bound_.step() );
		else
			widget->upper_bound_.value( widget->lower_bound_.value() + widget->upper_bound_.step() );
	}

	widget->upper_bound_.value( widget->upper_bound_.value() );
	widget->lower_bound_.value( widget->lower_bound_.value() );

	const double low = widget->lower_bound_.value() * pow( 10., -widget->common_power_ );
	const double high = widget->upper_bound_.value() * pow( 10., -widget->common_power_ );

	widget->image_widget_.AdjustContrast( NumberRange{ low, high } );
	widget->contrast_changed_ = true;

	widget->do_callback();

}