/*
This file is part of the TrackStar package.
Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
License: MIT License. See LICENSE in top-level directory
at: https://github.com/giganano/TrackStar.git.
*/

#include <stdlib.h>
#include <string.h>
#include "datum.h"
#include "matrix.h"
#include "utils.h"


/*
.. cpp:function:: extern DATUM *datum_initialize(double *arr, unsigned short
	dim);

Allocate memory for an return a pointer to a ``DATUM`` object.
Automatically initializes the diagonal elements of the covariance matrix and
its inverse to 1 while keeping all off-diagonal elements at zero.

Parameters
----------
arr : ``double *``
	The input array to treat as a data vector.
labels : ``char **``
	String labels to attach to each component of the data vector, matched
	component-wise.
dim : ``unsigned short``
	The dimensionality of the data vector (i.e., the number of entries in
	``arr``).

Returns
-------
d : ``DATUM *``
	The newly constructed data vector object.
*/
extern DATUM *datum_initialize(double *arr, char **labels, unsigned short dim) {

	DATUM *d = (DATUM *) matrix_initialize(1u, dim);
	d = (DATUM *) realloc (d, sizeof(DATUM));
	d -> cov = (COVARIANCE_MATRIX *) matrix_initialize(dim, dim);
	d -> cov = (COVARIANCE_MATRIX *) realloc (d -> cov,
		sizeof(COVARIANCE_MATRIX));
	d -> cov -> inv = matrix_initialize(dim, dim);
	d -> labels = (char **) malloc (dim * sizeof(char *));

	unsigned short i;
	for (i = 0u; i < dim; i++) {
		d -> vector[0u][i] = arr[i];
		d -> cov -> matrix[i][i] = 1.0;
		d -> cov -> inv -> matrix[i][i] = 1.0;
		d -> labels[i] = (char *) malloc (MAX_LABEL_SIZE * sizeof(char));
		memset(d -> labels[i], '\0', MAX_LABEL_SIZE);
		strcpy(d -> labels[i], labels[i]);
	}
	return d;

}


/*
.. cpp:function:: extern void datum_free(DATUM *d);

Free up the memory associated with a ``DATUM`` object.

Parameters
----------
d : ``DATUM *``
	The datum to be freed.
*/
extern void datum_free(DATUM *d) {

	if (d != NULL) {

		if ((*d).labels != NULL) {
			unsigned short i;
			for (i = 0u; i < (*d).n_cols; i++) free(d -> labels[i]);
			free(d -> labels);
		} else {}
		if ((*d).cov != NULL) covariance_matrix_free(
			(COVARIANCE_MATRIX *) d -> cov);
		matrix_free( (MATRIX *) d);

	} else {}

}


/*
.. cpp:function:: extern double datum_get_item(DATUM d, char *label);

Obtain a component of the data vector from its string label.

Parameters
----------
d : ``DATUM``
	The ``DATUM`` object itself.
label : ``char *``
	The string label associated with the value of interest.

Returns
-------
value : ``double``
	The numerical value of that component of the data vector. ``NAN`` if the
	label is not associated with ``d``. -9999 if NAN is not defined on the
	user's system.
*/
extern double datum_get_item(DATUM d, char *label) {

	signed short idx = strindex(d.labels, label, d.n_cols);
	if (idx >= 0) {
		return d.vector[0u][idx];
	} else {
		#ifdef NAN
			return NAN;
		#else
			return -9999;
		#endif /* NAN */
	}

}
