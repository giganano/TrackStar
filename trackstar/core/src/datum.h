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
extern DATUM *datum_initialize(double *arr, char **labels, unsigned short dim);

/*
.. cpp:function:: extern void datum_free(DATUM *d);

Free up the memory associated with a ``DATUM`` object.

Parameters
----------
d : ``DATUM *``
	The datum to be freed.
*/
extern void datum_free(DATUM *d);

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
extern double datum_get_item(DATUM d, char *label);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DATUM_H */
