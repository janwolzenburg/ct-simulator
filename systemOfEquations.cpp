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
	if (number_of_rows_ == 0) { CheckForAndOutputError( MathError::Input, "Matrix rows amount must be greater than zero!" ); number_of_rows_ = 1; }
	if (number_of_columns_ == 0) { CheckForAndOutputError( MathError::Input, "Matrix columns amount must be greater than zero!" ); number_of_columns_ = 1; }
}

string Matrix::ToString( [[maybe_unused]] const unsigned int newline_tabulators ) const{

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
	if (row >= number_of_rows_) { CheckForAndOutputError( MathError::Input, "Row exceeds matrix size!" ); return data_.at( number_of_columns_ * (number_of_rows_ - 1) + (number_of_columns_ - 1) ); }
	if (col >= number_of_columns_) { CheckForAndOutputError( MathError::Input, "Column exceeds matrix size!" ); return data_.at( number_of_columns_ * (number_of_rows_ - 1) + (number_of_columns_ - 1) ); }

	return data_.at(number_of_columns_ * row + col);
}

double Matrix::operator() ( const size_t col, const size_t row ) const {
	if (row >= number_of_rows_) { CheckForAndOutputError( MathError::Input, "Row exceeds matrix size!" ); return data_.at( number_of_columns_ * (number_of_rows_ - 1) + (number_of_columns_ - 1) ); }
	if (col >= number_of_columns_) { CheckForAndOutputError( MathError::Input, "Column exceeds matrix size!" ); return data_.at( number_of_columns_ * (number_of_rows_ - 1) + (number_of_columns_ - 1) ); }
	return data_.at(number_of_columns_ * row + col);
}

MathematicalObject::MathError Matrix::SwapColumns( const size_t c1, const size_t c2 ) {
	if (c1 >= number_of_columns_ || c2 >= number_of_columns_) return CheckForAndOutputError( MathError::Bounds, "Column or row indices exceed matrix size!" );

	double tempVal;

	// Iterate each row
	for (size_t curR = 0; curR < number_of_rows_; curR++) {
		tempVal = (*this)(c1, curR);				// Temporary storage
		(*this)(c1, curR) = (*this)(c2, curR);		// Swap values
		(*this)(c2, curR) = tempVal;
	}

	return MathError::Ok;
}

MathematicalObject::MathError Matrix::SwapRows( const size_t r1, const size_t r2 ) {
	if (r1 >= number_of_rows_ || r2 >= number_of_rows_) return CheckForAndOutputError( MathError::Bounds, "Column or row indices exceed matrix size!" );

	double tempVal;

	// Iterate each column
	for (size_t curC = 0; curC < number_of_columns_; curC++) {
		tempVal = (*this)(curC, r1);					// Temporary storage
		(*this)(curC, r1) = (*this)(curC, r2);		// Swap values
		(*this)(curC, r2) = tempVal;
	}

	return MathError::Ok;
}

GridIndex Matrix::FindMaximum( const GridIndex topCorner, const GridIndex botCorner ) {
	double curMax = 0, curVal;
	GridIndex curMaxIndx = topCorner;


	if (topCorner.r >= number_of_rows_ || topCorner.c >= number_of_columns_ || botCorner.r >= number_of_rows_ || botCorner.c >= number_of_columns_) {
		CheckForAndOutputError( MathError::Bounds, "Top of bottom corner of submatrix is outside matrix!" );
		return curMaxIndx;
	}

	// Iterate elements in submatrix
	for (size_t curR = topCorner.r; curR <= botCorner.r; curR++) {
		for (size_t curC = topCorner.c; curC <= botCorner.c; curC++) {
			// Read cell
			curVal = abs( (*this)(curC, curR) );

			// Absolute value in cell greater?
			if (curVal > curMax) {
				curMax = curVal;
				curMaxIndx.r = curR;	// Store indices
				curMaxIndx.c = curC;
			}
		}
	}

	return curMaxIndx;
}

MathematicalObject::MathError Matrix::ScaleRow( const size_t r, const double scalar ) {
	if (r >= number_of_rows_) return CheckForAndOutputError( MathError::Bounds, "Row index exceeds matrix size!" );
	// Iterate all columns
	for (unsigned int c = 0; c < number_of_columns_; c++) {
		(*this)(c, r) = (*this)(c, r) * scalar;		// Scale cell
	}

	return MathError::Ok;
}

