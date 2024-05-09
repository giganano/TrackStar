/*
This file is part of the TrackStar package.
Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
License: MIT License. See LICENSE in top-level directory
at: https://github.com/giganano/TrackStar.git.
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
Parameters for computing line segment length corrections along the track.

*_TOLERANCE : The numerical tolerance (maximum allowed relative error).
*_MIN_ITERS : The starting number of quadrature bins.
*_MAX_ITERS : The maximum allowed number of quadrature bins. At this value,
	the integration stops and the value is reported, regardless of whether or
	not the calculation has converged to within the tolerance interval.
*/
#define LINE_SEGMENT_CORRECTION_TOLERANCE 1e-3
#define LINE_SEGMENT_CORRECTION_MIN_ITERS 64ul
#define LINE_SEGMENT_CORRECTION_MAX_ITERS 1e6

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
extern double loglikelihood_sample(SAMPLE *s, TRACK *t);

/*
.. cpp:function:: extern double loglikelihood_datum_backen(DATUM *d, TRACK *t);

Compute the natural logarithm of the likelihood that an individual datum will
be observed from its vector and the model-predicted track in the observed
space. The track may contain predictions for observables that are not available
for the present datum ``d``; this possibility is taken into account
automatically.

The exact functional form of the likelihood function is described in section 3
of Johnson et al. (2022) [1]_ and derived in detail in their Appendix A.

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
	full length of the track and weighted according to the predicted observed
	density along the track.

References
----------
.. [1] Johnson J.W., et al., 2022, MNRAS, 526, 5084
*/
extern double loglikelihood_datum(DATUM *d, TRACK *t);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIKELIHOOD_H */

