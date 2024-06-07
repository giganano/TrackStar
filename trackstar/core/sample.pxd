# cython: language_level = 3, boundscheck = False
#
# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/TrackStar.git.

from .datum cimport DATUM, datum
from .track cimport TRACK

cdef extern from "./src/sample.h":
	ctypedef struct SAMPLE:
		DATUM **data
		unsigned long n_vectors

	SAMPLE *sample_initialize()
	void sample_free(SAMPLE *s)
	void sample_free_everything(SAMPLE *s)
	void sample_add_datum(SAMPLE *s, DATUM *d)
	SAMPLE *sample_specific_quantities(SAMPLE s, char **labels,
		unsigned short n_labels)
	unsigned long *sample_filter_indices(SAMPLE s, char *label,
		unsigned short condition_indicator, double value,
		unsigned short keep_missing_measurements)


cdef extern from "./src/likelihood.h":
	double loglikelihood_sample(SAMPLE s, TRACK *t)


cdef class sample:
	cdef SAMPLE *_s
	cdef list _data

