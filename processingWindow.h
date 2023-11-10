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

 #include "FL/Fl_Window.H"

 #include "fl_AdjustableGrayscaleImage.h"
 #include "Fl_Selector.h"
 #include "Fl_Plot.h"
 #include "grayscaleImage.h"
 #include "lineplot.h"

 #include "programState.h"
 #include "projections.h"



/*!
 * @brief Window for processing the projections
*/
class processingWindow : public Fl_Window{

	public:

	/*!
	 * @brief Constructor
	 * @param x x-position
	 * @param y y-position
	 * @param w Width
	 * @param h Height
	*/
	processingWindow( int w, int h, const char* label, Projections newSinogram );
	
	/*!
	 * @brief Recalculate the filtered projections 
	*/
	void recalcFilteredProjections( void );

	
	private:

	Projections currentprojections;
	FilteredProjections currentFilteredProjections;		/*!<Current filtered projections*/
	ReconstrucedImage currentReconstrucedImage;			/*!<Current image reconstructed from filtered projections*/


	Fl_Group sinogramGrp;	/*!<Group for sinogram view*/
	Fl_AdjustableGrayscaleImage sinogramWidget;	/*!<Widget for sinogram display*/
	
	Fl_Group filterGrp;		/*!<Group for filter*/
	Fl_Selector filterTypeSelector;		/*!<Filter selector*/
	Fl_Plot<DotPlot> filterPlot;		/*!<Filter plot*/

	Fl_Group filteredProjGrp;				/*!<Group for the fitlered projections*/
	Fl_AdjustableGrayscaleImage filteredProjWidget;	/*!<Widget for filtered projections display*/

	Fl_Group reconstructionGrp;			/*!<Group for the reconstructed image*/
	Fl_AdjustableGrayscaleImage reconstructionImageWidget;	/*!<Widget for the reconstruced image*/


	static void HandleFilterChange( Fl_Widget* selector_widget, void* window );


 };