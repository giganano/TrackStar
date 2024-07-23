/*
This file is part of the TrackStar package.
Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
License: MIT License. See LICENSE in top-level directory
at: https://github.com/giganano/TrackStar.git.
*/

#include <stdlib.h>
#include "quadrature.h"
#include "utils.h"

/* ---------- static function comment headers not duplicated here ---------- */
static double simpsons_rule(double (*func)(double *), const double lower,
	const double upper, const unsigned long n_bins, const double *extra_args,
	const unsigned short n_extra_args);
static double trapezoid_rule(double (*func)(double *), const double lower,
	const double upper, const unsigned long n_bins, const double *extra_args,
	const unsigned short n_extra_args);
static double *bin_edges(const double start, const double stop, 
	const unsigned long n_bins);
static double absval(double x);
static signed short sign(double x);


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
extern unsigned short quad(INTEGRAL *intgrl) {

	unsigned long n = (*intgrl).n_min;
	if (n % 2ul) n++;
	double old_int = 0, new_int;

	do {

		new_int = simpsons_rule((*intgrl).func, (*intgrl).lower,
			(*intgrl).upper, n, (*intgrl).extra_args, (*intgrl).n_extra_args);
		if (new_int) {
			intgrl -> error = absval(old_int / new_int - 1);
		} else{
			/* avoid numerical errors from division by zero. */
			intgrl -> error = 1;
		}
		old_int = new_int;
		n *= 2ul;

	} while ((*intgrl).error > (*intgrl).tolerance && n < (*intgrl).n_max);

	intgrl -> result = new_int;
	intgrl -> iters = n;
	return (*intgrl).error > (*intgrl).tolerance;

}


/*
.. c:function:: static double simpsons_rule(double (*func)(double *), const double lower, const double upper, const unsigned long n_bins, const double *extra_args, const unsigned long n_extra_args);

	Evaluate a Reimann sum according to Simpson's Rule (see Chapter 4 of Press
	et al. 2007 [1]_).

	Parameters
	----------
	func : ``double (*)(double *)``
		A pointer to a function that accepts a double pointer as its only
		argument. The first argument should be the one over which the integral
		is being evaluated.
	lower : ``double``
		The lower bound of the integral.
	upper : ``double``
		The upper bound of the integral.
	n_bins : ``unsigned long``
		The number of quadrature bins in the Reimann sum.
	extra_args : ``double *``
		Any extra values to be passed to the integrand function.
	n_extra_args : ``unsigned short``
		The number of elements in ``extra_args``.

	Returns
	-------
	s : ``double``
		The value of the Reimann sum according to Simpson's Rule, which can be
		understood as an extension of Trapezoid Rule.

	.. [1] Press, Teukolsky, Vetterling, Flannery, 2007, Numerical Recipes,
		Cambridge University Press
*/
static double simpsons_rule(double (*func)(double *), const double lower,
	const double upper, const unsigned long n_bins, const double *extra_args,
	const unsigned short n_extra_args) {

	return (
		4 * trapezoid_rule(
			func, lower, upper, n_bins, extra_args, n_extra_args) -
		trapezoid_rule(
			func, lower, upper, n_bins / 2ul, extra_args, n_extra_args)
	) / 3;

}


/*
.. c:function:: static double trapezoid_rule(double (*func)(double *), const double lower, const double upper, const unsigned long n_bins, const double *extra_args, const unsigned long n_extra_args);

	Evaluate a Reimann sum according to Trapezoid Rule (see Chapter 4 of Press
	et al. 2007 [1]_).

	Parameters
	----------
	func : ``double (*)(double *)``
		A pointer to a function that accepts a double pointer as its only
		argument. The first argument should be the one over which the integral
		is being evaluated.
	lower : ``double``
		The lower bound of the integral.
	upper : ``double``
		The upper bound of the integral.
	n_bins : ``unsigned long``
		The number of quadrature bins in the Reimann sum.
	extra_args : ``double *``
		Any extra values to be passed to the integrand function.
	n_extra_args : ``unsigned short``
		The number of elements in ``extra_args``.

	Returns
	-------
	s : ``double``
		The value of the Reimann sum according to Trapezoid rule, defined by
		connecting each (x, y) point the function is sampled along, which
		allows the integral to be approximated as a series of trapezoids.

	.. [1] Press, Teukolsky, Vetterling, Flannery, 2007, Numerical Recipes,
		Cambridge University Press
*/
static double trapezoid_rule(double (*func)(double *), const double lower,
	const double upper, const unsigned long n_bins, const double *extra_args,
	const unsigned short n_extra_args) {

	double bin_width = (upper - lower) / n_bins;
	double *x = bin_edges(lower, upper, n_bins);
	double *funcofx = (double *) malloc ((n_bins + 1ul) * sizeof(double));
	double *args = (double *) malloc ((n_extra_args + 1u) * sizeof(double));
	for (unsigned short i = 0ul; i < n_extra_args; i++) {
		args[i + 1u] = extra_args[i];
	}
	for (unsigned long i = 0ul; i <= n_bins; i++) {
		args[0] = x[i];
		funcofx[i] = func(args);
	}
	double total = sum(funcofx, n_bins + 1ul);
	total -= (funcofx[0] + funcofx[n_bins]) / 2;
	free(x);
	free(args);
	free(funcofx);
	return bin_width * total;

}


/*
.. c:function:: static double bin_edges(const double start, const double stop, const unsigned long n_bins);

	Compute the desired bin edges for some known range of values and known
	number of bins.

	Parameters
	----------
	start : ``double``
		The bottom-most edge of the bins.
	stop : ``double``
		The upper-most edge of the bins.
	n_bins : ``unsigned long``
		The number of bins to separate the range between ``start`` and ``stop``
		into.

	Returns
	-------
	arr : ``double *``
		A pointer to the array of bin edges, in ascending order.
		Has length ``n_bins + 1``.
*/
static double *bin_edges(const double start, const double stop, 
	const unsigned long n_bins) {

	double *arr = (double *) malloc ((n_bins + 1l) * sizeof(double));
	double dx = (stop - start) / n_bins;
	for (unsigned long i = 0ul; i <= n_bins; i++) arr[i] = start + i * dx;
	return arr;

}


/*
.. c:function:: static double absval(double x);

	Compute the absolute value of ``x``..
*/
static double absval(double x) {

	return sign(x) * x;

}


/*
.. c:function:: static signed short sign(double x);

	Determine the sign of ``x``. ``+1`` if ``x > 0``, ``-1`` if ``x < 0``, and
	``0`` if ``x == 0``.
*/
static signed short sign(double x) {

	return (x > 0) - (x < 0);

}

