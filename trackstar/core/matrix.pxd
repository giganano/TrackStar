# cython: language_level = 3, boundscheck = False
#
# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/TrackStar.git.

cdef extern from "./src/matrix.h":
	ctypedef struct MATRIX:
		double **matrix
		unsigned short n_rows
		unsigned short n_cols

	MATRIX *matrix_initialize(unsigned short n_rows, unsigned short n_cols)
	void matrix_free(MATRIX *m)
	MATRIX *matrix_add(MATRIX m1, MATRIX m2, MATRIX *result)
	MATRIX *matrix_subtract(MATRIX m1, MATRIX m2, MATRIX *result)
	MATRIX *matrix_multiply(MATRIX m1, MATRIX m2, MATRIX *result)
	MATRIX *matrix_invert(MATRIX m, MATRIX *result)
	MATRIX *matrix_transpose(MATRIX m, MATRIX *result)
	double matrix_determinant(MATRIX m)

cdef class matrix:
	cdef MATRIX *_m

