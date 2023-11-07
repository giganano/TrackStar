/*
This file is part of the TrackStar package.
Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
License: MIT License. See LICENSE in top-level directory
at: https://github.com/giganano/TrackStar.git
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
	.. cpp:type:: SAMPLE

	A collection of data vectors in some observed space. Some vectors may be
	missing measurements for certain quantities.

	Attributes
	----------
	data : ``DATUM **``
		The collection of vectors themselves, each stored as a pointer to a
		``DATUM`` object.
	n_vectors : ``unsigned long``
		The number of vectors in ``data`` (i.e., the sample size).
	n_threads : ``unsigned short``
		The number of threads to use in looping over the sample.
	*/

	DATUM **data;
	unsigned long n_vectors;
	unsigned short n_threads;

} SAMPLE;

/*
.. cpp:function:: extern SAMPLE *sample_initialize(unsigned long sample_size);

Allocate memory for and return a pointer to a ``SAMPLE`` object.
Automatically allocates memory for the appropriate number of ``DATUM`` objects,
but does not allocate memory for each individual ``DATUM`` stored. That is,
after calling this function, the user will have to call ``datum_initialize``
the appropriate number of times to copy their data into this object.

Parameters
----------
sample_size : ``unsigned long``
	The number of data vectors in the sample. This will be copied over to the
	attribute ``n_vectors`` of the newly constructed object.

Returns
-------
s : ``SAMPLE *``
	The newly constructed sample, ready to have its vectors initialized.
*/
extern SAMPLE *sample_initialize(unsigned long sample_size);

/*
.. cpp:function:: extern void sample_free(SAMPLE *s);

Free up the memory associated with a ``SAMPLE`` object.

Parameters
----------
s : ``SAMPLE *``
	The sample to be freed.
*/
extern void sample_free(SAMPLE *s);

/*
.. cpp:function:: extern unsigned short invert_covariance_matrices(SAMPLE *s);

Invert all covariance matrices stored within a ``SAMPLE`` object.

Parameters
----------
s : ``SAMPLE *``
	The sample, containing some number of data vectors, whose covariance
	matrices are to be inverted.

Returns
-------
0 on success. 1 if one or more covariance matrices is not invertible.
*/
extern unsigned short invert_covariance_matrices(SAMPLE *s);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SAMPLE_H */
