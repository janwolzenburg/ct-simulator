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

#include <FL/Fl.H>
#include <FL/Fl_Window.H>

#include "generelMath.h"
#include "image.h"



 /*********************************************************************
	Definitions
 *********************************************************************/

class relPosition{
	
	public:

	
	relPosition( double x_, double y_, double w_, double h_ ) :
		w( Fmax( Fmin( w_, 0. ), 1. ) ),
		x( Fmax( Fmin( x_, 0. ), 1. - w ) ),
		h( Fmax( Fmin( h_, 0. ), 1. ) ),
		y( Fmax( Fmin( y_, 0. ), 1. - h ) )
	{

	}

	// Getters
	inline double W( void ) const{ return w; };
	inline double H( void ) const{ return h; };
	inline double X( void ) const{ return x; };
	inline double Y( void ) const{ return y; };
	
	inline int W( const int parentW ) const{ return (int) ( (double) parentW * w ); };
	inline int H( const int parentH ) const{ return (int) ( (double) parentH * h ); };
	inline int X( const int parentX ) const{ return (int) ( (double) parentX * x ); };
	inline int Y( const int parentY ) const{ return (int) ( (double) parentY * y ); };

	private:

	double w, h;
	double x, y;

 };

class relPosition_Hor : public relPosition{

	public:

	relPosition_Hor( double y_, double w_, double h_ ) :
		relPosition{ ( 1. - w_ ) / 2., y_, w_, h_ }	
	{}
 };

class relPosition_Ver : public relPosition{

	public:

	relPosition_Ver( double x_, double w_, double h_ ) :
		relPosition{ x_, ( 1. - h_ ) / 2., w_, h_ }
	{}
};


class relPosition_VerHor : public relPosition{

	public:

	relPosition_VerHor(  double w_, double h_ ) :
		relPosition{ ( 1. - w_ ) / 2., ( 1. - h_ ) / 2., w_, h_ }
	{}
};



class window : private Fl_Window{

	public:
	
	window( int w, int h ) : 
		Fl_Window{ w, h }
	{
		Fl_Window::resizable( *this );
	}

	~window( void ){
		
		using WiPtr_It = vector<Fl_Widget*>::reverse_iterator;
		
		for( WiPtr_It it = widgets.rbegin(); it < widgets.rend(); it++ ){
			delete *it;
		}
	
		widgets.clear();

	}

	inline Fl_Window& getFl( void ){ return (Fl_Window&) *this; };

	template<class W>
	W& add( const relPosition position, Fl_Group& parent, const char* label ){

		static_assert( std::is_base_of_v< Fl_Widget, W > );

		int x = parent.x() + (int) ( (double) parent.w() * position.X() );
		int y = parent.y() + (int) ( (double) parent.h() * position.Y() );

		int w = (int) ( (double) parent.w() * position.W() );
		int h = (int) ( (double) parent.h() * position.H() );

		W* newWidget = new W{ x, y, w, h, label };
		widgets.push_back( newWidget );

		parent.add( newWidget );

		return *newWidget;
	}

	template<class W>
	W& add( const relPosition position, Fl_Group& parent ){
		return add<W>( position, parent, "" );
	};

	template<class W>
	W& add( const relPosition position, const char* label ){
		return add<W>( position, *this, label );
	};

	template<class W>
	W& add( const relPosition position ){
		return add<W>( position, *this );
	};

	private:

	vector<Fl_Widget*> widgets;

 };

class Fl_Image_Widget : public Fl_Widget{


	public:

	Fl_Image_Widget( int x, int y, int w, int h, const char* label = 0L );

	virtual void draw( void );

	virtual void resize( int x, int y, int w, int h );

	void assignImage( const greyImage& img );

	void calculateScaled( void );


	private:

	greyImage originalImage;
	greyImage scaledImage;
};
