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
 #include <FL/Fl_Hor_Slider.H>

#include "monoImage.h"
#include "rgbImage.h"
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

	void assignImage( const grid<voxData>& modGrid, const bool normalize = false );

	void calculateScaled( void );

	void updateScaled( void );

	inline bool imageAssigned( void ) const{ return imgAssigned; };

	private:

	bool imgAssigned;

	monoImage originalImage;


	bool hasOverlay;
	//vector<vector<pair<bool, rgb_Int>>> overlay;
	vector<pair<idx2CR, rgb_Int>> overlay;

	rgbImage colorImage;

	static rgb_Int bgColor;

};




class Fl_GridImage_Adjust : public Fl_Group{

	public:

	Fl_GridImage_Adjust( int x, int y, int w, int h, const char* label = 0L );

	void changeContrast( const range bounds );

	void assignImage( const monoImage& img );

	void assignImage( const grid<voxData>& modGrid, const bool normalize = false );

	void setSliderBounds( const range newBound );

	bool handleEvents( void );

	void setSliderBoundsFromImage( void );

	inline range getContrast( void ) const{ return range( lowerBound.value(), upperBound.value() ); };

	inline bool imageAssigned( void ) const{ return imgWidget.imageAssigned(); };

	private:
	
	Fl_GridImage imgWidget;
	Fl_OwnValuator<Fl_Hor_Slider> lowerBound;
	Fl_OwnValuator<Fl_Hor_Slider> upperBound;
	bool boundsSet;


};