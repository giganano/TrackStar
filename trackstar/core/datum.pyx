# cython: language_level = 3, boundscheck = False
#
# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/trackstar.git.

all = ["datum"]
import numbers
from .utils import copy_array_like_object, copy_cstring
from .utils cimport copy_pystring, strindex
from libc.stdlib cimport malloc, free
from libc.string cimport strlen


cdef class datum:
	
	r"""
	.. class:: trackstar.datum(**kwargs)

	A generic datum class for constructing data vectors with descriptive
	labels attached to each vector component.

	.. seealso:: ``trackstar.matrix``

	.. todo:: round out the documentation on this object.
	"""

	def __cinit__(self, **kwargs):
		keys = list(kwargs.keys())
		for key in keys:
			if not isinstance(kwargs[key], numbers.Number): raise TypeError("""\
Datum must contain only numerical values. Got: %s""" % (type(kwargs[key])))

		cdef double *vector = <double *> malloc (len(keys) * sizeof(double))
		cdef char **labels = <char **> malloc (len(keys) * sizeof(char *))
		for i in range(len(keys)):
			vector[i] = <double> kwargs[keys[i]]
			labels[i] = copy_pystring(keys[i])
		try:
			self._d = datum_initialize(vector, labels, len(keys))
			self._m = <MATRIX *> self._d
			self._cov = covariance_matrix.identity(len(keys))
			self._d[0].cov = self._cov._cov
		finally:
			free(vector)
			for i in range(len(keys)): free(labels[i])
			free(labels)


	def __init__(self, **kwargs):
		pass # need to change the __init__ signature to accept kwargs

	def __dealloc__(self):
		r"""
		Free up the memory stored by a ``datum`` object. User access strongly
		discouraged.
		"""
		datum_free(self._d)


	def __enter__(self):
		r"""Opens a with statement."""
		return self


	def __exit__(self, exc_type, exc_value, exc_tb):
		r"""Raises all exceptions inside with statements."""
		return exc_value is None


	def __repr__(self):
		rep = "datum([\n"
		keys = self.keys()
		for i in range(len(keys)):
			rep += "    %s " % (keys[i])
			for j in range(15 - len(keys[i])): rep += "-"
			rep += "> %.4e\n" % (self._d[0].vector[0][i])
		rep += "])"
		return rep


	def __getitem__(self, key):
		cdef char *copy
		if isinstance(key, str):
			copy = copy_pystring(key)
			try:
				idx = strindex(self._d[0].labels, copy, self._d[0].n_cols)
			finally:
				free(copy)
			if idx != -1:
				return self._d[0].vector[0][idx]
			else:
				raise ValueError("Unrecognized datum label: %s" % (key))
		else:
			raise TypeError("Datum label must be a string. Got: %s" % (
				type(key)))


	def __setitem__(self, key, value):
		cdef char *copy
		if isinstance(key, str):
			if isinstance(value, numbers.Number):
				copy = copy_pystring(key)
				try:
					idx = strindex(self._d[0].labels, copy, self._d[0].n_cols)
				finally:
					free(copy)
				if idx != -1:
					self._d[0].vector[0][idx] = <double> value
				else:
					raise ValueError("""\
Unrecognized datum label: %s. If additional vector components are to be added, \
a new datum object must be created.""" % (key))
			else:
				raise ValueError("""\
Datum vector component must be a real number. Got: %s""" % (type(value)))
		else:
			raise TypeError("Datum label must be a string. Got: %s" % (
				type(key)))


	def __eq__(self, datum other):
		r"""
		Returns ``True`` if both vectors contain the same labels and the values
		stored for each of those labels passes an equivalence comparison.
		"""
		if self._d[0].n_cols == other._d[0].n_cols:
			eq = True
			for i in range(other._d[0].n_cols):
				idx = strindex(self._d[0].labels, other._d[0].labels[i],
					self._d[0].n_cols)
				if idx != -1:
					eq &= self._d[0].vector[0][i] == other._d[0].vector[0][idx]
				else:
					return False
				if not eq: break
			return eq
		else:
			return False


	def __pos__(self):
		return self


	def __neg__(self):
		new_vector = {}
		for key in self.keys(): new_vector[key] = -self[key]
		return self.__class__(**new_vector)


	def __add__(self, datum other):
		new_vector = {}
		for i in range(self._d[0].n_cols):
			key = copy_cstring(self._d[0].labels[i])
			new_vector[key] = self._d[0].vector[0][i]
		for i in range(other._d[0].n_cols):
			# to identify the vector components the two data have in common,
			# use routine for searching throuch character labels in C rather
			# than comparing dictionary keys in python -> faster
			key = copy_cstring(other._d[0].labels[i])
			idx = strindex(self._d[0].labels, other._d[0].labels[i],
				self._d[0].n_cols)
			if idx != -1:
				new_vector[key] += other._d[0].vector[0][i]
			else:
				new_vector[key] = other._d[0].vector[0][i]
		return self.__class__(**new_vector)


	def __sub__(self, datum other):
		return self.__add__(other.__neg__())


	def __mul__(self, datum other):
		if self._d[0].n_cols == other._d[0].n_cols:
			# vector components may be out of order between the two
			dotprod = 0
			for i in range(other._d[0].n_cols):
				idx = strindex(self._d[0].labels, other._d[0].labels[i],
					self._d[0].n_cols)
				if idx != -1:
					dotprod += self._d[0].vector[0][i] * other._d[0].vector[0][idx]
				else:
					raise ValueError("""\
Vectors must have the same components in order to evaluate dot product. \
Got: %s, %s.""" % (str(self.keys()), str(other.keys())))
			return dotprod
		else:
			raise ValueError("""\
Vectors must have the same dimensionality in order to evaluate dot product. \
Got: %d, %d.""" % (self._d[0].n_cols, other._d[0].n_cols))


	@property
	def cov(self):
		r"""
		The covariance matrix.

		.. seealso:: trackstar.covariance_matrix
		"""
		return self._cov


	def keys(self):
		r"""
		The keys.
		"""
		_keys = []
		for i in range(self._d[0].n_cols): _keys.append(
			copy_cstring(self._d[0].labels[i]))
		return _keys
