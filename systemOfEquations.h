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
	 * @brief Convert matrice's data_ to string
	 * @param newline_tabulators Amount of tabulators to insert after each Line break
	 * @return String with Line's data_
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
	 * @brief Substract rows with direction_2_ = direction_2_ - direction_1_
	 * @param row_1 Row to substract
	 * @param row_2 Row to substract direction_1_ from
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
 * @details Only systems with one equation per variable solvable. Inherited matrix contains coefficients. Only implemented for system with two or three variables
*/
class SystemOfEquations : private Matrix{

	friend class SystemOfEquationsSolution;

	public:

	/*!
	 * @brief Constructor
	 * @param number_of_variables Number of variables
	*/
	SystemOfEquations( const size_t number_of_variables );

	/*!
	 * @brief Convert system's data_ to string
	 * @return String with system's data_
	*/
	string ToString( unsigned int newline_tabulators = 0 ) const override;

	/*!
	 * @brief Populate column of three variable equation system with vector
	 * @param coefficients Vector to populate column with
	 * @return Error code
	*/
	MathError PopulateColumn( const Tuple3D coefficients );

	/*!
	 * @brief Add column to system of equation with two variables
	 * @param coefficients Column to add
	 * @return Error code
	*/
	MathError PopulateColumn( const Tuple2D coefficients );

	/*!
	 * @brief Check population status
	 * @return True when all columns are populated and system is ready to be solved
	*/
	bool IsPopulated( void ) const{ return currently_populated_columns_ == number_of_variables_ + 1; };

	/*!
	 * @brief Solve the system
	 * @return Solution of linear system of equation
	*/
	class SystemOfEquationsSolution Solve( void );


	private:

	const size_t number_of_variables_;			/*!<Number of variables*/
	size_t currently_populated_columns_;		/*!<Number of populated columns*/
};



/*!
 * @brief Class describing solution of linear system of equations
*/
class SystemOfEquationsSolution : public MathematicalObject{

	public:

	/*!
	 * @brief Constructor
	 * @param sys System of equation for which this instance holds the solution
	*/
	SystemOfEquationsSolution( const size_t number_of_variiables );

	/*!
	 * @brief Deleted default constructor
	*/
	SystemOfEquationsSolution( void ) = delete;

	/*!
	 * @brief Convert solutions's data_ to string
	 * @return String with solutions's data_
	*/
	string ToString( unsigned int newline_tabulators = 0 ) const override;

	/*!
	 * @brief Get amount of variables
	 * @return Amount of variables
	*/
	size_t number_of_variables( void ){ return number_of_variables_; };

	/*!
	 * @brief Get value of solution_found_ variable
	 * @return Solution solution_found_
	*/
	bool solution_found( void ){ return solution_found_; };

	/*!
	 * @brief Set solution_found_ value
	 * @param value Value to set to
	 * @return Set value
	*/
	bool solution_found( const bool value ){ return solution_found_ = value; };

	/*!
	 * @brief Set variable's value
	 * @param index Index of variable to set
	 * @param value Value to set to
	*/
	void SetVariableValue( const size_t index, const double value );

	/*!
	 * @brief Get value of variable
	 * @param index Index of variable
	 * @return Value
	*/
	double GetVariableValue( const size_t index ) const;


	private:

	size_t number_of_variables_;				/*!<Amount of variables*/
	vector<double> variable_values_;		/*!<Array of variables*/
	bool solution_found_;				/*!<System has solution*/
};