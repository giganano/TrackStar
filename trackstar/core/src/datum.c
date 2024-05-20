/*
This file is part of the TrackStar package.
Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
License: MIT License. See LICENSE in top-level directory
at: https://github.com/giganano/TrackStar.git.
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "datum.h"
#include "matrix.h"
#include "utils.h"


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
extern DATUM *datum_initialize(unsigned short dim) {

	DATUM *d = (DATUM *) matrix_initialize(1u, dim);
	d = (DATUM *) realloc (d, sizeof(DATUM));
	d -> labels = (char **) malloc (dim * sizeof(char *));
	for (unsigned short i = 0u; i < dim; i++) {
		d -> labels[i] = (char *) malloc (MAX_LABEL_SIZE * sizeof(char));
		memset(d -> labels[i], '\0', MAX_LABEL_SIZE);
	}
	return d;

}


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
extern void datum_free(DATUM *d) {

	if (d != NULL) {

		if ((*d).labels != NULL) {
			for (unsigned short i = 0u; i < (*d).n_cols; i++) {
				free(d -> labels[i]);
			}
			free(d -> labels);
		} else {}
		free(d);

	} else {}

}


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
extern void datum_free_everything(DATUM *d) {

	covariance_matrix_free_everything(d -> cov);
	if (d != NULL) {
		if ((*d).labels != NULL) {
			for (unsigned short i = 0u; i < (*d).n_cols; i++) {
				free(d -> labels[i]);
			}
			free(d -> labels);
		}
		matrix_free((MATRIX *) d);
	} else {}

}


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
	unsigned short n_labels) {

	/*
	Start by grabbing the integer indices of each label in the data vector.
	Copy their label strings while we're at it.
	*/
	unsigned short n_indices = 0u, *indices = NULL;
	char **label_copies = NULL;
	for (unsigned short i = 0u; i < n_labels; i++) {
		signed short idx = strindex(d.labels, labels[i], d.n_cols);
		if (idx >= 0) {
			if (indices == NULL) {
				indices = (unsigned short *) malloc (sizeof(unsigned short));
				label_copies = (char **) malloc (sizeof(char *));
			} else {
				indices = (unsigned short *) realloc (indices,
					(n_indices + 1u) * sizeof(unsigned short));
				label_copies = (char **) realloc (label_copies,
					(n_indices + 1u) * sizeof(char));
			}
			indices[n_indices] = (unsigned) idx;
			label_copies[n_indices++] = d.labels[idx];
		} else {
			/*
			Doing nothing allows ``sample.loglikelihood`` to work as intended,
			by working only with the available measurements in the event a
			given datum doesn't have measurements for every quantity.
			``datum.likelihood`` raises an error before this function gets to
			this point on the python side.
			*/
		}
	}

	/*
	Now just amass all of the information needed for ``datum_intialize``, and
	once we've got it, copy the covariance matrix over and invert it.
	*/
	if (indices == NULL) return NULL; /* see note in else block above */
	DATUM *sub = datum_initialize(n_indices);
	for (unsigned short i = 0u; i < n_indices; i++) {
		sub -> vector[0][i] = d.vector[0][indices[i]];
		strcpy(sub -> labels[i], label_copies[i]);
	}
	sub -> cov = covariance_matrix_initialize(n_indices);

	for (unsigned short i = 0u; i < n_indices; i++) {
		for (unsigned short j = 0u; j < n_indices; j++) {
			sub -> cov -> matrix[i][j] = (*d.cov).matrix[indices[i]][indices[j]];
		}
	}
	sub -> cov -> inv = matrix_invert( *((MATRIX *) (*sub).cov), NULL );
	free(label_copies);
	free(indices);

	return sub;

}

