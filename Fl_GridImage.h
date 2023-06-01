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

#include "monoImage.h"
#include "Fl_OwnValuator.h"



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

	void calculateScaled( void );

	void updateScaled( void );


	private:

	monoImage originalImage;
	monoImage scaledImage;
};




class Fl_GridImage_Adjust : public Fl_Group{

	public:

	Fl_GridImage_Adjust( int x, int y, int w, int h, const char* label = 0L );

	void changeContrast( const range bounds );

	void assignImage( const monoImage& img );

	bool handleEvents( void );

	inline range getContrast( void ) const{ return range( lowerBound.value(), upperBound.value() ); };

	private:
	
	Fl_GridImage imgWidget;
	Fl_OwnValuator<Fl_Hor_Value_Slider> lowerBound;
	Fl_OwnValuator<Fl_Hor_Value_Slider> upperBound;

};