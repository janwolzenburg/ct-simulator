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

#include <FL/Fl.H>
#include <FL/Fl_Window.H>

#include "image.h"



 /*********************************************************************
	Definitions
 *********************************************************************/


class Fl_Image_Widget : public Fl_Widget{


	public:

	Fl_Image_Widget( int x, int y, int w, int h, const char* label = 0L );

	void assignImage( const greyImage& img );

	virtual void draw( void );


	private:

	greyImage originalImage;
	greyImage scaledImage;
};
