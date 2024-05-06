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
import numbers
import array
import sys
from . cimport utils
from libc.stdlib cimport malloc
from libc.string cimport memset, strlen


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
