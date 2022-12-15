#pragma once
/*********************************************************************
 * @file   equationSystem.xpp
 * @brief  Implementations
 *
 * @author Jan Wolzenburg
 * @date   December 2022
 *********************************************************************/



 /*********************************************************************
	Includes
 *********************************************************************/
 #include <cmath>
 #include <vector>
 using std::vector;

 #include "generelMath.h"
 #include "equationSystem.h"



 /*********************************************************************
	Implementations
 *********************************************************************/


/*
	matx implementation
*/

matx::matx( const size_t n_, const size_t m_ )
	: n( n_ ),
	m( m_ ),
	A( new double[n * m] ) {
	if (n == 0) { checkErr( MATH_ERR::INPUT, "Matrix rows amount must be greater than zero!" ); n = 1; }
	if (m == 0) { checkErr( MATH_ERR::INPUT, "Matrix columns amount must be greater than zero!" ); m = 1; }
};

string matx::toStr( [[maybe_unused]] const unsigned int newLineTabulators ) const{

	std::string str;
	char tempCharArr[ 256 ];

	for( unsigned int r = 0; r < n; r++ ){
		for( unsigned int c = 0; c < m; c++ ){
			snprintf( tempCharArr, 256, "%.6f,", ( *this )( r, c ) );
			str += tempCharArr;
			if( c == m - 1 ) str += ";";
		}
	}

	return str;
}

matx::matx( const matx& mtx )
	: n( mtx.n ),
	m( mtx.m ),
	A( new double[n * m] ) {
	memcpy( A, mtx.A, n * m * sizeof( double ) );	// Copy matrix data
};

matx& matx::operator=( const matx& mtx ) {
	n = mtx.n;
	m = mtx.m;

	delete[] A;
	A = new double[n * m];

	memcpy( A, mtx.A, n * m * sizeof( double ) );	// Copy matrix data

	return *this;
};

matx::~matx() {
	delete[] A;
};

double& matx::operator() ( const size_t row, const size_t col ) {
	if (row >= n) { checkErr( MATH_ERR::INPUT, "Row exceeds matrix size!" ); return A[m * (n - 1) + (m - 1)]; }
	if (col >= m) { checkErr( MATH_ERR::INPUT, "Column exceeds matrix size!" ); return A[m * (n - 1) + (m - 1)]; }

	return A[m * row + col];
};

double& matx::operator() ( const idx2 idx ) {
	return (*this)(idx.y, idx.x);
};

double matx::operator() ( const size_t row, const size_t col ) const {
	if (row >= n) { checkErr( MATH_ERR::INPUT, "Row exceeds matrix size!" ); return A[m * (n - 1) + (m - 1)]; }
	if (col >= m) { checkErr( MATH_ERR::INPUT, "Column exceeds matrix size!" ); return A[m * (n - 1) + (m - 1)]; }
	return A[m * row + col];
};

double matx::operator() ( const idx2 idx ) const {
	return (*this)( idx.y, idx.x );
};

mathObj::MATH_ERR matx::swapCols( const size_t c1, const size_t c2 ) {
	if (c1 >= m || c2 >= m) return checkErr( MATH_ERR::BOUNDS, "Column or row indices exceed matrix size!" );

	double tempVal;

	// Iterate each row
	for (size_t curR = 0; curR < n; curR++) {
		tempVal = (*this)(curR, c1);				// Temporary storage
		(*this)(curR, c1) = (*this)(curR, c2);		// Swap values
		(*this)(curR, c2) = tempVal;
	}

	return MATH_ERR::OK;
}

mathObj::MATH_ERR matx::swapRows( const size_t r1, const size_t r2 ) {
	if (r1 >= n || r2 >= n) return checkErr( MATH_ERR::BOUNDS, "Column or row indices exceed matrix size!" );

	double tempVal;

	// Iterate each column
	for (size_t curC = 0; curC < m; curC++) {
		tempVal = (*this)(r1, curC);					// Temporary storage
		(*this)(r1, curC) = (*this)(r2, curC);		// Swap values
		(*this)(r2, curC) = tempVal;
	}

	return MATH_ERR::OK;
}

