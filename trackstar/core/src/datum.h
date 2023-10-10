/*
This file is part of the TrackStar package.
Copyright (C) James W. Johnson (giganano9@gmail.com)
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

	// MATRIX *vector;
	double **vector;
	unsigned short n_rows;
	unsigned short n_cols;
	COVARIANCE_MATRIX *cov;

} DATUM;

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
extern DATUM *datum_initialize(double *arr, unsigned short dim);

/*
.. cpp:function:: extern void datum_free(DATUM *d);

Free up the memory associated with a ``DATUM`` object.

Parameters
----------
d : ``DATUM *``
	The datum to be freed.
*/
extern void datum_free(DATUM *d);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DATUM_H */


