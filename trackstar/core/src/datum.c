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
	d -> labels = (char **) malloc (dim * sizeof(char *));

	unsigned short i;
	for (i = 0u; i < dim; i++) {
		d -> vector[0u][i] = arr[i];
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

		/*
		Do not call covariance_matrix_free( (COVARIANCE_MATRIX *) d -> cov)
		or matrix_free( (MATRIX *) d) here. With the inheritance structure
		implemented in Cython wrapping compatible pointer types, the attribute
		datum._m has the same address as datum._d. Cython will call
		matrix.__dealloc__ for this object automatically. The same is true for
		its covariance_matrix attribute (see also comments in
		covariance_matrix_free function in matrix.c).

		Calling free(d) does not cause an error. That memory block needs to be
		opened up anyway, but the specific pointers stored within the datum
		still need to be freed.
		*/

		if ((*d).labels != NULL) {
			unsigned short i;
			for (i = 0u; i < (*d).n_cols; i++) free(d -> labels[i]);
			free(d -> labels);
		} else {}
		free(d);

	} else {}

}

