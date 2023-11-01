#pragma once
/*********************************************************************
 * @file   Fl_GridImage.h
 * @brief
 *
 * @author Jan Wolzenburg
 * @date   September 2023
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/

 #include <FL/Fl_Widget.H>

#include "grayscaleImage.h"
#include "rgbImage.h"


 /*********************************************************************
	Definitions
 *********************************************************************/

/*!
 * @brief Class for a grayscale image with optional colored overlay
*/
class Fl_GridImage : public Fl_Widget{

	friend class Fl_GridImage_Adjust;

	public:

	/*!
	 * @brief Constructor
	 * @param x x-position
	 * @param y y-position
	 * @param w Width
	 * @param h Height
	 * @param label Label
	*/
	Fl_GridImage( int x, int y, int w, int h, const char* label = 0L );

	/*!
	 * @brief Draw the image
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
	 * @brief Assign grayscale image as new image data_
	 * @param img Grayscale image
	*/
	void assignImage( const GrayscaleImage& img );

	/*!
	 * @brief Assign gridded voxel data 
	 * @param modGrid Data grid
	 * @param Normalise Flag for normalisation
	*/
	void assignImage( const DataGrid<VoxelData>& modGrid, const bool normalise = false );

	/*!
	 * @brief Calculate new image when size changed
	*/
	void calculateScaled( void );

	/*!
	 * @brief Update and redraw image when size changed
	*/
	void updateScaled( void );

	/*!
	 * @brief Check if image is assigned
	 * @return True when an image was previously assigned
	*/
	bool imageAssigned( void ) const{ return imgAssigned; };


	private:

	bool imgAssigned;			/*!<Flag to track whether an image has been assigned*/

	GrayscaleImage originalImage;	/*!<Raw grayscale imagedata*/

	bool hasOverlay;						/*!<Flag indicating existing overlay*/
	vector<pair<bool, rgb_Int>> overlay;	/*!<Overlay as collection of a flag and color value. At assignment initialised with an element for each pixel*/

	rgbImage colorImage;		/*!<Colored image resulting from grayscale image with overlayed color data_*/

	static rgb_Int bgColor;		/*!<Background color for pixel without data_*/

};
