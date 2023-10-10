/*
This file is part of the TrackStar package.
Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
License: MIT License. See LICENSE in top-level directory
at: https://github.com/giganano/TrackStar.git.
*/


#include <stdlib.h>
#include "datum.h"
#include "matrix.h"


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
dim : ``unsigned short``
	The dimensionality of the data vector (i.e., the number of entries in
	``arr``).

Returns
-------
d : ``DATUM *``
	The newly constructed data vector object.
*/
extern DATUM *datum_initialize(double *arr, unsigned short dim) {

	DATUM *d = (DATUM *) matrix_initialize(1u, dim);
	d = (DATUM *) realloc (d, sizeof(DATUM));
	d -> cov = (COVARIANCE_MATRIX *) matrix_initialize(dim, dim);
	d -> cov = (COVARIANCE_MATRIX *) realloc (d -> cov,
		sizeof(COVARIANCE_MATRIX));
	d -> cov -> inv = matrix_initialize(dim, dim);

	unsigned short i;
	for (i = 0u; i < dim; i++) {
		d -> vector[0u][i] = arr[i];
		d -> cov -> matrix[i][i] = 1.0;
		d -> cov -> inv -> matrix[i][i] = 1.0;
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

		if ((*d).cov != NULL) covariance_matrix_free(
			(COVARIANCE_MATRIX *) d -> cov);
		matrix_free( (MATRIX *) d);

	} else {}

}
