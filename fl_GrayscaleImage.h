#pragma once
/*********************************************************************
 * @file   fl_GrayscaleImage.h
 * @brief
 *
 * @author Jan Wolzenburg
 * @date   September 2023
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/

 #include <FL/Fl_Widget.H>
 #include <optional>
 using std::optional;

#include "grayscaleImage.h"
#include "colorImage.h"
#include <FL/fl_draw.H>

 /*********************************************************************
	Definitions
 *********************************************************************/


/*!
 * @brief Class for a Fl_Widget to display grayscale image with optional colored overlay
*/
class Fl_GrayscaleImage : public Fl_Widget{

	public:

	static const RGB background_color;		/*!< Background color for pixel without data*/
	static const RGB metal_color;			/*!< Overlay color of metal*/

	/*!
	 * @brief Constructor
	 * @param x x-position
	 * @param y y-position
	 * @param w Width
	 * @param h Height
	 * @param label Label
	*/
	Fl_GrayscaleImage( int x, int y, int w, int h, const char* label = 0L );

	/*!
	 * @brief Check if image is assigned
	 * @return True when an image was previously assigned
	*/
	bool image_assigned( void ) const{ return image_assigned_; };

	/*!
	 * @brief Get minimum of image data
	 * @return Minimum value in data
	*/
	double GetMinimum( void ) const{ return grayscale_image_.GetMinimum(); };

	/*!
	 * @brief Get maximum of image data
	 * @return maximum value in data
	*/
	double GetMaximum( void ) const{ return grayscale_image_.GetMaximum(); };

	/*!
	 * @brief Get value for given x and y coordinate
	 * @param x Local x coordinate
	 * @param y Local y coordinate
	 * @return Raw and color value when x and y are inside image
	*/
	optional<pair<double, RGB>> GetValue( int x, int y ) const;

	/*!
	 * @brief Assign grayscale image as new image data
	 * @param grayscale_image Grayscale image
	*/
	void AssignImage( const GrayscaleImage& grayscale_image );

	/*!
	 * @brief Assign gridded voxel data 
	 * @param data_grid Data grid
	 * @param auto_adjust_contrast Flag for normalisation
	*/
	void AssignImage( const DataGrid<VoxelData>& data_grid, const bool auto_adjust_contrast = true );

	/*!
	 * @brief Change the images contrast to given range
	 * @param data_range Range of value to show. Value over or under values in range will be bounded
	*/
	void AdjustContrast( const NumberRange new_contrast);
	
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

	Index2D GetImageSize( void ) const{ return {grayscale_image_scaled_.width(), grayscale_image_scaled_.height() }; };
	Index2D GetOriginalImageSize( void ) const{ return {grayscale_image_.width(), grayscale_image_.height() }; };

	private:

	GrayscaleImage grayscale_image_;	/*!< Raw grayscale imagedata*/
	bool image_assigned_;				/*!< Flag to track whether an image has been assigned*/
	
	vector<pair<bool, RGB>> overlay_;	/*!< Overlay as collection of a flag and color value. At assignment initialised with an element for each pixel*/
	bool has_overlay_;					/*!< Flag indicating existing overlay*/
	
	GrayscaleImage grayscale_image_scaled_;	/*!< Scaled copy of original image*/
	ColorImage color_image_;			/*!< Colored image resulting from grayscale image with overlayed color data_*/


	/*!
	 * @brief Calculate new image when size changed
	*/
	void CalculateScaled( void );

	/*!
	 * @brief Update and redraw image when size changed
	*/
	void Update( void );
};


class Fl_GrayscaleImageWithAxis : public Fl_Group, public Fl_GrayscaleImage{

	public:

	Fl_GrayscaleImageWithAxis( int x, int y, int w, int h, const char* label = 0L ) :
		Fl_Group{ x, y, w, h },
		Fl_GrayscaleImage{ x + axis_space_, y, w - axis_space_, h - axis_space_ }
	{
		Fl_Group::add( static_cast<Fl_GrayscaleImage*>( this ) );
		Fl_Group::resizable( static_cast<Fl_GrayscaleImage*>( this ) );


	};

	int x( void )const { return Fl_Group::x(); };
	int y( void )const { return Fl_Group::y(); };
	int w( void )const { return Fl_Group::w(); };
	int h( void )const { return Fl_Group::h(); };

	void SetAxisLabel( const pair<string, string> label ){
		
		axis_label_ = label;

	}

	void SetAxis( const Tuple2D pixel_start, const Tuple2D pixel_size ){
		
		image_start_ = pixel_start;
		pixel_size_ = pixel_size;
	}

