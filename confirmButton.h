#pragma once

#include "generel.h"

#include "FL/Fl_Group.h"
#include "FL/Fl_Button.h"
#include "FL/Fl_Multiline_Output.h"
#include "FL/Fl_Window.h"

#include "widgets.h"


class Fl_Confirm_Button : public Fl_Button{

	public:
	Fl_Confirm_Button( int x, int y, int w, int h, const char* label ) :
		Fl_Button{ x, y, w, h, label },
		confirmationWindow{ 400, 150, "Confirmation"},
		confirmationText{ X( confirmationWindow, 0. ), Y( confirmationWindow, 0. ), W( confirmationWindow, 1. ), H( confirmationWindow, .75 ), "Confirm your decision"},
		yesButton{ X( confirmationWindow, 0.1 ), Y( confirmationWindow, .775 ), W( confirmationWindow, .3 ), H( confirmationWindow, .2 ), "Yes" },
		noButton{  X( confirmationWindow, 0.6 ), Y( confirmationWindow, .775 ), W( confirmationWindow, .3 ), H( confirmationWindow, .2 ), "No" },
		confirmationString{},
		buttonPressed( false ),
		yesPressed( false ),
		noPressed( false ),
		confirmationFlag( false )
	{

		//confirmationWindow.
		//Fl_Button::window()->add( confirmationWindow );

		confirmationWindow.add( confirmationText );
		confirmationWindow.add( yesButton );
		confirmationWindow.add( noButton );

		yesButton.labelsize( 20 );
		noButton.labelsize( 20 );

		confirmationWindow.hide();

		Fl_Button::callback( button_cb, &buttonPressed );
		yesButton.callback( button_cb, &yesPressed );
		noButton.callback( button_cb, &noPressed );
	};

	void setTitle( const string title ){
		confirmationWindow.copy_label( title.c_str() );
	}

	void setWindowText( string text ){
		
		confirmationText.value( text.c_str() );
	}

	void setButtonTexts( const string yesText, const string noText ){
		yesButton.copy_label( yesText.c_str() );
		noButton.copy_label( noText.c_str() );
	}


	void handleEvents( void ){

		if( unsetFlag( buttonPressed ) ){
			confirmationWindow.show();
		}

		if( unsetFlag( yesPressed ) ){
			confirmationFlag = true;
			confirmationWindow.hide();
		}

		if( unsetFlag( noPressed ) ){
			confirmationWindow.hide();
		}

	}

	bool buttonPressConfirmed( void ){ return unsetFlag( confirmationFlag ); };

	private:

	Fl_Window confirmationWindow;
	Fl_Multiline_Output confirmationText;
	Fl_Button yesButton;
	Fl_Button noButton;

	string confirmationString;

	bool buttonPressed;
	bool yesPressed;
	bool noPressed;
	bool confirmationFlag;
};