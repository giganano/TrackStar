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
.. cpp:function:: extern TRACK *track_initialize(double **predictions,
	char **labels, double *weights, unsigned short n_vectors,
	unsigned short dim);

Allocate memory for and return a pointer to a ``TRACK`` object, containing the
data vectors assocaited with model predictions along an infinitely thin line
in some observed space.

Parameters
----------
predictions : ``double **``
	The raw input table of the model predictions as a two-dimensional array.
	The first axis indexes individual vectors while the second indexes
	individual components of single prediction vectors.
labels : ``char **``
	String labels associated with the second axis of indexing in
	``predictions``. In practice, these labels will be used to match the
	columns of the ``TRACK`` to the components of individual data vectors.
weights : ``double *``
	The weights to attach to individual points along the track. In practice,
	these weights should scale as the product of the intrinsic density of
	points expected in the model and the selection function of the data, the
	latter of which may be difficult to quantify.
n_vectors : ``unsigned short``
	The number of prediction vectors (i.e., the number of elements along the
	*first* axis of indexing in ``predictions``).
dim : ``unsigned short``
	The dimensionality of the predictions (i.e., the number of elements along
	the *second* axis of indexing in ``predictions``).

Returns
-------
t : ``TRACK *``
	The newly constructed ``TRACK`` object.
*/
extern TRACK *track_initialize(double **predictions, char **labels,
	double *weights, unsigned short n_vectors, unsigned short dim) {

	TRACK *t = (TRACK *) malloc (sizeof(TRACK));
	t -> predictions = (double **) malloc (n_vectors * sizeof(double *));
	t -> n_rows = n_vectors;
	t -> n_cols = dim;
	t -> labels = (char **) malloc (dim * sizeof(char *));
	t -> weights = (double *) malloc (n_vectors * sizeof(double));

	unsigned short i;
	for (i = 0u; i < (*t).n_rows; i++) {
		t -> weights[i] = weights[i];
		t -> predictions[i] = (double *) malloc ((*t).n_cols * sizeof(double));
		unsigned short j;
		for (j = 0u; j < (*t).n_cols; j++) {
			t -> predictions[i][j] = predictions[i][j];
		}
	}

	for (i = 0u; i < (*t).n_cols; i++) {
		t -> labels[i] = (char *) malloc (MAX_LABEL_SIZE * sizeof(char));
		memset(t -> labels[i], '\0', MAX_LABEL_SIZE);
		strcpy(t -> labels[i], labels[i]);
	}

	return t;

}


/*
.. cpp:function:: extern void track_free(TRACK *t);

Free up the memory stored by a ``TRACK`` object.

Parameters
----------
t : ``TRACK *``
	The track object to be freed.
*/
extern void track_free(TRACK *t) {

	if (t != NULL) {

		if ((*t).labels != NULL) {
			unsigned short i;
			for (i = 0u; i < (*t).n_cols; i++) free(t -> labels[i]);
			free(t -> labels);
		} else {}
		if ((*t).weights != NULL) free(t -> weights);
		matrix_free( (MATRIX *) t);

	} else {}

}
