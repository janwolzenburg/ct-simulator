#pragma once
/*********************************************************************
 * @file   tube.h
 * @brief  Class for roentgen tube
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/


 #include <vector>
 using std::vector;


 #include "rays.h"
 

 constexpr double c_mPerS = 299792456; 
 constexpr double e_ATimS = 1.6021917;
 constexpr double h_JTimS = 6.626196E-34; 


 class pnt2{
	 public:
	 double x;
	 double y;
};
 

class xRay_spectrum{

	public:
	xRay_spectrum( const double anodeVoltage ) :
		voltage( anodeVoltage ),
		lambda_min( c_mPerS * h_JTimS / ( e_ATimS* voltage ) )
	{}

	public:

	double voltage;
	double lambda_min;

	vector<pnt2> data;


 };


class tube{


	private:


 };