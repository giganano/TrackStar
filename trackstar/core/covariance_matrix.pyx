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

	.. seealso::

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

	def __init__(self, arr):
		super().__init__(arr)
		matrix_invert(self._m[0], self._cov[0].inv)


	def __dealloc__(self):
		covariance_matrix_free(self._cov)


	def __repr__(self):
		return super().__repr__().replace("matrix", "covmat")


	def __setitem__(self, indices, value):
		self._check_indices(indices)
		if isinstance(value, numbers.Number):
			if indices[0] == indices[1]:
				if value > 0:
					if value > 1e-12:
						self._cov[0].matrix[indices[0]][indices[1]] = value
					else:
						raise ValueError("""\
As a safeguard against numerical artifacts, TrackStar does not allow values \
smaller than 1e-12 along the diagonals of covariance matrices. If your \
uncertainties are this small, please apply a constant, overall multiplicative \
factor to the relevant data quantities.""")
				else:
					raise ValueError("""\
Diagonal elements of covariance matrix must be positive.""")
			else:
				self._cov[0].matrix[indices[0]][indices[1]] = value
				self._cov[0].matrix[indices[1]][indices[0]] = value
		else:
			raise TypeError("""\
Item assignment requires a real number. Got: %s""" % (type(value)))


	@property
	def inv(self):
		r"""
		Type : ``trackstar.matrix``

		The inverse of this covariance matrix, :math:`C^{-1}`.
		"""
		inv = matrix.zeros(self.n_rows, self.n_cols)
		for i in range(self.n_rows):
			for j in range(self.n_cols):
				inv[i, j] = self._cov[0].inv.matrix[i][j]
		return inv
