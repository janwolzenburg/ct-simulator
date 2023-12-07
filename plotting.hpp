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
#include "ray.h"



  /*********************************************************************
	 implementations
  *********************************************************************/

template<class C>
void addSingleObject<C>( ofstream& axis, const string name, const C object ){

	axis << ReplaceSpace(name);
	axis << ": " << getObjectString( object ) << '\n';

}

template<class C>
void addSingleObject<C>( ofstream& axis, const string name, const C object, const string data ){

	axis << ReplaceSpace(name);
	axis << ": " << getObjectString( object ) << ' ' << data << '\n';

}


template<class C, typename T>
void addSingleObject<C, T>(ofstream& axis, const string name, const C object, const string data, const T additional) {

	axis << ReplaceSpace(name);
	axis << ": " << getObjectString<C, T>(object, additional) << ' ' << data << '\n';

}