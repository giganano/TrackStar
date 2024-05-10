# cython: language_level = 3, boundscheck = False
#
# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/trackstar.git.

__all__ = ["sample"]
import numbers
from .utils import copy_array_like_object, copy_cstring
from .utils cimport copy_pystring, strindex
from .datum cimport datum
from . cimport sample
from .track cimport track

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
			rep += "> %s\n" % (track._repr_format_array_(self[key]))
		rep += "])"
		return rep


	def __getitem__(self, key):
		r"""
		.. todo:: Allow indexing according to rule sample[row, col]
		"""
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
				values = []
				for i in range(self.size):
					if key in self._data[i].keys():
						values.append(self._data[i][key])
					else:
						values.append(float("nan"))
				return values
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
string. Got: %s""" % (type(key[0]), type(key[1])))
			else:
				raise IndexError("""\
Sample indexing requires at most two parameters. Got: %d""" % (len(key)))
		else:
			raise IndexError("""\
Sample index must be either a data vector index (int) or a quantity label \
(str). Got: %s""" % (type(key)))


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
