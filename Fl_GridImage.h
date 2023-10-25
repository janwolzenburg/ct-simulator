#pragma once
/*********************************************************************
 * @file   Fl_GridImage.h
 * @brief
 *
 * @author Jan Wolzenburg
 * @date   March 2023
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/

 #include <FL/Fl_Widget.H>

#include "monoImage.h"
#include "rgbImage.h"



 /*********************************************************************
	Definitions
 *********************************************************************/


class Fl_GridImage : public Fl_Widget{

	friend class Fl_GridImage_Adjust;

	public:

	Fl_GridImage( int x, int y, int w, int h, const char* label = 0L );

	virtual void draw( void );

	virtual void resize( int x, int y, int w, int h );

	void assignImage( const monoImage& img );

	void assignImage( const grid<voxData>& modGrid, const bool normalize = false );

	void calculateScaled( void );

	void updateScaled( void );

	bool imageAssigned( void ) const{ return imgAssigned; };

	private:

	bool imgAssigned;

	monoImage originalImage;

	bool hasOverlay;
	vector<pair<bool, rgb_Int>> overlay;

	rgbImage colorImage;

	static rgb_Int bgColor;

};