	/*!
	 * @brief Draw the image
	*/
	void draw( void ) override{
		Fl_GrayscaleImage::draw();
		

		Index2D image_size{ Fl_GrayscaleImage::GetImageSize() };


		Index2D axis_origin{ x() + ( w() - image_size.x ) / 2 - axis_space_ / 2, y() + ( h() - image_size.y ) / 2 + image_size.y + axis_space_ / 2 };
		Index2D x_axis_tip{ axis_origin.x + image_size.x + axis_space_, axis_origin.y };
		Index2D y_axis_tip{ axis_origin.x, axis_origin.y - image_size.y - axis_space_ };

		fl_line_style( FL_SOLID, 4, 0 );

		fl_line( axis_origin.x, axis_origin.y, x_axis_tip.x, x_axis_tip.y );
		fl_line( axis_origin.x, axis_origin.y, y_axis_tip.x, y_axis_tip.y );

		fl_line( x_axis_tip.x, x_axis_tip.y, x_axis_tip.x - 10, x_axis_tip.y - 10 );
		fl_line( x_axis_tip.x, x_axis_tip.y, x_axis_tip.x - 10, x_axis_tip.y + 10 );
		fl_line( y_axis_tip.x, y_axis_tip.y, y_axis_tip.x - 10, y_axis_tip.y + 10 );
		fl_line( y_axis_tip.x, y_axis_tip.y, y_axis_tip.x + 10, y_axis_tip.y + 10 );
		
		fl_line_style( 0 );


		fl_font( FL_HELVETICA, 15 );

		fl_draw( axis_label_.first.c_str(), x_axis_tip.x + 5, x_axis_tip.y + 5 );

		int label_width, label_height;
		fl_measure( axis_label_.second.c_str(), label_width, label_height );
		fl_draw( axis_label_.second.c_str(), y_axis_tip.x - label_width / 2, y_axis_tip.y - 5);

		const int number_of_tics = 6;
		for( int i = 0; i < number_of_tics; i++ ){

			const double x_axis_tic = image_start_.x + static_cast<double>( i ) * static_cast<double>( Fl_GrayscaleImage::GetOriginalImageSize().x - 1 ) / static_cast<double>( number_of_tics - 1 ) * ( pixel_size_.x );
		
			int digits_befor_comma = 0;
			for( double divided_tic = x_axis_tic ; abs( divided_tic ) > 1.; divided_tic /= 10 )
				digits_befor_comma++;

			string number_string = ToString( x_axis_tic, 3 - digits_befor_comma );
			
			Tuple2D tic_position{ axis_origin.x + axis_space_ / 2 + i * ( x_axis_tip.x - axis_origin.x - axis_space_ ) / (number_of_tics - 1),  axis_origin.y };
			
			int x,y;
			fl_measure( number_string.c_str(), x, y );
			fl_draw(number_string.c_str(), tic_position.x - x/2, tic_position.y + 25 );
			fl_line( tic_position.x, tic_position.y + 8, tic_position.x, tic_position.y - 8 );
	

			
			const double y_axis_tic = image_start_.y + static_cast<double>( i ) * static_cast<double>( Fl_GrayscaleImage::GetOriginalImageSize().y - 1 ) / static_cast<double>( number_of_tics - 1 ) * ( pixel_size_.y );
		
			digits_befor_comma = 0;
			for( double divided_tic = y_axis_tic ; abs( divided_tic ) > 1.; divided_tic /= 10 )
				digits_befor_comma++;

			number_string = ToString( y_axis_tic, 3 - digits_befor_comma );
			
			tic_position.x = axis_origin.x;
			tic_position.y = axis_origin.y - axis_space_ / 2 - i * ( axis_origin.y - y_axis_tip.y - axis_space_ ) / (number_of_tics - 1);
			
			fl_measure( number_string.c_str(), x, y );
			fl_draw( 90, number_string.c_str(), tic_position.x - 25, tic_position.y + y/2 );
			fl_line( tic_position.x - 8, tic_position.y, tic_position.x + 8, tic_position.y);
		}



	};

	
	/*!
	 * @brief Get value for given x and y coordinate
	 * @param x Local x coordinate
	 * @param y Local y coordinate
	 * @return Raw and color value when x and y are inside image
	
	*/
	optional<pair<double, RGB>> GetValue( int x, int y ) const{ return Fl_GrayscaleImage::GetValue( x - axis_space_ / 2, y - axis_space_ / 2); };



	private:

	constexpr static int axis_space_ = 30;
	
	pair<string, string> axis_label_;
	Tuple2D image_start_;
	Tuple2D pixel_size_;

};