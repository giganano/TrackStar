/*
This file is part of the TrackStar package.
Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
License: MIT License. See LICENSE in top-level directory
at: https://github.com/giganano/TrackStar.git.
*/

#ifndef TRACK_H
#define TRACK_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct track {

	/*
	.. cpp:type:: TRACK

	A collection of arbitrary data vectors describing a model prediction,
	assumed to be an arbitrary, infinitely thin curve in some observed space.

	Attributes
	----------
	predictions : ``double **``
		The vectors in the observed space themselves. The first axis of indexing
		corresponds to different vectors, and the second axis corresponds to
		different axes of the observed space for the same vector (i.e.,
		different vector components).
	n_rows : ``unsigned short``
		The number of vectors in the observed space (i.e., the number of
		elements along the first axis of indexing in ``predictions``).
	n_cols : ``unsigned short``
		The dimensionality of the observed space (i.e., the number of
		elements along the second axis of indexing in ``predictions``).
	labels : ``char **``
		An array of strings describing the quantities measured (i.e., a label
		for each axis of the observed space). This will be used to match the
		quantities contained within each ``DATUM`` object to compute
		statistical likelihood estimates.
	weights : ``double *``
		Weights to attach to each individual vector in the observed space.
		In practice, these weights should scale as the product of the intrinsic
		density predicted by the model and the selection function of the data,
		the latter of which may be difficult to quantify.
	*/

	double **predictions;
	unsigned short n_rows;
	unsigned short n_cols;
	unsigned short n_threads;
	char **labels;
	double *weights;
	unsigned short use_line_segment_corrections;
	double line_segment_correction_tolerance;
	unsigned short line_segment_correction_flag;

} TRACK;

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
	double *weights, unsigned short n_vectors, unsigned short dim);

/*
.. cpp:function:: extern void track_free(TRACK *t);

Free up the memory stored by a ``TRACK`` object.

Parameters
----------
t : ``TRACK *``
	The track object to be freed.
*/
extern void track_free(TRACK *t);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* TRACK_H */
