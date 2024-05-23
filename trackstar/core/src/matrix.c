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
struct row_column_index;
static struct row_column_index matrix_determinant_ideal_axis(MATRIX m);
static unsigned short matrix_zeros_along_axis(MATRIX m,
	const unsigned short index, const unsigned short along_row);
static MATRIX *matrix_unary_minus(MATRIX m, MATRIX *result);
static MATRIX *matrix_adjoint(MATRIX m, MATRIX *result);
static MATRIX *matrix_cofactors(MATRIX m, MATRIX *result);
static MATRIX *matrix_minor(MATRIX m, unsigned short axis[2], MATRIX *result);
static void matrix_resize(MATRIX *m, const unsigned short n_rows,
	const unsigned short n_cols);


/*
.. c:function:: extern MATRIX *matrix_initialize(unsigned short n_rows, unsigned short n_cols);

	Allocate memory for an return a pointer to a :c:type:`MATRIX` objbect.

	Parameters
	----------
	n_rows : ``unsigned short``
		The desired number of rows in the matrix.
	n_cols : ``unsigned short``
		The desired number of columns in the matrix.

	Returns
	-------
	m : ``MATRIX *``
		A pointer to the newly constructed ``n_rows`` x ``n_cols`` matrix, with
		each element of :c:member:`MATRIX.matrix` set to zero (i.e.
		:math:`m_{ij} = 0` for all :math:`i,j`) and the values of the
		attributes :c:member:`MATRIX.n_rows` and :c:member:`MATRIX.n_cols`
		assigned accordingly.
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
.. c:function:: extern void matrix_free(MATRIX *m);

	Free up the memory associated with a :c:type:`MATRIX` object.

	Parameters
	----------
	m : ``MATRIX *``
		A pointer to the :c:type:`MATRIX` object whose memory is to be freed.
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
.. c:function:: extern COVARIANCE_MATRIX *covariance_matrix_initialize(unsigned short size);


	Allocate memory for and return a pointer to a :c:type:`COVARIANCE_MATRIX`
	object.

	Parameters
	----------
	size : ``unsigned short``
		The desired number of rows and columns in the matrix.

	Returns
	-------
	cov : ``COVARIANCE_MATRIX *``
		A pointer to the newly constructed ``size`` x ``size`` covariance
		matrix, with each matrix element assigned an initial value of zero.
		The struct members ``inv`` and ``labels`` are given initial values
		of ``NULL``.
 */
extern COVARIANCE_MATRIX *covariance_matrix_initialize(unsigned short size) {

	COVARIANCE_MATRIX *cov = (COVARIANCE_MATRIX *) matrix_initialize(size, size);
	cov = (COVARIANCE_MATRIX *) realloc (cov, sizeof(COVARIANCE_MATRIX));
	cov -> inv = NULL;
	cov -> labels = NULL;
	return cov;

}


/*
.. c:function:: extern void covariance_matrix_free(COVARIANCE_MATRIX *cov);

	Free up the memory associated with a :c:type:`COVARIANCE_MATRIX` object
	that is user-facing.

	Parameters
	----------
	cov : ``COVARIANCE_MATRIX *``
		The covariance matrix to be freed.

	Notes
	-----
	In practice, this function should only be called upon exiting the python
	interpreter, or when a user calls ``del`` on their
	:c:type:`COVARIANCE_MATRIX` object.

	The important difference between this function and
	:c:func:`covariance_matrix_free_everything` is that this function does
	not free *every* block of memory stored by a :c:type:`COVARIANCE_MATRIX`.
	Doing so causes memory errors because Cython automatically calls the
	necessary ``__dealloc__`` functions that do free up the required blocks of
	memory. Namely, this function does not call ``matrix_free((MATRIX *) cov)``,
	because Cython calls ``matrix.__dealloc__`` with the same address as
	``cov._cov``.
*/
extern void covariance_matrix_free(COVARIANCE_MATRIX *cov) {

	if (cov != NULL) {
		if ((*cov).inv != NULL) matrix_free(cov -> inv);
	} else {}

}


