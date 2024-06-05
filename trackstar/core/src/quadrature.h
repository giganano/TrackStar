/*
This file is part of the TrackStar package.
Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
License: MIT License. See LICENSE in top-level directory
at: https://github.com/giganano/TrackStar.git.

**Source File**: ``trackstar/core/src/quadrature.c``
*/

#ifndef QUADRATURE_H
#define QUADRATURE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct integral {

	/*
	.. c:type:: INTEGRAL

		An integral to be evaluated numerically.

		.. note::

			In the current version of trackstar, quadrature is required for
			only purpose: correction for the finite lengths of line segments
			along the track (see notes for function ``quad``).

		.. c:member:: double (*func)(double *)

			A pointer to the function to be integrated. The first element of
			the pointer it accepts is the variable to be integrated with
			respect to.

		.. c:member:: double lower

			The lower bound of the integral.

		.. c:member:: double upper

			The upper bound of the integral.

		.. c:member:: double tolerance

			The maximum allowed numerical error.

		.. c:member:: unsigned long n_min

			The minimum number of bins in quadrature.

		.. c:member:: unsigned long n_max

			The maximum number of bins in quadrature. Safeguard against
			non-convergent solutions.

		.. c:member:: unsigned long iters

			The number of iterations required before the integral converged
			(assigned during integration).

		.. c:member:: unsigned long result

			The approximated solution to the integral (assigned during
			integration).

		.. c:member:: double error

			The numerical uncertainty (assigned during integration).

		.. c:member:: double *extra_args

			Any extra arguments to pass to the integrand function.

		.. c:member:: unsigned short n_extra_args

			The number of elements in :c:member:`extra_args`.
	*/

	double (*func)(double *);
	double lower;
	double upper;
	double tolerance;
	unsigned long n_min;
	unsigned long n_max;
	unsigned long iters;
	double result;
	double error;
	double *extra_args;
	unsigned short n_extra_args;

} INTEGRAL;

/*
.. c:function:: extern unsigned short quad(INTEGRAL *intgrl);

	Evaluate an integral numerically.

	Parameters
	----------
	intgrl : ``INTEGRAL *``
		A pointer to the :c:type:`INTEGRAL` struct to run the quadrature
		routine over.

	Returns
	-------
	0u if the integral has converged. 1u if the maximum number of iterations
	was reached before the error converged within the specified tolerance. The
	result, numerical error, and number of iterations will be stored within
	``intgrl``.

	Notes
	-----
	In the current version of TrackStar, this function is called for only one
	purpose: evaluating corrective factors for the lengths of each individual
	line segment that make up a track (see science documentation for further
	details). Therefore, in practice, the functional form of the integrand is
	known, and it is smooth. Although it does have an analytic solution, it is
	an example of the rare case where the analytic form is not numerically
	stable while the numerical integral is. In this instance, the issue arises
	because the solution involves the product of an extremely large number and
	an extremely small number, both of which challenge the limits of double
	floating point precision.
*/
extern unsigned short quad(INTEGRAL *intgrl);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* QUADRATURE_H */
