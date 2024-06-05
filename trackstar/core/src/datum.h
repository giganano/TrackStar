/*
This file is part of the TrackStar package.
Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
License: MIT License. See LICENSE in top-level directory
at: https://github.com/giganano/TrackStar.git.

This header file includes the features associated with data vectors and
measurement uncertainties.

**Source File**: ``trackstar/core/src/datum.c``
*/

#ifndef DATUM_H
#define DATUM_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "matrix.h"

typedef struct datum {

	/*
	.. c:type:: DATUM

		An arbitrary data vector in some observed space.

		.. c:member:: double **vector

			The data vector itself, stored as a pointer to an array.

		.. c:member:: unsigned short n_rows

			The number of rows in the vector. In practice, this will be
			exactly 1 for row vectors, but >1 for column vectors.

		.. c:member:: unsigned short n_cols

			The number of columns in the vector. In practice, this will be
			exactly 1 for column vectors, but >1 for row vectors.

		.. c:member:: COVARIANCE_MATRIX *cov

			The covariance matrix containing information on the measurement
			uncertainties associated with the data vector.

		.. c:member:: char **labels

			An array of string labels describing the quantities that are
			measured for this datum.

		.. note::

			While it is redundant for this ``struct`` to have both
			:c:member:`n_rows` and :c:member:`n_cols` attributes, the inclusion
			of both makes this ``struct`` a compatible pointer type with
			:c:type:`MATRIX`, allowing type-casting. This is also the
			motivation behind giving :c:member:`vector` the type ``double **``
			as opposed to ``double *``. This approach greatly simplifies
			TrackStar's backend by allowing different types to be passed to the
			matrix algebra routines declared in ``matrix.h``.
	*/

	double **vector;
	unsigned short n_rows;
	unsigned short n_cols;
	COVARIANCE_MATRIX *cov;
	char **labels;

} DATUM;

/*
.. c:function:: extern DATUM *datum_initialize(double *arr, unsigned short dim);

	Allocate memory for an return a pointer to a :c:type:`DATUM` object.

	Parameters
	----------
	dim : ``unsigned short``
		The dimensionality of the data vector (i.e., the number of entries in
		``arr``).

	Returns
	-------
	d : ``DATUM *``
		The newly constructed data vector.
*/
extern DATUM *datum_initialize(unsigned short dim);

/*
.. c:function:: extern void datum_free(DATUM *d);

	Free up the memory associated with a :c:type:`DATUM` object that is
	user-facing.

	Parameters
	----------
	d : ``DATUM *``
		The datum to be freed.

	Notes
	-----
	In practice, this function should only be called upon exiting the python
	interpreter, or when a user calls ``del`` on their ``DATUM`` object.

	The important difference between this function and ``datum_free_everything``
	is that this function does not free *every* block of memory stored by a
	``DATUM``. Doing so causes memory errors because Cython automatically calls
	the necessary ``__dealloc__`` functions that do free up the required blocks
	of memory. Namely, this function does not call
	``covariance_matrix_free(d -> cov))`` or ``matrix_free( (MATRIX *) d)``,
	because Cython calls ``matrix.__dealloc__`` with the same address as
	``datum._d``.
*/
extern void datum_free(DATUM *d);

/*
.. c:function:: extern void datum_free_everything(DATUM *d);

	Free up the memory associated with a :c:type:`DATUM` object that is *not*
	user-facing.

	Parameters
	----------
	d : ``DATUM *``
		The data vector to be freed.

	Notes
	-----
	In practice, this function should only be called for ``DATUM`` objects
	created in TrackStar's C library or cdef'ed instances created in Cython
	that are not returned to the user.

	.. seealso::

		See "Notes" under function ``datum_free`` for details on the
		differences between the two functions.
*/
extern void datum_free_everything(DATUM *d);

/*
.. c:function:: extern DATUM *datum_specific_quantities(DATUM d, char **labels, unsigned short n_labels);

	Obtain a pointer to a :c:type:`DATUM` object containing the relevant
	information for only *some* of the quantities stored in a given
	:c:type:`DATUM`.

	Parameters
	----------
	d : ``DATUM``
		The input data vector.
	labels : ``char **``
		The column labels to pull from the datum object.
	n_labels : ``unsigned short``
		The number of elements in ``labels``.

	Returns
	-------
	sub : ``DATUM *``
		A new :c:type:`DATUM`, containing only the labels, vector components,
		and covariance matrix entries associated with particular measurements.
		``NULL`` if none of the the elements of ``labels`` match the vector
		components of ``d``.
*/
extern DATUM *datum_specific_quantities(DATUM d, char **labels,
	unsigned short n_labels);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DATUM_H */
