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
 #include <FL/Fl_Hor_Value_Slider.H>

#include "image.h"
#include "Own_Valuator.h"



 /*********************************************************************
	Definitions
 *********************************************************************/


class Fl_Image_Widget : public Fl_Widget{

	friend class Fl_EditImage_Widget;

	public:

	Fl_Image_Widget( int x, int y, int w, int h, const char* label = 0L );

	virtual void draw( void );

	virtual void resize( int x, int y, int w, int h );

	void assignImage( const greyImage& img );

	void calculateScaled( void );

	void updateScaled( void );


	private:

	greyImage originalImage;
	greyImage scaledImage;
};




class Fl_EditImage_Widget : public Fl_Group{

	public:

	Fl_EditImage_Widget( int x, int y, int w, int h, const char* label = 0L );

	void assignImage( const greyImage& img );

	void handleEvents( void );


	private:
	
	Fl_Image_Widget imgWidget;
	Own_Valuator<Fl_Hor_Value_Slider> lowerBound;
	Own_Valuator<Fl_Hor_Value_Slider> upperBound;

};