/*
.. c:function:: extern void covariance_matrix_free_everything(COVARIANCE_MATRIX *cov);

	Free up the memory associated with a ``COVARIANCE_MATRIX`` object that is
	*not* user-facing.

	Parameters
	----------
	cov : ``COVARIANCE_MATRIX *``
		The covariance matrix to be freed.

	Notes
	-----
	In practice, this function should only be called for
	:c:type:`COVARIANCE_MATRIX` objects created in TrackStar's C library or
	``cdef``'ed instances created in Cython that are not returned to the user.

	.. seealso::

		See "Notes" under :c:func:`covariance_matrix_free` for details on the
		differences between the two functions.
*/
extern void covariance_matrix_free_everything(COVARIANCE_MATRIX *cov) {

	covariance_matrix_free(cov);
	if (cov != NULL) free(cov);

}


/*
.. c:function:: extern MATRIX *matrix_add(MATRIX m1, MATRIX m2, MATRIX *result);

	Parameters
	----------
	m1 : ``MATRIX``
		The first matrix in the addition operation.
	m2 : ``MATRIX``
		The second matrix in the addition operation.
	result : ``MATRIX *``
		A pointer to an already-initialized :c:type:`MATRIX` to store the
		resultant matrix in, if applicable. If ``NULL``, memory will be
		allocated automatically. It a pointer is provided, the same pointer
		will be returned.

	Returns
	-------
	result : ``MATRIX *``
		A :c:type:`MATRIX` :math:`M` defined based on ``m1`` and ``m2`` such
		that

		.. math:: M_{ij} = m_{1,ij} + m_{2,ij}.
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
.. c:function:: extern MATRIX *matrix_subtract(MATRIX m1, MATRIX m2, MATRIX *result);

	Subtract two matrices.

	Parameters
	----------
	m1 : ``MATRIX``
		The first matrix in the subtraction operation.
	m2 : ``MATRIX``
		The second matrix in the subtraction operation.
	result : ``MATRIX *``
		A pointer to an already-initialized :c:type:`MATRIX` object to store
		the resultant matrix, if applicable. If ``NULL``, memory will be
		allocated automatically. If a pointer is provided, the same pointer
		will be returned.

	Returns
	-------
	result : ``MATRIX *``
		A :c:type:`MATRIX` :math:`M` defined based on ``m1`` and ``m2`` such
		that

		.. math:: M_{ij} = m_{1,ij} - m_{2,ij}
*/
extern MATRIX *matrix_subtract(MATRIX m1, MATRIX m2, MATRIX *result) {

	MATRIX *minus_m2 = matrix_unary_minus(m2, NULL);
	result = matrix_add(m1, *minus_m2, result);
	matrix_free(minus_m2);
	return result;

}


