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

#include <FL/Fl.H>
#include <FL/fl_draw.H>

#include "fl_AdjustableGrayscaleImage.h"
#include "widgets.h"


/*********************************************************************
  Implementations
*********************************************************************/

Fl_AdjustableGrayscaleImage::Fl_AdjustableGrayscaleImage( int x, int y, int w, int h, const char* label ) :
	Fl_Group{ x, y, w, h, label },
	image_widget_{			X( *this, 0. ),		Y( *this, 0. ),		W( *this, 1. ), H( *this, .75 ), "Image" },
	lower_bound_{			X( *this, 0.1 ),	Y( *this, 0.84 ),	W( *this, .6 ), H( *this, .075 ), "Low" },
	upper_bound_{			X( *this, 0.1 ),	Y( *this, 0.925),	W( *this, .6 ), H( *this, .075 ), "High" },
	current_value_text_{	X( *this, 0.7 ),	Y( *this, 0.84 ),	W( *this, .2 ), H( *this, .1 ), "" },
	current_value_tip_{		X( *this, 0.7 ),	Y( *this, 0.94 ),	W( *this, .2 ), H( *this, .05 ), "" },
	common_factor_text_{	X( *this, 0.9 ),	Y( *this, 0.84 ),	W( *this, .1 ), H( *this, .15 ), "" },
	common_power_( 0 ),
	bounds_set_( false ),
	contrast_changed_( false )
{
	
	Fl_Group::add( static_cast<Fl_Group*>( &image_widget_ ) );
	Fl_Group::add( lower_bound_ );
	Fl_Group::add( upper_bound_ );
	
	Fl_Group::add( current_value_text_ );
	Fl_Group::add( common_factor_text_ );
	Fl_Group::add( current_value_tip_ );

	lower_bound_.type( FL_HOR_NICE_SLIDER  );
	upper_bound_.type( FL_HOR_NICE_SLIDER  );

	lower_bound_.textsize( 10 );
	upper_bound_.textsize( 10 );

	lower_bound_.align( FL_ALIGN_LEFT );
	upper_bound_.align( FL_ALIGN_LEFT );
	current_value_text_.align( FL_ALIGN_CENTER );
	common_factor_text_.align( FL_ALIGN_CENTER );
	current_value_tip_.align( FL_ALIGN_CENTER );

	lower_bound_.precision( 0 );
	upper_bound_.precision( 0 );

	lower_bound_.callback( HandleValueChange, this );
	upper_bound_.callback( HandleValueChange, this );
	
	current_value_text_.type( FL_NO_BOX );
	common_factor_text_.type( FL_NO_BOX );
	current_value_tip_.type( FL_NO_BOX );
	
	current_value_text_.labelsize( 20 );
	current_value_tip_.labelsize( 13 );

	this->hide();

}

int Fl_AdjustableGrayscaleImage::handle( int event ){

	bool event_used = false;
	if( Fl_Group::handle( event ) == 1 ) event_used = true;

	if( !image_widget_.image_assigned() ) return 0;

	switch( event ){

		case FL_ENTER: 
			
			return 1;


		case FL_MOVE:
		{
			int x_widget_position;
			int y_widget_position;

			if ( top_window_offset( x_widget_position, y_widget_position ) == NULL ){
				x_widget_position = 0; y_widget_position = 0;
			}
	
			// curser position in widget
			int x_position = ForceRange( Fl::event_x() - x_widget_position, 0, w() - 1 );
			int y_position = ForceRange( Fl::event_y() - y_widget_position, 0, h() - 1 );

			optional<pair<double, RGB>> pixel_value =  image_widget_.GetValue( x_position, y_position );

			if( pixel_value.has_value() ){
			
				current_value_text_.show();

				if( pixel_value.value().second.red == Fl_GrayscaleImage::metal_color.red &&
					pixel_value.value().second.green == Fl_GrayscaleImage::metal_color.green &&
					pixel_value.value().second.blue == Fl_GrayscaleImage::metal_color.blue ){
					
					current_value_text_.copy_label( "Metal" );
				}
				else{

					double factored_value = pixel_value.value().first * pow( 10., common_power_ );
					

					string value_string = string{ ToString( factored_value, 0 ) };
					current_value_text_.copy_label( value_string.c_str() );
				}
			}
			else{
				current_value_text_.hide();
			}

			return 1;
		}

		case FL_LEAVE:

			current_value_text_.hide();
			return 1;
	
		default:
			return event_used;

	}

}

void Fl_AdjustableGrayscaleImage::ChangeSliderValues( const NumberRange bounds ){

	lower_bound_.value( ForceRange( bounds.start() * pow( 10., common_power_ ) , lower_bound_.minimum(), lower_bound_.maximum() ) );
	upper_bound_.value( ForceRange( bounds.end()  * pow( 10., common_power_ ), upper_bound_.minimum(), upper_bound_.maximum() ) );
	contrast_changed_ = true;
}

void Fl_AdjustableGrayscaleImage::AssignImage( const GrayscaleImage& img ){
	image_widget_.AssignImage( img );
	SetSliderBoundsFromImage();
	this->show();
}

void Fl_AdjustableGrayscaleImage::AssignImage( const DataGrid<VoxelData>& modGrid ){//, const bool normalise ){

	image_widget_.AssignImage( modGrid, false );
	SetSliderBoundsFromImage();
	
	this->show();
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
		tenth_power_low = 12;
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
		tenth_power_high = 12;
	}

	if( IsNearlyEqual( high, 0., 1e-9, ComparisonMode::Absolute ) && IsNearlyEqual( low, 0., 1e-9, ComparisonMode::Absolute ) ){
		common_power_ = 1;
	}
	else
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

	image_widget_.AdjustContrast( NumberRange{ lower_bound_.value() * pow( 10., -common_power_ ), upper_bound_.value() * pow( 10., -common_power_ ) } );

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



void Fl_AdjustableGrayscaleImage::SetValueTip( const string tooltip_text ){


	current_value_tip_.copy_label( tooltip_text.c_str() );

}