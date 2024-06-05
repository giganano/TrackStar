# cython: language_level = 3, boundscheck = False
#
# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/TrackStar.git.

from .matrix cimport MATRIX

cdef extern from "./src/track.h":
	ctypedef struct TRACK:
		double **predictions
		unsigned short n_vectors
		unsigned short dim
		unsigned short n_threads
		char **labels
		double *weights
		unsigned short use_line_segment_corrections
		unsigned short normalize_weights

	TRACK *track_initialize(unsigned short n_vectors, unsigned short dim)
	void track_free(TRACK *t)


cdef class track:
	cdef MATRIX *_m
	cdef TRACK *_t