MathematicalObject::MathError Matrix::SubstractRows( const size_t r1, const size_t r2 ) {
	if (r1 >= number_of_rows_ || r2 >= number_of_rows_) return CheckForAndOutputError( MathError::Bounds, "Row index exceeds matrix size!" );

	// Iterate all columns
	for (size_t c = 0; c < number_of_columns_; c++) {
		(*this)(c, r2) = (*this)(c, r2) - (*this)(c, r1);		// Substract cell
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
	if( number_of_variables_ == 0 ) CheckForAndOutputError( MathError::Input, "Number of variables must be greater than 0!" );
};

std::string SystemOfEquations::ToString( const unsigned int newline_tabulators ) const{

	std::string str;
	std::string newLine = { '\n' };

	for( unsigned int i = 0; i < newline_tabulators; i++ ) newLine += '\t';


	str += "varNum=" + std::to_string( number_of_variables_ );
	str += newLine + "popCols=" + std::to_string( currently_populated_columns_ );

	str += newLine + Matrix::ToString();

	return str;

}

MathematicalObject::MathError SystemOfEquations::PopulateColumn( const Tuple3D v ){

	// For now only implemented with three variable_values_
	if( IsPopulated() || number_of_variables_ != 3 ) return CheckForAndOutputError( MathError::Input, "System already populated or variables amount not equal to three!" );

	// Assign vector values to matrix columns
	( *this )( currently_populated_columns_, 0 ) = v.x;
	( *this )( currently_populated_columns_, 1 ) = v.y;
	( *this )( currently_populated_columns_, 2 ) = v.z;
	currently_populated_columns_++;

	return MathError::Ok;
}

MathematicalObject::MathError SystemOfEquations::PopulateColumn( const Tuple2D v ){

	// For now only implemented with three variable_values_
	if( IsPopulated() || number_of_variables_ != 2 ) return CheckForAndOutputError( MathError::Input, "System already populated or variables amount not equal to two!" );

	// Assign vector values to matrix columns
	( *this )( currently_populated_columns_, 0 ) = v.x;
	( *this )( currently_populated_columns_, 1 ) = v.y;
	currently_populated_columns_++;

	return MathError::Ok;
}

SystemOfEquationsSolution SystemOfEquations::Solve( void ){

	SystemOfEquationsSolution sol( number_of_variables_ );			// Create solution instance

	// System filled with coefficients?
	if( !IsPopulated() ) return sol;

	// Get size_ of coefficient matrix 
	size_t rows = number_of_rows(); size_t cols = number_of_columns();


	GridIndex topC{};									// Top-left corner of submatrix						
	GridIndex botC{ rows - 1, rows - 1 };				// Bottom-right corner of submatrix			

	GridIndex maxIndx;							// Indices of maximum in submatrix
	double maxVal;							// Value of maximum in submatrix



	// Matrix with variable indices
	vector<unsigned int> varIdx( number_of_variables_ );
	for( unsigned int i = 0; i < number_of_variables_; i++ ) varIdx.at( i ) = i;

	// Iterate all rows to create echelon form of matrix
	for( size_t k = 0; k < rows; k++ ){

		// Top-left corner changes each iteration
		topC.r = k; topC.c = k;

		// Search maximum absolute value in quadratic submatrix from (k, k) to (rows - 1, rows - 1)
		maxIndx = FindMaximum( topC, botC );

		// Get value
		maxVal = ( *this )( maxIndx );

		// Return if maximum is zero -> no solution
		if( IsNearlyEqualDistance( maxVal, 0. ) ) return sol;


		// Swap rows and columns to bring the cell with maximum value to (k,k)
		if( maxIndx.r != k ) SwapRows( maxIndx.r, k );
		if( maxIndx.c != k ){
			SwapColumns( maxIndx.c, k );

			// Variable in columns swapped
			unsigned int temp = varIdx.at( k );
			varIdx.at( k ) = varIdx.at( maxIndx.c );
			varIdx.at( maxIndx.c ) = temp;
		}

		ScaleRow( k, 1 / maxVal );			// Scale k-row by reciprocal of (k, k) value

		// Substract k-row from k+1 to n-row to eliminate cells
		for( size_t row = k + 1; row < rows; row++ ){

			// Target cell not zero?
			if( !IsNearlyEqualDistance( ( *this )( k, row ), 0. ) ){
				ScaleRow( row, 1 / ( *this )( k, row ) );		// Make (row, k) to one by scaling row with reciprocal
				SubstractRows( k, row );						// Substract k-row from row
			}
		}
	}


	// Eliminate remaining cells 'above' the diagonal

	// Iterate  coefficient columns 
	for( size_t c = cols - 2; c > 0; c-- ){

		// Iterate rows above diagonal
		for( size_t r = 0; r < c; r++ ){

			// Current cell not already zero?
			if( !IsNearlyEqualDistance( ( *this )( c, r ), 0 ) ){

				// Calculate result column cell as if current cell is being eliminated 
				( *this )( cols - 1, r ) -= ( ( *this )( cols - 1, c ) * ( *this )( c, r ) );
			}
		}
	}


	size_t varsIndx;
	// Copy result column entries to var-array
	for( size_t i = 0; i < rows; i++ ){

		varsIndx = varIdx.at( i );		// Index for swapped variable

		// Check if index is in allowed range
		if( varsIndx < sol.number_of_variables() ) sol.SetVariableValue( varsIndx, ( *this )( cols - 1, i ) );
		else return sol;
	}

	sol.solution_found( true );
	return sol;

}



/*
	SystemOfEquationsSolution implementation
*/

SystemOfEquationsSolution::SystemOfEquationsSolution( const size_t number_of_variiables )
	: number_of_variables_( number_of_variiables ),
	variable_values_( number_of_variables_, 0. ),
	solution_found_( false ){};


string SystemOfEquationsSolution::ToString( const unsigned int newline_tabulators ) const{
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
	if( idx >= number_of_variables_ ) CheckForAndOutputError( MathError::Input, "Index exceeds amount of variables!" );

	variable_values_[ idx ] = val;
}

double SystemOfEquationsSolution::GetVariableValue( const size_t idx ) const{
	if( idx >= number_of_variables_ ) CheckForAndOutputError( MathError::Input, "Index exceeds amount of variables!" );
	return variable_values_[ idx ];
}