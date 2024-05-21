# cython: language_level = 3, boundscheck = False
#
# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/trackstar.git.

__all__ = ["covariance_matrix"]
import textwrap
import numbers
from .utils import copy_cstring
from .utils cimport copy_pystring, strindex
from libc.stdlib cimport realloc, free
from libc.string cimport strlen
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

		self._cov[0].labels = NULL

	def __init__(self, arr):
		super().__init__(arr)
		matrix_invert(self._m[0], self._cov[0].inv)


	def __dealloc__(self):
		covariance_matrix_free(self._cov)


	def __repr__(self):
		base = super().__repr__().replace("matrix", "covariance matrix")
		if self._cov[0].labels is not NULL:
			lines = base.split("\n")
			new_line = "    Quantities (in the order of indexing): ["
			new_line += "%s" % (copy_cstring(self._cov[0].labels[0]))
			for i in range(1, self.n_rows):
				new_line += ", %s" % (copy_cstring(self._cov[0].labels[i]))
			new_line += "]"
			lines.insert(-1, new_line)
			return "\n".join(lines)
		else:
			return base


	def __getitem__(self, keys):
		if self._cov[0].labels is not NULL:
			return super().__getitem__(self._indices_from_labels_(keys))
		else:
			return super().__getitem__(keys)


	def __setitem__(self, indices, value):
		if self._cov[0].labels is not NULL:
			indices = self._indices_from_labels_(indices)
		else: pass
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
		matrix_invert(self._m[0], self._cov[0].inv)


	def _indices_from_labels_(self, keys):
		cdef char *copy
		if isinstance(keys, tuple):
			indices = []
			if len(keys) == 2:
				for i in range(len(keys)):
					if isinstance(keys[i], str):
						copy = copy_pystring(keys[i])
						try:
							idx = strindex(self._cov[0].labels, copy,
								self._cov[0].n_rows)
						finally:
							free(copy)
						if idx == -1: raise KeyError("""\
Unrecognized datum label: %s""" % (keys[i]))
						indices.append(idx)
					elif isinstance(keys[i], numbers.Number):
						indices.append(keys[i])
					else:
						raise KeyError("""\
Covariance matrix indices must be either a string or an integer. Got: %s""" % (
							type(keys[i])))
				return tuple(indices)
			else:
				raise KeyError("""\
One or two labels or indices are required for covariance matrix indexing. \
Got: %d""" % (len(keys)))
		elif isinstance(keys, str) or isinstance(keys, numbers.Number):
			return self._indices_from_labels_((keys, keys))
		else:
			raise KeyError("""\
Covariance matrix indices must be either a string or an integer. Got: %s""" % (
				type(keys)))


	@property
	def inv(self):
		r"""
		Type : ``trackstar.matrix``

		The inverse of this covariance matrix, :math:`C^{-1}`. This object
		passes a copy of the current inverse covariance matrix, so item
		assignment does nothing.
		"""
		inv = matrix.zeros(self.n_rows, self.n_cols)
		for i in range(self.n_rows):
			for j in range(self.n_cols):
				inv[i, j] = self._cov[0].inv.matrix[i][j]
		return inv


	def keys(self):
		r"""
		Get the dictionary labels, if applicable.
		"""
		if self._cov[0].labels is not NULL:
			_keys = []
			for i in range(self.n_cols):
				_keys.append(copy_cstring(self._cov[0].labels[i]))
			return _keys
		else:
			return None
