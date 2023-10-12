#pragma once

#include "generel.h"

#include "FL/Fl_Group.h"
#include "FL/Fl_Button.h"
#include "FL/Fl_Output.h"
#include "FL/Fl_Window.h"

#include "widgets.h"


class Fl_Confirm_Button : public Fl_Button{

	public:
	Fl_Confirm_Button( int x, int y, int w, int h, const char* label ) :
		Fl_Button{ x, y, w, h, label },
		confirmationWindow{ 400, 270, "Confirmation" },
		confirmationText{ X( confirmationWindow, 0. ), Y( confirmationWindow, 0. ), W( confirmationWindow, 1. ), H( confirmationWindow, .75 ), "Confirm your decision"},
		yesButton{ X( confirmationWindow, 0.1 ), Y( confirmationWindow, .8 ), W( confirmationWindow, .3 ), H( confirmationWindow, .2 ), "Yes" },
		noButton{  X( confirmationWindow, 0.6 ), Y( confirmationWindow, .8 ), W( confirmationWindow, .3 ), H( confirmationWindow, .2 ), "No" },
		confirmationString{},
		flag( nullptr )

	{
		confirmationWindow.hide();

		confirmationWindow.add( confirmationText );
		confirmationWindow.add( yesButton );
		confirmationWindow.add( noButton );


	};

	

	private:

	Fl_Window confirmationWindow;
	Fl_Output confirmationText;
	Fl_Button yesButton;
	Fl_Button noButton;

	string confirmationString;

	bool* flag;

};