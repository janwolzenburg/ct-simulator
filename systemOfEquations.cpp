
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

Matrix::Matrix( const size_t number_of_columns, const size_t number_of_rows ) :
	number_of_columns_( number_of_columns ),
	number_of_rows_( number_of_rows ),
	data_( number_of_rows_ * number_of_columns_, 0. ) {
	if (number_of_rows_ == 0) { CheckForAndOutputError( MathError::Input, "matrix rows amount must be greater than zero!" ); number_of_rows_ = 1; }
	if (number_of_columns_ == 0) { CheckForAndOutputError( MathError::Input, "matrix columns amount must be greater than zero!" ); number_of_columns_ = 1; }
}

string Matrix::ConvertToString( [[maybe_unused]] const unsigned int newline_tabulators ) const{

	std::string new_string;
	char tempory_character_array[ 256 ];

	for( unsigned int row = 0; row < number_of_rows_; row++ ){
		for( unsigned int column = 0; column < number_of_columns_; column++ ){
			snprintf( tempory_character_array, 256, "%.6f,", ( *this )( row, column ) );
			new_string += tempory_character_array;
			if( column == number_of_columns_ - 1 ) new_string += ";";
		}
	}

	return new_string;
}

double& Matrix::operator() ( const size_t column, const size_t row ) {
	if (row >= number_of_rows_) { CheckForAndOutputError( MathError::Input, "row exceeds matrix size!" ); return data_.at( number_of_columns_ * (number_of_rows_ - 1) + (number_of_columns_ - 1) ); }
	if (column >= number_of_columns_) { CheckForAndOutputError( MathError::Input, "column exceeds matrix size!" ); return data_.at( number_of_columns_ * (number_of_rows_ - 1) + (number_of_columns_ - 1) ); }

	return data_.at(number_of_columns_ * row + column);
}

double Matrix::operator() ( const size_t column, const size_t row ) const {
	if (row >= number_of_rows_) { CheckForAndOutputError( MathError::Input, "row exceeds matrix size!" ); return data_.at( number_of_columns_ * (number_of_rows_ - 1) + (number_of_columns_ - 1) ); }
	if (column >= number_of_columns_) { CheckForAndOutputError( MathError::Input, "column exceeds matrix size!" ); return data_.at( number_of_columns_ * (number_of_rows_ - 1) + (number_of_columns_ - 1) ); }
	return data_.at(number_of_columns_ * row + column);
}

MathematicalObject::MathError Matrix::SwapColumns( const size_t column_1, const size_t column_2 ) {
	if (column_1 >= number_of_columns_ || column_2 >= number_of_columns_) return CheckForAndOutputError( MathError::Bounds, "column or row indices exceed matrix size!" );

	double tempVal;

	// iterate each row
	for (size_t current_row = 0; current_row < number_of_rows_; current_row++) {
		tempVal = data_.at( number_of_columns_ * current_row + column_1 );				// temporary storage
		data_.at( number_of_columns_ * current_row + column_1 ) = data_.at( number_of_columns_ * current_row + column_2 );		// swap values
		data_.at( number_of_columns_ * current_row + column_2 ) = tempVal;
	}

	return MathError::Ok;
}

MathematicalObject::MathError Matrix::SwapRows( const size_t row_1, const size_t row_2 ) {
	if (row_1 >= number_of_rows_ || row_2 >= number_of_rows_) return CheckForAndOutputError( MathError::Bounds, "column or row indices exceed matrix size!" );

	double tempory_value;

	// iterate each column
	for (size_t current_column = 0; current_column < number_of_columns_; current_column++) {
		tempory_value = data_.at( number_of_columns_ * row_1 + current_column );					// temporary storage
		data_.at( number_of_columns_ * row_1 + current_column ) = data_.at( number_of_columns_ * row_2 + current_column ); // swap values
		data_.at( number_of_columns_ * row_2 + current_column ) = tempory_value;
	}

	return MathError::Ok;
}

GridIndex Matrix::FindMaximum( const GridIndex top_corner, const GridIndex bottom_corner ) {
	double current_maximum = 0, current_value;
	GridIndex current_maximum_index = top_corner;


	if (top_corner.r >= number_of_rows_ || top_corner.c >= number_of_columns_ || bottom_corner.r >= number_of_rows_ || bottom_corner.c >= number_of_columns_) {
		CheckForAndOutputError( MathError::Bounds, "top of bottom corner of submatrix is outside matrix!" );
		return current_maximum_index;
	}

	// iterate elements in submatrix
	for (size_t current_row = top_corner.r; current_row <= bottom_corner.r; current_row++) {
		for (size_t current_column = top_corner.c; current_column <= bottom_corner.c; current_column++) {
			// read cell
			current_value = abs( data_.at( number_of_columns_ * current_row + current_column ) );

			// absolute value in cell greater?
			if (current_value > current_maximum) {
				current_maximum = current_value;
				current_maximum_index.r = current_row;	// store indices
				current_maximum_index.c = current_column;
			}
		}
	}

	return current_maximum_index;
}

