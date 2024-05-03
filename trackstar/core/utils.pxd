# cython: language_level = 3, boundscheck = False
#
# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/trackstar.git.

cdef extern from "./src/utils.h":
	signed short strindex(char **strlist, char *test,
		unsigned short strlistlength)
	unsigned short MAX_LABEL_SIZE

cdef char *copy_pystring(pystr) except *
