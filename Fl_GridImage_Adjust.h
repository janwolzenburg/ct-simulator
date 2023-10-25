#pragma once

#include <FL/Fl_Group.H>
#include <FL/Fl_Hor_Slider.H>
#include "Fl_OwnValuator.h"
#include "Fl_GridImage.h"

class Fl_GridImage_Adjust : public Fl_Group{

	public:

	Fl_GridImage_Adjust( int x, int y, int w, int h, const char* label = 0L );

	void changeContrast( const range bounds );

	void assignImage( const monoImage& img );

	void assignImage( const grid<voxData>& modGrid, const bool normalize = false );

	void setSliderBounds( const range newBound );

	bool handleEvents( void );

	void setSliderBoundsFromImage( void );

	range getContrast( void ) const{ return range( lowerBound.value(), upperBound.value() ); };

	bool imageAssigned( void ) const{ return imgWidget.imageAssigned(); };

	private:
	
	Fl_GridImage imgWidget;
	Fl_OwnValuator<Fl_Hor_Slider> lowerBound;
	Fl_OwnValuator<Fl_Hor_Slider> upperBound;
	bool boundsSet;


};