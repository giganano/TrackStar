# cython: language_level = 3, boundscheck = False
#
# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/trackstar.git.

from .covariance_matrix cimport COVARIANCE_MATRIX, covariance_matrix
from .matrix cimport MATRIX, matrix
from .track cimport TRACK

cdef extern from "./src/datum.h":
	ctypedef struct DATUM:
		double **vector
		unsigned short n_rows
		unsigned short n_cols
		COVARIANCE_MATRIX *cov
		char **labels

	DATUM *datum_initialize(unsigned short dim)
	void datum_free(DATUM *d)
	void datum_free_everything(DATUM *d)
	double datum_get_item(DATUM d, char *label)
	DATUM *datum_specific_quantities(DATUM d, char **labels,
		unsigned short n_labels)


cdef extern from "./src/likelihood.h":
	double loglikelihood_datum(DATUM d, TRACK *t)


cdef class datum:
	cdef DATUM *_d
	cdef MATRIX *_m
	cdef covariance_matrix _cov
	cdef object _extra
	cdef object _shadow_keys
