/*
This file is part of the TrackStar package.
Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
License: MIT License. See LICENSE in top-level directory
at: https://github.com/giganano/trackstar.git.

Contents
--------
.. cpp:type:: MATRIX
	An arbitrary, 2-dimensional matrix of real numbers.
.. cpp:type:: COVARIANCE_MATRIX
	An arbitrary covariance matrix, a pointer type compatible with MATRIX.
.. function: MATRIX *matrix_initialize
	Allocate memory for and obtain a pointer to a MATRIX object.
.. cpp:function:: void matrix_free
	Free up the memory stored by a MATRIX object.
.. cpp:function:: MATRIX *matrix_add
	The addition operation for two similarly-sized matrices.

Source: ``trackstar/core/src/matrix.c``
*/

#ifndef MATRIX_H
#define MATRIX_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct matrix {

	/* 
	.. cpp:type:: MATRIX

	An arbitrary, 2-dimensional matrix of real numbers.
	
	Attributes
	----------
	matrix : ``double **``
		The matrix itself.
	n_rows : ``unsigned short``
		The number of rows in the matrix.
	n_cols : ``unsigned short``
		The number of columns in the matrix.
	*/

	double **matrix;
	unsigned short n_rows;
	unsigned short n_cols;

} MATRIX;

typedef struct covariance_matrix {

	/*
	.. cpp:type:: COVARIANCE_MATRIX

	An arbitrary covariance matrix, a pointer type compatible with MATRIX.
	In practice, ``n_rows`` and ``n_cols`` must be equal, and TrackStar's
	python API enforces this.
	
	matrix : ``double **``
		The covariance matrix itself.
	n_rows : ``unsigned short``
		The number of rows in the matrix.
	n_cols : ``unsigned short``
		The number of columns in the matrix. Equal to n_rows for this
		particular matrix type.
	inv : ``MATRIX *``
		The inverse of this particular covariance matrix.
	*/

	double **matrix;
	unsigned short n_rows;
	unsigned short n_cols;
	MATRIX *inv;

} COVARIANCE_MATRIX;

/*
.. cpp:function:: extern MATRIX *matrix_initialize(unsigned short n_rows,
	unsigned short n_cols);

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
.. cpp:function:: extern void matrix_free(MATRIX *m);

Free up the memory associated with a MATRIX object.

Parameters
----------
m : ``MATRIX *``
	The matrix to free itself.
*/
extern void matrix_free(MATRIX *m);

/*
.. cpp:function:: extern MATRIX *matrix_add(MATRIX m1, MATRIX m2,
	MATRIX *result);

Parameters
----------
m1 : ``MATRIX``
	The first matrix in the addition operation.
m2 : ``MATRIX``
	The second matrix in the addition operation.
result : ``MATRIX *``
	A pointer to an already-initialized MATRIX to store the resultant matrix in,
	if applicable. If ``NULL``, memory will be allocated automatically.

Returns
-------
result : ``MATRIX *``
	A MATRIX :math:`M` defined based on ``m1`` and ``m2`` such that

	.. math:: M_{ij} = m1_{ij} + m2_{ij}.

Notes
-----
If a pointer is provided for ``result`` as an argument to this function, the
return value will be the same memory address.
*/
extern MATRIX *matrix_add(MATRIX m1, MATRIX m2, MATRIX *result);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MATRIX_H */
