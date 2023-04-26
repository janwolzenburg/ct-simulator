#pragma once
/*********************************************************************
 * @file   window.h
 * @brief
 *
 * @author Jan Wolzenburg
 * @date   March 2023
 *********************************************************************/


 /*********************************************************************
	Includes
 *********************************************************************/



/*********************************************************************
   Definitions
*********************************************************************/


/*!
 * @brief Class for a raltive position inside a parent
*/
class relPosition{

	public:

	relPosition( double x_, double y_, double w_, double h_ );

	inline double W( void ) const{ return w; };
	inline double H( void ) const{ return h; };
	inline double X( void ) const{ return x; };
	inline double Y( void ) const{ return y; };


	private:

	double w, h;
	double x, y;

};


/*!
 * @brief Class for a relative position centered horizontally in parent
*/
class relPosition_Hor : public relPosition{

	public:

	relPosition_Hor( double y_, double w_, double h_ );

};


/*!
 * @brief Class for a relative position centered vertically
*/
class relPosition_Ver : public relPosition{

	public:

	relPosition_Ver( double x_, double w_, double h_ );

};


/*!
 * @brief Class for a relative position centered horizontally and vertically
*/
class relPosition_VerHor : public relPosition{

	public:

	relPosition_VerHor( double w_, double h_ );

};