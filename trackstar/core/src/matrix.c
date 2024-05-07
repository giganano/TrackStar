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
static MATRIX *matrix_unary_minus(MATRIX m, MATRIX *result);
static MATRIX *matrix_adjoint(MATRIX m, MATRIX *result);
static MATRIX *matrix_cofactors(MATRIX m, MATRIX *result);
static MATRIX *matrix_minor(MATRIX m, unsigned short axis[2], MATRIX *result);
static void matrix_resize(MATRIX *m, const unsigned short n_rows,
	const unsigned short n_cols);


/*
.. cpp:function:: extern MATRIX *matrix_initialize(unsigned short n_rows,
	unsigned short n_cols);

Allocate memory for and return a pointer to a ``MATRIX`` object.
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

Free up the memory associated with a ``MATRIX`` object.

Parameters
----------
m : ``MATRIX *``
	The matrix to be freed.
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
.. cpp:function:: extern void covariance_matrix_free(COVARIANCE_MATRIX *cov);

Free up the memory associated with a ``COVARIANCE_MATRIX`` object.

Parameters
----------
cov : ``COVARIANCE_MATRIX *``
	The covariance matrix to be freed.
*/
extern void covariance_matrix_free(COVARIANCE_MATRIX *cov) {

	if (cov != NULL) {

		/*
		Due to the inheritance structure implemented with compatible pointer
		types in the backend, calling matrix_free( (MATRIX *) cov) here results
		in a segmentation fault when quitting the Python interpreter. This
		arises because the attribute covariance_matrix._m is assigned the same
		pointer value as covariance_matrix._cov. Since covariance_matrix
		inherits from matrix, Cython calls covariance_matrix.__dealloc__
		followed by matrix.__dealloc__ automatically, so the memory blocks
		associated with the base matrix object will get freed automatically.
		*/

		if ((*cov).inv != NULL) matrix_free(cov -> inv);

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
			matrix_resize(result, m1.n_rows, m1.n_cols);
		}
		for (unsigned short i = 0u; i < m1.n_rows; i++) {
			for (unsigned short j = 0u; j < m1.n_cols; j++) {
				result -> matrix[i][j] = m1.matrix[i][j] + m2.matrix[i][j];
			}
		}
		return result;

	} else {
		fatal_print("%s\n", "Matrix dimensions incompatible for addition.");
	}

}


/*
.. cpp:function:: extern MATRIX *matrix_subtract(MATRIX m1, MATRIX m2,
	MATRIX *result);

Subtract two matrices.

Parameters
----------
m1 : ``MATRIX``
	The first matrix in the subtraction operation.
m2 : ``MATRIX``
	The second matrix in the subtraction operation.
result : ``MATRIX *``
	A pointer to an already-initialized ``MATRIX`` object to store the
	resultant matrix, if applicable. If ``NULL``, memory will be allocated
	automatically.

Returns
-------
result : ``MATRIX *``
	The matrix :math:`m` defined by :math:`m_{ij} = m1_{ij} - m2_{ij}`.

.. note::

	If a pointer is provided for ``result`` as an argument to this function,
	the return value will be the same memory address.
*/
extern MATRIX *matrix_subtract(MATRIX m1, MATRIX m2, MATRIX *result) {

	MATRIX *minus_m2 = matrix_unary_minus(m2, NULL);
	result = matrix_add(m1, *minus_m2, result);
	matrix_free(minus_m2);
	return result;

}


/*
.. cpp:function:: static MATRIX *matrix_unary_minus(MATRIX m, MATRIX *result);

Take the unary negative of a matrix by multiplying :math:`m_{ij}` by
:math:`-1` for all :math:`i` and :math:`j`.

Parameters
----------
m : ``MATRIX``
	The original matrix.
result : ``MATRIX *``
	A pointer to an already-initialized ``MATRIX`` object to store the
	resultant matrix, if applicable. If ``NULL``, memory will be allocated
	automatically.

Returns
-------
result : ``MATRIX *``
	The negative matrix, defined as :math:`M_{ij} = -m_{ij}`.

.. note::

	If a pointer is provided for ``result`` as an argument to this function,
	the return value will be the same memory address.
*/
static MATRIX *matrix_unary_minus(MATRIX m, MATRIX *result) {

	if (result == NULL) {
		result = matrix_initialize(m.n_rows, m.n_cols);
	} else {
		matrix_resize(result, m.n_rows, m.n_cols);
	}
	for (unsigned short i = 0u; i < m.n_rows; i++) {
		for (unsigned short j = 0u; j < m.n_cols; j++) {
			result -> matrix[i][j] = -m.matrix[i][j];
		}
	}
	return result;

}


