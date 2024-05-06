/*
This file is part of the TrackStar package.
Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
License: MIT License. See LICENSE in top-level directory
at: https://github.com/giganano/TrackStar.git.
*/

// #if defined(_OPENMP)
// 	#include <omp.h>
// #endif
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "multithread.h"
#include "likelihood.h"
#include "utils.h"
#include "debug.h"

/* ---------- Static function comment headers not duplicated here ---------- */
static double chi_squared(DATUM d, TRACK t, const unsigned short index);
static double corrective_factor(DATUM d, TRACK t, const unsigned short index);
static TRACK *track_subset(DATUM d, TRACK t);
static MATRIX *trackpoint(TRACK t, const unsigned short index);


/*
.. cpp:function:: extern double loglikelihood_sample(SAMPLE *s, TRACK *t);

Compute the natural logarithm of the likelihood that some sample of data vectors
would be observed given some model-predicted track through the observed space.
Not every datum need have a measurement for every quantity (i.e., some
difficult-to-measure quantities may not be available for every datum; this will
be taken care of automatically).

The exact functional form of the likelihood function is described in section 3
of Johnson et al. (2022) [1]_ and derived in detail in their Appendix A. See
the TrackStar Science Documentation for discussion and further details.

Parameters
----------
s : ``SAMPLE *``
	The sample to fit the model to.
t : ``TRACK *``
	The model-predicted track through the observed space.

Returns
-------
logl : ``double``
	The natural logarithm of the likelihood of observation, marginalized over
	the full length of the track and weighted according to the predicted observed
	density along the track.

References
----------
.. [1] Johnson J.W., et al., 2022, MNRAS, 526, 5084
*/
extern double loglikelihood_sample(SAMPLE *s, TRACK *t) {

	double logl = 0.0;
	#if defined(_OPENMP)
		/*
		Parallelized likelihood calculation is done separately for each
		individual thread, because an iterative sum is not thread-safe. Total
		is then added up at the end, after the threads have closed.
		*/
		double *by_thread = (double *) malloc ((*s).n_threads * sizeof(double));
		#pragma omp parallel for num_threads((*s).n_threads)
	#endif
	for (unsigned short i = 0u; i < (*s).n_vectors; i++) {
		#if defined(_OPENMP)
			by_thread[omp_get_thread_num()] += loglikelihood_datum(
				s -> data[i], t);
		#else
			logl += loglikelihood_datum(s -> data[i], t);
		#endif
	}
	#if defined(_OPENMP)
		for (unsigned short i = 0u; i < (*s).n_threads; i++) {
			logl += by_thread[i];
		}
		free(by_thread);
	#endif
	for (unsigned short i = 0u; i < (*t).n_rows; i++) logl -= (*t).weights[i];
	return logl;

}


/*
.. cpp:function:: extern double loglikelihood_datum_backen(DATUM *d, TRACK *t);

Compute the natural logarithm of the likelihood that an individual datum would
be observed from its vector and the model-predicted track in the observed
space. The track may contain predictions for observables that are not available
for the present datum ``d``; this possibility is taken into account
automatically.

The exact functional form of the likelihood function is described in section 3
of Johnson et al. (2022) [1]_ and derived in detail in their Appendix A. See
the TrackStar Science Documentation for discussion and further details.

Parameters
----------
d : ``DATUM``
	The datum to compute the likelihood of observation.
t : ``TRACK *``
	The model-predicted track through the observed space.

Returns
-------
logl : ``double``
	The natural logarithm of the likelihood of observation, marginalized over
	the full length of the track and weighted according to the predicted observed
	density along the track.

References
----------
.. [1] Johnson J.W., et al., 2022, MNRAS, 526, 5084
*/
extern double loglikelihood_datum(DATUM *d, TRACK *t) {

	// matrix_invert(*((MATRIX *) d -> cov), d -> cov -> inv);
	TRACK *sub = track_subset(*d, *t);
	double result = 0;
	#if defined(_OPENMP)
		#pragma omp parallel for num_threads((*t).n_threads)
	#endif
	for (unsigned short i = 0u; i < (*sub).n_rows; i++) {
		double s = (*sub).weights[i] * exp(-0.5 * chi_squared(*d, *sub, i));
		if ((*t).use_line_segment_corrections) {
			/*
			Compute the corrective factor using the subsampled track, but use
			the setting of whether or not to even do the calculation and the
			comparison against the specified tolerance from the input track to
			ensure that it will always end up in the right place. Also
			eliminates the need to copy information over between the input and
			subsampled track.
			*/
			double correction = corrective_factor(*d, *sub, i);
			s *= correction;
			t -> line_segment_correction_flag |= (correction >
				(*t).line_segment_correction_tolerance);
		} else {}
		#if defined(_OPENMP)
			/* iterative sums aren't thread safe, so put lock on increment */
			#pragma omp critical
			{
				result += s;
			}
		#else
			result += s;
		#endif
	}
	track_free(sub);
	return log(result);

}


