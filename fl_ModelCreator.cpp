/*********************************************************************
 * @file   fl_ModelCreation.cpp
 * @brief  Implementations
 *
 * @author Jan Wolzenburg
 * @date   November 2023
 *********************************************************************/


  /*********************************************************************
	Includes
 *********************************************************************/

 #include "fl_ModelCreator.h"


 const std::map<Fl_ModelFeature::Shape, string> Fl_ModelFeature::shape_names{
	{ Sphere,	"Sphere" },
	{ Cube,		"Cube" } 
};


 Fl_ModelFeature::Shape Fl_ModelFeature::GetShapeEnum( const string shape_string ){

	for( auto& [matEnum, value] : Fl_ModelFeature::shape_names ){
		if( shape_string == value )
			return matEnum;
		}

	return Fl_ModelFeature::Shape::Sphere;
}