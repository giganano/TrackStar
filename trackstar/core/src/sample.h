/*
This file is part of the TrackStar package.
Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
License: MIT License. See LICENSE in top-level directory
at: https://github.com/giganano/TrackStar.git.

**Source File**: ``trackstar/core/src/sample.c``
*/

#ifndef SAMPLE_H
#define SAMPLE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "matrix.h"
#include "datum.h"

typedef struct sample {

	/*
	.. c:type:: SAMPLE

		A collection of data vectors in some observed space. Not all vectors
		need to have a measurement for every quantity.

		.. c:member:: DATUM **d

			The collection of data vectors themselves, each stored as a pointer
			to a :c:type:`DATUM`.

		.. c:member:: unsigned long n_vectors

			The number of vectors in :c:member:`data` (i.e. the sample size).
	*/

	DATUM **data;
	unsigned long n_vectors;

} SAMPLE;

/*
.. c:function:: extern SAMPLE *sample_initialize(unsigned long sample_size);

	Allocate memory for and return a pointer to a :c:type:`SAMPLE` object.
	Initially creates an empty sample, after which
	:c:func:`sample_add_datum` should be called with the appropriate
	:c:type:`DATUM` objects.

	Returns
	-------
	s : ``SAMPLE *``
		The newly constructed sample.
*/
extern SAMPLE *sample_initialize(void);

/*
.. c:function:: extern void sample_free(SAMPLE *s);

	Free up the memory associated with a :c:type:`SAMPLE` object that is
	user-facing.

	Parameters
	----------
	s : ``SAMPLE *``
		The sample to be freed.

	Notes
	-----
	In practice, this function should only be called upon exiting the python
	interpreter, or when a user calls ``del`` on their :c:type:`SAMPLE` object.

	The important difference between this function and
	:c:func:`sample_free_everything` is that this function does not free
	*every* block of memory stored by a :c:type:`SAMPLE` object. Doing so
	causes memory errors because Cython automatically calls the necessary
	``__dealloc__`` functions that do free up the required blocks of memory.
	Namely, this function does not call :c:func:`datum_free` for each
	individual :c:type:`DATUM` stored by ``s``, because Cython calls
	``datum.__dealloc__`` with the same addresses as ``sample.data``.
*/
extern void sample_free(SAMPLE *s);

/*
.. c:function:: extern void sample_free_everything(SAMPLE *s);

	Free up the memory associated with a :c:type:`SAMPLE` object that is *not*
	user-facing.

	Notes
	-----
	In practice, this function should only be called for :c:type:`SAMPLE`
	objects created in TrackStar's C library or ``cdef``'ed instances created
	in Cython that are not returned to the user.

	.. seealso::

		See "Notes" under function :c:func:`sample_free` for details on the
		differences between these two functions.
*/
extern void sample_free_everything(SAMPLE *s);

/*
.. c:function:: extern void sample_add_datum(SAMPLE *s, DATUM *d);

	Add a new data vector to the sample.

	Parameters
	----------
	s : ``SAMPLE *``
		The :c:type:`SAMPLE` object, which potentially already contains other
		data vectors already added to the sample.
	d : ``DATUM *``
		The vector to be included in this sample.
*/
extern void sample_add_datum(SAMPLE *s, DATUM *d);

/*
.. c:function:: extern SAMPLE *sample_specific_quantities(SAMPLE s, char **labels, unsigned short n_labels);

	Obtain a pointer to a :c:type:`SAMPLE` object containing the relevant
	information for only *some* of the quantities stored in a given
	:c:type:`SAMPLE`.

	Parameters
	----------
	s : ``SAMPLE``
		The input sample of data vectors.
	labels : ``char **``
		The column labels to pull from the sample object.
	n_labels : ``unsigned short``
		The number of elements in ``label``.

	Returns
	-------
	sub : ``SAMPLE *``
		A new :c:type:`SAMPLE` object, containing only the labels, vector
		components, and covariance matrix entries associated with particular
		measurements.
*/
extern SAMPLE *sample_specific_quantities(SAMPLE s, char **labels,
	const unsigned short n_labels);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SAMPLE_H */