/*
.. cpp:function:: static double chi_squared(DATUM t, TRACK t,
	const unsigned short index);

Compute the value of :math:`\chi^2` for one specific datum and one specific
point along a model-predicted track.

Parameters
----------
d : ``DATUM``
	The input datum vector.
t : ``TRACK``
	The model-predicted track, containing each predicted vector.
index : ``const unsigned short``
	The index of the point along the track to take in computing a value of
	:math:`\chi^2`.

Returns
-------
chisq : ``double``
	The computed value of :math:`\chi^2`, defined according to
	:math:`\chi^2 = \Delta C^{-1} \Delta^T`, where
	:math:`C^{-1}` is the datum's covariance matrix describing its measurement
	uncertainties and :math:`\Delta = d - t_{index}` is the vector difference
	between the datum and the ``index``'th vector along the track.
*/
static double chi_squared(DATUM d, TRACK t, const unsigned short index) {

	MATRIX *tpoint = trackpoint(t, index);
	MATRIX *delta = matrix_subtract(*((MATRIX *) &d), *tpoint, NULL);
	MATRIX *delta_T = matrix_transpose(*delta, NULL);
	MATRIX *first = matrix_multiply(*delta, *(*d.cov).inv, NULL);
	MATRIX *second = matrix_multiply(*first, *delta_T, NULL);
	if ((*second).n_rows == 1u && (*second).n_cols == 1u) {
		double result = second -> matrix[0][0];
		matrix_free(tpoint);
		matrix_free(delta);
		matrix_free(delta_T);
		matrix_free(first);
		matrix_free(second);
		return result;
	} else {
		fatal_print("%s\n",
			"Chi-squared calculation resulted in a matrix larger than 1x1.");
	}

}


/*
.. cpp:function:: static double corrective_factor(DATUM d, TRACK t,
	const unsigned short index);

Compute the corrective factor in the likelihood estimate that accounts for the
finite length of the line segment connecting two consecutive vectors in the
model-predicted track.

Parameters
----------
d : ``DATUM``
	The datum whose likelihood of observation is being computed.
t : ``TRACK``
	The model-predicted track.
index : ``const unsigned short``
	The index of the vector along the track to compute the corrective factor
	for (i.e. which line segment).

Returns
-------
correction : ``double``
	:math:`\beta_{ij}`, defined according to equation A12 in Johnson et al.
	(2022) [1]_.

References
----------
.. [1] Johnson J.W., et al., 2022, MNRAS, 526, 5084
*/
static double corrective_factor(DATUM d, TRACK t, const unsigned short index) {

	if (index < t.n_rows - 1u) {
		MATRIX *tpoint_index = trackpoint(t, index);
		MATRIX *tpoint_next = trackpoint(t, index + 1u);
		MATRIX *delta = matrix_subtract(*((MATRIX *) &d), *tpoint_index, NULL);
		MATRIX *linesegment = matrix_subtract(*tpoint_next, *tpoint_index, NULL);
		MATRIX *linesegment_T = matrix_transpose(*linesegment, NULL);
		MATRIX *first = matrix_multiply(*linesegment, *(*d.cov).inv, NULL);
		MATRIX *second = matrix_multiply(*first, *linesegment_T, NULL);
		double a = second -> matrix[0][0];
		first = matrix_multiply(*delta, *(*d.cov).inv, first);
		second = matrix_multiply(*first, *linesegment_T, second);
		double b = second -> matrix[0][0];
		double correction = sqrt(PI / (2 * a));
		correction *= exp(b * b / (2 * a));
		correction *= erf((a - b) / sqrt(2 * a)) - erf(b / sqrt(2 * a));
		matrix_free(tpoint_index);
		matrix_free(tpoint_next);
		matrix_free(delta);
		matrix_free(linesegment);
		matrix_free(linesegment_T);
		matrix_free(first);
		matrix_free(second);
		return correction;
	} else {
		/*
		The line segment correction implicitly integrates over the full length
		of the line segment. Therefore, the point at the end of the track can
		be treated as a line segment of length 0 that therefore does not
		contribute to the likelihood of observation.
		*/
		return 0.0;
	}

}


