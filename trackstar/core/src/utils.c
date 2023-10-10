/*
This file is part of the TrackStar package.
Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
License: MIT License. See LICENSE in top-level directory
at: https://github.com/giganano/TrackStar.git.
*/

#include <string.h>
#include "utils.h"

/*
.. cpp:function:: extern signed short strindex(char **strlist, char *test,
	unsigned short strlistlength);

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
