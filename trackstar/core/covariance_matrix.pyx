# cython: language_level = 3, boundscheck = False
#
# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/trackstar.git.

__all__ = ["covariance_matrix"]
import numbers
from libc.stdlib cimport realloc
from . cimport covariance_matrix
from .matrix cimport matrix_initialize

cdef class covariance_matrix(matrix):

	r"""
	.. class:: trackstar.covariance_matrix(arr)

	A covariance matrix.

	.. sealso::

		- ``trackstar.matrix``
		- ``trackstar.vector``

	.. warning::

		Users are discouraged from constructing a covariance matrix object
		themselves. TrackStar is designed to construct covariance matrices for
		the user, so in most scenarios, it is best to simply modify the
		covariance matrices already attached to individual data vectors.
	"""

	def __cinit__(self, arr):
		self._cov = <COVARIANCE_MATRIX *> realloc (self._m,
			sizeof(COVARIANCE_MATRIX))

		# for inheritance, give self._m same address as self._cov
		self._m = <MATRIX *> self._cov
		
		# n_rows and n_cols should be equal for a covariance matrix
		self._cov[0].inv = matrix_initialize(self._cov[0].n_rows,
			self._cov[0].n_cols)


	def __dealloc__(self):
		covariance_matrix_free(self._cov)

