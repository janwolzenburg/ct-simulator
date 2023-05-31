#pragma once
/*********************************************************************
 * @file   guiPlot.h
 * @brief
 *
 * @author Jan Wolzenburg
 * @date   April 2023
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/

#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_Widget.H>
#include "plots.h"


template< class plotType >
class Fl_Plot : public Fl_Widget{


	public:

	Fl_Plot( int x, int y, int w, int h,  const char* label = 0L );

	~Fl_Plot();

	void initializePlot( const path path_, const string xlabel_, const string ylabel_,
						 const plotLimits limits_, const string xFormat_, const string yFormat_, const bool axisEqual_, const bool grid_ );

	inline plotType& plotRef( void ){ return plotInstance; };

	void assignData( void );

	virtual void draw( void );

	virtual void resize( int x, int y, int w, int h );

	void calculateScaled( void );


	private:

	void assignImage( const path filename );


	private:

	string label;
	plotType plotInstance;
	Fl_PNG_Image* sourceImage;
	Fl_Image* image;
};


#include "Fl_plots.hpp"