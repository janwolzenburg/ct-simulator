#pragma once
/*********************************************************************
 * @file   widgets.h
 * @brief
 *
 * @author Jan Wolzenburg
 * @date   March 2023
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/

 #include <FL/Fl_Widget.H>

#include "image.h"
#include <FL/Fl_Value_Slider.H>



 /*********************************************************************
	Definitions
 *********************************************************************/


class Fl_Image_Widget : public Fl_Widget{


	public:

	Fl_Image_Widget( int x, int y, int w, int h, const char* label = 0L );

	virtual void draw( void );

	virtual void resize( int x, int y, int w, int h );

	void assignImage( const greyImage& img );

	void calculateScaled( void );




	private:

	greyImage originalImage;
	greyImage scaledImage;
};




class Fl_EditImage_Widget : public Fl_Group{

	public:

	Fl_EditImage_Widget( int x, int y, int w, int h, const char* label = 0L ) : 
		Fl_Group( x, y, w, h, label ),
		imgWidget( X( *this, 0. ), Y( *this, 0. ), W( *this, 1. ), H( *this, .5), "Image" ),
		lowerBound( X( *this, 0. ), Y( *this, 0.6 ), W( *this, 1. ), H( *this, .15 ), "Low" ),
		upperBound( X( *this, 0. ), Y( *this, 0.85 ), W( *this, 1. ), H( *this, .15 ), "High" )
	{
		Fl_Group::add( imgWidget );
		Fl_Group::add( lowerBound );
		Fl_Group::add( upperBound );
		
		this->hide();
	
	}

	void assignImage( const greyImage& img ){
		imgWidget.assignImage( img );
	
		

		lowerBound.bounds(0.,1.);
	
	
	}


	private:
	
	Fl_Image_Widget imgWidget;
	Own_Valuator<Fl_Value_Slider> lowerBound;
	Own_Valuator<Fl_Value_Slider> upperBound;

};