/*
.. cpp:function:: static TRACK *track_subset(DATUM d, TRACK t);

Obtain a track object containing only the quantities present in a particular
datum by comparing their column labels.

Parameters
----------
d : ``DATUM``
	The datum vector itself.
t : ``TRACK``
	The model-predicted track through the observed space, which may contain
	predictions for some number of quantities that are not measured for the
	present datum ``d``. Those quantities may however be measured for other
	data vectors in the sample.

Returns
-------
sub : ``TRACK *``
	A new ``TRACK`` object whose columns occur in the same order as they do
	for the datum ``d``, expediting the matrix multiplications that compute
	the likelihood of observing the datum.
*/
static TRACK *track_subset(DATUM d, TRACK t) {

	TRACK *sub = (TRACK *) matrix_initialize(t.n_rows, d.n_cols);
	sub = (TRACK *) realloc (sub, sizeof(TRACK));
	sub -> labels = (char **) malloc ((*sub).n_cols * sizeof(char *));
	for (unsigned short i = 0u; i < (*sub).n_cols; i++) {
		signed short index = strindex(t.labels, d.labels[i], t.n_cols);
		switch(index) {

			case -1:
				free(sub);
				return NULL;

			default:
				sub -> labels[i] = (char *) malloc (
					MAX_LABEL_SIZE * sizeof(char));
				memset(sub -> labels[i], '\0', MAX_LABEL_SIZE);
				strcpy(sub -> labels[i], d.labels[index]);
				#if defined(_OPENMP)
					#pragma omp parallel for num_threads(t.n_threads)
				#endif
				for (unsigned short j = 0u; j < t.n_rows; j++) {
					sub -> predictions[j][i] = t.predictions[j][index];
				}
				break;

		}

	}

	sub -> weights = (double *) malloc ((*sub).n_rows * sizeof(double));
	for (unsigned short i = 0u; i < (*sub).n_rows; i++) {
		sub -> weights[i] = t.weights[i];
	}
	return sub;

}


/*
.. cpp:function:: static MATRIX *trackpoint(TRACK t, const unsigned short index);

Obtain the vector for a specific point along the model predicted track.

Parameters
----------
t : ``TRACK``
	The input track itself, containing some arbitrary number of vectors.
index : ``const unsigned short``
	The index of the vector along the track to capture as a stand-alone matrix.

Returns
-------
mat : ``MATRIX *``
	A pointer to matrix object containing only one row, whose elements are a
	component-wise match to ``t.predictions[index]``.
*/
static MATRIX *trackpoint(TRACK t, const unsigned short index) {

	MATRIX *point = matrix_initialize(1u, t.n_cols);
	for (unsigned short i = 0u; i < t.n_cols; i++) {
		point -> matrix[0][i] = t.predictions[index][i];
	}
	return point;

}
