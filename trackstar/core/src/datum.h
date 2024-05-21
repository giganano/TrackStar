/*
This file is part of the TrackStar package.
Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
License: MIT License. See LICENSE in top-level directory
at: https://github.com/giganano/TrackStar.git.
*/

#ifndef DATUM_H
#define DATUM_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "matrix.h"

typedef struct datum {

	/*
	.. cpp:type:: DATUM

	An arbitrary data vector in some observed space, a pointer type compatible
	with ``MATRIX``.

	Attributes
	----------
	vector : ``double **``
		The data vector itself, stored as a pointer to an array.
	n_rows : ``unsigned short``
		The number of rows in the vector. In practice, this will be exactly 1
		for row vectors, but >1 for column vectors.
	n_cols : ``unsigned short``
		The number of columns in the vector. In practice, this will be exactly
		1 for column vectors, but >1 for row vectors.
	cov : ``COVARIANCE_MATRIX *``
		The covariance matrix containing information on the measurement
		uncertainties associated with the data vector.
	labels : ``char **``
		An array of string labels describing the quantities that are measured
		for this datum.

	.. note::

		Though it is redundant for vectors to have both ``n_rows`` and
		``n_cols`` attributes, the inclusion of both of them allows this
		object to be type-cast to a ``MATRIX`` through pointer compatibility.
		This feature simplifies the implementation by allowing this object
		to be passed to the library of more generic matrix algebra routines
		declared in ``matrix.h``.
	*/

	double **vector;
	unsigned short n_rows;
	unsigned short n_cols;
	COVARIANCE_MATRIX *cov;
	char **labels;

} DATUM;

/*
.. cpp:function:: extern DATUM *datum_initialize(double *arr, unsigned short
	dim);

Allocate memory for an return a pointer to a ``DATUM`` object.

Parameters
----------
dim : ``unsigned short``
	The dimensionality of the data vector (i.e., the number of entries in
	``arr``).

Returns
-------
d : ``DATUM *``
	The newly constructed data vector object.
*/
extern DATUM *datum_initialize(unsigned short dim);

/*
.. cpp:function:: extern void datum_free(DATUM *d);

Free up the memory associated with a ``DATUM`` object that is user-facing.

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
the necessary ``__dealloc__`` functions that do free up the required blocks of
memory. Namely, this function does not call
``covariance_matrix_free(d -> cov))`` or ``matrix_free( (MATRIX *) d)``,
because Cython calls ``matrix.__dealloc__`` with the same address as
``datum._d``.
*/
extern void datum_free(DATUM *d);

/*
.. cpp:function:: extern void datum_free_everything(DATUM *d);

Free up the memory associated with a ``DATUM`` object that is *not* user-facing.

Parameters
----------
d : ``DATUM *``
	The datum to be freed.

Notes
-----
In practice, this function should only be called for ``DATUM`` objects created
in TrackStar's C library or cdef'ed instances created in Cython that are not
returned to the user.

.. seealso::

	See "Notes" under function ``datum_free`` for details on the differences
	between the two functions.
*/
extern void datum_free_everything(DATUM *d);

/*
.. cpp:function:: extern DATUM *datum_specific_quantities(DATUM d,
	char **labels, unsigned short n_labels);

Obtain a pointer to a ``DATUM`` object containing the relevant information for
only *some* of the quantities stored in a given ``DATUM``.

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
	A new ``DATUM`` object, containing only the labels, vector components, and
	covariance matrix entries associated with particular measurements.
	``NULL`` if none of the the elements of ``labels`` match the vector
	components of ``d``.
*/
extern DATUM *datum_specific_quantities(DATUM d, char **labels,
	unsigned short n_labels);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DATUM_H */
