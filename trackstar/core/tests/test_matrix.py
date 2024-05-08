# cython: language_level = 3, boundscheck = False
#
# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/trackstar.git.

from trackstar import matrix
import numpy as np
import pytest

sizes = []
for i in range(2, 11):
	for j in range(2, 11):
		sizes.append((i, j))

@pytest.mark.parametrize(("n_rows", "n_cols"), sizes)
def test_zeros(n_rows, n_cols):
	r"""tests trackstar.matrix.zeros"""
	try:
		result = matrix.zeros(n_rows, n_cols)
	except:
		assert False
	assert isinstance(result, matrix)
	assert result.n_rows == n_rows
	assert result.n_cols == n_cols
	for i in range(n_rows):
		for j in range(n_cols):
			assert result[i, j] == 0


@pytest.mark.parametrize(("n_rows", "n_cols"), sizes)
class MatrixBaseFixture:

	r"""
	Base class for testing matrices of some size, from 2x2 up to 10x10, and
	all combinations in between, both square and non-square.
	"""

	@staticmethod
	@pytest.fixture
	def case(n_rows, n_cols):
		return matrix(np.random.random(size = [n_rows, n_cols]))


@pytest.mark.parametrize(("size"), list(range(2, 11)))
class SquareMatrixBaseFixture:

	r"""
	Base class for testing features specific to square matrices, from 2x2 up to
	10x10.
	"""

	@staticmethod
	@pytest.fixture
	def case(size):
		return matrix(np.random.random(size = [size, size]))


class TestMatrixFundamentals(MatrixBaseFixture):

	r"""
	Tests matrix fundamental behavior: initialization, size, indexing, and
	item assignment.
	"""

	@staticmethod
	def test_init(n_rows, n_cols, case):
		r"""tests trackstar.matrix.__init__"""
		assert isinstance(case, matrix)

	@staticmethod
	def test_size(n_rows, n_cols, case):
		r"""tests trackstar.matrix.n_rows and matrix.n_cols"""
		assert case.n_rows == n_rows and case.n_cols == n_cols


	@staticmethod
	def test_indexing(n_rows, n_cols, case):
		r"""tests trackstar.matrix.__getitem__"""
		for i in range(n_rows):
			for j in range(n_cols):
				assert isinstance(case[i, j], float)
				assert 0 <= case[i, j] < 1 # RNG range


	@staticmethod
	def test_item_assignment(n_rows, n_cols, case):
		r"""tests trackstar.matrix.__setitem__"""
		for i in range(n_rows):
			for j in range(n_cols):
				try:
					case[i, j] = 0
				except:
					assert False
				assert case[i, j] == 0
				try:
					case[i, j] = 0.2
				except:
					assert False
				assert case[i, j] == 0.2