/*
.. cpp:function:: extern MATRIX *matrix_multiply(MATRIX m2, MATRIX m2,
	MATRIX *result);

Multiply two matrices.

Parameters
----------
m1 : ``MATRIX``
	The first matrix in the multiplication operation.
m2 : ``MATRIX``
	The second matrix in the multiplication operation.
result : ``MATRIX``
	A pointer to an already-initialized ``MATRIX`` object to store the
	resultant matrix, if applicable. if ``NULL``, memory will be allocated
	automatically.

Returns
-------
``result``: ``MATRIX *``
	The product of the two matrices :math:`c`, defined such that
	:math:`c_{ij} = \sum_k m1_{ik} * m2_{kj}`.

.. note::

	If a pointer is provided for ``result`` as an argument to this function,
	the return value will be the same memory address.
*/
extern MATRIX *matrix_multiply(MATRIX m1, MATRIX m2, MATRIX *result) {

	if (m1.n_cols == m2.n_rows) {

		if (result == NULL) {
			result = matrix_initialize(m1.n_rows, m2.n_cols);
		} else {
			matrix_resize(result, m1.n_rows, m2.n_cols);
		}
		for (unsigned short i = 0u; i < (*result).n_rows; i++) {
			for (unsigned short j = 0u; j < (*result).n_cols; j++) {
				for (unsigned short k = 0u; k < m1.n_cols; k++) {
					result -> matrix[i][j] += m1.matrix[i][k] * m2.matrix[k][j];
				}
			}
		}

		return result;

	} else {
		fatal_print("%s: (%ux%u), (%ux%u)\n",
			"Incompatible matrix dimensions for multiplication",
			m1.n_rows, m1.n_cols, m2.n_rows, m2.n_cols);
	}

}


/*
.. cpp:function:: extern MATRIX *matrix_invert(MATRIX m, MATRIX *result);

Invert a square matrix.

Parameters
----------
m : ``MATRIX``
	The input matrix.
result : ``MATRIX *``
	A pointer to an already-initialized ``MATRIX`` object to store the inverse
	matrix, if applicable., if ``NULL``, memory will be allocated automatically.

Returns
-------
result : ``MATRIX *``
	The inverse :math:`m^{-1}` of the input matrix, defined such that
	:math:`m^{-1}m = mm^{-1} = I`, where :math:`I` is the identity matrix of
	the same size as :math:`m`. A return value of ``NULL`` is returned when
	:math:`det(m) = 0`; in these cases, the matrix is not invertible.

.. note::

	If a pointer is provided for ``result`` as an argument to this function,
	the return value will be the same memory address, unless the determinant is
	zero.
*/
extern MATRIX *matrix_invert(MATRIX m, MATRIX *result) {

	double det = matrix_determinant(m);
	if (det) {
		result = matrix_adjoint(m, result);
		for (unsigned short i = 0u; i < m.n_rows; i++) {
			for (unsigned short j = 0u; j < m.n_cols; j++) {
				result -> matrix[i][j] /= det;
			}
		}
		return result;
	} else {
		return NULL;
	}

}


/*
.. cpp:function:: extern MATRIX *matrix_transpose(MATRIX m, MATRIX *result);

Transpose a matrix.

Parameters
----------
m : ``MATRIX``
	The input matrix.
result : ``MATRIX *``
	A pointer to an already-initialized ``MATRIX`` object to store the
	transpose, if applicable. if ``NULL``, memory will be allocated
	automatically.

Returns
-------
result : ``MATRIX *``
	The transpose, defined as :math:`m_{ij}^T = m_{ji}`.

.. note::

	If a pointer is provided for ``result`` as an argument to this function,
	the return value will be the same memory address.
*/
extern MATRIX *matrix_transpose(MATRIX m, MATRIX *result) {

	if (result == NULL) {
		result = matrix_initialize(m.n_cols, m.n_rows);
	} else {
		matrix_resize(result, m.n_cols, m.n_rows);
	}
	for (unsigned short i = 0u; i < m.n_rows; i++) {
		for (unsigned short j = 0u; j < m.n_cols; j++) {
			result -> matrix[j][i] = m.matrix[i][j];
		}
	}

	return result;

}


/*
.. cpp:function:: extern double matrix_determinant(MATRIX m);

Compute the determinant of a square matrix.

Parameters
----------
m : ``MATRIX``
	The matrix itself.

Returns
-------
det : ``double``
	:math:`det(m)`, compute via expansion by minors in the first row of the
	matrix. The expansion by minors is handled recursively within an iterative
	sum, with the solution for a 2x2 matrix implemented as the base case.
	As a failsafe, the obvious solution for a 1x1 matrix is implemented as an
	additional base case.
*/
extern double matrix_determinant(MATRIX m) {

	if (m.n_rows == m.n_cols) {

		if (m.n_rows == 1u) {
			/* failsafe base case: a 1x1 matrix */
			return m.matrix[0][0];
		} else if (m.n_rows == 2u) {
			/* primary base case: a 2x2 matrix */
			return (
				m.matrix[0][0] * m.matrix[1][1] -
				m.matrix[0][1] * m.matrix[1][0]
			);
		} else {
			/* The recursive case: an NxN matrix where N > 2 */
			double result = 0;
			for (unsigned short i = 0u; i < m.n_cols; i++) {
				unsigned short axis[2] = {0, i};
				MATRIX *minor = matrix_minor(m, axis, NULL);
				result += pow(-1, i) * m.matrix[0][i] * matrix_determinant(
					*minor);
				matrix_free(minor);
			}
			return result;
		}

	} else {
		fatal_print("%s\n",
			"Cannot compute the determinant of a non-square matrix.");
	}

}


