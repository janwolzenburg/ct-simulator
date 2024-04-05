#pragma once
/*********************************************************************
 * @file   systemOfEquations.h
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
	 * @param number_of_columns amount of columns
	 * @param number_of_rows amount of rows
	*/
	Matrix(  const size_t number_of_columns, const size_t number_of_rows );
	
	/*!
	 * @brief convert matrice's data to string
	 * @param newline_tabulators amount of tabulators to insert after each line break
	 * @return string with Line's data
	*/
	string ConvertToString( const unsigned int newline_tabulators = 0 ) const override;

	/*!
	 * @brief element assignment
	 * @param column column index
	 * @param row row index
	 * @return reference to matrix element
	*/
	double& operator() ( const size_t column, const size_t row );

	/*!
	 * @brief element assignment
	 * @param index cell indices
	 * @return reference to matrix element
	*/
	double& operator() ( const GridIndex index ){ return (*this)( index.c, index.r ); };

	/*!
	 * @brief element read
	 * @param column column index
	 * @param row row index
	 * @return value of matrix element
	*/
	double operator() ( const size_t column, const size_t row ) const;
	/*!
	 * @brief element read
	 * @param index cell indices
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
	 * @param column_1 column 1
	 * @param column_2 column 2
	 * @return error code
	*/
	MathError SwapColumns( const size_t column_1, const size_t column_2 );

	/*!
	 * @brief swap two rows
	 * @param row_1 row 1
	 * @param row_2 row 2
	 * @return error code
	*/
	MathError SwapRows( const size_t row_1, const size_t row_2 );

	/*!
	 * @brief find indices of maximum absolute value in submatrix
	 * @param top_corner Top-left corner of submatrix
	 * @param bottom_corner Bottom-right corner of submatrix
	 * @return cell indices of maximum
	*/
	GridIndex FindMaximum( const GridIndex top_corner, const GridIndex bottom_corner );

	/*!
	 * @brief scale row by factor
	 * @param row row to scale
	 * @param scalar factor to scale by
	 * @return error code
	*/
	MathError ScaleRow( const size_t row, const double scalar );

	/*!
	 * @brief substract rows with direction_2_ = direction_2_ - direction_1_
	 * @param row_1 row to substract
	 * @param row_2 row to substract direction_1_ from
	 * @return error code
	*/
	MathError SubstractRows( const size_t row_1, const size_t row_2 );

	
	protected:

	
	size_t number_of_columns_;	/*!< number of columns*/
	size_t number_of_rows_;			/*!< number of rows*/
	vector<double> data_;				/*!< data-> Access with [index  * row + col] left to right - top to bottom*/
};



/*!
 * @brief class for linear system of equations
 * @details only systems with one equation per variable solvable. inherited matrix contains coefficients. only implemented for system with two or three variables
*/
class SystemOfEquations : private Matrix{

	friend class SystemOfEquationsSolution;

	public:

	/*!
	 * @brief constructor
	 * @param number_of_variables number of variables
	*/
	SystemOfEquations( const size_t number_of_variables );

	/*!
	 * @brief convert system's data to string
	 * @return string with system's data
	*/
	string ConvertToString( unsigned int newline_tabulators = 0 ) const override;

	/*!
	 * @brief populate column of three variable equation system with vector
	 * @param coefficients vector to populate column with
	 * @return error code
	*/
	MathError PopulateColumn( const Tuple3D coefficients );

	/*!
	 * @brief add column to system of equation with two variables
	 * @param coefficients column to add
	 * @return error code
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

	const size_t number_of_variables_;			/*!< number of variables*/
	size_t currently_populated_columns_;		/*!< number of populated columns*/
};



/*!
 * @brief class describing solution of linear system of equations
*/
class SystemOfEquationsSolution : public MathematicalObject{

	public:

	/*!
	 * @brief constructor
	 * @param number_of_variables number of variables
	*/
	SystemOfEquationsSolution( const size_t number_of_variables );

	/*!
	 * @brief deleted default constructor
	*/
	SystemOfEquationsSolution( void ) = delete;

	/*!
	 * @brief convert solutions's data to string
	 * @return string with solutions's data
	*/
	string ConvertToString( unsigned int newline_tabulators = 0 ) const override;

	/*!
	 * @brief get amount of variables
	 * @return amount of variables
	*/
	size_t number_of_variables( void ) const{ return number_of_variables_; };

	/*!
	 * @brief get solution_found_
	 * @return whether solution was found
	*/
	bool solution_found( void ) const{ return solution_found_; };

	/*!
	 * @brief set solution_found
	 * @param value value to set to
	 * @return the set value
	*/
	bool solution_found( const bool value ){ return solution_found_ = value; };

	/*!
	 * @brief set variable's value
	 * @param index index of variable to set
	 * @param value value to set to
	*/
	void SetVariableValue( const size_t index, const double value );

	/*!
	 * @brief get value of variable
	 * @param index index of variable
	 * @return value
	*/
	double GetVariableValue( const size_t index ) const;


	private:

	size_t number_of_variables_;				/*!< amount of variables*/
	vector<double> variable_values_;		/*!< array of variables*/
	bool solution_found_;								/*!< system has solution*/
};