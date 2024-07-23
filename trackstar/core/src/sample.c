/*
This file is part of the TrackStar package.
Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
License: MIT License. See LICENSE in top-level directory
at: https://github.com/giganano/TrackStar.git.
*/

#include <stdlib.h>
#include "sample.h"
#include "datum.h"
#include "utils.h"


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
extern SAMPLE *sample_initialize(void) {

	SAMPLE *s = (SAMPLE *) malloc (sizeof(SAMPLE));
	s -> n_vectors = 0ul;
	s -> data = NULL;
	return s;

}


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
extern void sample_free(SAMPLE *s) {

	if (s != NULL) {

		/*
		Cython will automatically call datum_free() for each individual data
		vector stored by the sample, because they are created separately and
		added to the sample via calls from python.
		*/

		if ((*s).data != NULL) free(s -> data);
		free(s);

	} else {}

}


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
extern void sample_free_everything(SAMPLE *s) {

	if (s != NULL) {

		for (unsigned long i = 0ul; i < (*s).n_vectors; i++) {
			datum_free_everything(s -> data[i]);
		}
		free(s -> data);
		free(s);

	} else {}

}


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
extern void sample_add_datum(SAMPLE *s, DATUM *d) {

	if ((*s).data == NULL) {
		s -> data = (DATUM **) malloc (sizeof(DATUM *));
	} else {
		s -> data = (DATUM **) realloc (s -> data,
			((*s).n_vectors + 1ul) * sizeof(DATUM *));
	}
	s -> data[s -> n_vectors++] = d;

}


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
	const unsigned short n_labels) {

	SAMPLE *sub = sample_initialize();
	for (unsigned long i = 0ul; i < s.n_vectors; i++) {
		DATUM *d = datum_specific_quantities(*s.data[i], labels, n_labels);
		if (d != NULL) sample_add_datum(sub, d);
	}
	return sub;

}


/*
.. c:function:: extern unsigned long *sample_filter_indices(SAMPLE s, char *label, unsigned short condition_indicator, double value, unsigned short keep_missing_measurements);

	Determine the indices of data vectors that pass some filter condition.

	Parameters
	----------
	s : ``SAMPLE``
		The sample to filter data from.
	label : ``char *``
		The label of the quantity to filter based on.
	condition_indicator : ``unsigned short``
		An integer index indicating the condition to apply. The conditions each
		value corresponds to are given by

		+-----------+-------+
		| Condition | Value |
		+-----------+-------+
		| ==        | 1     |
		+-----------+-------+
		| <         | 2     |
		+-----------+-------+
		| <=        | 3     |
		+-----------+-------+
		| >         | 4     |
		+-----------+-------+
		| >=        | 5     |
		+-----------+-------+

	value : ``double``
		The value to condition based on.
	keep_missing_measurements : ``unsigned short``
		If nonzero, all data vectors that do not have a measurement for the
		quantity being filtered based on will *remain* in the sample. If zero,
		they will be removed. Defaults to 0u from the python API.

	Returns
	-------
	idx : ``unsigned long *``
		A pointer to the number of data vectors that pass the filter and their
		indices in ``s.data``. That is, ``idx[0]`` is the number of elements of
		``s.data`` that satisfy the filter condition, and those data vectors
		can be found at ``s.data[idx[0]]``, ``s.data[idx[1]]``,
		``s.data[idx[2]]``, and so on.

	Example Code
	------------
	To filter a :c:type:`SAMPLE` ``s`` for all values of ``"foo"`` that are
	less than 1, discarding those without measurements of ``"foo"``:

	.. code-block:: c

		sample_filter_indices(s, "foo", 1u, 0u);

	To filter a :c:type:`SAMPLE` ``s`` for all values of ``"bar"`` that are
	greater than or equal to than 0, keeping those without a measurement of
	``"bar"``:

	.. code-block:: c

		sample_filter_indices(s, "bar", 5u, 1u);
*/
extern unsigned long *sample_filter_indices(SAMPLE s, char *label,
	unsigned short condition_indicator, double value,
	unsigned short keep_missing_measurements) {

	unsigned long *indices = (unsigned long *) malloc (sizeof(unsigned long));
	indices[0] = 0ul;

	for (unsigned long i = 0ul; i < s.n_vectors; i++) {

		unsigned short pass;
		signed short colidx = strindex((*s.data[i]).labels, label,
			(*s.data[i]).n_cols);

		if (colidx == -1) {
			pass = keep_missing_measurements;
		} else {
			switch (condition_indicator) {

				case 1:
					/* == */
					pass = (*s.data[i]).vector[0][colidx] == value;
					break;

				case 2:
					/* < */
					pass = (*s.data[i]).vector[0][colidx] < value;
					break;

				case 3:
					/* <= */
					pass = (*s.data[i]).vector[0][colidx] <= value;
					break;

				case 4:
					/* > */
					pass = (*s.data[i]).vector[0][colidx] > value;
					break;

				case 5:
					/* >= */
					pass = (*s.data[i]).vector[0][colidx] >= value;
					break;

				default:
					if (indices != NULL) free(indices);
					return NULL;
			}
		}

		if (pass) {
			indices = (unsigned long *) realloc (indices,
				(indices[0] + 2ul) * sizeof(unsigned long));
			indices[++indices[0]] = i;
		} else {}

	}

	return indices;

}
