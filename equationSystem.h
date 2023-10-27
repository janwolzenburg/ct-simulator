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
class Matrix : public MathematicalObject{

	public:

	/*!
	 * @brief Constructor
	 * @param number_of_columns Amount of columns
	 * @param number_of_rows Amount of rows
	*/
	Matrix(  const size_t number_of_columns, const size_t number_of_rows );
	
	/*!
	 * @brief Convert matrice's data to string
	 * @param newline_tabulators Amount of tabulators to insert after each line break
	 * @return String with line's data
	*/
	string ToString( const unsigned int newline_tabulators = 0 ) const override;

	/*!
	 * @brief Element assignment
	 * @param column Column index
	 * @param row Row index
	 * @return Reference to matrix element
	*/
	double& operator() ( const size_t column, const size_t row );

	/*!
	 * @brief Element assignment
	 * @param index Cell indices
	 * @return Reference to matrix element
	*/
	double& operator() ( const GridIndex index ){ return (*this)( index.c, index.r ); };

	/*!
	 * @brief Element read
	 * @param column Column index
	 * @param row Row index
	 * @return Value of matrix element
	*/
	double operator() ( const size_t column, const size_t row ) const;
	/*!
	 * @brief Element read
	 * @param index Cell indices
	 * @return Value of matrix element
	*/
	double operator() ( const GridIndex index ) const{ return (*this)( index.c, index.r ); };

	/*!
	 * @brief Get amount of rows
	 * @return Amount of rows in matrix
	*/
	size_t number_of_rows( void ) const{ return number_of_rows_; };

	/*!
	 * @brief Get amount of columns
	 * @return Amount of columns in matrix
	*/
	size_t number_of_columns( void ) const{ return number_of_columns_; };

	/*!
	 * @brief Swap two columns
	 * @param column_1 Column 1
	 * @param column_2 Column 2
	 * @return Error code
	*/
	MathError SwapColumns( const size_t column_1, const size_t column_2 );

	/*!
	 * @brief Swap two rows
	 * @param row_1 Row 1
	 * @param row_2 Row 2
	 * @return Error code
	*/
	MathError SwapRows( const size_t row_1, const size_t row_2 );

	/*!
	 * @brief Find indices of maximum absolute value in submatrix
	 * @param topCorner Top-left corner of submatrix
	 * @param botCorner Bottom-right corner of submatrix
	 * @return Cell indices of maximum
	*/
	GridIndex FindMaximum( const GridIndex top_corner, const GridIndex bottom_corner );

	/*!
	 * @brief Scale row by factor
	 * @param row Row to scale
	 * @param scalar Factor to scale by
	 * @return Error code
	*/
	MathError ScaleRow( const size_t row, const double scalar );

	/*!
	 * @brief Substract rows with r2 = r2 - r1
	 * @param row_1 Row to substract
	 * @param row_2 Row to substract r1 from
	 * @return Error code
	*/
	MathError SubstractRows( const size_t row_1, const size_t row_2 );

	
	private:

	
	size_t number_of_columns_;	/*!<Columns*/
	size_t number_of_rows_;		/*!<Rows*/
	vector<double> data_;		/*!<Data-> Access with [index  * row + col] left to right - top to bottom*/
};



/*!
 * @brief Class for linear system of equations
 * @details Only systems with one equation per variable solvable. Inherited matrix contains coefficients
*/
class eqnSys : private Matrix{

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
	string ToString( unsigned int newline_tabulators = 0 ) const override;

	/*!
	 * @brief Assignment operator deleted
	*/
	eqnSys& operator=( const eqnSys& sys ) = delete;

	/*!
	 * @brief Populate column of three variable equation system with vector
	 * @param v Vector to populate column with
	 * @return Error code
	*/
	MathError populateColumn( const Tuple3D v );

	/*!
	 * @brief Add column to system of equation with two variables
	 * @param v Column to add
	 * @return Error code
	*/
	MathError populateColumn( const Tuple2D v );

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
class eqnSysSolution : public MathematicalObject{

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
	string ToString( unsigned int newline_tabulators = 0 ) const override;

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