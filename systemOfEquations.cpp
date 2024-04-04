#pragma once
/*********************************************************************
 * @file   equationSystem.cpp
 * @brief  Implementations
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/



 /*********************************************************************
	Includes
 *********************************************************************/
 #include <cmath>

 #include "generelMath.h"
 #include "systemOfEquations.h"



 /*********************************************************************
	Implementations
 *********************************************************************/


/*
	matx implementation
*/

Matrix::Matrix( const size_t n_, const size_t m_ ) :
	number_of_columns_( n_ ),
	number_of_rows_( m_ ),
	data_( number_of_rows_ * number_of_columns_, 0. ) {
	if (number_of_rows_ == 0) { CheckForAndOutputError( MathError::Input, "matrix rows amount must be greater than zero!" ); number_of_rows_ = 1; }
	if (number_of_columns_ == 0) { CheckForAndOutputError( MathError::Input, "matrix columns amount must be greater than zero!" ); number_of_columns_ = 1; }
}

string Matrix::ConvertToString( [[maybe_unused]] const unsigned int newline_tabulators ) const{

	std::string str;
	char tempCharArr[ 256 ];

	for( unsigned int r = 0; r < number_of_rows_; r++ ){
		for( unsigned int c = 0; c < number_of_columns_; c++ ){
			snprintf( tempCharArr, 256, "%.6f,", ( *this )( r, c ) );
			str += tempCharArr;
			if( c == number_of_columns_ - 1 ) str += ";";
		}
	}

	return str;
}

double& Matrix::operator() ( const size_t col, const size_t row ) {
	if (row >= number_of_rows_) { CheckForAndOutputError( MathError::Input, "row exceeds matrix size!" ); return data_.at( number_of_columns_ * (number_of_rows_ - 1) + (number_of_columns_ - 1) ); }
	if (col >= number_of_columns_) { CheckForAndOutputError( MathError::Input, "column exceeds matrix size!" ); return data_.at( number_of_columns_ * (number_of_rows_ - 1) + (number_of_columns_ - 1) ); }

	return data_.at(number_of_columns_ * row + col);
}

double Matrix::operator() ( const size_t col, const size_t row ) const {
	if (row >= number_of_rows_) { CheckForAndOutputError( MathError::Input, "row exceeds matrix size!" ); return data_.at( number_of_columns_ * (number_of_rows_ - 1) + (number_of_columns_ - 1) ); }
	if (col >= number_of_columns_) { CheckForAndOutputError( MathError::Input, "column exceeds matrix size!" ); return data_.at( number_of_columns_ * (number_of_rows_ - 1) + (number_of_columns_ - 1) ); }
	return data_.at(number_of_columns_ * row + col);
}

MathematicalObject::MathError Matrix::SwapColumns( const size_t c1, const size_t c2 ) {
	if (c1 >= number_of_columns_ || c2 >= number_of_columns_) return CheckForAndOutputError( MathError::Bounds, "column or row indices exceed matrix size!" );

	double tempVal;

	// iterate each row
	for (size_t curR = 0; curR < number_of_rows_; curR++) {
		tempVal = data_.at( number_of_columns_ * curR + c1 );				// temporary storage
		data_.at( number_of_columns_ * curR + c1 ) = data_.at( number_of_columns_ * curR + c2 );		// swap values
		data_.at( number_of_columns_ * curR + c2 ) = tempVal;
	}

	return MathError::Ok;
}

MathematicalObject::MathError Matrix::SwapRows( const size_t r1, const size_t r2 ) {
	if (r1 >= number_of_rows_ || r2 >= number_of_rows_) return CheckForAndOutputError( MathError::Bounds, "column or row indices exceed matrix size!" );

	double tempVal;

	// iterate each column
	for (size_t curC = 0; curC < number_of_columns_; curC++) {
		tempVal = data_.at( number_of_columns_ * r1 + curC );					// temporary storage
		data_.at( number_of_columns_ * r1 + curC ) = data_.at( number_of_columns_ * r2 + curC ); // swap values
		data_.at( number_of_columns_ * r2 + curC ) = tempVal;
	}

	return MathError::Ok;
}

