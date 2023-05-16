#pragma once
/*********************************************************************
 * @file   processingWindow.h
 * @brief
 *
 * @author Jan Wolzenburg
 * @date   May 2023
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/

 #include "widgets.h"

 #include "FL/Fl_Window.H"

 #include "Fl_GreyImage.h"
 #include "image.h"

 #include "radonTransform.h"


class processingView : public Fl_Window{

	public:

	processingView( int w, int h, const char* label ) :
		Fl_Window( w, h, label ),
		
		sinogramGrp( X( *this, .05 ), Y( *this, .05 ), W( *this, .4 ), H( *this, .4 ) ),
 		sinogramWidget(		X( sinogramGrp, .0 ),		Y( sinogramGrp, .0 ),		W( sinogramGrp, 1. ),			H( sinogramGrp, 1. ) )
	{
		Fl_Window::add( sinogramGrp );
		Fl_Window::resizable( *this );

		sinogramGrp.add( sinogramWidget );
		sinogramWidget.box( FL_BORDER_BOX );
	}

	void assignSinogram( const radonTransformed newSinogram ){

		sinogram = newSinogram;
		sinogramImg = greyImage( sinogram.Data() );

		sinogramWidget.assignImage( sinogramImg );

	}



	public:

	radonTransformed sinogram;

	Fl_Group sinogramGrp;
	greyImage sinogramImg;
	Fl_Image_Widget sinogramWidget;


 };