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
	If ``obj`` is a ``tuple`` or ``list``, it will pull with the copy with
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
