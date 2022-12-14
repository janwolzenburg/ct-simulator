/*********************************************************************
 * @file   main.cpp
 * @brief  Main file
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/
#include <iostream>
using std::cerr;  using std::endl;

 #include "tests_linear_algebra.h"



/*!
 * @brief Main function
 * @param  
 * @return 
*/
int main( void ){

	if( !test_linear_algebra() ) cerr << "Test for linear algebra failed!";

	return 0;
}
