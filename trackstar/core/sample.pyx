# cython: language_level = 3, boundscheck = False
#
# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/TrackStar.git.

__all__ = ["sample"]
import math as m
import warnings
import numbers
from .datum import datum_extra
from .utils import copy_array_like_object, copy_cstring
from .utils cimport copy_pystring, strindex, linked_list
from .matrix cimport matrix_free
from .covariance_matrix cimport covariance_matrix_free
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

	def __cinit__(self, *args, extra = {}):
		if len(args) == 1:
			arg = args[0]
			if isinstance(arg, dict):
				keys = list(arg.keys())
				for key in keys:
					if isinstance(key, str):
						try:
							copy = copy_array_like_object(arg[key])
						except TypeError:
							raise TypeError("""\
Sample initialization from dictionary requires array-like objects. \
Got: %s""" % (type(arg[key])))
						if len(copy) != len(arg[keys[0]]):
							raise ValueError("""\
Array-length mismatch: sample initialization from dictionary requires \
array-like objects of equal length. \
Got: %d, %d""" % (len(copy), len(arg[keys[0]])))
						else: pass
					else:
						raise TypeError("""\
Sample initialization from dictionary requires keys of type str. \
Got: %s""" % (type(key)))
			else:
				raise TypeError("""\
Sample initialization requires either no argument (empty sample) or type
dict. Got: %s""" % (type(arg)))
		elif len(args) > 1:
			raise TypeError("""\
Sample initialization requires either no argument (empty sample) or a \
single argument of type dict. Got: %d arguments.""" % (len(args)))
		else: pass

		self._s = sample_initialize()
		self._data = []


	def __init__(self, *args, extra = {}):
		if len(args) == 1:
			err_tag = lambda x: x.startswith("err_") or x.endswith("_err")
			arg = args[0]
			keys = list(arg.keys())
			qtys = list(filter(lambda x: not err_tag(x), keys))
			for i in range(len(arg[qtys[0]])):
				this_datum = {}
				for qty in qtys:
					if not m.isnan(arg[qty][i]):
						this_datum[qty] = arg[qty][i]
						if "%s_err" % (qty) in keys:
							err = arg["%s_err" % (qty)][i]
						elif "err_%s" % (qty) in keys:
							err = arg["err_%s" % (qty)][i]
						else:
							err = float("nan")
						if not m.isnan(err): this_datum["%s_err" % (qty)] = err
					else: pass
				self.add_datum(datum(this_datum))
		else: pass
		if isinstance(extra, dict):
			for key in extra.keys():
				self.extra[key] = extra[key]
		else:
			raise TypeError("""\
Keyword arg \'extra\' must be of type dict. Got: %s""" % (type(extra)))


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
		rep = "sample(\n"
		rep += "    N = %d\n" % (self.size)
		for key in self.keys():
			rep += "    %s " % (key)
			for j in range(15 - len(key)): rep += '-'
			rep += "> %s\n" % (linked_list._repr_format_array_(self[key]))
		extra = self.extra
		if len(extra.keys()):
			rep += "\n"
			rep += "    extra\n"
			rep += "    -----\n"
			lines = extra.__repr__().split("\n")
			for relevant_line in lines[1:-1]: rep += "%s\n" % (relevant_line)
		else: pass
		rep += ")"
		return rep


	def __len__(self):
		return self._s[0].n_vectors


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
					# out of range and floating point IndexError
					# datum._shadow_keys should return the necessary NaNs
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
		elif isinstance(key, slice):
			sl = linked_list._indexing_handle_slice_(key)
			vectors = self._data[sl]
			subset = sample()
			for v in vectors: subset.add_datum(v)
			return subset
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
column label requires a NaN at this position. Cannot assign this item a real \
number value as it would change the dimensionality of the data and result in \
memory errors.""" % (i, index))
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
		for key in self.keys():
			for i in range(self.size):
				if (key not in self._data[i].keys() and
					key not in self._data[i]._shadow_keys):
					self._data[i]._shadow_keys.add(key)


	def loglikelihood(self, track t, quantities = None,
		normalize_weights = True, use_line_segment_corrections = False):
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
		cdef SAMPLE *sub
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
			return loglikelihood_sample(self._s[0], t._t)
		elif isinstance(quantities, list) or isinstance(quantities, tuple):
			for qty in quantities:
				if not isinstance(qty, str): raise TypeError("""\
Elements of keyword arg 'quantities' must all be of type str. Got: %s""" % (
					type(qty)))
				if qty not in self_keys: raise ValueError("""\
Sample does not have measurements for quantity labeled %s.""" % (qty))
				if qty not in track_keys: raise ValueError("""\
Track does not have predictions for quantity labeled %s.""" % (qty))
			labels = <char **> malloc (
				len(quantities) * sizeof(char *))
			for i in range(len(quantities)):
				labels[i] = copy_pystring(quantities[i])
			sub = sample_specific_quantities(self._s[0], labels,
				len(quantities))
			try:
				return loglikelihood_sample(sub[0], t._t)
			finally:
				sample_free_everything(sub)
				for i in range(len(quantities)): free(labels[i])
				free(labels)
		else:
			raise TypeError("""\
