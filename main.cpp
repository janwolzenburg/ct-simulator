/*********************************************************************
 * @file   main.cpp
 * @brief  Main file
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/
#include <iostream>
using std::cerr;  using std::endl;

 #include "test_all.h"



/*!
 * @brief Main function
 * @param  
 * @return 
*/
int main( void ){

	if( !test_linear_algebra() ) cerr << "Test for linear algebra failed!";

	if( !test_tube() ) cerr << "Test for xRay tube failed!";

	if (!test_detector()) cerr << "Test for detector failed!";

	return 0;
}
