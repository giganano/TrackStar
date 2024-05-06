# cython: language_level = 3, boundscheck = False
#
# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/trackstar.git.

__all__ = ["track"]
import numbers
from .utils import copy_array_like_object, copy_cstring
from .utils cimport copy_pystring, strindex
from . cimport track
from libc.stdlib cimport malloc, free

cdef class track:

	r"""
	.. class:: trackstar.track(predictions, weights = None)

	The model predicted "track" or "curve" through the observed space.

	.. todo:: Finish documentation on this class.
	"""

	def __cinit__(self, predictions, weights = None,
		use_line_segment_corrections = True,
		line_segment_correction_tolerance = 1.01):
		if not isinstance(predictions, dict): raise TypeError("""\
Track must be initialized from type dict. Got: %s""" % (type(predictions)))
		copy = {}
		keys = list(predictions.keys())
		for key in keys:
			if not isinstance(key, str): raise TypeError("""\
Dictionary keys must be of type str. Got: %s""" % (type(key)))
			try:
				item = copy_array_like_object(predictions[key])
			except TypeError:
				raise TypeError("""\
Dictionary must store array-like objects to construct a track. Got: %s""" % (
					type(predictions[key])))
			if not all([isinstance(_, numbers.Number) for _ in item]):
				raise ValueError("""\
Non-numerical value detected in predictions[%s]. All stored values must be \
real numbers to construct a track.""" % (key))
			elif len(copy.keys()):
				if len(item) == len(copy[keys[0]]):
					copy[key] = item
				else:
					raise ValueError("""\
Array-length mismatch. Got: %d, %d. Must be equal to construct a track.""" % (
						len(copy[keys[0]]), len(item)))
			else:
				copy[key] = item


		if "weights" in keys:
			if weights is None:
				weights = copy["weights"]
			else: raise ValueError(
				"Received multiple inputs for track weights.")
		else:
			if weights is None:
				weights = len(copy[keys[0]]) * [1.]
			else:
				try:
					weights = copy_array_like_object(weights)
				except TypeError:
					raise TypeError("""\
Track weights must be an array-like object. Got: %s""" % (type(weights)))
				if len(weights) != len(copy[keys[0]]):
					raise ValueError("""\
Array-length mismath. Got: %d, %d. Weights must have the same number of \
elements as the track predictions.""" % (len(weights), len(copy[keys[0]])))
				else: pass

		cdef double *w = <double *> malloc (len(weights) * sizeof(double))
		cdef double **data = <double **> malloc (
			len(copy[keys[0]]) * sizeof(double *))
		cdef char **labels = <char **> malloc (len(keys) * sizeof(char *))
		for i in range(len(keys)):
			labels[i] = copy_pystring(keys[i])
		for i in range(len(copy[keys[0]])):
			w[i] = weights[i]
			data[i] = <double *> malloc (len(keys) * sizeof(double))
			for j in range(len(keys)):
				data[i][j] = copy[keys[j]][i]
		try:
			self._t = track_initialize(data, labels, w,
				len(weights), len(keys))
			self._m = <MATRIX *> self._t
		finally:
			free(w)
			free(data)
			for i in range(len(keys)): free(labels[i])
			free(labels)


	def __init__(self, predictions, weights = None,
		use_line_segment_corrections = True,
		line_segment_correction_tolerance = 1.01):
		self.use_line_segment_corrections = use_line_segment_corrections
		self.line_segment_correction_tolerance = line_segment_correction_tolerance


	def __dealloc__(self):
		r"""
		Frees up the memory stored by a ``track`` object. User access strongly
		discouraged.
		"""
		track_free(self._t)


	def __enter__(self):
		r"""Opens a with statement."""
		return self


	def __exit__(self, exc_type, exc_value, exc_tb):
		r"""Raises all exceptions inside with statements."""
		return exc_value is None


	def __repr__(self):
		r"""Returns a string representation of the track."""
		rep = "track([\n"
		keys = self.keys() + ["weights"]
		for key in keys:
			rep += "       %s " % (key)
			for j in range(15 - len(key)): rep += '-'
			rep += "> %s\n" % (self._repr_format_array_(self.__getitem__(key)))
		rep += "])"
		return rep


	@staticmethod
	def _repr_format_array_(arr):
		if len(arr) > 10:
			rep = "[%.4e, %.4e, %.4e, ..., %.4e, %.4e, %.4e]" % (
				arr[0], arr[1], arr[2], arr[-3], arr[-2], arr[-1])
		else:
			rep = "[%.4e" % (arr[0])
			for i in range(1, len(arr)):
				rep += ", %.4e" % (arr[i])
			rep += "]"
		return rep


	def __getitem__(self, key):
		if isinstance(key, str):
			return self._getitem_str_(key)
		elif isinstance(key, numbers.Number):
			return self._getitem_number_(key)
		elif isinstance(key, slice):
			return self._getitem_slice_(key)
		elif isinstance(key, tuple):
			return self._getitem_tuple_(key)
		else:
			raise IndexError("""\
Track indexing requires a row number (int), a column label (str), or a \
combination of the two. Got: %s""" % (type(key)))


	def _getitem_tuple_(self, key):
		if len(key) == 2:
			if isinstance(key[0], str):
				col = self._indexing_handle_str_(key[0])
				if isinstance(key[1], numbers.Number):
					row = self._indexing_handle_number_(key[1])
					if col == -2:
						return self._t[0].weights[row]
					else:
						return self._t[0].predictions[row][col]
				elif isinstance(key[1], slice):
					sl = self._indexing_handle_slice_(key[1])
					start = sl.start
					stop = sl.stop
					step = sl.step
					if col == -2:
						return [self._t[0].weights[i] for i in range(
							start, stop, step)]
					else:
						return [self._t[0].predictions[i][col] for i in range(
							start, stop, step)]
				else:
					raise IndexError("""\
Track indexing requires a row number (int), a column label (str), or a \
combination of the two. Got: %s and %s""" % (type(key[0]), type(key[1])))
			elif isinstance(key[1], str):
				return self._getitem_tuple_((key[1], key[0]))
			else:
				raise IndexError("""\
Track indexing requires a row number (int), a column label (str), or a \
combination of the two. Got: %s and %s""" % (type(key[0]), type(key[1])))
		else:
			raise IndexError("""\
Track indexing accepts 1 or 2 parameters. Got: %d""" % (len(key)))


	def _getitem_str_(self, label):
		assert isinstance(label, str), "Internal Error."
		colidx = self._indexing_handle_str_(label)
		if colidx == -2:
			return [self._t[0].weights[row] for row in range(self._t[0].n_rows)]
		else:
			return [self._t[0].predictions[row][colidx] for row in range(
				self._t[0].n_rows)]


	def _getitem_number_(self, number):
		assert isinstance(number, numbers.Number), "Internal Error."
		row = self._indexing_handle_number_(number)
		arr = [self._t[0].predictions[row][col] for col in range(
			self._t[0].n_cols)]
		result = dict(zip(self.keys(), arr))
		result["weights"] = self._t[0].weights[row]
		return result


	def _getitem_slice_(self, sl):
		assert isinstance(sl, slice), "Internal Error."
		sl = self._indexing_handle_slice_(sl)
		start = sl.start
		stop = sl.stop
		step = sl.step
		track_subset = {}
		for label in self.keys():
			track_subset[label] = self._getitem_str_(label)[start:stop:step]
		weights = [self._t[0].weights[i] for i in range(start, stop, step)]
		return track(track_subset, weights = weights,
			use_line_segment_corrections = self.use_line_segment_corrections,
			line_segment_correction_tolerance =
				self.line_segment_correction_tolerance)


	def __setitem__(self, key, value):
		if isinstance(key, str):
			self._setitem_str_(key, value)
		elif isinstance(key, numbers.Number):
			self._setitem_number_(key, value)
		elif isinstance(key, tuple):
			self._setitem_tuple_(key, value)
		else:
			raise IndexError("""\
Track item assignment requires a row number (int), a column label (str), or a \
combination of the two. Got: %s""" % (type(key)))


	def _setitem_str_(self, label, value):
		assert isinstance(label, str), "Internal Error."
		colidx = self._indexing_handle_str_(label)
		try:
			value = copy_array_like_object(value)
		except TypeError:
			raise TypeError("""\
Item assignment via column label requires an array-like object. Got: %s""" % (
				type(value)))
		if len(value) == self._t[0].n_rows:
			if all([isinstance(_, numbers.Number) for _ in value]):
				labelcopy = copy_pystring(label)
				try:
					colidx = strindex(self._t[0].labels, labelcopy,
						self._t[0].n_cols)
				finally:
					free(labelcopy)
				for i in range(self._t[0].n_rows):
					if colidx == -2:
						self._t[0].weights[i] = value[i]
					else:
						self._t[0].predictions[i][colidx] = value[i]
			else:
				raise TypeError("""\
Non-numerical value detected. Track only supports storing numerical data.""")
		else:
			raise ValueError("""\
Array-length mismath. Got: %d for track sampled at %d points.""" % (
				len(value), self._t[0].n_rows))


	def _setitem_number_(self, row, value):
		cdef char *labelcopy
		assert isinstance(row, numbers.Number), "Internal Error."
		row = self._indexing_handle_number_(row)
		if isinstance(value, dict):
			value_keys = list(value.keys())
			self_keys = list(self.keys())
			if set(value_keys) == set(self_keys):
				if all([isinstance(
					value[_], numbers.Number) for _ in value_keys]):
					for key in value_keys:
						labelcopy = copy_pystring(key)
						try:
							colidx = strindex(self._t[0].labels, labelcopy,
								self._t[0].n_cols)
						finally:
							free(labelcopy)
						if colidx >= 0:
							self._t[0].predictions[row][colidx] = value[key]
						else:
							# failsafe: should've been caught already
							assert False, "Internal Error."
				else:
					raise TypeError("""\
Non-numerical value detected. Track only supports storing numerical data.""")
			else:
				raise ValueError("""\
Column labels do not match. Track: %s. New data: %s.""" % (self_keys,
					value_keys))
		else:
			raise TypeError("""\
Track item assignment via row number requires type dict. Got: %s""" % (
				type(value)))


	def _setitem_tuple_(self, key, value):
		assert isinstance(key, tuple), "Internal Error."
		if len(key) == 2:
			if isinstance(key[0], str) and isinstance(key[1], numbers.Number):
				if isinstance(value, numbers.Number):
					colidx = self._indexing_handle_str_(key[0])
					row = self._indexing_handle_number_(key[1])
					if colidx == -2:
						self._t[0].weights[row] = value
					else:
						self._t[0].predictions[row][colidx] = value
				else:
					raise TypeError("""\
Track only supports storage of real numbers. Got: %s""" % (type(value)))
			elif isinstance(key[0], numbers.Number) and isinstance(key[1], str):
				return self._setitem_tuple_((key[1], key[0]), value)
			else:
				raise IndexError("""\
Track item assignment via row-column pair requires the column label (str) and \
the row number (int). Got: %s, %s.""" % (type(key[0]), type(key[1])))
		else:
			raise IndexError("""\
Track Item assignment via row-column pair only accepts two parameters. \
Got: %d""" % (len(key)))


	def _indexing_handle_str_(self, label):
		assert isinstance(label, str), "Internal Error."
		cdef char *labelcopy
		if label == "weights":
			return -2
		else:
			labelcopy = copy_pystring(label)
			try:
				colidx = strindex(self._t[0].labels, labelcopy,
					self._t[0].n_cols)
			finally:
				free(labelcopy)
			if colidx != -1:
				return colidx
			else:
				raise KeyError("Unrecognized quantity label: %s." % (label))


	def _indexing_handle_number_(self, number):
		assert isinstance(number, numbers.Number), "Internal Error."
		if number % 1 == 0:
			row = int(number)
			if -self._t[0].n_rows <= row < 0: row += self._t[0].n_rows
			if not 0 <= row < self._t[0].n_rows: raise IndexError("""\
Index %d out of range for track sampled at %d points.""" % (
				row, self._t[0].n_rows))
			return row
		else:
			raise IndexError("""\
Track row number must be an integer, not float.""")


	def _indexing_handle_slice_(self, sl):
		assert isinstance(sl, slice), "Internal Error."
		if sl.start is not None:
			if isinstance(sl.start, numbers.Number):
				if sl.start % 1 == 0:
					start = sl.start
				else:
					raise IndexError("""\
Track starting index must be an integer, not float.""")
			else:
				raise IndexError("""\
Track starting index must be an integer. Got: %s""" % (type(sl.start)))
		else:
			start = 0
		if sl.stop is not None:
			if isinstance(sl.stop, numbers.Number):
				if sl.stop % 1 == 0:
					stop = sl.stop
				else:
					raise IndexError("""\
Track stopping index must be an integer, not float.""")
			else:
				raise IndexError("""\
Track starting index must be an integer. Got: %s""" % (type(sl.stop)))
		else:
			stop = self._t[0].n_rows
		if sl.step is not None:
			if isinstance(sl.step, numbers.Number):
				if sl.step % 1 == 0:
					step = sl.step
				else:
					raise IndexError("""\
Track step-size must be an integer, not float.""")
			else:
				raise IndexError("""\
Track step-size must be an integer. Got: %s""" % (type(sl.step)))
		else:
			step = 1
		return slice(start, stop, step)


	@property
	def n_rows(self):
		r"""
		Type : ``int`` [positive definite]

		The number of points that the track is sampled along.
		"""
		return self._t[0].n_rows


	@property
	def n_cols(self):
		r"""
		Type : ``int`` [positive definite]

		The dimensionality of the track (i.e. the number of quantities
		predicted at each position).
		"""
		return self._t[0].n_cols


	@property
	def use_line_segment_corrections(self):
		r"""
		Type : ``bool`` [default : ``False``]

		Compute corrective factors for the length of each line segment
		connecting the points along the track. In practice, if these corrective
		factors are large, then it is a sign that the track should be more
		densely sampled than the current input.
		"""
		return bool(self._t[0].use_line_segment_corrections)


	@use_line_segment_corrections.setter
	def use_line_segment_corrections(self, value):
		if isinstance(value, bool):
			self._t[0].use_line_segment_corrections = int(value)
		else:
			raise TypeError("""\
Attribute 'use_line_segment_corrections' must be of type bool. Got: %s""" % (
				type(value)))


	@property
	def line_segment_correction_tolerance(self):
		r"""
		Type : ``float`` [default : 1.01]

		Must be larger than 1 (non-inclusive).

		A threshold above which a warning will be raised regarding the
		numerical stability of the computed likelihood. This tolerance
		quantifies how "jagged" the track is allowed to be before it begins to
		affect the inferred likelihood, introducing numerical artifacts. In
		these cases, a denser sampling of the predicted track is recommended.

		.. note::

			This parameter is only used if the attribute
			``track.use_line_segment_correction`` is ``True``. Otherwise,
			the corrections for the lengths of each line segment along the
			track (i.e. its "jaggedness") are not included.

		.. seealso:: trackstar.track.use_line_segment_correction
		"""
		return self._t[0].line_segment_correction_tolerance


	@line_segment_correction_tolerance.setter
	def line_segment_correction_tolerance(self, value):
		if isinstance(value, numbers.Number):
			if value > 1:
				self._t[0].line_segment_correction_tolerance = value
			else:
				raise ValueError("""\
Line segment correction tolerance must be larger than 1 (non-inclusive).""")
		else:
			raise TypeError("""\
Line segment correction tolerance must be a float. Got: %s""" % (type(value)))


	def keys(self):
		r"""
		Returns a list of the labels of each quantity reported at each point
		along the track (i.e. the "names" of the axes).
		"""
		_keys = []
		for i in range(self._t[0].n_cols): _keys.append(
			copy_cstring(self._t[0].labels[i]))
		return _keys
