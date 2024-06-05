/*
This file is part of the TrackStar package.
Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
License: MIT License. See LICENSE in top-level directory
at https://github.com/giganano/TrackStar.git.
*/

#include <stdlib.h>
#include <string.h>
#include "track.h"
#include "matrix.h"
#include "datum.h"
#include "utils.h"

/*
.. c:function:: extern TRACK *track_initialize(double **predictions, char **labels, double *weights, unsigned short n_vectors, unsigned short dim);

	Allocate memory for and return a pointer to a :c:type::`TRACK` object,
	containing the data vectors assocaited with model predictions along an
	infinitely thin line in some observed space.

	Parameters
	----------
	n_vectors : ``unsigned short``
		The number of prediction vectors (i.e., the number of elements along
		the *first* axis of indexing in :c:member:`predictions`).
	dim : ``unsigned short``
		The dimensionality of the predictions (i.e., the number of elements
		along the *second* axis of indexing in :c:member:`predictions`).

	Returns
	-------
	t : ``TRACK *``
		The newly constructed :c:type:`TRACK` object.
*/
extern TRACK *track_initialize(unsigned short n_vectors, unsigned short dim) {

	TRACK *t = (TRACK *) malloc (sizeof(TRACK));
	t -> n_vectors = n_vectors;
	t -> dim = dim;
	t -> n_threads = 1u;
	t -> normalize_weights = 1u;
	t -> use_line_segment_corrections = 0u;
	t -> predictions = (double **) malloc (n_vectors * sizeof(double *));
	t -> labels = (char **) malloc (dim * sizeof(char *));
	t -> weights = (double *) malloc (n_vectors * sizeof(double));

	for (unsigned short i = 0u; i < n_vectors; i++) {
		t -> predictions[i] = (double *) malloc (dim * sizeof(double));
	}

	for (unsigned short i = 0u; i < dim; i++) {
		t -> labels[i] = (char *) malloc (MAX_LABEL_SIZE * sizeof(char));
		memset(t -> labels[i], '\0', MAX_LABEL_SIZE);
	}

	return t;

}


/*
.. c:function:: extern void track_free(TRACK *t);

	Free up the memory stored by a :c:type:`TRACK` object.

	Parameters
	----------
	t : ``TRACK *``
		The :c:type:`TRACK` object to be freed.
*/
extern void track_free(TRACK *t) {

	if (t != NULL) {

		if ((*t).labels != NULL) {
			unsigned short i;
			for (i = 0u; i < (*t).dim; i++) free(t -> labels[i]);
			free(t -> labels);
		} else {}
		if ((*t).weights != NULL) free(t -> weights);
		matrix_free( (MATRIX *) t);

	} else {}

}