/*
.. cpp:function:: static MATRIX *matrix_adjoint(MATRIX m, MATRIX *result);

Compute the adjoint of a square matrix.

Parameters
----------
m : ``MATRIX``
	The input matrix itself.
result : ``MATRIX *``
	A pointer to an already-initialized ``MATRIX`` object to store the adjoint
	matrix, if applicable. If ``NULL``, memory will be allocated automatically.

Returns
-------
result : ``MATRIX *``
	The adjoint, defined as the transpose of the matrix of cofactors.

.. note::

	If a pointer is provided for ``result`` as an argument to this function,
	the return value will be the same memory address.

.. note::

	Some textbooks and authors use the term adjugate or adjunt instead of
	adjoint. Though we use the term adjoint here, they refer to the same thing.
*/
static MATRIX *matrix_adjoint(MATRIX m, MATRIX *result) {

	MATRIX *cofactors = matrix_cofactors(m, NULL);
	result = matrix_transpose(*cofactors, result);
	matrix_free(cofactors);
	return result;

}


/*
.. cpp:function:: static MATRIX *matrix_cofactors(MATRIX m, MATRIX *result);

Compute the matrix of cofactors for a square matrix.

Parameters
----------
m : ``MATRIX``
	The input matrix itself.
result : ``MATRIX *``
	A pointer to an already-initialized ``MATRIX`` object to store the minor,
	if applicable. If ``NULL``, memory will be allocated automatically.

Returns
-------
result : ``MATRIX *``
	The matrix of cofactors, defined according to

	.. math:: A_{ij} = (-1)^{i + j} det(m_{ij}),

	where :math:`m_{ij}` is the :math:`ij`'th minor of ``m``.

.. note::

	If a pointer is provided for ``result`` as an argument to this function,
	the return value will be the same memory address.
*/
static MATRIX *matrix_cofactors(MATRIX m, MATRIX *result) {

	if (m.n_rows == m.n_cols) {
		if (result == NULL) {
			result = matrix_initialize(m.n_rows, m.n_cols);
		} else {
			matrix_resize(result, m.n_rows, m.n_cols);
		}
		for (unsigned short i = 0u; i < m.n_rows; i++) {
			for (unsigned short j = 0u; j < m.n_cols; j++) {
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
	A pointer to an already-initialized ``MATRIX`` object to store the minor,
	if applicable. If ``NULL``, memory will be allocated automatically.

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
		matrix_resize(result, m.n_rows - 1u, m.n_cols - 1u);
	}

	/*
	Don't use multi-threading here. Thread safety would be a nightmare as one
	would have to access the current thread number as opposed to simply
	incrementing n1 and n2. Since we're simply copying values over, opening
	and closing threads would likely slow things down anyway.
	*/
	unsigned short n1 = 0u;
	for (unsigned short i = 0u; i < m.n_rows; i++) {
		if (i != axis[0]) {
			unsigned short n2 = 0u;
			for (unsigned short j = 0u; j < m.n_cols; j++) {
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
.. cpp:function:: static void matrix_resize(MATRIX *M,
	const unsigned short n_rows, const unsigned short n_cols);

Update the amount of memory reserved for the matrix based on new dimensions,
and set all entries to zero (i.e. :math:`M_{ij} = 0` for all :math:`i` and
:math:`j` after calling this function).

Parameters
----------
m : ``MATRIX *``
	The matrix to reszie.
n_rows : ``const unsigned short``
	The new number of rows in the matrix.
n_cols : ``const unsigned short``
	The new number of columns in the matrix.
*/
static void matrix_resize(MATRIX *m, const unsigned short n_rows,
	const unsigned short n_cols) {

	m -> n_rows = n_rows;
	m -> n_cols = n_cols;
	m -> matrix = (double **) realloc (m -> matrix,
		(*m).n_rows * sizeof(double *));
	for (unsigned short i = 0u; i < n_rows; i++) {
		m -> matrix[i] = (double *) realloc (m -> matrix[i],
			n_cols * sizeof(double));
		for (unsigned short j = 0u; j < n_cols; j++) m -> matrix[i][j] = 0.f;
	}

}