class TestMatrixArithmetic(MatrixBaseFixture):

	r"""
	Tests matrix arithmetic routines: addition, subtraction, multiplication,
	equivalence comparison, and transposition.
	"""

	@staticmethod
	def test_add(n_rows, n_cols):
		r"""tests trackstar.matrix.__add__"""
		try:
			case1 = matrix(np.random.random(size = [n_rows, n_cols]))
			case2 = matrix(np.random.random(size = [n_rows, n_cols]))
		except:
			pytest.skip("Matrix initialization failed.")
		try:
			result = case1 + case2
		except:
			assert False
		assert isinstance(result, matrix)
		assert result.n_rows == n_rows
		assert result.n_cols == n_cols
		for i in range(n_rows):
			for j in range(n_cols):
				assert result[i, j] == pytest.approx(case1[i, j] + case2[i, j])


	@staticmethod
	def test_subtract(n_rows, n_cols):
		r"""tests trackstar.matrix.__sub__"""
		try:
			case1 = matrix(np.random.random(size = [n_rows, n_cols]))
			case2 = matrix(np.random.random(size = [n_rows, n_cols]))
		except:
			pytest.skip("Matrix initialization failed.")
		try:
			result = case1 - case2
		except:
			assert False
		assert isinstance(result, matrix)
		assert result.n_rows == n_rows
		assert result.n_cols == n_cols
		for i in range(n_rows):
			for j in range(n_cols):
				assert result[i, j] == pytest.approx(case1[i, j] - case2[i, j])


	@staticmethod
	def test_multiply(n_rows, n_cols, case):
		r"""tests trackstar.matrix.__mul__"""
		for other_dimension in range(2, 11):
			try:
				left = matrix(np.random.random(size = [other_dimension, n_rows]))
				right = matrix(np.random.random(size = [n_cols, other_dimension]))
			except:
				pytest.skip("Matrix initialization failed.")
			try:
				result = left * case
			except:
				assert False
			assert isinstance(result, matrix)
			assert result.n_rows == left.n_rows
			assert result.n_cols == case.n_cols
			for i in range(result.n_rows):
				for j in range(result.n_cols):
					s = 0
					for k in range(left.n_cols):
						s += left[i, k] * case[k, j]
					assert result[i, j] == pytest.approx(s)
			try:
				result = case * right
			except:
				assert False
			assert isinstance(result, matrix)
			assert result.n_rows == case.n_rows
			assert result.n_cols == right.n_cols
			for i in range(result.n_rows):
				for j in range(result.n_cols):
					s = 0
					for k in range(case.n_cols):
						s += case[i, k] * right[k, j]
					assert result[i, j] == pytest.approx(s)


	@staticmethod
	def test_prefactor(n_rows, n_cols, case):
		r"""tests trackstar.matrix.prefactor"""
		factor = np.random.random()
		try:
			result = case.prefactor(factor)
		except:
			assert False
		for i in range(n_rows):
			for j in range(n_cols):
				assert result[i, j] == pytest.approx(factor * case[i, j])


	@staticmethod
	def test_equivalence(n_rows, n_cols, case):
		r"""tests trackstar.matrix.__eq__"""
		assert case == case
		try:
			other = matrix(np.random.random(size = (n_rows, n_cols)))
		except:
			pytest.skip("Matrix initialization failed.")
		assert case != other


	@staticmethod
	def test_transpose(n_rows, n_cols, case):
		r"""tests trackstar.matrix.transpose"""
		try:
			trans = case.transpose()
		except:
			assert False
		assert trans.n_rows == n_cols
		assert trans.n_cols == n_rows
		for i in range(n_rows):
			for j in range(n_cols):
				assert case[i, j] == trans[j, i]


class TestSquareMatrix(SquareMatrixBaseFixture):

	r"""
	Tests routines that are specific to square matrices: identity matrix
	construction, determinants, and inversion.
	"""

	@staticmethod
	def test_identity(size):
		r"""tests trackstar.matrix.identity"""
		try:
			result = matrix.identity(size)
		except:
			assert False
		assert isinstance(result, matrix)
		assert result.n_rows == size
		assert result.n_cols == size
		for i in range(size):
			for j in range(size):
				assert result[i, j] == int(i == j)


	@staticmethod
	def test_determinant(size, case):
		r"""tests trackstar.matrix.determinant"""
		try:
			det = case.determinant()
		except:
			assert False
		assert isinstance(det, float)
		for i in range(size):
			for j in range(size):
				case[i, j] = int(i == j)
		assert case.determinant() == 1
		value = np.random.random()
		for i in range(size): case[i, i] = value
		assert case.determinant() == pytest.approx(value**size)
		for i in range(size):
			for j in range(size):
				case[i, j] = np.random.random()
		assert case.determinant() != pytest.approx(value**size)


	@staticmethod
	def test_invert(size, case):
		r"""tests trackstar.matrix.invert"""
		for i in range(1, size):
			for j in range(i):
				try:
					# make it symmetric so that it's definitely invertible
					case[i, j] = case[j, i]
				except:
					pytest.skip("Matrix item assignment failed.")
		try:
			inv = case.invert()
		except:
			assert False
		assert isinstance(inv, matrix)
		assert inv.n_rows == size
		assert inv.n_cols == size
		try:
			left = inv * case
			right = case * inv
		except:
			pytest.skip("Matrix multiplication failed.")
		for i in range(size):
			for j in range(size):
				assert left[i, j] == pytest.approx(int(i == j))
				assert right[i, j] == pytest.approx(int(i == j))

