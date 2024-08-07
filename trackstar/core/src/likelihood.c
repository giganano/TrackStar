/*
This file is part of the TrackStar package.
Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
License: MIT License. See LICENSE in top-level directory
at: https://github.com/giganano/TrackStar.git.
*/

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "multithread.h"
#include "likelihood.h"
#include "quadrature.h"
#include "matrix.h"
#include "utils.h"
#include "debug.h"
#include "utils.h"

/* ---------- Static function comment headers not duplicated here ---------- */
static double normalize_weights(TRACK *t);
static void unnormalize_weights(TRACK *t, const double weight_norm);
static double chi_squared(DATUM d, TRACK t, const unsigned short index);
static double delta_model(TRACK t, const unsigned short index);
static double corrective_factor(DATUM d, TRACK t, const unsigned short index);
static double corrective_factor_marginalization_integrand(double *args);
static TRACK *track_subset(DATUM d, TRACK t);
static MATRIX *trackpoint(TRACK t, const unsigned short index);
static unsigned short CALLING_FUNCTION = 0u;


/*
.. c:function:: extern double loglikelihood_sample(SAMPLE *s, TRACK *t);

	Compute the natural logarithm of the likelihood that some sample of data
	vectors would be observed given some model-predicted track through the
	observed space. Not every datum need have a measurement for every quantity
	(i.e., some difficult-to-measure quantities may not be available for every
	datum; this will be taken care of automatically).

	The exact functional form of the likelihood function is described in
	section 3 of Johnson et al. (2022) [1]_ and derived in detail in their
	Appendix A. See the TrackStar Science Documentation for discussion and
	further details.

	Parameters
	----------
	s : ``SAMPLE *``
		The sample to fit the model to.
	t : ``TRACK *``
		The model-predicted track through the observed space.

	Returns
	-------
	logl : ``double``
		The natural logarithm of the likelihood of observation, marginalized
		over the full length of the track and weighted according to the
		predicted observed density along the track.

	References
	----------
	.. [1] Johnson J.W., et al., 2022, MNRAS, 526, 5084
*/
extern double loglikelihood_sample(SAMPLE s, TRACK *t) {

	CALLING_FUNCTION = 1u;
	double logl = 0.f, weight_norm = 1.f;
	if ((*t).normalize_weights) weight_norm = normalize_weights(t);
	#if defined(_OPENMP)
		/*
		Parallelized likelihood calculation is done separately for each
		individual thread, because an iterative sum is not thread-safe. Total
		is then added up at the end, after the threads have closed.
		*/
		double *by_thread = (double *) malloc ((*t).n_threads * sizeof(double));
		for (unsigned short i = 0u; i < (*t).n_threads; i++) by_thread[i] = 0;
		#pragma omp parallel for num_threads((*t).n_threads)
	#endif
	for (unsigned long i = 0ul; i < s.n_vectors; i++) {
		#if defined(_OPENMP)
			by_thread[omp_get_thread_num()] += loglikelihood_datum(
				*s.data[i], t);
		#else
			logl += loglikelihood_datum(*s.data[i], t);
		#endif
	}
	#if defined(_OPENMP)
		for (unsigned short i = 0u; i < (*t).n_threads; i++) {
			logl += by_thread[i];
		}
		free(by_thread);
	#endif
	if ((*t).normalize_weights) {
		unnormalize_weights(t, weight_norm);
	} else {
		for (unsigned short i = 0u; i < (*t).n_vectors; i++) {
			logl -= (*t).weights[i];
		}
	}
	CALLING_FUNCTION = 0u;
	return logl;

}


/*
.. c:function:: extern double loglikelihood_datum(DATUM *d, TRACK *t);

	Compute the natural logarithm of the likelihood that an individual datum
	will be observed from its vector and the model-predicted track in the
	observed space. The track may contain predictions for observables that are
	not available for the present datum ``d``; this possibility is taken into
	account automatically.

	The exact functional form of the likelihood function is described in
	section 3 of Johnson et al. (2022) [2]_ and derived in detail in their
	Appendix A.

	Parameters
	----------
	d : ``DATUM``
		The datum to compute the likelihood of observation.
	t : ``TRACK *``
		The model-predicted track through the observed space.

	Returns
	-------
	logl : ``double``
		The natural log of the likelihood of observation, marginalized over the
		full length of the track and weighted according to the predicted
		observed density along the track.

	References
	----------
	.. [2] Johnson J.W., et al., 2022, MNRAS, 526, 5084
*/
extern double loglikelihood_datum(DATUM d, TRACK *t) {

	double weight_norm = 1.f;
	if (CALLING_FUNCTION && (*t).normalize_weights) {
		weight_norm = normalize_weights(t);
	} else {}
	TRACK *sub = track_subset(d, *t);
	if (sub == NULL) fatal_print("%s\n", "track_subset returned NULL.");
	double result = 0;
	#if defined(_OPENMP)
		double *by_thread = (double *) malloc ((*t).n_threads * sizeof(double));
		for (unsigned short i = 0u; i < (*t).n_threads; i++) by_thread[i] = 0;
		#pragma omp parallel for num_threads((*t).n_threads)
	#endif
	for (unsigned short i = 0u; i < (*sub).n_vectors; i++) {
		double s = (*sub).weights[i];
		s *= exp(-0.5 * chi_squared(d, *sub, i));
		s *= delta_model(*sub, i);
		if ((*t).use_line_segment_corrections) {
			/*
			Compute the corrective factor using the subsampled track, but use
			the setting of whether or not to even do the calculation and the
			comparison against the specified tolerance from the input track to
			ensure that it will always end up in the right place. Also
			eliminates the need to copy information over between the input and
			subsampled track.
			*/
			s *= corrective_factor(d, *sub, i);
		} else {}
		#if defined(_OPENMP)
			by_thread[omp_get_thread_num()] += s;
		#else
			result += s;
		#endif
	}
	#if defined(_OPENMP)
		for (unsigned short i = 0u; i < (*t).n_threads; i++) {
			result += by_thread[i];
		}
		free(by_thread);
	#endif
	if (CALLING_FUNCTION && (*t).normalize_weights) {
		unnormalize_weights(t, weight_norm);
	} else {}
	track_free(sub);
	result /= sqrt(2 * PI * matrix_determinant( *((MATRIX *) d.cov) ));
	return log(result);

}


