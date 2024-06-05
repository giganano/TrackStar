# cython: language_level = 3, boundscheck = False
#
# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/TrackStar.git.

__all__ = ["datum"]
import numbers
from .utils import copy_array_like_object, copy_cstring
from .utils cimport copy_pystring, strindex
from libc.stdlib cimport malloc, free
from libc.string cimport strlen, strcpy
from .matrix cimport matrix
from .track cimport track
from . cimport datum


cdef class datum:

	r"""
	.. class:: trackstar.datum(vector)

	A generic datum class for constructing data vectors with descriptive
	labels attached to each vector component.

	.. seealso:: ``trackstar.matrix``

	.. todo:: round out the documentation on this object.
	"""

	def __cinit__(self, vector, extra = {}):
		if not isinstance(vector, dict):
			raise TypeError("""\
Datum initialization requires type dict. Got: %s""" % (type(vector)))
		elif not all([isinstance(_, str) for _ in vector.keys()]):
			raise TypeError("""\
Datum initialization requires all dictionary keys to be of type str.""")
		elif not all([isinstance(
			vector[_], numbers.Number) for _ in vector.keys()]):
			raise TypeError("""\
Datum initialization requires a dictionary that stores only real numbers.""")
		else: pass

		err_tag = lambda x: x.startswith("err_") or x.endswith("_err")
		keys = list(vector.keys())
		errs = list(filter(err_tag, keys))
		if len(errs):
			for err in errs:
				if err.startswith("err_"):
					label = err[4:]
				else:
					label = err[:-4]
				if label not in keys:
					raise ValueError("""\
Datum initialization received measurement errors for quantity labeled %s, but \
input vector has no such label.""" % (label))
		else: pass

		qtys = list(filter(lambda x: not err_tag(x), keys))
		self._d = datum_initialize(len(qtys))
		self._m = <MATRIX *> self._d
		self._cov = covariance_matrix.identity(len(qtys))
		self._d[0].cov = self._cov._cov
		self._d[0].cov[0].labels = self._d[0].labels


	def __init__(self, vector, extra = {}):
		cdef char *copy
		err_tag = lambda x: x.startswith("err_") or x.endswith("_err")
		keys = list(vector.keys())
		qtys = list(filter(lambda x: not err_tag(x), keys))
		for i in range(len(qtys)):
			copy = copy_pystring(qtys[i])
			try:
				strcpy(self._d[0].labels[i], copy)
			finally:
				free(copy)
			self._d[0].vector[0][i] = vector[qtys[i]]
			if "%s_err" % (qtys[i]) in keys:
				self._d[0].cov[0].matrix[i][i] = vector["%s_err" % (qtys[i])]**2
			elif "err_%s" % (qtys[i]) in keys:
				self._d[0].cov[0].matrix[i][i] = vector["err_%s" % (qtys[i])]**2
			else: pass
		self.extra = extra
		self._shadow_keys = set([])


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
		rep = "datum(\n"
		keys = self.keys()
		for i in range(len(keys)):
			rep += "    %s " % (keys[i])
			for _ in range(15 - len(keys[i])): rep += "-"
			rep += "> %.4e\n" % (self._d[0].vector[0][i])
		if len(self.extra.keys()):
			rep += "\n"
			rep += "    extra\n"
			rep += "    -----\n"
			lines = self.extra.__repr__().split("\n")
			for relevant_line in lines[1:-1]: rep += "%s\n" % (relevant_line)
		else: pass
		rep += ")"
		return rep


	def __getitem__(self, key):
		cdef char *copy
		if isinstance(key, str):
			if key in self._shadow_keys: return float("nan")
			copy = copy_pystring(key)
			try:
				idx = strindex(self._d[0].labels, copy, self._d[0].n_cols)
			finally:
				free(copy)
			if idx != -1:
				return self._d[0].vector[0][idx]
			else:
				raise KeyError("Unrecognized datum label: %s" % (key))
		else:
			raise KeyError("Datum label must be a string. Got: %s" % (
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
					raise KeyError("""\
Unrecognized datum label: %s. If additional vector components are to be added, \
a new datum object must be created.""" % (key))
			else:
				raise TypeError("""\
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
		return datum(new_vector)


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
		return datum(new_vector)


	def __sub__(self, datum other):
		return self.__add__(other.__neg__())


	def __mul__(self, other):
		if isinstance(other, numbers.Number):
			return self._mul_prefactor_(other)
		elif isinstance(other, datum):
			return self._mul_datum_(other)
		elif isinstance(other, matrix):
			return self._mul_matrix_(other)
		else:
			raise TypeError("""\
Datum multiplication requires either a scalar prefactor or another datum to \
take the dot product with. Got: %s""" % (type(other)))


	def __rmul__(self, other):
		if isinstance(other, numbers.Number):
			return self.__mul__(other)
		else:
			raise TypeError("""\
Datum multiplication requires either a scalar prefactor or another datum to \
take the dot product with. Got: %s""" % (type(other)))


	def _mul_prefactor_(self, other):
		assert isinstance(other, numbers.Number), "Internal Error."
		new_vector = {}
		for key in self.keys():
			new_vector[key] = other * self.__getitem__(key)
		return datum(new_vector)


	def _mul_datum_(self, datum other):
		assert isinstance(other, datum)
		result = 0
		common_keys = list(set(self.keys()) & set(other.keys()))
		for key in common_keys:
			result += self.__getitem__(key) * other.__getitem__(key)
		return result


	@property
	def cov(self):
		r"""
		The covariance matrix.

		.. seealso:: trackstar.covariance_matrix
		"""
		return self._cov


	@property
	def extra(self):
		r"""
		Type : ``dict``

		Extra information associated with a datum that is not relevant to the
		fit but is useful nonetheless.
		"""
		return self._extra


	@extra.setter
	def extra(self, value):
		if isinstance(value, dict):
			self._extra = datum_extra(**value)
		else:
			raise TypeError("""\
Attribte 'extra' must be of type dict. Got: %s""" % (value))


	@property
	def _shadow_keys(self):
		r"""
		Type : ``set``

		.. warning::

			User access or modification of this class attribute is discouraged.

		A list of additional column labels that belong the sample that this
		datum is embedded within, if applicable. The ``sample`` object keeps
		track of this attribute automatically every time ``add_datum`` is
		called. Whenever ``datum.__getitem__`` is called with a string that
		is an element of this list, a ``nan`` will be returned.
		"""
		return self._shadow_keys


	def loglikelihood(self, track t, quantities = None,
		normalize_weights = True, use_line_segment_corrections = False):
		cdef DATUM *sub
		cdef char **labels
		if isinstance(normalize_weights, bool):
			t._t[0].normalize_weights = int(normalize_weights)
		else:
			raise TypeError("""\
Keyword arg 'normalize_weights' must be of type bool. Got: %s""" % (
				type(normalize_weights)))
		if isinstance(use_line_segment_corrections, bool):
			t._t[0].use_line_segment_corrections = int(
				use_line_segment_corrections)
		else:
			raise TypeError("""\
Keyword arg 'use_line_segment_corrections' must be of type bool. Got: %s""" % (
				type(use_line_segment_corrections)))
		self_keys = self.keys()
		track_keys = t.keys()
		if quantities is None:
			for key in self_keys:
				if key not in track_keys: raise ValueError("""\
Track does not have predictions for quantity labeled %s.""" % (key))
			return loglikelihood_datum(self._d[0], t._t)
		elif isinstance(quantities, list) or isinstance(quantities, tuple):
			for qty in quantities:
				if not isinstance(qty, str): raise TypeError("""\
Elements of keyword arg 'quantities' must all be of type str. Got: %s""" % (
					type(qty)))
				if qty not in self_keys: raise ValueError("""\
Datum does have measurements for quantity labeled %s.""" % (qty))
				if qty not in track_keys: raise ValueError("""\
Track does not have predictions for quantity labeled %s.""" % (qty))
			labels = <char **> malloc (len(quantities) * sizeof(char *))
			for i in range(len(quantities)):
				labels[i] = copy_pystring(quantities[i])
			sub = datum_specific_quantities(self._d[0], labels, len(quantities))
			try:
				return loglikelihood_datum(sub[0], t._t)
			finally:
				datum_free_everything(sub)
				for i in range(len(quantities)): free(labels[i])
				free(labels)
		else:
			raise TypeError("""\
Keyword arg 'quantities' must be of type list, tuple, or None. Got: %s""" % (
				type(quantities)))


	def keys(self):
		r"""
		The keys.
		"""
		_keys = []
		for i in range(self._d[0].n_cols): _keys.append(
			copy_cstring(self._d[0].labels[i]))
		return _keys


	def tomatrix(self):
		r"""
		Get the data vector as a matrix.
		"""
		return matrix([[self[key] for key in self.keys()]])


class datum_extra(dict):

	# Does nothing more than print the dictionary in the same format as
	# data vectors.

	def __repr__(self):
		rep = "extra(\n"
		keys = self.keys()
		for key in keys:
			rep += "    %s " % (key)
			for _ in range(15 - len(key)): rep += "-"
			rep += "> %s\n" % (self.__getitem__(key).__repr__())
		rep += ")"
		return rep
