/*
This file is part of the TrackStar package.
Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
License: MIT License. See LICENSE in top-level directory
at: https://github.com/giganano/trackstar.git.

This header file includes all of the core matrix algebra types and routines
that are central to TrackStar's backend.

**Source File**: ``trackstar/core/src/matrix.c``
*/

#ifndef MATRIX_H
#define MATRIX_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef PI /* the mathematical constant */
#define PI 3.14159265358979
#endif /* PI */

typedef struct matrix {

	/*
	.. c:type:: MATRIX

		An arbitrary, 2-dimensional matrix of real numbers.

		.. c:member:: double **matrix

			The matrix itself, stored as a pointer to each row.

		.. c:member:: unsigned short n_rows

			The number of rows in the matrix.

		.. c:member:: unsigned short n_cols

			The number of columns in the matrix.

		To obtain the :math:`i,j`'th element of some matrix :math:`m_{ij}`,
		it should be indexed first with row number and second with column
		number according to the rule

		.. code-block:: c

			double item = mat -> matrix[row][column];

		where ``row`` = :math:`i` and ``column`` = :math:`j`.
	*/

	double **matrix;
	unsigned short n_rows;
	unsigned short n_cols;

} MATRIX;

typedef struct covariance_matrix {

	/*
	.. c:type:: COVARIANCE_MATRIX

		An arbitrary covariance matrix describing measurement uncertainties.

		.. c:member:: double **matrix

			The covariance matrix itself, stored as a pointer to each row.

		.. c:member:: unsigned short n_rows

			The number of rows in the covariance matrix.

		.. c:member:: unsigned short n_cols

			The number of columns in the covariance matrix. See note below.

		.. c:member:: MATRIX *inv

			The inverse of a particular covariance matrix.

		In practice, ``n_rows`` and ``n_cols`` must be equal, and TrackStar's
		python API enforces this requirement. Although these two attributes are
		redundant, including them both allows :c:type:`COVARIANCE_MATRIX` and
		:c:type:`MATRIX` to be compatible pointer types, which enables
		type-casting. Because of this implementation, a
		:c:type:`COVARIANCE_MATRIX` can be passed to TrackStar's core matrix
		algebra routines. For example:

		.. code-block::

			double det = matrix_determinant( *((MATRIX *) cov) );
	*/

	double **matrix;
	unsigned short n_rows;
	unsigned short n_cols;
	MATRIX *inv;

} COVARIANCE_MATRIX;

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
extern MATRIX *matrix_initialize(unsigned short n_rows, unsigned short n_cols);


/*
.. c:function:: extern void matrix_free(MATRIX *m);

	Free up the memory associated with a :c:type:`MATRIX` object.

	Parameters
	----------
	m : ``MATRIX *``
		A pointer to the :c:type:`MATRIX` object whose memory is to be freed.
*/
extern void matrix_free(MATRIX *m);

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
extern void covariance_matrix_free(COVARIANCE_MATRIX *cov);

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
extern void covariance_matrix_free_everything(COVARIANCE_MATRIX *cov);

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
extern MATRIX *matrix_add(MATRIX m1, MATRIX m2, MATRIX *result);

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
extern MATRIX *matrix_subtract(MATRIX m1, MATRIX m2, MATRIX *result);

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
extern MATRIX *matrix_multiply(MATRIX m1, MATRIX m2, MATRIX *result);

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
extern MATRIX *matrix_invert(MATRIX m, MATRIX *result);

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
extern MATRIX *matrix_transpose(MATRIX m, MATRIX *result);

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
extern double matrix_determinant(MATRIX m);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MATRIX_H */
