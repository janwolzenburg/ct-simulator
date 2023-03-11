#pragma once
/*********************************************************************
 * @file   programState.h
 * @brief  
 *
 * @author Jan Wolzenburg
 * @date   March 2023
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/

 #include <string>
 using std::string;

 #include "model.h"
 #include "fileChooser.h"


 /*********************************************************************
	Definitions
 *********************************************************************/


class programmState{

	void loadModel( void ){

		string fileFilter{ "*.ctMod" };

	};

	public:

	model currentModel;



 };
