/*
This file is part of the TrackStar package.
Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
License: MIT License. See LICENSE in top-level directory
at: https://github.com/giganano/TrackStar.git.

**Source Files**: ``trackstar/core/src/utilc.``
*/

#ifndef UTILS_H
#define UTILS_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus  */

#include <math.h>

#ifndef NAN
/*
.. c:macro:: NAN

	Defines the ``NAN`` macro if it is not already defined.
*/
#define NAN 0.f / 0.f
#endif
#ifndef isnan
/*
.. c:macro:: isnan

	Defines the ``isnan`` function from the ``math.h`` header if it is not
	already defined.
*/
#define isnan(x) { x != x; }
#endif

/*
.. c:function:: extern signed short strindex(char **strlist, char *test, unsigned short strlistlength);

	Determine the index of a string within an array of strings.

	Parameters
	----------
	strlist : ``char **``
		The list of strings to search through
	test : ``char *``
		The string to search for within ``strlist``.
	strlistlength : ``unsigned short``
		The number of entries in ``strlist``.

	Returns
	-------
	idx : ``signed short``
		If >= 0, then ``strlist[idx]`` is a character-by-character match to
		``test``. -1 if there is no such element within ``strlist``.
*/
extern signed short strindex(char **strlist, char *test,
	unsigned short strlistlength);

/*
.. c:function:: extern double sum(const double *arr, const unsigned long length);

	Compute the sum of an array of real numbers.

	Parameters
	----------
	arr : ``const double *``
		The array to be summed.
	lentgth : ``const unsigned long``
		The number of elements in ``arr``.

	Returns
	-------
	s : ``double``
		``arr[0]`` + ``arr[1]`` + ``arr[2]`` + ... + ``arr[length - 3]`` +
		``arr[length - 2]`` + `arr[length - 1]``.
*/
extern double sum(const double *arr, const unsigned long length);

#ifdef __cplusplus
}
#endif /* __cplusplus  */

#endif /* UTILS_H */