idx2 matx::findMax( const idx2 topCorner, const idx2 botCorner ) {
	double curMax = 0, curVal;
	idx2 curMaxIndx = { .x = topCorner.x, .y = topCorner.y};


	if (topCorner.y >= n || topCorner.x >= m || botCorner.y >= n || botCorner.x >= m) {
		checkErr( MATH_ERR::BOUNDS, "Top of bottom corner of submatrix is outside matrix!" );
		return curMaxIndx;
	}

	// Iterate elements in submatrix
	for (size_t curR = topCorner.y; curR <= botCorner.y; curR++) {
		for (size_t curC = topCorner.x; curC <= botCorner.x; curC++) {
			// Read cell
			curVal = abs( (*this)(curR, curC) );

			// Absolute value in cell greater?
			if (curVal > curMax) {
				curMax = curVal;
				curMaxIndx.y = curR;	// Store indices
				curMaxIndx.x = curC;
			}
		}
	}

	return curMaxIndx;
}

mathObj::MATH_ERR matx::scaleRow( const size_t r, const double scalar ) {
	if (r >= n) return checkErr( MATH_ERR::BOUNDS, "Row index exceeds matrix size!" );
	// Iterate all columns
	for (unsigned int c = 0; c < m; c++) {
		(*this)(r, c) = (*this)(r, c) * scalar;		// Scale cell
	}

	return MATH_ERR::OK;
}

mathObj::MATH_ERR matx::subRows( const size_t r1, const size_t r2 ) {
	if (r1 >= n || r2 >= n) return checkErr( MATH_ERR::BOUNDS, "Row index exceeds matrix size!" );

	// Iterate all columns
	for (size_t c = 0; c < m; c++) {
		(*this)(r2, c) = (*this)(r2, c) - (*this)(r1, c);		// Substract cell
	}

	return MATH_ERR::OK;
}



/*
	eqnSys implementation
*/

eqnSys::eqnSys( const size_t varNum_ )
	: matx ( varNum_, varNum_ + 1 ),
	varNum( varNum_ ),
	numPopulatedColumns( 0 ){
	if( varNum == 0 ) checkErr( MATH_ERR::INPUT, "Number of variables must be greater than 0!" );
};

std::string eqnSys::toStr( const unsigned int newLineTabulators ) const{

	std::string str;
	std::string newLine = { '\n' };

	for( unsigned int i = 0; i < newLineTabulators; i++ ) newLine += '\t';


	str += "varNum=" + std::to_string( varNum );
	str += newLine + "popCols=" + std::to_string( numPopulatedColumns );

	str += newLine + matx::toStr();

	return str;

}

mathObj::MATH_ERR eqnSys::populateColumn( const v3 v ){

	// For now only implemented with three vars
	if( isPopulated() || varNum != 3 ) return checkErr( MATH_ERR::INPUT, "System already populated or variables amount not equal to three!" );

	// Assign vector values to matrix columns
	( *this )( 0, numPopulatedColumns ) = v.x;
	( *this )( 1, numPopulatedColumns ) = v.y;
	( *this )( 2, numPopulatedColumns ) = v.z;
	numPopulatedColumns++;

	return MATH_ERR::OK;
}