/*
.. c:function:: static double normalize_weights(TRACK *t);

	Normalize the weights stored by a :c:type:`TRACK` object such that they add
	up to 1.

	Parameters
	----------
	t : ``TRACK *``
		A pointer to the input track itself

	Returns
	-------
	weight_norm : ``double``
		The sum of the weights prior to normalization.
*/
static double normalize_weights(TRACK *t) {

	double weight_norm = sum((*t).weights, (*t).n_vectors);
	weight_norm *= 1000.f / (*t).n_vectors;
	for (unsigned short i = 0u; i < (*t).n_vectors; i++) {
		t -> weights[i] /= weight_norm;
	}
	return weight_norm;

}


/*
.. c:function:: static void unnormalize_weights(TRACK *t, const double weight_norm);

	Undo the action of :c:function:`normalize_weights``, restting the track
	weights to their original values.

	Parameters
	----------
	t : ``TRACK *``
		A pointer to the input track itself.
	weight_norm : ``double``
		The value returned by :c:type:`normalize_weights`.
*/
static void unnormalize_weights(TRACK *t, const double weight_norm) {

	for (unsigned short i = 0u; i < (*t).n_vectors; i++) {
		t -> weights[i] *= weight_norm;
	}

}


/*
.. c:function:: static double chi_squared(DATUM t, TRACK t, const unsigned short index);

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
		:math:`C^{-1}` is the datum's covariance matrix describing its
		measurement uncertainties and :math:`\Delta = d - t_{index}` is the
		vector difference between the datum and the ``index``'th vector along
		the track.
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
.. c:function:: static delta delta_model(TRACK t, const unsigned short index);

	Computes the magnitude of the vector displacement between neighboring points
	on the track.

	Parameters
	----------
	t : ``TRACK``
		The track itself, containing all vectors.
	index : ``unsigned short``
		The index of the vector along the track to compute the :math:`\Delta M`
		at.

	Returns
	-------
	delta : ``double``
		The magnitude of the :math:`\Delta M_j = M_{j + 1} - M_j` vector.
*/
static double delta_model(TRACK t, const unsigned short index) {

	if (index < t.n_vectors - 1ul) {
		MATRIX *current = trackpoint(t, index);
		MATRIX *next = trackpoint(t, index + 1ul);
		MATRIX *delta = matrix_subtract(*next, *current, NULL);
		/* compute magnitude of delta vector in data space */
		double mag = 0;
		for (unsigned short i = 0u; i < (*delta).n_cols; i++) {
			mag += (*delta).matrix[0][i] * (*delta).matrix[0][i];
		}
		mag = sqrt(mag);
		free(current);
		free(next);
		free(delta);
		return mag;
	} else {
		return 0.f;
	}

}