/*
.. cpp:function:: static MATRIX *matrix_unary_minus(MATRIX m, MATRIX *result);

	Take the unary negative of a matrix.

	Parameters
	----------
	m : ``MATRIX``
		The original matrix.
	result : ``MATRIX *``
		A pointer to an already-initialized :c:type:`MATRIX` object to store
		the resultant matrix, if applicable. If ``NULL``, memory will be
		allocated automatically. If a pointer is provided, the same pointer
		will be returned.

	Returns
	-------
	result : ``MATRIX *``
		The negative matrix, defined such that

		.. math:: `M_{ij} = -m_{ij}`
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
.. c:function:: extern MATRIX *matrix_multiply(MATRIX m1, MATRIX m2, MATRIX *result);

	Multiply two matrices.

	Parameters
	----------
	m1 : ``MATRIX``
		The first matrix in the multiplication operation.
	m2 : ``MATRIX``
		The second matrix in the multiplication operation.
	result : ``MATRIX``
		A pointer to an already-initialized :c:type:`MATRIX` object to store
		the resultant matrix, if applicable. if ``NULL``, memory will be
		allocated automatically. If a pointer is provided, the same pointer
		will be returned.

	Returns
	-------
	``result``: ``MATRIX *``
		A :c:type:`MATRIX` :math:`M` defined based on ``m1`` and ``m2`` such
		that

		.. math:: M_{ij} = \sum_k m_{1i,k} m_{2k,j}
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
.. c:function:: extern MATRIX *matrix_invert(MATRIX m, MATRIX *result);

	Invert a square matrix.

	Parameters
	----------
	m : ``MATRIX``
		The input matrix.
	result : ``MATRIX *``
		A pointer to an already-initialized :c:type:`MATRIX` object to store
		the inverse matrix, if applicable., if ``NULL``, memory will be
		allocated automatically. If a pointer is provided, the same pointer
		will be returned.

	Returns
	-------
	result : ``MATRIX *``
		The :c:type:`MATRXIX` :math:`m^{-1}`, defined such that

		.. math:: m^{-1}m = mm^{-1} = I

		where :math:`I` is the identity matrix of the same size as :math:`m`.
		``NULL`` is returned when :math:`det(m) = 0`, because such matrices
		are not invertible.

	.. note::

		While some non-square matrices have left- and right-inverses, TrackStar
		only supports inversion of square matrices.
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
.. c:function:: extern MATRIX *matrix_transpose(MATRIX m, MATRIX *result);

	Transpose a matrix.

	Parameters
	----------
	m : ``MATRIX``
		The input matrix.
	result : ``MATRIX *``
		A pointer to an already-initialized :c:type:`MATRIX` object to store
		the transpose, if applicable. If ``NULL``, memory will be allocated
		automatically. IF a pointer is provided, the same pointer will be
		returned.

	Returns
	-------
	result : ``MATRIX *``
		The :c:type:`MATRIX` :math:`m^T`, defined such that

		.. math:: m^T_{ij} = m_{ji}		
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


struct row_column_index {

	/*
	.. c:struct:: row_column_index;

		A ``struct`` used to determine which row or column along which it would
		be ideal to run expansion by minors. The ideal row or column is
		determined by figuring out which one has the most zeros.

		.. c:member:: unsigned short index;

			A row or column number.

		.. c:member:: along_row;

			1u if ``index`` refers to a row. 0u if it refers to a column.
	*/

	unsigned short index;
	unsigned short along_row;

};


/*
.. c:function:: extern double matrix_determinant(MATRIX m);

	Compute the determinant of a square matrix.

	Parameters
	----------
	m : ``MATRIX``
		The matrix itself.

	Returns
	-------
	det : ``double``
		:math:`det(m)`, computed via expansion by minors in the first row of
		the matrix. 

	Notes
	-----
	The expansion by minors is implemented recursively within an iterative sum,
	with the solution for a 2x2 matrix implemented as the base case. As a
	failsafe, the obvious solution for a 1x1 matrix is implemented as an
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
			double result = 0.f;
			struct row_column_index ideal_axis = matrix_determinant_ideal_axis(m);
			if (ideal_axis.along_row) {
				for (unsigned short j = 0u; j < m.n_cols; j++) {
					if (m.matrix[ideal_axis.index][j] != 0) {
						unsigned short axis[2] = {ideal_axis.index, j};
						MATRIX *minor = matrix_minor(m, axis, NULL);
						result += m.matrix[ideal_axis.index][j] * pow(
							-1, ideal_axis.index + j) * matrix_determinant(
							*minor);
						matrix_free(minor);
					}
				}
			} else {
				for (unsigned short i = 0u; i < m.n_rows; i++) {
					if (m.matrix[i][ideal_axis.index] != 0) {
						unsigned short axis[2] = {i, ideal_axis.index};
						MATRIX *minor = matrix_minor(m, axis, NULL);
						result += m.matrix[i][ideal_axis.index] * pow(
							-1, i + ideal_axis.index) * matrix_determinant(
							*minor);
						matrix_free(minor);
					} else {}
				}
			}
			return result;
		}

	} else {
		fatal_print("%s\n",
			"Cannot compute the determinant of a non-square matrix.");
	}

}


/*
.. c:function:: static struct row_column_index matrix_determinant_ideal_axis(MATRIX m);

	Determine the ideal axis along which expansion by minors will be most
	efficient by determining which row or column has the most zeros.

	Parameters
	----------
	m : ``MATRIX``
		The matrix whose determinant is to be computed.

	Returns
	-------
	axis : ``struct row_column_index``
		The ``struct`` containing the index of the row or column and a boolean
		flag describing whether it is a row or a column.
*/
static struct row_column_index matrix_determinant_ideal_axis(MATRIX m) {

