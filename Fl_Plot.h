#pragma once
/*********************************************************************
 * @file   Fl_Plot.h
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

/*!
 * @brief A plot widget
 * @tparam plotType Type of plot
*/
template< class plotType >
class Fl_Plot : public Fl_Widget{

	static_assert( std::is_base_of_v<plot, plotType> );

	public:

	/*!
	 * @brief Constructor
	 * @param x x-position
	 * @param y y-position
	 * @param w Width
	 * @param h Height
	 * @param label Label
	*/
	Fl_Plot( int x, int y, int w, int h,  const char* label = 0L );

	/*!
	 * @brief Destructor
	*/
	~Fl_Plot();

	/*!
	 * @brief Initialise plot
	 * @param path_ 
	 * @param xlabel_ 
	 * @param ylabel_ 
	 * @param limits_ 
	 * @param xFormat_ 
	 * @param yFormat_ 
	 * @param axisEqual_ 
	 * @param grid_ 
	*/
	void initializePlot( const path path_, const string xlabel_, const string ylabel_,
						 const plotLimits limits_, const string xFormat_, const string yFormat_, const bool axisEqual_, const bool grid_ );

	void setLimits( const plotLimits newLimits ){ plotInstance.setLimits( newLimits ); };

	plotType& plotRef( void ){ return plotInstance; };

	void assignData( void ){ assignImage( plotInstance.getImgPath() ); };

	void draw( void ) override;

	void resize( int x, int y, int w, int h ) override;

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