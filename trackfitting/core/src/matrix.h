/*
This file is part of the TrackStar package.
Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
License: MIT License. See LICENSE in top-level directory
at: https://github.com/giganano/trackstar.git.
*/

#ifndef MATRIX_H
#define MATRIX_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct matrix {

	/* 
	This struct holds the data associated with an arbitrary, 2-dimensional
	matrix of real numbers.
	
	matrix : ``double **``
		The matrix itself.
	n_rows : ``const unsigned short``
		The number of rows in the matrix.
	n_cols : ``const unsigned short``
		The number of columns in the matrix.
	*/

	double **matrix;
	const unsigned short n_rows;
	const unsigned short n_cols;

} MATRIX;

typedef struct covariance_matrix {

	/*
	This struct holds the data associated with a covariance matrix and is a
	pointer type compatible with MATRIX. In practice, n_rows and n_cols
	must be equal, and TrackStar's python API enforces this.
	
	matrix : ``double **``
		The covariance matrix itself.
	n_rows : ``const unsigned short``
		The number of rows in the matrix.
	n_cols : ``const unsigned short``
		The number of columns in the matrix. Equal to n_rows for this
		particular matrix type.
	inv : ``MATRIX *``
		The inverse of this particular covariance matrix.
	*/

	double **matrix;
	const unsigned short n_rows;
	const unsigned short n_cols;
	MATRIX *inv;

} COVARIANCE_MATRIX;

/*
Allocate memory for and return a pointer to a MATRIX object.
Automatically initializes all matrix elements to zero.

Parameters
----------
n_rows : ``unsigned short``
	The number of rows in the matrix.
n_cols : ``unsigned short``
	The number of columns in the matrix.

Returns
-------
mat : ``MATRIX *``
	The newly constructed ``n_rows`` x ``n_cols`` matrix with each element
	``mat``:math:`_{ij}` initialized to zero.
*/
extern MATRIX *matrix_initialize(unsigned short n_rows, unsigned short n_cols);

/*
Free up the memory associated with a MATRIX object.

Parameters
----------
m : ``MATRIX *``
	The matrix to free itself.
*/
extern void matrix_free(MATRIX *m);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MATRIX_H */
