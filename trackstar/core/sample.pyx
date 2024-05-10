# cython: language_level = 3, boundscheck = False
#
# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/trackstar.git.

__all__ = ["sample"]
import math as m
import numbers
from .utils import copy_array_like_object, copy_cstring
from .utils cimport copy_pystring, strindex, linked_list
from .datum cimport datum
from . cimport sample
from .track cimport track
from libc.stdlib cimport malloc, free
from libc.stdint cimport uintptr_t

cdef class sample:

	r"""
	.. class:: trackstar.sample()

	The data in some observed space.

	.. todo::

		Finish documentation on this class. Correct and tidy up comment headers
		in src/sample.c and src/sample.h.

	.. todo:: Allow item assignment according to rule sample[row, col]
	"""

	def __cinit__(self):
		self._s = sample_initialize()
		self._data = []


	def __dealloc__(self):
		r"""
		Frees up the memory stored by a ``sample`` object. User access strongly
		discouraged.
		"""
		sample_free(self._s)


	def __enter__(self):
		r"""Opens a with statement."""
		return self


	def __exit__(self, exc_type, exc_value, exc_tb):
		r"""Raises all exceptions inside with statements."""
		return exc_value is None


	def __repr__(self):
		r"""Returns a string representation of the sample."""
		rep = "sample([\n"
		rep += "        N = %d\n" % (self.size)
		for key in self.keys():
			rep += "        %s " % (key)
			for j in range(15 - len(key)): rep += '-'
			rep += "> %s\n" % (linked_list._repr_format_array_(self[key]))
		rep += "])"
		return rep


	def __getitem__(self, key):
		r"""
		.. todo:: Allow indexing according to rule sample[row, col]
		"""
		cdef double **copies
		cdef char *label
		if isinstance(key, numbers.Number):
			if key % 1 == 0:
				key = int(key)
				if -self.size <= key < 0: key += self.size
				if not 0 <= key < self.size: raise IndexError("""\
Index %d out of range for sample of size N = %d.""" % (key, self.size))
				return self._data[key]
			else: raise IndexError("Sample index must be an integer, not float.")
		elif isinstance(key, str):
			if key in self.keys():
				copies = <double **> malloc (self.size * sizeof(double *))
				for i in range(self.size):
					label = copy_pystring(key)
					idx = strindex(self._s[0].data[i][0].labels, label,
						self._s[0].data[i][0].n_cols)
					if idx != -1:
						copies[i] = &self._s[0].data[i][0].vector[0][idx]
					else:
						copies[i] = <double *> malloc (sizeof(double))
						copies[i][0] = float("nan")
				return linked_list(<uintptr_t> copies, self.size)
			else:
				raise KeyError("Sample quantity label %s not recognized." % (
					key))
		elif isinstance(key, tuple):
			if len(key) == 2:
				if (isinstance(key[0], str) and 
					isinstance(key[1], numbers.Number)):
					# hand it off to datum.__getitem__
					# error handling in number instance above will take care of
					# floating point IndexError
					return self.__getitem__(key[1])[key[0]]
				elif (isinstance(key[1], str) and 
					isinstance(key[0], numbers.Number)):
					# same as above, just different order
					return self[key[1], key[0]]
				else:
					raise IndexError("""\
Sample indexing with both row number and column label requires an int and a \
string. Got: %s, %s""" % (type(key[0]), type(key[1])))
			else:
				raise IndexError("""\
Sample indexing requires at most two parameters. Got: %d""" % (len(key)))
		else:
			raise IndexError("""\
Sample index must be either a data vector index (int) or a quantity label \
(str). Got: %s""" % (type(key)))


	def __setitem__(self, index, value):
		cdef char *copy
		if isinstance(index, numbers.Number):
			if index % 1 == 0:
				index = int(index)
				if -self.size <= index < 0: index += self.size
				if not 0 <= index < self.size: raise IndexError("""\
Index %d out of bounds for sample of size N = %d.""" % (index, self.size))
				if isinstance(value, datum):
					if set(value.keys()) == set(self._data[index].keys()):
						for key in value.keys():
							self._data[index][key] = value[key]
					else:
						raise ValueError("""\
Sample item assignment by row number requires dictionary or datum keys to \
match.""")
				elif isinstance(value, dict):
					if set(value.keys()) == set(self._data[index].keys()):
						for key in value.keys():
							if isinstance(value[key], numbers.Number):
								self._data[index][key] = value[key]
							else:
								raise TypeError("""\
Sample only supports real numbers. Got: %s""" % (type(value[key])))
					else:
						raise ValueError("""\
Sample item assignment by row number requires dictionary or datum keys to \
match.""")
				else:
					raise TypeError("""\
Sample item assignment by row number requires a dictionary or datum. \
Got: %s""" % (type(value)))
			else:
				raise TypeError("""\
Sample item assignment by row number requires an int, not foat.""")
		elif isinstance(index, str):
			if index not in self.keys(): raise ValueError("""\
Column label not recognized: %s""" % (index))
			try:
				value = copy_array_like_object(value)
			except TypeError:
				raise TypeError("""\
Sample item assignment by column label requires an array-like object. \
Got: %s""" % (type(value)))
			if len(value) != self.size: raise ValueError("""\
Array length mismatch: %d for sample of size %d.""" % (len(value), self.size))
			if all([isinstance(_, numbers.Number) for _ in value]):
				copy = copy_pystring(index)
				for i in range(self.size):
					idx = strindex(self._s[0].data[i][0].labels, copy,
						self._s[0].data[i][0].n_cols)
					if idx != -1 and not m.isnan(value[i]):
						self._s[0].data[i][0].vector[0][idx] = value[i]
					elif idx != -1 and m.isnan(value[i]):
						raise ValueError("""\
Datum at index %d stores a quantity labeled %s. Cannot assign item at this \
index to NaN as it would change dimensionality of the data and result in \
memory errors.""" % (i, index))
					elif idx == -1 and m.isnan(value[i]):
						pass
					else: # idx == -1 and not m.isnan(value[i])
						raise ValueError("""\
Datum at index %d does not have a quantity labeled %s. Item assignment by \
column label requires a NaN at this position.""" % (i, index))
				free(copy)
			else:
				raise TypeError("""\
Sample item assignment detected non-numerical value in array-like object.""")
		elif isinstance(index, tuple):
			if len(index) == 2:
				if (isinstance(index[0], str) and
					isinstance(index[1], numbers.Number)):
					if index[1] % 1 == 0:
						row = int(index[1])
						if -self.size <= row < 0: row += self.size
						if not 0 <= row < self.size: raise IndexError("""\
Index %d out of bounds for sample of size N = %d.""" % (row, self.size))
						if not isinstance(value, numbers.Number):
							raise TypeError("""\
Sample item assignment requires a real number. Got: %s""" % (type(value)))
						if index[0] in self._data[row].keys():
							if not m.isnan(value):
								self._data[row][index[0]] = value
							else:
								raise ValueError("""\
Datum at index %d stores a quantity labeled %s. Cannot assign item at this \
index to NaN as it would change dimensionality of the data and result in \
memory errors.""" % (row, index[0]))
						else:
							if not m.isnan(value): raise KeyError("""\
Datum at index %d does not store a quantity labeled %s. Only item assignment \
to NaN is allowed.""" % (row, index[0]))
					else:
						raise IndexError("""\
Sample row number must be an int, not float.""")
				elif (isinstance(index[1], str) and
					isinstance(index[0], numbers.Number)):
					self[index[1], index[0]] = value
				else:
					raise IndexError("""\
Sample item assignment by row number and column label requires an int and a \
string. Got: %s, %s""" % (type(index[0]), type(index[1])))
			else:
				raise IndexError("""\
Sample item assignment requires at most two parameters. Got: %d""" % (
					len(index)))
		else:
			raise IndexError("""\
Sample assignment requires a row number (int), column label (str), or some \
combination of the two. Got: %s""" % (type(index)))


	def add_datum(self, datum measurement):
		r"""
		Add a data vector to the sample.
		"""
		sample_add_datum(self._s, measurement._d)
		self._data.append(measurement)


	def loglikelihood(self, track t, normalize_weights = True):
		r"""
		Compute natural logarithm of the likelihood that this sample would be
		observed by the model predicted track ``t``.

		.. todo::

			Error handling for case where the input track does not have
			predictions for each quantity in the sample. This causes a
			segmentation fault in the C library, because ``track_subset``
			in ./likelihood.c returns ``NULL``.

		.. todo:: Raise a warning when normalize_weights is False
		"""
		if isinstance(normalize_weights, bool):
			t._t[0].normalize_weights = int(normalize_weights)
			return loglikelihood_sample(self._s, t._t)
		else:
			raise TypeError("""\
Keyword arg 'normalize_weights' must be of type bool. Got: %s""" % (
				type(normalize_weights)))


	@property
	def size(self):
		r"""
		Type : ``int`` [non-negative]

		Sample size: the number of data vectors stored by this object.
		"""
		return self._s[0].n_vectors


	def keys(self):
		_keys = []
		for i in range(self.size):
			for test in self._data[i].keys():
				if test not in _keys: _keys.append(test)
		return _keys
