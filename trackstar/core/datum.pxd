# cython: language_level = 3, boundscheck = False
#
# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/trackstar.git.

from .matrix cimport MATRIX

cdef extern from "./src/datum.h":
	ctypedef struct DATUM:
		double **vector
		unsigned short n_rows
		unsigned short n_cols
		unsigned short n_threads

	DATUM *datum_initialize(double *arr, char **labels, unsigned short dim)
	void datum_free(DATUM *d)
	double datum_get_item(DATUM d, char *label)

cdef class datum:
	cdef DATUM *_d
