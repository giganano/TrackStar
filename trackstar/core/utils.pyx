# cython: language_level = 3, boundscheck = False
#
# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/trackstar.git.

try:
	# NumPy compatible but not NumPy dependent
	import numpy as np
except (ModuleNotFoundError, ImportError):
	pass
try:
	# Pandas compatible but not Pandas dependent
	import pandas as pd
except (ModuleNotFoundError, ImportError):
	pass
import math as m
import numbers
import array
import sys
from . cimport utils
from libc.stdlib cimport malloc, free
from libc.string cimport memset, strlen
from libc.stdint cimport uintptr_t
from libc.math cimport isnan

cdef class linked_list:

	r"""
	.. todo:: documentation on this object.
	"""

	def __init__(self, unsigned long address, unsigned long length):
		self._arr = <double **> (<uintptr_t> address)
		self._length = length


	def __dealloc__(self):
		for i in range(len(self)):
			if isnan(self._arr[i][0]): free(self._arr[i])
		free(self._arr)


	def __enter__(self):
		r"""Opens a with statement."""
		return self


	def __exit__(self, exc_type, exc_value, exc_tb):
		r"""Raises all errors inside of with statements."""
		return exc_value is None


	def __len__(self):
		return self._length


	def __iter__(self):
		for i in range(len(self)): yield self._arr[i][0]


	def __repr__(self):
		copy = [x for x in self]
		return "linked_array(%s)" % (self._repr_format_array_(copy))


	@staticmethod
	def _repr_format_array_(arr):
		def format_number(num):
			# ensures all numbers, including nans, become strings with the
			# same length.
			if m.isnan(num):
				s = " nan       "
			elif str(num).startswith("-"): # -0.0 fails the num >= 0 condition
				s = "%.4e" % (num)
			else:
				s = " %.4e" % (num)
			return s
		if len(arr) > 10:
			rep = "[%s, %s, %s, ..., %s, %s, %s]" % (
				format_number(arr[0]),
				format_number(arr[1]),
				format_number(arr[2]),
				format_number(arr[-3]),
				format_number(arr[-2]),
				format_number(arr[-1]))
		else:
			rep = "[%s" % (format_number(arr[0]))
			for i in range(1, len(arr)):
				rep += ", %s" % (format_number(arr[i]))
			rep += "]"
		return rep


	def __getitem__(self, index):
		if isinstance(index, numbers.Number):
			if index % 1 == 0:
				index = int(index)
				if -len(self) <= index < 0: index += len(self)
				if not 0 <= index < len(self): raise IndexError("""\
Index %d is out of bounds for list of length %d.""" % (index, len(self)))
				return self._arr[index][0]
			else:
				raise IndexError("List index should be an int, not float.")
		else:
			raise IndexError("List index should be an int. Got: %s" % (
				type(index)))


	def __setitem__(self, index, value):
		if isinstance(index, numbers.Number):
			if index % 1 == 0:
				if isinstance(value, numbers.Number):
					index = int(index)
					if -len(self) <= index < 0: index += len(self)
					if not 0 <= index < len(self): raise IndexError("""\
Index %d is out of bounds for list of length %d.""" % (index, len(self)))
					if isnan(self._arr[index][0]) and not m.isnan(value):
						raise ValueError("""\
Item assignment does not allow changing a NaN to a real number as it changes \
the dimensionality of the data, resulting in memory errors.""")
					elif not isnan(self._arr[index][0]) and m.isnan(value):
						raise ValueError("""\
Item assignment does not allow changing a real number to a NaN as it changes \
the dimensionality of the data, resulting in memory errors.""")
					else:
						self._arr[index][0] = <double> value
				else:
					raise TypeError("""\
Item assignment requires a real number. Got: %s""" % (type(value)))
			else:
				raise IndexError("List index should be an int, not float.")
		else:
			raise IndexError("List index should be an int. Got: %s" % (
				type(index)))


