#pragma once
/*********************************************************************
 * @file   equationSystem.h
 * @brief  Classes for systems of equations
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/



 /*********************************************************************
	Includes
 *********************************************************************/
#include "generel.h"
#include "generelMath.h"



/*********************************************************************
   Definitions
*********************************************************************/

/*!
 * @brief Class for a matrix
*/
class matx : public mathObj{

	public:

	/*!
	 * @brief Constructor
	 * @param n_ Amount of rows
	 * @param m_ Amount of columns
	*/
	matx( const size_t n_, const size_t m_ );

	/*!
	 * @brief Element assignment
	 * @param row Row index
	 * @param col Column index
	 * @return Reference to matrix element
	*/
	double& operator() ( const size_t row, const size_t col );

	/*!
	 * @brief Element assignment
	 * @param idx Cell indices
	 * @return Reference to matrix element
	*/
	double& operator() ( const idx2 idx ){ return (*this)(idx.y, idx.x); };

	/*!
	 * @brief Element read
	 * @param row Row index
	 * @param col Column index
	 * @return Value of matrix element
	*/
	double operator() ( const size_t row, const size_t col ) const;
	/*!
	 * @brief Element read
	 * @param idx Cell indices
	 * @return Value of matrix element
	*/
	double operator() ( const idx2 idx ) const{ return (*this)(idx.y, idx.x); };

	/*!
	 * @brief Convert matrice's data to string
	 * @param newLineTabulators Amount of tabulators to insert after each line break
	 * @return String with line's data
	*/
	string toStr( const unsigned int newLineTabulators = 0 ) const override;

	/*!
	 * @brief Get amount of rows
	 * @return Amount of rows in matrix
	*/
	size_t getRows( void ) const{ return n; };

	/*!
	 * @brief Get amount of columns
	 * @return Amount of columns in matrix
	*/
	size_t getCols( void ) const{ return m; };

	/*!
	 * @brief Swap two columns
	 * @param c1 Column 1
	 * @param c2 Column 2
	 * @return Error code
	*/
	MATH_ERR swapCols( const size_t c1, const size_t c2 );

	/*!
	 * @brief Swap two rows
	 * @param r1 Row 1
	 * @param r2 Row 2
	 * @return Error code
	*/
	MATH_ERR swapRows( const size_t r1, const size_t r2 );

	/*!
	 * @brief Find indices of maximum absolute value in submatrix
	 * @param topCorner Top-left corner of submatrix
	 * @param botCorner Bottom-right corner of submatrix
	 * @return Cell indices of maximum
	*/
	idx2 findMax( const idx2 topCorner, const idx2 botCorner );

	/*!
	 * @brief Scale row by factor
	 * @param r Row to scale
	 * @param scalar Factor to scale by
	 * @return Error code
	*/
	MATH_ERR scaleRow( const size_t r, const double scalar );

	/*!
	 * @brief Substract rows with r2 = r2 - r1
	 * @param r1 Row to substract
	 * @param r2 Row to substract r1 from
	 * @return Error code
	*/
	MATH_ERR subRows( const size_t r1, const size_t r2 );


	
	private:

	size_t n;				/*!<Rows*/
	size_t m;				/*!<Columns*/
	vector<double> A;		/*!<Data-> Access with [m  * row + col] left to right - top to bottom*/
};



/*!
 * @brief Class for linear system of equations
 * @details Only systems with one equation per variable solvable. Inherited matrix contains coefficients
*/
class eqnSys : private matx{

	friend class eqnSysSolution;

	public:

	/*!
	 * @brief Constructor
	 * @param varNum_ Number of variables
	*/
	eqnSys( const size_t varNum_ );

	/*!
	 * @brief Convert system's data to string
	 * @return String with system's data
	*/
	string toStr( unsigned int newLineTabulators = 0 ) const override;

	/*!
	 * @brief Assignment operator deleted
	*/
	eqnSys& operator=( const eqnSys& sys ) = delete;

	/*!
	 * @brief Populate column of three variable equation system with vector
	 * @param v Vector to populate column with
	 * @return Error code
	*/
	MATH_ERR populateColumn( const v3 v );

	/*!
	 * @brief Add column to system of equation with two variables
	 * @param v Column to add
	 * @return Error code
	*/
	MATH_ERR populateColumn( const v2 v );

	/*!
	 * @brief Check population status
	 * @return True when all columns are populated and system is ready to be solved
	*/
	bool isPopulated( void ) const{ return numPopulatedColumns == varNum + 1; };

	/*!
	 * @brief Solve the system
	 * @return Solution of linear system of equation
	*/
	class eqnSysSolution solve( void );


	private:

	const size_t varNum;			/*!<Number of variables*/
	size_t numPopulatedColumns;		/*!<Number of populated columns*/
};



/*!
 * @brief Class describing solution of linear system of equations
*/
class eqnSysSolution : public mathObj{

	public:

	/*!
	 * @brief Constructor
	 * @param sys System of equation for which this instance holds the solution
	*/
	eqnSysSolution( const eqnSys sys );

	/*!
	 * @brief Deleted default constructor
	*/
	eqnSysSolution( void ) = delete;

	/*!
	 * @brief Convert solutions's data to string
	 * @return String with solutions's data
	*/
	string toStr( unsigned int newLineTabulators = 0 ) const override;

	/*!
	 * @brief Get amount of variables
	 * @return Amount of variables
	*/
	size_t getVarNum( void ){ return varNum; };

	/*!
	 * @brief Get value of success variable
	 * @return Solution success
	*/
	bool Success( void ){ return success; };

	/*!
	 * @brief Set success value
	 * @param val Value to set to
	 * @return Set value
	*/
	bool Success( const bool val ){ return success = val; };

	/*!
	 * @brief Set variable's value
	 * @param idx Index of variable to set
	 * @param val Value to set to
	*/
	void setVar( const size_t idx, const double val );

	/*!
	 * @brief Get value of variable
	 * @param idx Index of variable
	 * @return Value
	*/
	double getVar( const size_t idx ) const;


	private:

	size_t varNum;				/*!<Amount of variables*/
	vector<double> vars;		/*!<Array of variables*/
	bool success;				/*!<System has solution*/
};