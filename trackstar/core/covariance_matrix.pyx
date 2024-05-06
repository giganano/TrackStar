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

	.. todo:: Finish documentation on this class.

	.. sealso::

		- ``trackstar.matrix``
		- ``trackstar.vector``

	.. warning::

		Users are discouraged from constructing a covariance matrix object
		themselves. TrackStar is designed to construct covariance matrices for
		the user, so in most scenarios, it is best to simply modify the
		covariance matrices already attached to individual data vectors.
	"""

	def __cinit__(self, arr, n_threads = 1):
		self._cov = <COVARIANCE_MATRIX *> realloc (self._m,
			sizeof(COVARIANCE_MATRIX))

		# for inheritance, give self._m same address as self._cov
		self._m = <MATRIX *> self._cov
		
		# n_rows and n_cols should be equal for a covariance matrix
		self._cov[0].inv = matrix_initialize(self._cov[0].n_rows,
			self._cov[0].n_cols)

	def __init__(self, arr, n_threads = 1):
		super().__init__(arr)
		matrix_invert(self._m[0], self._cov[0].inv)


	def __dealloc__(self):
		covariance_matrix_free(self._cov)


	def __repr__(self):
		return super().__repr__().replace("matrix", "covmat")


	def __setitem__(self, indices, value):
		super().__setitem__(indices, value)
		super().__setitem__(indices[::-1], value) # ensures diagonality
		matrix_invert(self._m[0], self._cov[0].inv)


	@property
	def inv(self):
		r"""
		Type : ``trackstar.matrix``

		The inverse of this covariance matrix, :math:`C^{-1}`.
		"""
		inv = matrix.zeroes(self.n_rows, self.n_cols)
		for i in range(self.n_rows):
			for j in range(self.n_cols):
				inv[i, j] = self._cov[0].inv.matrix[i][j]
		return inv