cdef class linked_dict:

	r"""
	.. todo:: documentation on this object.
	"""

	def __init__(self, unsigned long data_address, unsigned long keys_address,
		unsigned short n_elements):
		self._arr = <double **> (<uintptr_t> data_address)
		self._keys = <char **> (<uintptr_t> keys_address)
		self._n_elements = n_elements


	def __dealloc__(self):
		free(self._arr)
		free(self._keys)


	def __enter__(self):
		r"""Opens a with statement."""
		return self


	def __exit__(self, exc_type, exc_value, exc_tb):
		r"""Raises all exceptions inside of with statements."""
		return exc_value is None


	def __repr__(self):
		rep = "linked_dictionary({\n"
		for i in range(self._n_elements):
			rep += "  \"%s\": %.4e,\n" % (copy_cstring(self._keys[i]),
				self._arr[i][0])
		rep += "})"
		return rep


	def __getitem__(self, key):
		cdef char *copy
		if isinstance(key, str):
			copy = copy_pystring(key)
			try:
				idx = strindex(self._keys, copy, self._n_elements)
			finally:
				free(copy)
			if idx != -1:
				return self._arr[idx][0]
			else:
				raise KeyError("Unrecognized quantity label: %s." % (key))
		else:
			raise KeyError("Expected a string. Got: %s" % (type(key)))


	def __setitem__(self, key, value):
		cdef char *copy
		if isinstance(key, str):
			if isinstance(value, numbers.Number):
				copy = copy_pystring(key)
				try:
					idx = strindex(self._keys, copy, self._n_elements)
				finally:
					free(copy)
				if idx != -1:
					if isnan(self._arr[idx][0]) and not m.isnan(value):
						raise ValueError("""\
Item assignment does not allow changing a NaN to a real number as it changes \
the dimensionality of the data, resulting in memory errors.""")
					elif not isnan(self._arr[idx][0]) and m.isnan(value):
						raise ValueError("""\
Item assignment does not allow changing a real number to a NaN as it changes \
the dimensionality of the data, resulting in memory errors.""")
					else:
						self._arr[idx][0] = <double> value
				else:
					raise KeyError("Unrecognized quantity label: %s" % (key))
			else:
				raise TypeError("Expected a real number. Got: %s" % (
					type(value)))
		else:
			raise KeyError("Expected a string. Got: %s" % (type(key)))


	def keys(self):
		_keys = []
		for i in range(self._n_elements):
			_keys.append(copy_cstring(self._keys[i]))
		return _keys



def copy_array_like_object(obj):
	r"""
	.. function:: copy_array_like_object(obj)

	Pull a copy of an array-like object into a Python ``list``.

	Parameters
	----------
	obj : ``array-like``
		Some array-like object, such as a NumPy ``ndarray``, a Pandas
		``DataFrame``, a Python built-in ``array``. If the object is none of
		these types, this function will look for a ``__getitem__`` function,
		and attempt to pull each element from 0 to ``len(obj)``.

	Returns
	-------
	copy : ``list``
		``obj`` copied to a ``list``.

	Raises
	------
	* ``TypeError``
		- Unable to convert ``obj`` to an array.

	Notes
	-----
	If ``obj`` is a NumPy ``ndarray`` or a built-in Python ``array``, this
	function will copy it with ``.tolist()``. If ``obj`` is a Pandas
	``DataFrame``, it will use ``[obj[_] for _ in obj.values.tolist()]``.
	If ``obj`` is a ``tuple`` or ``list``, it will pull the copy with
	``obj[:]``. If ``obj`` is none of these types, this function will look for
	``"__getitem__"`` and ``"__len__"`` attributes, and then pull a copy with
	``[obj[_] for _ in range(len(obj))]``.
	"""
	if isinstance(obj, array.array) or ("numpy" in sys.modules and
		isinstance(obj, np.ndarray)):
		copy = obj.tolist()
	elif "pandas" in sys.modules and isinstance(obj, pd.DataFrame):
		copy = [_[0] for _ in obj.values.tolist()]
	elif isinstance(obj, list) or isinstance(obj, tuple):
		copy = obj[:]
	elif hasattr(obj, "__getitem__") and hasattr(obj, "__len__"):
		copy = [obj[_] for _ in range(len(obj))]
	else:
		raise TypeError("""\
Unable to convert array-like object into list. Got: %s""" % (type(obj)))
	return copy


def is_ascii(pystr):
	r"""
	Determine if a string is made of entirely ascii characters.

	Parameters
	----------
	pystr : str
		The python string itself

	Returns
	-------
	is_ascii : bool
		True if ``pystr`` is made entirely of ascii characters; False
		otherwise.

	Raises
	------
	* TypeError
		- ``pystr`` is not a string.
	"""
	if isinstance(pystr, str):
		return all([ord(c) < 128 for c in pystr])
	else:
		raise TypeError("Must be of type str. Got: %s" % (type(pystr)))


cdef char *copy_pystring(pystr) except *:
	r"""
	Obtain a copy of a python string as a C pointer.

	Parameters
	----------
	pystr : ``str``
		The input python string. Must contain only ascii characters.

	Returns
	-------
	copy : ``char *``
		A character pointer containing a copy of the same string.

	Raises
	------
	* TypeError
		- ``pystr`` is not a string.
	* ValueError
		- ``pystr`` contains non-ascii characters.
	"""
	cdef char *copy
	if isinstance(pystr, str):
		if is_ascii(pystr):
			copy = <char *> malloc (MAX_LABEL_SIZE * sizeof(char))
			memset(copy, 0, MAX_LABEL_SIZE)
			for i in range(len(pystr)): copy[i] = <int> ord(pystr[i])
			return copy
		else:
			raise ValueError("""\
TrackStar does not support non-ascii characters in strings. Got: %s""" % (
				pystr))
	else:
		raise TypeError("Expected a string. Got: %s" % (type(pystr)))


def copy_cstring(char *cstr):
	r"""
	Obtain a copy of a C char pointer as a python string.

	Parameters
	----------
	cstr : ``char *``
		The input string, as a pointer to the first character.

	Returns
	-------
	copy : ``str``
		The same string, but as a python variable.
	"""
	copy = ""
	for i in range(strlen(cstr)): copy += chr(cstr[i])
	return copy
