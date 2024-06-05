/*
This file is part of the TrackStar package.
Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
License: MIT License. See LICENSE in top-level directory
at: https://github.com/giganano/TrackStar.git.

This header file includes the features for computing the log of the likelihood
function for a given data-model pair.

**Source File**: ``trackstar/core/src/likelihood.c``
*/

#ifndef LIKELIHOOD_H
#define LIKELIHOOD_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "matrix.h"
#include "sample.h"
#include "datum.h"
#include "track.h"

/*
The following macros are relevant for computing multiplicative factor
corrections for the finite lengths of individual line segments along the track
and the continuous change in the likelihood of observation along the line
segment. They are only relevant when the user specifies the keyword argument
``use_line_segment_corrections = True`` in their call to either
``sample.loglikelihood`` or ``datum.loglikelihood``.

.. c:macro:: LINE_SEGMENT_CORRECTION_TOLERANCE

	``1e-3``. The maximum allowed numerical error.

.. c:macro:: LINE_SEGMENT_CORRECTION_MIN_ITERS

	``64ul``. The minimum number of bins in quadrature.

.. c:macro:: LINE_SEGMENT_CORRECTION_MAX_ITERS

	``1e6``. The maximum number of bins in quadrature. If the integration
	exceeds this number, the result has not converged to within
	:c:macro:`LINE_SEGMENT_CORRECTION_TOLERANCE`, but a value will be returned
	anyway.
*/
#define LINE_SEGMENT_CORRECTION_TOLERANCE 1e-3
#define LINE_SEGMENT_CORRECTION_MIN_ITERS 64ul
#define LINE_SEGMENT_CORRECTION_MAX_ITERS 1e6

/*
.. c:function:: extern double loglikelihood_sample(SAMPLE *s, TRACK *t);

	This function is called when the user calls ``sample.loglikelihood``.

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
extern double loglikelihood_sample(SAMPLE s, TRACK *t);

/*
.. c:function:: extern double loglikelihood_datum(DATUM *d, TRACK *t);

	This function is called when the user calls ``datum.loglikelihood``.

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
extern double loglikelihood_datum(DATUM d, TRACK *t);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIKELIHOOD_H */

