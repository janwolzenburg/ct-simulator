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
	 * @param path_ Storage path for plot image
	 * @param xlabel_ x-axis label
	 * @param ylabel_ y-axis label
	 * @param limits_ Plot limits
	 * @param xFormat_ x-ticks format string
	 * @param yFormat_ y-ticks format string
	 * @param axisEqual_ Flag to make axis equally scaled
	 * @param grid_ Flag to show grid
	*/
	void initialisePlot( const path path_, const string xlabel_, const string ylabel_,
						 const plotLimits limits_, const string xFormat_, const string yFormat_, const bool axisEqual_, const bool grid_ );

	/*!
	 * @brief Set new axis limits
	 * @param newLimits New limits
	*/
	void setLimits( const plotLimits newLimits ){ plotInstance.setLimits( newLimits ); };

	/*!
	 * @brief Get reference to plot
	 * @return Underlying plot
	*/
	plotType& plotRef( void ){ return plotInstance; };

	/*!
	 * @brief Assign image data from persisting storage
	*/
	void assignData( void ){ assignImage( plotInstance.getImgPath() ); };

	/*!
	 * @brief Draw plot as image
	*/
	void draw( void ) override;

	/*!
	 * @brief Resize the image
	 * @param x New x postion
	 * @param y New y positon
	 * @param w New width
	 * @param h New height
	*/
	void resize( int x, int y, int w, int h ) override;

	/*!
	 * @brief Calculate scaled plot image
	*/
	void calculateScaled( void );


	private:

	/*!
	 * @brief Assign plot image
	 * @param filename Path to image
	*/
	void assignImage( const path filename );


	private:

	string label;				/*!<Label*/
	plotType plotInstance;		/*!<A plot*/
	Fl_PNG_Image* sourceImage;	/*!<Raw image*/
	Fl_Image* image;			/*!<Scaled FL-image*/
};


#include "Fl_plots.hpp"