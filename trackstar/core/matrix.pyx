# cython: language_level = 3, boundscheck = False
#
# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/trackstar.git.

__all__ = ["matrix"]
import numbers
from .utils import copy_array_like_object
from . cimport matrix

cdef class matrix:

	r"""
	.. class:: trackstar.matrix(arr)

	A generic matrix object for linear algebra operations.

	.. seealso:: ``trackstar.datum``

	.. todo::

		Replace example code outputs with true uses of this class. Include
		note and example about scalar multiplication with ``prefactor``
		instancemethod.

	Parameters
	----------
	arr : ``array-like``
		The matrix itself, type-casting a 2-dimensional array-like object into
		a matrix. Must be rectangular (i.e., each "row," or element on the
		first axis, must be the same length as all of the others). Each element
		must be a numerical value. If the matrix is to have only one row, a
		1-dimensional array can also be passed for this parameter.

	.. note::

		Once a ``matrix`` has been created, its size cannot be changed.
		If a change in size is necessary, please create a new ``matrix``.

	Attributes
	----------
	n_rows : ``int``
		The number of rows in the matrix (i.e., the number of elements along
		the first axis).
	n_cols : ``int``
		The number of columns in the matrix (i.e., the number of elements
		along the second axis).

	Indexing and Item Assignment
	----------------------------
	Both proceed with two integers separated by a comma (e.g.,
	``mat[0, 0]``, ``mat[1, 0] = 0.8``), and the assigned value must be a
	real number. Negative indices are not supported.

	Functions
	---------
	zeros : ``classmethod``
		Returns a matrix of a given size where every entry is set to zero.
	identity : ``classmethod``
		Returns the identity matrix for a given size.
	prefactor : ``instancemethod``
		Multiply each matrix element by some multiplicative factor.
	determinant : ``instancemethod``
		Compute the determinant of the matrix.
	inverse : ``instancemethod``
		Compute the inverse of the matrix.
	transpose : ``instancemethod``
		Compute the transpose of the matrix.

	Example Code
	------------
	>>> from trackstar import matrix
	>>> import numpy as np

	Simple matrices, such as those full of zeros or the square identity
	matrix which can act as starting points in constructing more complex
	matrices, can be obtained using the ``matrix.zeros`` and
	``matrix.identity`` ``classmethods``:

	>>> example = matrix.zeros(2, 3) # 2 rows by 3 columns
	>>> example
	matrix([0.00e+00    0.00e+00    0.00e+00]
	       [0.00e+00    0.00e+00    0.00e+00])
	>>> example = matrix.identity(5) # 5x5 square identity matrix
	>>> example
	matrix([1.00e+00    0.00e+00    0.00e+00    0.00e+00    0.00e+00]
	       [0.00e+00    1.00e+00    0.00e+00    0.00e+00    0.00e+00]
	       [0.00e+00    0.00e+00    1.00e+00    0.00e+00    0.00e+00]
	       [0.00e+00    0.00e+00    0.00e+00    1.00e+00    0.00e+00]
	       [0.00e+00    0.00e+00    0.00e+00    0.00e+00    1.00e+00])

	Indexing and item assignment proceed with two integers ``i`` and ``j``,
	separated by a comma, to obtain or modify the matrix element :math:`x_{ij}`:

	>>> example = matrix.identity(5)
	>>> example[0, 0]
	1.0
	>>> example[0, 1]
	0.0
	>>> example[2, 0] = np.pi
	>>> example[2, 0]
	3.141592653589793
	>>> example
	matrix([1.00e+00    0.00e+00    0.00e+00    0.00e+00    0.00e+00]
	       [0.00e+00    1.00e+00    0.00e+00    0.00e+00    0.00e+00]
	       [3.14e+00    0.00e+00    1.00e+00    0.00e+00    0.00e+00]
	       [0.00e+00    0.00e+00    0.00e+00    1.00e+00    0.00e+00]
	       [0.00e+00    0.00e+00    0.00e+00    0.00e+00    1.00e+00])

	Equivalence comparison returns ``True`` if and only if the matrices have
	the same dimensions and each component passes an equivalence comparison
	(i.e., :math:`A_{ij} = B_{ij}` for all :math:`i` and :math:`j`):

	>>> example = matrix.identity(5)
	>>> for i in range(5):
	>>>     for j in range(5):
	>>>         example[i, j] = 10 * np.random.rand()
	>>> example
	matrix([1.12e+00    8.59e-01    8.95e+00    5.00e+00    6.12e+00]
	       [5.17e+00    1.78e+00    1.38e+00    3.82e+00    1.19e+00]
	       [2.39e+00    6.94e+00    3.38e+00    8.58e-01    5.62e+00]
	       [5.14e+00    1.41e+00    1.52e+00    9.56e+00    1.53e+00]
	       [8.69e+00    1.22e+00    8.29e+00    7.94e+00    8.56e+00])
	>>> copy = matrix.identity(5)
	>>> for i in range(5):
	>>>     for j in range(5):
	>>>         copy[i, j] = example[i, j]
	>>> copy == example
	True
	>>> example == matrix.identity(5)
	False
	>>> example = matrix.zeros(2, 4)
	False
	>>> example = matrix.identity(4)
	>>> example == example.inverse()
	True

	For matrices with the same dimensions, addition and subtraction proceed as
	usual with the ``+`` and ``-`` operators:

	>>> example1 = matrix.zeros(3, 4)
	>>> example2 = matrix.zeros(3, 4)
	>>> for i in range(example1.n_rows):
	>>>     for j in range(example1.n_cols):
	>>>         example1[i, j] = 10 * np.random.rand()
	>>>         example2[i, j] = 10 * np.random.rand()
	>>> example1
	matrix([6.40e+00    6.39e+00    2.73e+00    6.39e+00]
	       [6.87e+00    1.98e-01    4.55e+00    1.85e+00]
	       [7.98e+00    4.17e+00    5.72e+00    6.98e+00])
	>>> example2
	matrix([5.46e+00    6.09e+00    6.40e+00    8.44e+00]
	       [9.25e+00    3.63e+00    9.81e-01    6.08e+00]
	       [9.18e+00    7.54e+00    9.35e+00    7.81e+00])
	>>> result = example1 + example2
	>>> result
	matrix([1.19e+01    1.25e+01    9.13e+00    1.48e+01]
	       [1.61e+01    3.83e+00    5.54e+00    7.93e+00]
	       [1.72e+01    1.17e+01    1.51e+01    1.48e+01])
	>>> for i in range(result.n_rows):
	>>>     for j in range(result.n_cols):
	>>>         assert result[i, j] == example1[i, j] + example2[i, j]
	>>> result = example1 - example2
	>>> result
	matrix([9.39e-01    3.07e-01    -3.66e+00    -2.05e+00]
	       [-2.38e+00   -3.43e+00   3.57e+00     -4.23e+00]
	       [-1.20e+00   -3.37e+00   -3.63e+00    -8.25e-01])
	>>> for i in range(result.n_rows):
	>>>     for j in range(result.n_cols):
	>>>         assert result[i, j] == example1[i, j] - example2[i, j]

	Multiplication proceeds as usual with the ``*`` operator for multiplication
	by both scalars and other matrices. For matrix multiplication, the number
	of rows in the second operand must equal the number of columns in the
	first operand for the operation to be possible.
	
	>>> 3 * matrix.identity(5)
	matrix([3.00e+00    0.00e+00    0.00e+00    0.00e+00    0.00e+00]
	       [0.00e+00    3.00e+00    0.00e+00    0.00e+00    0.00e+00]
	       [0.00e+00    0.00e+00    3.00e+00    0.00e+00    0.00e+00]
	       [0.00e+00    0.00e+00    0.00e+00    3.00e+00    0.00e+00]
	       [0.00e+00    0.00e+00    0.00e+00    0.00e+00    3.00e+00])
	>>> 2 * matrix.identity(5) == matrix.identity(5) + matrix.identity(5)
	True
	>>> example1 = matrix.zeros(3, 4) # 3 rows by 4 columns
	>>> example2 = matrix.zeros(4, 2) # 4 rows by 2 columns
	>>> for i in range(example1.n_rows):
	>>>     for j in range(example1.n_cols):
	>>>         example1[i, j] = 10 * np.random.rand()
	>>> for i in range(example2.n_rows):
	>>>     for j in range(example2.n_cols):
	>>>         example2[i, j] = 10 * np.random.rand()
	>>> example1
	matrix([3.19e-01    7.91e+00    8.74e+00    8.04e-02]
	       [9.41e-01    1.86e+00    4.02e+00    9.54e+00]
	       [4.28e+00    3.19e+00    3.70e+00    9.03e+00])
	>>> example1 * matrix.identity(4)
	matrix([3.19e-01    7.91e+00    8.74e+00    8.04e-02]
	       [9.41e-01    1.86e+00    4.02e+00    9.54e+00]
	       [4.28e+00    3.19e+00    3.70e+00    9.03e+00])
	>>> example2
	matrix([3.47e+00    5.48e-01]
	       [6.88e+00    8.34e-02]
	       [1.01e+00    8.74e+00]
	       [2.06e+00    8.84e+00])
	>>> np.pi * example2
	matrix([1.09e+01    1.72e+00]
	       [2.16e+01    2.62e-01]
	       [3.18e+00    2.75e+01]
	       [6.48e+00    2.78e+01])
	>>> result = example1 * example2 # result will be 3 rows by 2 columns
	>>> result
	matrix([6.45e+01    7.80e+01]
	       [3.98e+01    1.20e+02]
	       [5.92e+01    1.15e+02])

	Determinants of square matrices can be computed with ``x.determinant`` for
	a matrix ``x``:

	>>> example = matrix.zeros(4, 4)
	>>> for i in range(example.n_rows):
	>>>     for j in range(example.n_cols):
	>>>         example[i, j] = 10 * np.random.rand()
	>>> example
	matrix([3.25e-01    8.87e+00    4.08e+00    5.92e+00]
	       [7.68e+00    8.14e+00    2.28e+00    9.67e+00]
	       [9.87e+00    4.52e+00    2.77e+00    4.38e+00]
	       [3.79e+00    9.90e+00    6.06e+00    6.33e+00])
	>>> example.determinant()
	-215.20245282995597
	>>> matrix.identity(5).determinant()
	1.0

	Square matrices can be inverted with ``x.inverse`` for a matrix ``x``:

	>>> example = matrix.zeros(4, 4)
	>>> for i in range(4):
	>>>     for j in range(4):
	>>>         example[i, j] = 10 * np.random.rand()
	>>> example
	matrix([4.83e+00    4.26e+00    1.04e+00    9.79e+00]
	       [9.22e+00    5.79e+00    1.95e+00    5.59e+00]
	       [6.99e+00    7.55e-02    9.25e+00    3.35e+00]
	       [3.16e+00    9.48e+00    8.57e+00    8.32e+00])
	>>> example.inverse()
	matrix([-2.81e-02   1.18e-01    3.36e-02    -5.98e-02]
	       [-1.01e-01   9.32e-02    -9.60e-02   9.46e-02]
	       [-3.72e-02   -5.66e-02   7.74e-02    5.06e-02]
	       [1.64e-01    -9.28e-02   1.70e-02    -1.71e-02])
	>>> example * example.inverse()
	matrix([1.00e+00    0.00e+00    -8.33e-17   5.55e-17]
	       [1.11e-16    1.00e+00    -4.16e-17   1.11e-16]
	       [0.00e+00    -1.67e-16   1.00e+00    6.25e-17]
	       [2.22e-16    -1.11e-16   1.67e-16    1.00e+00])

	Note in the example above that the product of the 4x4 matrix and its
	inverse is not *exactly* the identity matrix, but the off-diagonal
	components are within floating-point precision of zero anyway.

	Matrices of any size can be transposed with ``x.transpose`` for a matrix
	``x``:

	>>> example = matrix.zeros(3, 4)
	>>> for i in range(3):
	>>>     for j in range(4):
	>>>         example[i, j] = 10 * np.random.rand()
	>>> example
	matrix([9.44e-01    8.25e+00    9.92e+00    2.30e+00]
	       [3.69e+00    3.18e+00    9.04e+00    7.58e+00]
	       [5.44e+00    5.94e+00    7.98e-01    1.36e+00])
	>>> example.transpose()
	matrix([9.44e-01    3.69e+00    5.44e+00]
	       [8.25e+00    3.18e+00    5.94e+00]
	       [9.92e+00    9.04e+00    7.98e-01]
	       [2.30e+00    7.58e+00    1.36e+00])
	>>> matrix.identity(5) == matrix.identity(5).transpose()
	True
	"""

	def __cinit__(self, arr):
		r"""
		Allocate memory for a matrix. User access of this function strongly
		discouraged.
		"""
		msg = """\
Matrix or vector must be a 1-dimensional or rectangular 2-dimensional \
array-like object containing only numerical values."""
		try:
			arr = copy_array_like_object(arr)
		except TypeError:
			raise TypeError(msg)

		# determine if this is a 1-d or 2-d object. If it's 1-d, make it 2-d
		# by simply enclosing it in a list (``[arr]``) and use the same
		# functionality.
		is2d = True
		for i in range(len(arr)): is2d &= hasattr(arr[i], "__getitem__")
		if not is2d:
			if all([isinstance(_, numbers.Number) for _ in arr]):
				arr = [arr]
			else:
				raise TypeError("""\
Matrix or vector must contain only numerical values.""")
		else: pass

		for i in range(len(arr)):
			try:
				arr[i] = copy_array_like_object(arr[i])
			except TypeError:
				raise TypeError(msg)
			if len(arr[i]) == len(arr[0]):
				for j in range(len(arr[i])):
					if not isinstance(arr[i][j], numbers.Number):
						raise TypeError(msg)
					else: pass
			else:
				raise TypeError(msg)

		self._m = matrix_initialize(<unsigned short> len(arr),
			<unsigned short> len(arr[0]))


	def __init__(self, arr):
		r"""
		Initialize a ``matrix`` by type-casting a 2-dimensional array-like
		object. See ``help(trackstar.matrix)`` for more information.
		"""
		# simply make 1-d arrays 2-d
		if all([isinstance(_, numbers.Number) for _ in arr]): arr = [arr]
		for i in range(self.n_rows):
			for j in range(self.n_cols):
				self._m[0].matrix[i][j] = <double> arr[i][j]


	def __dealloc__(self):
		r"""
		Free up the memory stored by a ``matrix`` object. User access strongly
		discouraged.
		"""
		matrix_free(self._m)


	def __enter__(self):
		r"""Opens a with statement."""
		return self


	def __exit__(self, exc_type, exc_value, exc_tb):
		r"""Raises all exceptions inside with statements."""
		return exc_value is None


	def __repr__(self):
		r"""Returns a string representation of the matrix."""
		rep = "matrix("
		for i in range(self.n_rows):
			if i:
				rep += "       ["
			else:
				rep += "["
			for j in range(self.n_cols):
				# Look for '-' char as opposed to < 0 b/c -0 fails that test
				if str(self._m[0].matrix[i][j]).startswith("-"):
					rep += "%.4e" % (self._m[0].matrix[i][j])
				else:
					rep += " %.4e" % (self._m[0].matrix[i][j])
				if j != self.n_cols - 1: rep += "    "
			rep += " ]"
			if i != self.n_rows - 1: rep += "\n"
		rep += ")"
		return rep


	def __getitem__(self, indices):
		r"""Index the matrix according to the rule ``matrix[i, j]``"""
		self._check_indices(indices)
		return self._m[0].matrix[indices[0]][indices[1]]


	def __setitem__(self, indices, value):
		r"""Item assignment according to the rule ``matrix[i, j] = x``"""
		self._check_indices(indices)
		if isinstance(value, numbers.Number):
			self._m[0].matrix[indices[0]][indices[1]] = <double> value
		else:
			raise TypeError("""\
Item assignment requires a real number. Got: %s""" % (type(value)))


	def _check_indices(self, indices):
		r"""Conducts all error-handling on matrix indexing."""
		if isinstance(indices, tuple):
			if len(indices) == 2:
				self._check_index(indices[0], self.n_rows, "row")
				self._check_index(indices[1], self.n_cols, "column")
				return
			elif len(indices) < 2:
				raise IndexError("""\
Too few indices for matrix: accepts 2, but %d were given.""" % (len(indices)))
			elif len(indices) > 2:
				raise IndexError("""\
Too many indices for matrix: accepts 2, but %d were given.""" % (len(indices)))
		elif isinstance(indices, numbers.Number) and indices % 1 == 0:
			raise IndexError("""\
Too few indices for matrix: accepts 2, but 1 was given. If you have indexed \
according to the rule `matrix[row][column]`, note that indexing instead \
proceeds according to `matrix[row, column]`.""")
		else:
			raise IndexError("""\
Matrix must be indexed with two integers. Got: %s""" % (type(indices)))


	@staticmethod
	def _check_index(value, maximum, row_or_column):
		r"""
		Determines if an array index is an integer and within the allowed
		range dictated by the size of the matrix.
		"""
		if not isinstance(value, numbers.Number):
			raise IndexError("Index must be an integer. Got: %s" % (
				type(value)))
		elif value % 1:
			raise IndexError("Index must be an integer, not float.")
		elif not 0 <= value < maximum:
			raise IndexError("""\
Index %d is out of range for matrix with %d %ss""" % (
				value, maximum, row_or_column))
		else: pass


	def __eq__(self, matrix other):
		if self.n_rows == other.n_rows and self.n_cols == other.n_cols:
			eq = True
			for i in range(self.n_rows):
				for j in range(self.n_cols):
					eq &= self[i, j] == other[i, j]
					if not eq: break
				if not eq: break
			return eq
		else:
			return False


	def __add__(self, matrix other):
		cdef matrix result
		if self.n_rows == other.n_rows and self.n_cols == other.n_cols:
			result = matrix.zeros(self.n_rows, self.n_cols)
			matrix_add(self._m[0], other._m[0], result._m)
			return result
		else:
			raise ValueError("""\
Matrix dimensions must be equal for addition. Got: %dx%d and %dx%d.""" % (
				self.n_rows, self.n_cols, other.n_rows, other.n_cols))


	def __sub__(self, matrix other):
		cdef matrix result
		if self.n_rows == other.n_rows and self.n_cols == other.n_cols:
			result = matrix.zeros(self.n_rows, self.n_cols)
			matrix_subtract(self._m[0], other._m[0], result._m)
			return result
		else:
			raise ValueError("""\
Matrix dimensions must be equal for subtraction. Got: %dx%d and %dx%d.""" % (
				self.n_rows, self.n_cols, other.n_rows, other.n_cols))


	def __mul__(self, other):
		if isinstance(other, numbers.Number):
			return self._mul_prefactor_(other)
		elif isinstance(other, matrix):
			return self._mul_matrix_(other)
		else:
			raise TypeError("""\
Matrix multiplication requires either a real number or another matrix of \
compatible size. Got: %s""" % (type(other)))


	def __rmul__(self, other):
		if isinstance(other, numbers.Number):
			return self.__mul__(other)
		else:
			raise TypeError("""\
Matrix multiplication requires either a real number or another matrix of \
compatible size. Got: %s""" % (type(other)))


	def _mul_prefactor_(self, other):
		assert isinstance(other, numbers.Number), "Internal Error."
		cdef matrix result = matrix.zeros(self.n_rows, self.n_cols)
		for i in range(self.n_rows):
			for j in range(self.n_cols):
				result[i, j] = other * self[i, j]
		return result


	def _mul_matrix_(self, matrix other):
		cdef matrix result
		if self.n_cols == other.n_rows:
			result = matrix.zeros(self.n_rows, other.n_cols)
			matrix_multiply(self._m[0], <MATRIX> other._m[0], result._m)
			return result
		else:
			raise ValueError("""\
Matrix dimensions incompatible for multiplication: %dx%d and %dx%d.""" % (
				self.n_rows, self.n_cols, other.n_rows, other.n_cols))


	@property
	def n_rows(self):
		r"""
		Type : ``int`` [positive definite]

		The number of rows in the matrix (i.e., the number of elements along
		the first axis of indexing or item assignment).

		.. note::

			Once a matrix object has been created, its size cannot be changed.
			If a change in size is necessary, please create a new matrix
			object.

		Example Code
		------------
		>>> from vice.modeling import matrix
		>>> example = matrix.zeros(2, 3) # 2 rows by 3 columns
		>>> example.n_rows
		2
		>>> example = matrix.identity(5)
		>>> example.n_rows
		5
		>>> example = matrix.zeros(4, 7) # 4 rows by 7 columns
		>>> example.n_rows 4
		"""
		return self._m[0].n_rows


	@property
	def n_cols(self):
		r"""
		Type : ``int`` [positive definite]

		The number of columns in the matrix (i.e., the number of elements along
		the second axis of indexing or item assignment).

		.. note::

			Once a matrix object has been created, its size cannot be changed.
			If a change in size is necessary, please create a new matrix
			object.

		Example Code
		------------
		>>> from vice.modeling import matrix
		>>> example = matrix.zeros(2, 3) # 2 rows by 3 columns
		>>> example.n_cols
		3
		>>> example = matrix.identity(5)
		>>> example.n_cols
		5
		>>> example = matrix.zeros(4, 7) # 4 rows by 7 columns
		>>> example.n_cols
		7
		"""
		return self._m[0].n_cols


	@classmethod
	def zeros(cls, n_rows, n_cols):
		r"""
		.. classmethod:: trackstar.matrix.zeros(n_rows, n_cols)

		Obtain an ``n_rows x n_cols`` matrix in which each element is set to
		zero.
		"""
		return cls(cls._zeros_from_size(n_rows, n_cols))


	@classmethod
	def identity(cls, size):
		r"""
		.. classmethod:: trackstar.matrix.identity(size)

		Obtain and NxN identity matrix, defined as having 1's along the
		diagonal and 0's off of it (:math:`I_{ij} = 1` for ``i == j`` and
		:math:`I_{ij} = 0` for ``i != j``).
		"""
		arr = cls._zeros_from_size(size, size)
		for i in range(size): arr[i][i] = 1
		return cls(arr)


	@staticmethod
	def _zeros_from_size(n_rows, n_cols):
		def _check_value(value, name):
			r"""
			Determines if n_rows or n_cols is an integer, and raises the
			appropriate error if not.
			"""
			if not isinstance(value, numbers.Number):
				raise TypeError("%s must be an integer. Got: %s" % (name,
					type(value)))
			elif value % 1:
				raise TypeError("%s must be an integer, not float." % (name))
			else: pass
		_check_value(n_rows, "n_rows")
		_check_value(n_cols, "n_cols")
		n_rows = int(n_rows)
		n_cols = int(n_cols)
		arr = n_rows * [None]
		for i in range(n_rows): arr[i] = n_cols * [0]
		return arr


	def invert(self):
		r"""Compute the inverse if the matrix is square."""
		cdef matrix result
		if self.n_rows == self.n_cols:
			result = matrix.zeros(self.n_rows, self.n_cols)
			matrix_invert(self._m[0], result._m)
			return result
		else:
			raise ValueError("""\
While some non-square matrices have left- and right-inverses, TrackStar only \
supports inversion of square matrices. Dimensions: %dx%d""" % (
				self.n_rows, self.n_cols))


	def determinant(self):
		r"""Compute the determinant if the matrix is square."""
		if self.n_rows == self.n_cols:
			return matrix_determinant(self._m[0])
		else:
			raise ValueError("""\
Cannot compute the determinant of a non-square matrix. Dimensions: %dx%d""" % (
				self.n_rows, self.n_cols))


	def transpose(self):
		r"""Compute the transpose of the matrix."""
		cdef matrix result
		result = matrix.zeros(self.n_cols, self.n_rows)
		matrix_transpose(self._m[0], result._m)
		return result