MathematicalObject::MathError Matrix::ScaleRow( const size_t row, const double scalar ) {
	if (row >= number_of_rows_) return CheckForAndOutputError( MathError::Bounds, "row index exceeds matrix size!" );
	// iterate all columns
	for (unsigned int column = 0; column < number_of_columns_; column++) {
		data_.at( number_of_columns_ * row + column ) = data_.at( number_of_columns_ * row + column ) * scalar;
	}

	return MathError::Ok;
}

MathematicalObject::MathError Matrix::SubstractRows( const size_t row_1, const size_t row_2 ) {
	if (row_1 >= number_of_rows_ || row_2 >= number_of_rows_) return CheckForAndOutputError( MathError::Bounds, "row index exceeds matrix size!" );

	// iterate all columns
	for (size_t c = 0; c < number_of_columns_; c++) {
		data_.at( number_of_columns_ * row_2 + c ) = data_.at( number_of_columns_ * row_2 + c ) - data_.at( number_of_columns_ * row_1 + c );
	}

	return MathError::Ok;
}



/*
	SystemOfEquations implementation
*/

SystemOfEquations::SystemOfEquations( const size_t number_of_variables )
	: Matrix ( number_of_variables + 1, number_of_variables ),
	number_of_variables_( number_of_variables ),
	currently_populated_columns_( 0 ){
	if( number_of_variables_ == 0 ) CheckForAndOutputError( MathError::Input, "number of variables must be greater than 0!" );
};

std::string SystemOfEquations::ConvertToString( const unsigned int newline_tabulators ) const{

	std::string new_string;
	std::string new_line = { '\n' };

	for( unsigned int i = 0; i < newline_tabulators; i++ ) new_line += '\t';


	new_string += "varNum=" + std::to_string( number_of_variables_ );
	new_string += new_line + "popCols=" + std::to_string( currently_populated_columns_ );

	new_string += new_line + Matrix::ConvertToString();

	return new_string;

}

MathematicalObject::MathError SystemOfEquations::PopulateColumn( const Tuple3D coefficients ){

	// for now only implemented with three variable_values_
	if( IsPopulated() || number_of_variables_ != 3 ) return CheckForAndOutputError( MathError::Input, "system already populated or variables amount not equal to three!" );

	// assign vector values to matrix columns
	( *this )( currently_populated_columns_, 0 ) = coefficients.x;
	( *this )( currently_populated_columns_, 1 ) = coefficients.y;
	( *this )( currently_populated_columns_, 2 ) = coefficients.z;
	currently_populated_columns_++;

	return MathError::Ok;
}

MathematicalObject::MathError SystemOfEquations::PopulateColumn( const Tuple2D coefficients ){

	// for now only implemented with three variable_values_
	if( IsPopulated() || number_of_variables_ != 2 ) return CheckForAndOutputError( MathError::Input, "system already populated or variables amount not equal to two!" );

	// assign vector values to matrix columns
	( *this )( currently_populated_columns_, 0 ) = coefficients.x;
	( *this )( currently_populated_columns_, 1 ) = coefficients.y;
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
			unsigned int tempory = variable_indices.at( k );
			variable_indices.at( k ) = variable_indices.at( index_of_maximum.c );
			variable_indices.at( index_of_maximum.c ) = tempory;
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
	string new_string;
	string new_line = { '\n' };

	for( unsigned int i = 0; i < newline_tabulators; i++ ) new_line += '\t';

	new_string += "varNum=" + number_of_variables_;
	new_string += new_line + "success=" + std::to_string( solution_found_ );
	new_string += new_line + "vars=";

	for( unsigned int i = 0; i < number_of_variables_; i++ ){
		new_string += std::to_string( variable_values_[ i ] );
		if( i < number_of_variables_ - 1 ) new_string += ", ";
	}

	return new_string;

}

void SystemOfEquationsSolution::SetVariableValue( const size_t index, const double value ){
	if( index >= number_of_variables_ ) CheckForAndOutputError( MathError::Input, "index exceeds amount of variables!" );

	variable_values_[ index ] = value;
}

double SystemOfEquationsSolution::GetVariableValue( const size_t index ) const{
	if( index >= number_of_variables_ ) CheckForAndOutputError( MathError::Input, "index exceeds amount of variables!" );
	return variable_values_[ index ];
}