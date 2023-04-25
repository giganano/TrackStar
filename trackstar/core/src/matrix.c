/*
This file is part of the TrackStar package.
Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
License: MIT License. See LICENSE in top-level directory
at: https://github.com/giganano/trackstar.git.
*/

#include <stdlib.h>
#include <math.h>
#include "matrix.h"
#include "debug.h"


/* ---------- Static function comment headers not duplicated here ---------- */
static MATRIX *matrix_minor(MATRIX m, unsigned short axis[2], MATRIX *result);
static void matrix_reset(MATRIX *m);


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
extern MATRIX *matrix_initialize(unsigned short n_rows, unsigned short n_cols) {

	unsigned short i, j;
	MATRIX *m = (MATRIX *) malloc (sizeof(MATRIX));
	m -> matrix = (double **) malloc (n_rows * sizeof(double *));
	for (i = 0u; i < n_rows; i++) {
		m -> matrix[i] = (double *) malloc (n_cols * sizeof(double));
		for (j = 0u; j < n_cols; j++) m -> matrix[i][j] = 0.0;
	}

	m -> n_rows = n_rows;
	m -> n_cols = n_cols;
	return m;

}


/*
.. cpp:function:: extern void matrix_free(MATRIX *m);

Free up the memory associated with a MATRIX object.

Parameters
----------
m : ``MATRIX *``
	The matrix to free itself.
*/
extern void matrix_free(MATRIX *m) {

	if (m != NULL) {

		if ((*m).matrix != NULL) {
			unsigned short i;
			for (i = 0u; i < (*m).n_rows; i++) {
				if ((*m).matrix[i] != NULL) free(m -> matrix[i]);
			}
			free(m -> matrix);
		} else {}

		free(m);

	} else {}

}


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

.. note::

	If a pointer is provided for ``result`` as an argument to this function,
	the return value will be the same memory address.
*/
extern MATRIX *matrix_add(MATRIX m1, MATRIX m2, MATRIX *result) {

	if (m1.n_rows == m2.n_rows && m1.n_cols == m2.n_cols) {

		if (result == NULL) {
			result = matrix_initialize(m1.n_rows, m1.n_cols);
		} else {
			result -> n_rows = m1.n_rows;
			result -> n_cols = m1.n_cols;
			matrix_reset(result);
		}
		unsigned short i;
		for (i = 0u; i < m1.n_rows; i++) {
			unsigned short j;
			for (j = 0u; j < m1.n_cols; j++) {
				result -> matrix[i][j] = m1.matrix[i][j] + m2.matrix[i][j];
			}
		}
		return result;

	} else {
		fatal_print("%s\n", "Matrix dimensions incompatible for addition.");
	}

}

































/*
.. cpp:function:: static MATRIX *matrix_cofactors(MATRIX m, MATRIX *result);

Compute the matrix of cofactors for a square matrix.

Parameters
----------
m : ``MATRIX``
	The input matrix itself.
result : ``MATRIX *``
	A pointer to an already-initialized MATRIX object to store the minor, if
	applicable. If ``NULL``, memory will be allocated automatically.

Returns
-------
result : ``MATRIX *``
	The matrix of cofactors, defined according to

	.. math:: A_{ij} = (-1)^{i + j} det(m_{ij}),

	where :math:`m_{ij}` is the :math:`ij`'th minor of ``m``.
*/
static MATRIX *matrix_cofactors(MATRIX m, MATRIX *result) {

	if (m.n_rows == m.n_cols) {
		if (result == NULL) {
			result = matrix_initialize(m.n_rows, m.n_cols);
		} else {
			result -> n_rows = m.n_rows;
			result -> n_cols = m.n_cols;
			matrix_reset(result);
		}
		unsigned short i;
		for (i = 0u; i < m.n_rows; i++) {
			unsigned short j;
			for (j = 0u; j < m.n_cols; j++) {
				unsigned short axis[2] = {i, j};
				MATRIX *minor = matrix_minor(m, axis, NULL);
				result -> matrix[i][j] = pow(-1,
					i + j) * matrix_determinant(*minor);
				matrix_free(minor);
			}
		}
		return result;
	} else {
		fatal_print("%s\n", "Matrix is not square.");
	}

}


/*
.. cpp:function:: static MATRIX *matrix_minor(MATRIX m, unsigned short axis[2],
	MATRIX *result);

Obtain one of a matrix's minors.

Parameters
----------
m : ``MATRIX``
	The input matrix itself.
axis : ``unsigned short [2]``
	Which minor to obtain. axis[0] refers to the row to omit, and axes[1] to
	the column to omit from ``m``.
result : ``MATRIX *``
	A pointer to an already-initialized MATRIX object to store the minor, if
	applicable. If ``NULL``, memory will be allocated automatically.

Returns
-------
result : ``matrix``
	If ``m`` is an :math:`NxN` matrix, the returned matrix will be
	:math:`(N-1)x(N-1)`, with the ``axis[0]``'th row and the
	``axes[1]``'st column omitted from the original.

.. note::

	If a pointer is provided for ``result`` as an argument to this function,
	the return value will be the same memory address.
*/
static MATRIX *matrix_minor(MATRIX m, unsigned short axis[2], MATRIX *result) {

	if (result == NULL) {
		result = matrix_initialize(m.n_rows - 1u, m.n_cols - 1u);
	} else {
		result -> n_rows = m.n_rows - 1u;
		result -> n_cols = m.n_cols - 1u;
		matrix_reset(result);
	}
	unsigned short i, n1 = 0u;
	for (i = 0u; i < m.n_rows; i++) {
		if (i != axis[0]) {
			unsigned short j, n2 = 0u;
			for (j = 0u; j < m.n_cols; j++) {
				if (j != axis[1]) {
					result -> matrix[n1][n2] = m.matrix[i][j];
					n2++;
				}
			}
			n1++;
		} else {}
	}

	return result;

}


/*
.. cpp:function:: static void matrix_reset(MATRIX *m)

Update the amount of memory reserved for the matrix based on new dimensions and
reset all values to zero.

Parameters
----------
m : ``MATRIX *``
	The matrix to reset. After calling this function, ``(*m).matrix`` will have
	the proper amount of memory allocated for an ``(*m).n_rows`` x
	``(*m).n_cols`` matrix, and all entries will be assigned a value of zero.
*/
static void matrix_reset(MATRIX *m) {

	unsigned short i;
	m -> matrix = (double **) realloc (m -> matrix,
		(*m).n_rows * sizeof(double *));
	for (i = 0u; i < (*m).n_rows; i++) {
		unsigned short j;
		m -> matrix[i] = (double *) realloc (m -> matrix[i],
			(*m).n_cols * sizeof(double));
		for (j = 0u; j < (*m).n_cols; j++) m -> matrix[i][j] = 0.0;
	}

}




















































