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
void addSingleObject<C>( ofstream& axis, const string name, const C object ){

	axis << name;
	axis << ": " << getObjectString( object ) << '\n';

}

template<class C>
void addSingleObject<C>( ofstream& axis, const string name, const C object, const string voxel_data_ ){

	axis << name;
	axis << ": " << getObjectString( object ) << ' ' << voxel_data_ << '\n';

}


template<class C, typename T>
void addSingleObject<C, T>(ofstream& axis, const string name, const C object, const string voxel_data_, const T additional) {

	axis << name;
	axis << ": " << getObjectString(object, additional) << ' ' << voxel_data_ << '\n';

}