GridIndex Matrix::FindMaximum( const GridIndex topCorner, const GridIndex botCorner ) {
	double curMax = 0, curVal;
	GridIndex curMaxIndx = topCorner;


	if (topCorner.r >= number_of_rows_ || topCorner.c >= number_of_columns_ || botCorner.r >= number_of_rows_ || botCorner.c >= number_of_columns_) {
		CheckForAndOutputError( MathError::Bounds, "top of bottom corner of submatrix is outside matrix!" );
		return curMaxIndx;
	}

	// iterate elements in submatrix
	for (size_t curR = topCorner.r; curR <= botCorner.r; curR++) {
		for (size_t curC = topCorner.c; curC <= botCorner.c; curC++) {
			// read cell
			curVal = abs( data_.at( number_of_columns_ * curR + curC ) );

			// absolute value in cell greater?
			if (curVal > curMax) {
				curMax = curVal;
				curMaxIndx.r = curR;	// store indices
				curMaxIndx.c = curC;
			}
		}
	}

	return curMaxIndx;
}

MathematicalObject::MathError Matrix::ScaleRow( const size_t r, const double scalar ) {
	if (r >= number_of_rows_) return CheckForAndOutputError( MathError::Bounds, "row index exceeds matrix size!" );
	// iterate all columns
	for (unsigned int c = 0; c < number_of_columns_; c++) {
		data_.at( number_of_columns_ * r + c ) = data_.at( number_of_columns_ * r + c ) * scalar;
	}

	return MathError::Ok;
}

MathematicalObject::MathError Matrix::SubstractRows( const size_t r1, const size_t r2 ) {
	if (r1 >= number_of_rows_ || r2 >= number_of_rows_) return CheckForAndOutputError( MathError::Bounds, "row index exceeds matrix size!" );

	// iterate all columns
	for (size_t c = 0; c < number_of_columns_; c++) {
		data_.at( number_of_columns_ * r2 + c ) = data_.at( number_of_columns_ * r2 + c ) - data_.at( number_of_columns_ * r1 + c );
	}

	return MathError::Ok;
}



/*
	SystemOfEquations implementation
*/

SystemOfEquations::SystemOfEquations( const size_t varNum_ )
	: Matrix ( varNum_ + 1, varNum_ ),
	number_of_variables_( varNum_ ),
	currently_populated_columns_( 0 ){
	if( number_of_variables_ == 0 ) CheckForAndOutputError( MathError::Input, "number of variables must be greater than 0!" );
};

std::string SystemOfEquations::ConvertToString( const unsigned int newline_tabulators ) const{

	std::string str;
	std::string newLine = { '\n' };

	for( unsigned int i = 0; i < newline_tabulators; i++ ) newLine += '\t';


	str += "varNum=" + std::to_string( number_of_variables_ );
	str += newLine + "popCols=" + std::to_string( currently_populated_columns_ );

	str += newLine + Matrix::ConvertToString();

	return str;

}

MathematicalObject::MathError SystemOfEquations::PopulateColumn( const Tuple3D v ){

	// for now only implemented with three variable_values_
	if( IsPopulated() || number_of_variables_ != 3 ) return CheckForAndOutputError( MathError::Input, "system already populated or variables amount not equal to three!" );

	// assign vector values to matrix columns
	( *this )( currently_populated_columns_, 0 ) = v.x;
	( *this )( currently_populated_columns_, 1 ) = v.y;
	( *this )( currently_populated_columns_, 2 ) = v.z;
	currently_populated_columns_++;

	return MathError::Ok;
}

MathematicalObject::MathError SystemOfEquations::PopulateColumn( const Tuple2D v ){

	// for now only implemented with three variable_values_
	if( IsPopulated() || number_of_variables_ != 2 ) return CheckForAndOutputError( MathError::Input, "system already populated or variables amount not equal to two!" );

	// assign vector values to matrix columns
	( *this )( currently_populated_columns_, 0 ) = v.x;
	( *this )( currently_populated_columns_, 1 ) = v.y;
	currently_populated_columns_++;

	return MathError::Ok;
}

