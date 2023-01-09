/*********************************************************************
 * @file   plotting.hpp
 * @brief  Template implementations
 *
 * @author Jan Wolzenburg
 * @date   January 2023
 *********************************************************************/


 /*********************************************************************
	 Includes
  *********************************************************************/

#include "plotting.h"
#include "rays.h"



  /*********************************************************************
	 implementations
  *********************************************************************/


template<class C>
void addSingleObject<C>( ofstream& axis, const string name, const C object, const string parameter ){

	axis << name;
	axis << ": " << getObjectString( object ) << ' ' << parameter << '\n';

}

template<class C, typename T>
void addSingleObject<C, T>(ofstream& axis, const string name, const C object, const string parameter, const T additional) {

	axis << name;
	axis << ": " << getObjectString(object, additional) << ' ' << parameter << '\n';

}