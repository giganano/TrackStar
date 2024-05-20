# cython: language_level = 3, boundscheck = False
#
# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/trackstar.git.

from trackstar import datum
import numpy as np
import pytest

_TEST_DATUM_ = {
	"x": np.random.random(),
	"y": np.random.random(),
	"z": np.random.random()
}

class DatumTestBase:

	@staticmethod
	@pytest.fixture
	def case():
		return datum(_TEST_DATUM_)


class TestDatumFundamentals(DatumTestBase):

	@staticmethod
	def test_init(case):
		assert isinstance(case, datum)

	@staticmethod
	def test_keys(case):
		assert case.keys() == list(_TEST_DATUM_.keys())

	@staticmethod
	def test_getitem(case):
		try:
			keys = case.keys()
		except:
			pytest.skip("datum.keys() failed.")
		for key in keys:
			try:
				value = case[key]
			except:
				assert False
			assert isinstance(value, float)
			assert value == _TEST_DATUM_[key]

	@staticmethod
	def test_setitem(case):
		try:
			keys = case.keys()
		except:
			pytest.skip("datum.keys() failed.")
		for key in keys:
			try:
				case[key] = 10
			except:
				assert False
			try:
				stored_value = case[key]
			except:
				pytest.skip("datum.__getitem__() failed.")
			assert stored_value == 10



