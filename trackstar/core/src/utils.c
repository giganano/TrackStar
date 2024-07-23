/*
This file is part of the TrackStar package.
Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
License: MIT License. See LICENSE in top-level directory
at: https://github.com/giganano/TrackStar.git.
*/

#include <string.h>
#include "utils.h"

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
	unsigned short strlistlength) {

	signed short i;
	for (i = 0; (unsigned) i < strlistlength; i++) {
		if (!strcmp(strlist[i], test)) return i;
	}
	return -1;

}


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
extern double sum(const double *arr, const unsigned long length) {

	double s = 0;
	for (unsigned long i = 0ul; i < length; i++) s += arr[i];
	return s;

}