SystemOfEquationsSolution SystemOfEquations::Solve( void ){

	SystemOfEquationsSolution solution( number_of_variables_ );	// create solution instance

	// system filled with coefficients?
	if( !IsPopulated() ) return solution;

	// get size of coefficient matrix 
	const size_t rows = number_of_rows(); const size_t columns = number_of_columns();

	GridIndex top_corner{};													// top-left corner of submatrix
	GridIndex bottom_corner{ rows - 1, rows - 1 };	// bottom-right corner of submatrix
	GridIndex index_of_maximum;											// indices of maximum in submatrix
	double maximum_value;														// value of maximum in submatrix

	// matrix with variable indices
	vector<unsigned int> variable_indices( number_of_variables_ );
	for( unsigned int i = 0; i < number_of_variables_; i++ ) 
		variable_indices.at( i ) = i;

	// iterate all rows to create echelon form of matrix
	for( size_t k = 0; k < rows; k++ ){

		// top-left corner changes each iteration
		top_corner.r = k; top_corner.c = k;

		// search maximum absolutionute value in quadratic submatrix
		// from (k, k) to (rows - 1, rows - 1)
		index_of_maximum = FindMaximum( top_corner, bottom_corner );

		// get value
		maximum_value = data_.at(number_of_columns_ * index_of_maximum.r + index_of_maximum.c);
		
		// return if maximum is zero -> no solutionution
		if( IsNearlyEqualDistance( maximum_value, 0. ) ) return solution;

		// swap rows and columns to bring the cell with maximum value to (k,k)
		if( index_of_maximum.r != k ) SwapRows( index_of_maximum.r, k );
		if( index_of_maximum.c != k ){
			SwapColumns( index_of_maximum.c, k );

			// variable in columns swapped
			unsigned int temp = variable_indices.at( k );
			variable_indices.at( k ) = variable_indices.at( index_of_maximum.c );
			variable_indices.at( index_of_maximum.c ) = temp;
		}

		ScaleRow( k, 1 / maximum_value );			// scale k-row by reciprocal of (k, k) value

		// substract k-row from k+1 to n-row to eliminate cells
		for( size_t row = k + 1; row < rows; row++ ){
			const double cell_value = data_.at( number_of_columns_ * row + k );
			// target cell not zero?
			if( !IsNearlyEqualDistance( cell_value, 0. ) ){
				// make (row, k) to one by scaling row with reciprocal
				ScaleRow( row, 1 / cell_value );		
				SubstractRows( k, row );						// substract k-row from row
			}
		}
	}

	// eliminate remaining cells 'above' the diagonal
	// iterate  coefficient columns 
	for( size_t c = columns - 2; c > 0; c-- ){
		
		// iterate rows above diagonal
		for( size_t r = 0; r < c; r++ ){
			const double cell_value = data_.at(number_of_columns_ * r + c );
			// current cell not already zero?
			if( !IsNearlyEqualDistance( cell_value, 0 ) ){
				// calculate result column cell as if current cell is being eliminated 
				double& result_column_cell = data_.at( number_of_columns_ * r + ( columns - 1 ) );
				const double current_cell = data_.at( number_of_columns_ * c + ( columns - 1 ) );
				result_column_cell -= ( current_cell * cell_value );
			}
		}
	}

	size_t variable_index;
	// copy result column entries to var-array
	for( size_t i = 0; i < rows; i++ ){
		variable_index = variable_indices.at( i );		// index for swapped variable
		// check if index is in allowed range
		if( variable_index < solution.number_of_variables() ) 
			solution.SetVariableValue( variable_index, 
																 data_.at(number_of_columns_ * i + columns - 1 ) );
		else 
			return solution;
	}
	solution.solution_found( true );
	return solution;
}



/*
	SystemOfEquationsSolution implementation
*/

SystemOfEquationsSolution::SystemOfEquationsSolution( const size_t number_of_variiables )
	: number_of_variables_( number_of_variiables ),
	variable_values_( number_of_variables_, 0. ),
	solution_found_( false ){};


string SystemOfEquationsSolution::ConvertToString( const unsigned int newline_tabulators ) const{
	string str;
	string newLine = { '\n' };

	for( unsigned int i = 0; i < newline_tabulators; i++ ) newLine += '\t';

	str += "varNum=" + number_of_variables_;
	str += newLine + "success=" + std::to_string( solution_found_ );
	str += newLine + "vars=";

	for( unsigned int i = 0; i < number_of_variables_; i++ ){
		str += std::to_string( variable_values_[ i ] );
		if( i < number_of_variables_ - 1 ) str += ", ";
	}

	return str;

}

void SystemOfEquationsSolution::SetVariableValue( const size_t idx, const double val ){
	if( idx >= number_of_variables_ ) CheckForAndOutputError( MathError::Input, "index exceeds amount of variables!" );

	variable_values_[ idx ] = val;
}

double SystemOfEquationsSolution::GetVariableValue( const size_t idx ) const{
	if( idx >= number_of_variables_ ) CheckForAndOutputError( MathError::Input, "index exceeds amount of variables!" );
	return variable_values_[ idx ];
}