	struct row_column_index axis;
	axis.index = 0u;
	axis.along_row = 1u;
	unsigned short current_max = matrix_zeros_along_axis(m,
		axis.index, axis.along_row);
	for (unsigned short i = 1u; i < m.n_rows; i++) {
		if (matrix_zeros_along_axis(m, i, 1u) > current_max) {
			axis.index = i;
		} else {}
	}
	for (unsigned short j = 0u; j < m.n_cols; j++) {
		if (matrix_zeros_along_axis(m, j, 0u) > current_max) {
			axis.index = j;
			axis.along_row = 0u;
		} else {}
	}
	return axis;

}


/*
.. c:function:: static unsigned short matrix_zeros_along_axis(MATRIX m, const unsigned short index, const unsigned short along_row);

	Determine the number of zeros along a given row or column of a matrix.

	Parameters
	----------
	m : ``MATRIX``
		The matrix in question.
	index : ``const unsigned short``
		The row or column number.
	along_row : ``const unsigned short``
		1u if ``index`` refers to a row number, 0u if it refers to a column.

	Returns
	-------
	n : ``unsigned short``
		The number of zeros along the given row or column.
*/
static unsigned short matrix_zeros_along_axis(MATRIX m,
	const unsigned short index, const unsigned short along_row) {

	unsigned short n = 0;
	if (along_row) {
		for (unsigned short j = 0u; j < m.n_cols; j++) {
			n += m.matrix[index][j] == 0;
		}
	} else {
		for (unsigned short i = 0u; i < m.n_rows; i++) {
			n += m.matrix[i][index] == 0;
		}
	}
	return n;

}


/*
.. cpp:function:: static MATRIX *matrix_adjoint(MATRIX m, MATRIX *result);

	Compute the adjoint of a square matrix.

	Parameters
	----------
	m : ``MATRIX``
		The input matrix itself.
	result : ``MATRIX *``
		A pointer to an already-initialized :c:type:`MATRIX` object to store
		the adjoint matrix, if applicable. If ``NULL``, memory will be
		allocated automatically. If a pointer is provided, the same pointer
		will be returned.

	Returns
	-------
	result : ``MATRIX *``
		The adjoint, defined as the transpose of the matrix of cofactors
		(i.e. :math:`\text{adj}(M) \equiv \text{cof}(M)^T`).

	.. note::

		Some textbooks and authors use the term adjugate or adjunt instead of
		adjoint. Though we use the term adjoint here, they refer to the same
		thing.
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
		A pointer to an already-initialized :c:type:`MATRIX` object to store
		the cofactor matrix, if applicable. If ``NULL``, memory will be
		allocated automatically. If a pointer is provided, the same pointer
		will be returned.

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
.. cpp:function:: static MATRIX *matrix_minor(MATRIX m, unsigned short axis[2], MATRIX *result);

	Obtain one of a matrix's minors.

	Parameters
	----------
	m : ``MATRIX``
		The input matrix itself.
	axis : ``unsigned short [2]``
		Which minor to obtain. axis[0] refers to the row to omit from ``m``,
		and axes[1] to the column.
	result : ``MATRIX *``
		A pointer to an already-initialized :c:type:`MATRIX` object to store
		the minor, if applicable. If ``NULL``, memory will be allocated
		automatically. If a pointer is provided, the same pointer will be
		returned.

	Returns
	-------
	result : ``matrix``
		If ``m`` is an :math:`MxN` matrix, the returned matrix will be
		:math:`(M-1)x(N-1)`, with the ``axis[0]``'th row and the
		``axes[1]``'st column omitted from the original.
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
.. cpp:function:: static void matrix_resize(MATRIX *M, const unsigned short n_rows, const unsigned short n_cols);

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
