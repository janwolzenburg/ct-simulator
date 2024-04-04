#pragma once
/*********************************************************************
 * @file   equationSystem.h
 * @brief  classes for systems of equations
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
 * @brief class for a matrix
*/
class Matrix : public MathematicalObject{

	public:

	/*!
	 * @brief constructor
	 * @param number_of_columns Amount of columns
	 * @param number_of_rows Amount of rows
	*/
	Matrix(  const size_t number_of_columns, const size_t number_of_rows );
	
	/*!
	 * @brief convert matrice's data to string
	 * @param newline_tabulators Amount of tabulators to insert after each Line break
	 * @return string with Line's data
	*/
	string ToString( const unsigned int newline_tabulators = 0 ) const override;

	/*!
	 * @brief element assignment
	 * @param column Column index
	 * @param row Row index
	 * @return Reference to matrix element
	*/
	double& operator() ( const size_t column, const size_t row );

	/*!
	 * @brief element assignment
	 * @param index Cell indices
	 * @return Reference to matrix element
	*/
	double& operator() ( const GridIndex index ){ return (*this)( index.c, index.r ); };

	/*!
	 * @brief element read
	 * @param column Column index
	 * @param row Row index
	 * @return value of matrix element
	*/
	double operator() ( const size_t column, const size_t row ) const;
	/*!
	 * @brief element read
	 * @param index Cell indices
	 * @return value of matrix element
	*/
	double operator() ( const GridIndex index ) const{ return (*this)( index.c, index.r ); };

	/*!
	 * @brief get amount of rows
	 * @return amount of rows in matrix
	*/
	size_t number_of_rows( void ) const{ return number_of_rows_; };

	/*!
	 * @brief get amount of columns
	 * @return amount of columns in matrix
	*/
	size_t number_of_columns( void ) const{ return number_of_columns_; };

	/*!
	 * @brief swap two columns
	 * @param column_1 Column 1
	 * @param column_2 Column 2
	 * @return Error code
	*/
	MathError SwapColumns( const size_t column_1, const size_t column_2 );

	/*!
	 * @brief swap two rows
	 * @param row_1 Row 1
	 * @param row_2 Row 2
	 * @return Error code
	*/
	MathError SwapRows( const size_t row_1, const size_t row_2 );

	/*!
	 * @brief find indices of maximum absolute value in submatrix
	 * @param topCorner Top-left corner of submatrix
	 * @param botCorner Bottom-right corner of submatrix
	 * @return Cell indices of maximum
	*/
	GridIndex FindMaximum( const GridIndex top_corner, const GridIndex bottom_corner );

	/*!
	 * @brief scale row by factor
	 * @param row Row to scale
	 * @param scalar Factor to scale by
	 * @return Error code
	*/
	MathError ScaleRow( const size_t row, const double scalar );

	/*!
	 * @brief substract rows with direction_2_ = direction_2_ - direction_1_
	 * @param row_1 Row to substract
	 * @param row_2 Row to substract direction_1_ from
	 * @return Error code
	*/
	MathError SubstractRows( const size_t row_1, const size_t row_2 );

	
	protected:

	
	size_t number_of_columns_;	/*!< columns*/
	size_t number_of_rows_;		/*!< rows*/
	vector<double> data_;		/*!< data-> Access with [index  * row + col] left to right - top to bottom*/
};



/*!
 * @brief class for linear system of equations
 * @details Only systems with one equation per variable solvable. Inherited matrix contains coefficients. Only implemented for system with two or three variables
*/
class SystemOfEquations : private Matrix{

	friend class SystemOfEquationsSolution;

	public:

	/*!
	 * @brief constructor
	 * @param number_of_variables Number of variables
	*/
	SystemOfEquations( const size_t number_of_variables );

	/*!
	 * @brief convert system's data to string
	 * @return string with system's data
	*/
	string ToString( unsigned int newline_tabulators = 0 ) const override;

	/*!
	 * @brief Populate column of three variable equation system with vector
	 * @param coefficients Vector to populate column with
	 * @return Error code
	*/
	MathError PopulateColumn( const Tuple3D coefficients );

	/*!
	 * @brief add column to system of equation with two variables
	 * @param coefficients Column to add
	 * @return Error code
	*/
	MathError PopulateColumn( const Tuple2D coefficients );

	/*!
	 * @brief check population status
	 * @return true when all columns are populated and system is ready to be solved
	*/
	bool IsPopulated( void ) const{ return currently_populated_columns_ == number_of_variables_ + 1; };

	/*!
	 * @brief solve the system
	 * @return solution of linear system of equation
	*/
	class SystemOfEquationsSolution Solve( void );


	private:

	const size_t number_of_variables_;			/*!< Number of variables*/
	size_t currently_populated_columns_;		/*!< Number of populated columns*/
};



/*!
 * @brief class describing solution of linear system of equations
*/
class SystemOfEquationsSolution : public MathematicalObject{

	public:

	/*!
	 * @brief constructor
	 * @param sys System of equation for which this instance holds the solution
	*/
	SystemOfEquationsSolution( const size_t number_of_variiables );

	/*!
	 * @brief deleted default constructor
	*/
	SystemOfEquationsSolution( void ) = delete;

	/*!
	 * @brief convert solutions's data to string
	 * @return string with solutions's data
	*/
	string ToString( unsigned int newline_tabulators = 0 ) const override;

	/*!
	 * @brief get amount of variables
	 * @return amount of variables
	*/
	size_t number_of_variables( void ){ return number_of_variables_; };

	/*!
	 * @brief get value of solution_found_ variable
	 * @return solution solution_found_
	*/
	bool solution_found( void ){ return solution_found_; };

	/*!
	 * @brief set solution_found_ value
	 * @param value Value to set to
	 * @return set value
	*/
	bool solution_found( const bool value ){ return solution_found_ = value; };

	/*!
	 * @brief set variable's value
	 * @param index Index of variable to set
	 * @param value Value to set to
	*/
	void SetVariableValue( const size_t index, const double value );

	/*!
	 * @brief get value of variable
	 * @param index Index of variable
	 * @return value
	*/
	double GetVariableValue( const size_t index ) const;


	private:

	size_t number_of_variables_;				/*!< amount of variables*/
	vector<double> variable_values_;		/*!< array of variables*/
	bool solution_found_;				/*!< system has solution*/
};