/*
.. c:function:: static double corrective_factor(DATUM d, TRACK t, const unsigned short index);

	Compute the corrective factor in the likelihood estimate that accounts for
	the finite length of the line segment connecting two consecutive vectors in
	the model-predicted track.

	Parameters
	----------
	d : ``DATUM``
		The datum whose likelihood of observation is being computed.
	t : ``TRACK``
		The model-predicted track.
	index : ``const unsigned short``
		The index of the vector along the track to compute the corrective
		factor for (i.e. which line segment).

	Returns
	-------
	correction : ``double``
		:math:`\beta_{ij}`, defined according to equation A12 in Johnson et al.
		(2022) [1]_.

	Notes
	-----
	Although the exact form of :math:`\beta_{ij}` is known, it is a rare case in
	 which the analytic solution is not numerically stable while the numerical
	solution is. In this case, the instability arises because :math:`\beta_{ij}`
	is the product of an extremely large number and an extremely small number,
	which challenge the limits of double floating point precision. Once the
	values of :math:`a` and :math:`b` are known, which define the value of
	:math:`beta` (see science documentation), its value is computed through
	quadrature.

	References
	----------
	.. [1] Johnson J.W., et al., 2022, MNRAS, 526, 5084
*/
static double corrective_factor(DATUM d, TRACK t, const unsigned short index) {

	if (index < t.n_vectors - 1u) {
		/*
		Determine the values of the a and b coefficients, which define the
		corrective factor.
		*/
		MATRIX *tpoint_index = trackpoint(t, index);
		MATRIX *tpoint_next = trackpoint(t, index + 1u);
		MATRIX *delta = matrix_subtract(*((MATRIX *) &d), *tpoint_index, NULL);
		MATRIX *linesegment = matrix_subtract(*tpoint_next, *tpoint_index, NULL);
		MATRIX *linesegment_T = matrix_transpose(*linesegment, NULL);
		MATRIX *first = matrix_multiply(*linesegment, *(*d.cov).inv, NULL);
		MATRIX *second = matrix_multiply(*first, *linesegment_T, NULL);
		if ((*second).n_rows != 1u || (*second).n_cols != 1u) {
			fatal_print("%s\n",
				"Line segment correction (a): matrix larger than 1x1.");
		} else {}
		double a = second -> matrix[0][0];
		first = matrix_multiply(*delta, *(*d.cov).inv, first);
		second = matrix_multiply(*first, *linesegment_T, second);
		if ((*second).n_rows != 1u || (*second).n_cols != 1u) {
			fatal_print("%s\n",
				"Line segment correction (b): matrix larger than 1x1.");
		} else {}
		double b = second -> matrix[0][0];

		/* Compute corrective factor numerically (see note above) */
		INTEGRAL intgrl;
		intgrl.func = &corrective_factor_marginalization_integrand;
		intgrl.lower = 0;
		intgrl.upper = 1;
		intgrl.tolerance = LINE_SEGMENT_CORRECTION_TOLERANCE;
		intgrl.n_min = LINE_SEGMENT_CORRECTION_MIN_ITERS;
		intgrl.n_max = LINE_SEGMENT_CORRECTION_MAX_ITERS;
		intgrl.extra_args = (double *) malloc (2 * sizeof(double));
		intgrl.extra_args[0] = a;
		intgrl.extra_args[1] = b;
		intgrl.n_extra_args = 2u;
		quad(&intgrl);
		double correction = intgrl.result;

		/* free up allocated memory */
		free(intgrl.extra_args);
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
		The correction integrates over the full length of the line segment.
		Therefore, the point at the end of the track can be treated as a line
		segment of length 0, therefore not contributing to the overall
		likelihood.
		*/
		return 0.f;
	}

}


/*
.. c:function:: static double corrective_factor_marginalization_integrand(double *args);

	The integrand for computing line segment length corrections along the track.

	Parameters
	----------
	args : ``double *``
		The integration parameters, :math:`q`, :math:`a`, and :math:`b`.

	Returns
	-------
	value : ``double``
		The integrand, defined as

		.. math:: \exp(\frac{-1}{2} (aq^2 - 2bq)),

		where ``q = args[0]``, ``a = args[1]``, and ``b = args[2]``.
*/
static double corrective_factor_marginalization_integrand(double *args) {

	double q = args[0], a = args[1], b = args[2];
	return exp(-0.5 * (a * q * q - 2 * b * q));

}


/*
.. c:function:: static TRACK *track_subset(DATUM d, TRACK t);

	Obtain a :c:type:`TRACK` object containing only the quantities present in a
	particular :c:type:`DATUM` by comparing their column labels.

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

	TRACK *sub = track_initialize(t.n_vectors, d.n_cols);
	sub -> n_threads = t.n_threads;
	sub -> use_line_segment_corrections = t.use_line_segment_corrections;

	for (unsigned short i = 0u; i < d.n_cols; i++) {
		signed short index = strindex(t.labels, d.labels[i], t.dim);
		switch(index) {

			case -1:
				track_free(sub);
				return NULL;

			default:
				strcpy(sub -> labels[i], t.labels[index]);
				for (unsigned short j = 0u; j < t.n_vectors; j++) {
					sub -> predictions[j][i] = t.predictions[j][index];
				}
				break;

		}

	}

	for (unsigned short i = 0u; i < (*sub).n_vectors; i++) {
		sub -> weights[i] = t.weights[i];
	}
	return sub;

}


/*
.. c:function:: static MATRIX *trackpoint(TRACK t, const unsigned short index);

	Obtain the vector for a specific point along the model predicted track.

	Parameters
	----------
	t : ``TRACK``
		The input track itself, containing some arbitrary number of vectors.
	index : ``const unsigned short``
		The index of the vector along the track to capture as a stand-alone
		matrix.

	Returns
	-------
	mat : ``MATRIX *``
		A pointer to matrix object containing only one row, whose elements are
		a component-wise match to ``t.predictions[index]``.
*/
static MATRIX *trackpoint(TRACK t, const unsigned short index) {

	MATRIX *point = matrix_initialize(1u, t.dim);
	for (unsigned short i = 0u; i < t.dim; i++) {
		point -> matrix[0][i] = t.predictions[index][i];
	}
	return point;

}
