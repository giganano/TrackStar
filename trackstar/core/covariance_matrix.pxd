# cython: language_level = 3, boundscheck = False
#
# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/trackstar.git.

from .matrix cimport MATRIX, matrix, matrix_invert

cdef extern from "./src/matrix.h":
	ctypedef struct COVARIANCE_MATRIX:
		double **matrix
		unsigned short n_rows
		unsigned short n_cols
		MATRIX *inv

	void covariance_matrix_free(COVARIANCE_MATRIX *cov)

cdef class covariance_matrix(matrix):
	cdef COVARIANCE_MATRIX *_cov
