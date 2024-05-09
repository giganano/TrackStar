/*
This file is part of the TrackStar package.
Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
License: MIT License. See LICENSE in top-level directory
at: https://github.com/giganano/TrackStar.git.
*/

#ifndef QUADRATURE_H
#define QUADRATURE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct integral {

	/*
	.. cpp:type:: INTEGRAL

	An integral to be evaluated numerically.

	Attributes
	----------
	func : ``double (*func)(double *)``
		A pointer to a function accepting a double pointer as its only
		argument. The first element of the pointer is the variable to be
		integrated with respect to.
	lower : ``double``
		The lower bound of the integral.
	upper : ``double``
		The upper bound of the integral.
	tolerance : ``double``
		The maximum allowed numerical error.
	n_min : ``unsigned long``
		The minimum number of bins in quadrature.
	n_max : ``unsigned long``
		The maximum number of bins in quadrature.
	iters : ``unsigned long``
		The number of iterations required before the integral converged
		(assigned during integration).
	result : ``double``
		The approximated solution to the integral (assigned during integration).
	error : ``double``
		The numerical error (assigned during integration).
	extra_args : ``double *``
		Any extra arguments to psss to the integrand function.
	n_extra_args : ``unsigned short``
		The number of elements in ``extra_args``.

	.. note::

		In the current version of trackstar, quadrature is required for only
		purpose: correction for the finite lengths of line segments along the
		track (see notes for function ``quad``).
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
.. cpp:function:: extern unsigned short quad(INTEGRAL *intgrl);

Evaluate an integral numerically.

Parameters
----------
intgrl : ``INTEGRAL *``
	A pointer to the integral struct to run the quadrature routine over
	(see definition in quadrature.h).

Returns
-------
0u if the integral has converged. 1u if the maximum number of iterations was
reached before the error converged within the specified tolerance. The result,
numerical error, and number of iterations will be stored within ``intgrl``.

Notes
-----
In the current version of TrackStar, this function is called for only one
purpose: evaluating corrective factors for the lengths of each individual
line segment that make up a track. Therefore, in practice, the functional
form of the integrand is known, and it is smooth. Although it does have
analytic solution, it is an example of the rare case where the analytic form is
not numerically stable while the numerical integral is. In this instance, the
issue arises because the solution involves the product of an extremely large
number and an extremely small number. See TrackStar's science documentation for
further details.
*/
extern unsigned short quad(INTEGRAL *intgrl);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* QUADRATURE_H */