Keyword arg 'quantities' must be of type list, tuple, or None. Got: %s""" % (
				type(quantities)))


	@property
	def size(self):
		r"""
		Type : ``int`` [non-negative]

		Sample size: the number of data vectors stored by this object.
		"""
		return self._s[0].n_vectors


	@property
	def extra(self):
		return sample_extra([self._data[i].extra for i in range(self.size)])


	def keys(self):
		_keys = []
		for i in range(self.size):
			for test in self._data[i].keys():
				if test not in _keys: _keys.append(test)
		return _keys


	def filter(self, label, condition, value,
		keep_missing_measurements = False):
		r"""
		Filter the sample based on some condition applied to one specific
		column.
		"""
		cdef char *copy
		cdef unsigned long *indices
		if isinstance(label, str):
			if isinstance(condition, str):
				if isinstance(value, numbers.Number):
					if isinstance(keep_missing_measurements, bool):
						indicators = {
							"=":   1,
							"==":  1,
							"<":   2,
							"<=":  3,
							">":   4,
							">=":  5
						}
						if condition in indicators.keys():
							copy = copy_pystring(label)
							indices = sample_filter_indices(self._s[0],
								copy, indicators[condition], value,
								int(keep_missing_measurements))
							assert indices is not NULL, "Internal Error."
						else:
							raise ValueError("""\
Argument \'condition\' must be either \'=\', \'==\', \'<\', \'<=\', \'>\', \
or \'>=\'. Got: %s""" % (condition))
						sub = sample()
						for i in range(1, indices[0]):
							sub.add_datum(self._data[indices[i]])
						if not sub.size: warnings.warn(
							"Filter resulted in an empty sample.",
							UserWarning)
						return sub
					else:
						raise TypeError("""\
Keyword arg \'keep_missing_measurements\' must be a boolean. Got: %s""" % (
							type(keep_missing_measurements)))
				else:
					raise TypeError("""\
Argument \'value\' must be a real number. Got: %s""" % (type(value)))
			else:
				raise TypeError("""\
Argument \'condition\' must be a string. Got: %s""" % (type(condition)))
		else:
			raise TypeError("""\
Argument \'label\' must be a string. Got: %s""" % (type(label)))


class sample_extra(list):

	def __init__(self, *args, **kwargs):
		super().__init__(*args, **kwargs)
		for item in self: assert isinstance(item, datum_extra), """\
sample_extra.__init__: Internal Error."""


	def __repr__(self):
		rep = "extra(\n"
		for key in self.keys():
			rep += "    %s " % (key)
			for _ in range(15 - len(key)): rep += "-"
			rep += "> %s\n" % (self._format_arr_(self.__getitem__(key)))
		rep += ")"
		return rep


	def __getitem__(self, idx):
		if isinstance(idx, str):
			recognized = False
			for item in self:
				recognized |= idx in item.keys()
				if recognized: break
			if recognized:
				result = self.__len__() * [None]
				for i in range(self.__len__()):
					if idx in item.keys():
						result[i] = super().__getitem__(i)[idx]
					else: pass
				return result
			else:
				raise KeyError("\'%s\'" % (idx))
		elif isinstance(idx, numbers.Number):
			return super().__getitem__(idx)
		elif isinstance(idx, tuple):
			if len(idx) == 2:
				return self.__getitem__(idx[0])[idx[1]]
			else:
				raise KeyError("Too many indices. Expected 1 or 2, got: %d" % (
					len(idx)))
		else:
			raise KeyError("Expected a string or an integer. Got: %s" % (
				type(idx)))


	def __setitem__(self, idx, value):
		if isinstance(idx, str):
			try:
				copy = value[:]
			except:
				raise TypeError("Expected an array-like object. Got: %s" % (
					type(value)))
			if len(copy) == self.__len__():
				for i in range(self.__len__()):
					if value[i] is not None:
						super().__getitem__(i)[idx] = value[i]
					else: pass
			else:
				raise ValueError("""\
Array-length mismatch: got %d for a sample of size %d""" % (
					len(copy), self.__len__()))
		elif isinstance(idx, numbers.Number):
			if isinstance(value, dict):
				super().__setitem__(idx, datum_extra(**value))
			else:
				raise TypeError("Expected a dict. Got: %s" % (type(value)))
		elif isinstance(idx, tuple):
			if len(idx) == 2:
				if (isinstance(idx[0], numbers.Number) and
					isinstance(idx[1], str)):
					self.__getitem__(idx[0]).__setitem__(idx[1], value)
				elif (isinstance(idx[1], numbers.Number) and
					isinstance(idx[0], str)):
					self.__getitem__(idx[1]).__setitem__(idx[0], value)
				else:
					raise KeyError("""\
Expected an integer and a string. Got: %s, %s""" % (type(idx[0]), type(idx[1])))
			else:
				raise KeyError("Too many indices. Expected 1 or 2, got: %d" % (
					len(idx)))
		else:
			raise KeyError("Expected a string or an integer. Got: %s" % (
				type(idx)))


	@staticmethod
	def _format_arr_(arr):
		if len(arr) > 10:
			rep = "[%s, %s, %s, ..., %s, %s, %s]" % (
				str(sample_extra._format_item_(arr[0])),
				str(sample_extra._format_item_(arr[1])),
				str(sample_extra._format_item_(arr[2])),
				str(sample_extra._format_item_(arr[-3])),
				str(sample_extra._format_item_(arr[-2])),
				str(sample_extra._format_item_(arr[-1])))
		else:
			rep = "[%s" % (str(sample_extra._format_item_(arr[0])))
			for i in range(1, len(arr)):
				rep += ", %s" % (str(sample_extra._format_item_(arr[i])))
			rep += "]"
		return rep


	@staticmethod
	def _format_item_(item):
		assert isinstance(item, str), """\
sample_extra._format_item_: Internal Error."""
		if len(item) > 10:
			return "%s...%s" % (item[:4], item[-4:])
		else:
			return item.rjust(11)


	def keys(self):
		_keys = []
		for i in range(self.__len__()):
			for key in super().__getitem__(i):
				if key not in _keys: _keys.append(key)
		return _keys