eqnSysSolution eqnSys::solve( void ){

	eqnSysSolution sol( *this );			// Create solution instance

	// System filled with coefficients?
	if( !isPopulated() ) return sol;

	// Get size of coefficient matrix 
	size_t rows = getRows(); size_t cols = getCols();


	idx2 topC{};										// Top-left corner of submatrix						
	idx2 botC{ .x = rows - 1, .y = rows - 1 };				// Bottom-right corner of submatrix			

	idx2 maxIndx;							// Indices of maximum in submatrix
	double maxVal;							// Value of maximum in submatrix



	// Matrix with variable indices
	vector<unsigned int> varIdx( varNum );
	for( unsigned int i = 0; i < varNum; i++ ) varIdx.at( i ) = i;

	// Iterate all rows to create echelon form of matrix
	for( size_t k = 0; k < rows; k++ ){

		// Top-left corner changes each iteration
		topC.y = k; topC.x = k;

		// Search maximum absolute value in quadratic submatrix from (k, k) to (rows - 1, rows - 1)
		maxIndx = findMax( topC, botC );

		// Get value
		maxVal = ( *this )( maxIndx );

		// Return if maximum is zero -> no solution
		if( iseqErr( maxVal, 0 ) ) return sol;


		// Swap rows and columns to bring the cell with maximum value to (k,k)
		if( maxIndx.y != k ) swapRows( maxIndx.y, k );
		if( maxIndx.x != k ){
			swapCols( maxIndx.x, k );

			// Variable in columns swapped
			unsigned int temp = varIdx.at( k );
			varIdx.at( k ) = varIdx.at( maxIndx.x );
			varIdx.at( temp );
		}

		scaleRow( k, 1 / maxVal );			// Scale k-row by reciprocal of (k, k) value

		// Substract k-row from k+1 to n-row to eliminate cells
		for( size_t row = k + 1; row < rows; row++ ){

			// Target cell not zero?
			if( !iseqErr( ( *this )( row, k ), 0 ) ){
				scaleRow( row, 1 / ( *this )( row, k ) );		// Make (row, k) to one by scaling row with reciprocal
				subRows( k, row );						// Substract k-row from row
			}
		}
	}


	// Eliminate remaining cells 'above' the diagonal

	// Iterate  coefficient columns 
	for( size_t c = cols - 2; c > 0; c-- ){

		// Iterate rows above diagonal
		for( size_t r = 0; r < c; r++ ){

			// Current cell not already zero?
			if( !iseqErr( ( *this )( r, c ), 0 ) ){

				// Calculate result column cell as if current cell is being eliminated 
				( *this )( r, cols - 1 ) -= ( ( *this )( c, cols - 1 ) * ( *this )( r, c ) );
			}
		}
	}


	size_t varsIndx;
	// Copy result column entries to var-array
	for( size_t i = 0; i < rows; i++ ){

		varsIndx = varIdx.at( i );		// Index for swapped variable

		// Check if index is in allowed range
		if( varsIndx < sol.getVarNum() ) sol.setVar( varsIndx, ( *this )( i, cols - 1 ) );
		else return sol;
	}

	sol.Success( true );
	return sol;

}



/*
	eqnSysSolution implementation
*/

eqnSysSolution::eqnSysSolution( const eqnSys sys )
	: varNum( sys.varNum ),
	vars( new double[ varNum ] ),
	success( false ){};

eqnSysSolution::eqnSysSolution( const eqnSysSolution& sysSol )
	: varNum( sysSol.varNum ),
	vars( new double[ varNum ] ),
	success( sysSol.success ){
	memcpy( vars, sysSol.vars, varNum * sizeof( double ) );	// Copy data
};

eqnSysSolution::~eqnSysSolution( void ){
	delete[] vars;
}

string eqnSysSolution::toStr( const unsigned int newLineTabulators ) const{
	string str;
	string newLine = { '\n' };

	for( unsigned int i = 0; i < newLineTabulators; i++ ) newLine += '\t';

	str += "varNum=" + varNum;
	str += newLine + "success=" + std::to_string( success );
	str += newLine + "vars=";

	for( unsigned int i = 0; i < varNum; i++ ){
		str += std::to_string( vars[ i ] );
		if( i < varNum - 1 ) str += ", ";
	}

	return str;

}

eqnSysSolution& eqnSysSolution::operator=( const eqnSysSolution& sysSol ){

	success = sysSol.success;
	varNum = sysSol.varNum;
	delete[] vars;

	vars = new double[ varNum ];

	memcpy( vars, sysSol.vars, varNum * sizeof( double ) );	// Copy data
	return *this;
};

void eqnSysSolution::setVar( const size_t idx, const double val ){
	if( idx >= varNum ) checkErr( MATH_ERR::INPUT, "Index exceeds amount of variables!" );

	vars[ idx ] = val;
};

double eqnSysSolution::getVar( const size_t idx ) const{
	if( idx >= varNum ) checkErr( MATH_ERR::INPUT, "Index exceeds amount of variables!" );
	return vars[ idx ];
};