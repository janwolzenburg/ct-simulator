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

 #include "Fl_GridImage_Adjust.h"
 #include "Fl_Selector.h"
 #include "Fl_Plot.h"
 #include "monoImage.h"
 #include "lineplot.h"

 #include "programState.h"
 #include "radonTransform.h"
 #include "backprojection.h"



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
	processingWindow( int w, int h, const char* label );

	/*!
	 * @brief Deactivate everything in window
	*/
	void deactivate( void );

	/*!
	 * @brief Activate everything in window
	*/
	void activate( void );

	/*!
	 * @brief Set flag for new sinogram 
	*/
	void setNewRTFlag( void ){ newRTFlag = true; };

	/*!
	 * @brief Handle event in main window
	*/
	void handleEvents( void );
	
	/*!
	 * @brief Recalculate the filtered projections 
	*/
	void recalcFilteredProjections( void );

	
	private:

	/*!
	 * @brief Assign new sinogram
	 * @param newSinogram New sinogram to assign
	*/
	void assignSinogram( const radonTransformed newSinogram );


	bool newRTFlag;			/*!<Flag indicating that a new sinogram is available*/
	Fl_Group sinogramGrp;	/*!<Group for sinogram view*/
	monoImage sinogramImg;	/*!<Sinogram as image*/
	Fl_GridImage_Adjust sinogramWidget;	/*!<Widget for sinogram display*/
	
	bool filterChanged;		/*!<Flag to indicate that the filter changed*/
	Fl_Group filterGrp;		/*!<Group for filter*/
	Fl_Selector filterTypeSelector;		/*!<Filter selector*/
	Fl_Plot<dotplot> filterPlot;		/*!<Filter plot*/

	Fl_Group filteredProjGrp;				/*!<Group for the fitlered projections*/
	monoImage filteredProjImage;			/*!<Filtered projections as image*/
	Fl_GridImage_Adjust filteredProjWidget;	/*!<Widget for filtered projections display*/

	Fl_Group reconstructionGrp;			/*!<Group for the reconstructed image*/
	monoImage reconstructionImage;		/*!<Reconstruced image*/
	Fl_GridImage_Adjust reconstructionImageWidget;	/*!<Widget for the reconstruced image*/


 };