#!/usr/bin/env python
#
# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/TrackStar.git.

from trackstar import datum, covariance_matrix
from trackstar.core.datum import datum_extra
from trackstar.core.tests.test_datum import _TEST_DATUM_
import numpy as np
import numbers
import pytest

class TestCovarianceMatrix:

	r"""
	Tests covariances matrices, both those associated with data vectors and
	those created as standalone objects.
	"""

	@staticmethod
	@pytest.fixture
	def datum():
		# For some seemingly unknown reason, these unit tests throw a
		# segmentation fault when the covariance_matrix attribute of the datum
		# is returned. It's possible pytest is taking copies of objects,
		# resulting in modified pointer addresses. TrackStar manages this
		# relatively closely, so if that suspicion is true, then we would
		# indeed expect memory errors. We are unable to reproduce a
		# segmentation fault when saving the covariance matrix attribute as
		# a variable and running the same function calls separate from pytest.
		return datum(_TEST_DATUM_)


	@staticmethod
	@pytest.fixture
	def standalone():
		arr = np.zeros((5, 5))
		for i in range(len(arr)): arr[i][i] = 1
		return covariance_matrix(arr)


	@staticmethod
	def test_init_from_datum(datum):
		r"""tests trackstar.covariance_matrix.__init__ with a datum."""
		assert isinstance(datum.cov, covariance_matrix)


	@staticmethod
	def test_init_standalone(standalone):
		r"""tests trackstar.covariance_matrix.__init__ as a standalone."""
		assert isinstance(standalone, covariance_matrix)


	@staticmethod
	def test_keys_from_datum(datum):
		r"""tests trackstar.covariance_matrix.keys with a datum."""
		assert datum.cov.keys() == list(_TEST_DATUM_.keys())


	@staticmethod
	def test_keys_standalone(standalone):
		r"""tests trackstar.covariance_matrix.keys as a standalone."""
		assert standalone.keys() is None


	@staticmethod
	def test_getitem_from_datum(datum):
		r"""tests trackstar.covariance_matrix.__getitem__ with a datum."""
		for key1 in datum.cov.keys():
			for key2 in datum.cov.keys():
				try:
					value = datum.cov[key1, key2]
				except:
					assert False
				assert isinstance(value, numbers.Number)
				assert value == int(key1 == key2)


	@staticmethod
	def test_getitem_standalone(standalone):
		r"""tests trackstar.covariance_matrix.__getitem__ as a standalone."""
		for i in range(standalone.n_rows):
			for j in range(standalone.n_rows):
				try:
					value = standalone[i, j]
				except:
					assert False
				assert isinstance(value, numbers.Number)
				assert value == int(i == j)


	@staticmethod
	def test_setitem_from_datum(datum):
		r"""tests trackstar.covariance_matrix.__setitem__ with a datum."""
		# modify each value with an RNG, and then assert diagonality after
		for key1 in datum.cov.keys():
			for key2 in datum.cov.keys():
				try:
					datum.cov[key1, key2] = np.random.random()
				except:
					assert False
		for key1 in datum.cov.keys():
			for key2 in datum.cov.keys():
				assert datum.cov[key1, key2] == datum.cov[key2, key1]


	@staticmethod
	def test_setitem_standalone(standalone):
		r"""tests trackstar.covariance_matrix.__setitem__ as a standalone."""
		# modify each value with an RNG, and then assert diagonality after
		for i in range(standalone.n_rows):
			for j in range(standalone.n_rows):
				try:
					standalone[i, j] = np.random.random()
				except:
					assert False
		for i in range(standalone.n_rows):
			for j in range(standalone.n_rows):
				assert standalone[i, j] == standalone[j, i]


	@staticmethod
	def test_inv_from_datum(datum):
		r"""tests trackstar.covariance_matrix.inv with a datum."""
		for key1 in datum.cov.keys():
			for key2 in datum.cov.keys():
				try:
					datum.cov[key1, key2] = np.random.random()
				except:
					pytest.skip("covariance_matrix.__setitem__ failed.")
		try:
			inv = datum.cov.inv
		except:
			assert False
		try:
			prod = datum.cov * datum.cov.inv
		except:
			pytest.skip("matrix.__mul__ failed.")
		for i in range(len(datum.cov.keys())):
			for j in range(len(datum.cov.keys())):
				assert prod[i, j] == pytest.approx(int(i == j), rel = 1e-15)


	@staticmethod
	def test_inv_standalone(standalone):
		r"""tests trackstar.covariance_matrix.inv as a standalone."""
		for i in range(standalone.n_rows):
			for j in range(standalone.n_rows):
				try:
					standalone[i, j] = np.random.random()
				except:
					pytest.skip("covariance_matrix.__setitem__ failed.")
		try:
			inv = standalone.inv
		except:
			assert False
		try:
			prod = standalone * inv
		except:
			pytest.skip("matrix.__mul__ failed.")
		for i in range(standalone.n_rows):
			for j in range(standalone.n_rows):
				assert prod[i, j] == pytest.approx(int(i == j), rel = 1e-15)

