#!/usr/bin/env python
#
# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/trackstar.git.

from trackstar import datum, covariance_matrix
from trackstar.core.datum import datum_extra
import numpy as np
import numbers
import pytest

_TEST_DATUM_ = {
	"x": np.random.random(),
	"y": np.random.random(),
	"z": np.random.random()
}

class DatumTestBase:

	r"""Base class for testing data vectors."""

	@staticmethod
	@pytest.fixture
	def case():
		return datum(_TEST_DATUM_)


class TestDatumFundamentals(DatumTestBase):

	r"""
	Tests data vector fundamental behavior: initialization, indexing, item
	assignment, keys, covariance matrix, extra information, and conversion to
	matrix type.
	"""

	@staticmethod
	def test_init(case):
		r"""tests trackstar.datum.__init__"""
		assert isinstance(case, datum)
		with pytest.raises(TypeError):
			datum(0)
			datum([0, 1, 2])
			datum({"a": 1, "b": 2, 3: 4})
			datum({"a": "1", "b": "2"})
		with pytest.raises(ValueError):
			datum({"a": 1, "err_b": 0.1})

	@staticmethod
	def test_getitem(case):
		r"""tests trackstar.datum.__getitem__"""
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
		with pytest.raises(KeyError):
			case["a"]
			case[0]

	@staticmethod
	def test_setitem(case):
		r"""tests trackstar.datum.__setitem__"""
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
		with pytest.raises(KeyError):
			case["a"] = 1
		with pytest.raises(TypeError):
			case["x"] = "foo"
			case[0] = 1

	@staticmethod
	def test_keys(case):
		r"""tests trackstar.datum.keys()"""
		assert case.keys() == list(_TEST_DATUM_.keys())

	@staticmethod
	def test_cov(case):
		r"""tests trackstar.datum.cov"""
		assert isinstance(case.cov, covariance_matrix)

	@staticmethod
	def test_extra(case):
		r"""tests trackstar.datum.extra"""
		assert isinstance(case.extra, datum_extra)
		assert isinstance(case.extra, dict)
		assert list(case.extra.keys()) == []
		try:
			case.extra["foo"] = "bar"
		except:
			assert False
		assert list(case.extra.keys()) == ["foo"]
		assert case.extra["foo"] == "bar"
		try:
			case.extra = {"bar": "baz"}
		except:
			assert False
		assert list(case.extra.keys()) == ["bar"]
		assert case.extra["bar"] == "baz"
		with pytest.raises(TypeError):
			case.extra = list(range(10))
			case.extra = 4

	@staticmethod
	def test_tomatrix(case):
		r"""tests trackstar.datum.tomatrix()"""
		try:
			copy = case.tomatrix()
		except:
			assert False
		for i in range(len(_TEST_DATUM_.keys())):
			assert copy[0, i] == _TEST_DATUM_[list(_TEST_DATUM_.keys())[i]]


class TestDatumArithmetic(DatumTestBase):

	r"""
	Tests data vector arithmetic: addition, subtraction, and multiplication.
	"""

	@staticmethod
	def test_eq(case):
		r"""tests trackstar.datum.__eq__"""
		assert case == case
		try:
			other = datum({"a": 1, "b": 2, "c": 3})
		except:
			pytest.skip("datum.__init__ failed")
		assert case != other

	@staticmethod
	def test_add(case):
		r"""tests trackstar.datum.__add__"""
		try:
			result = case + case
		except:
			assert False
		assert isinstance(result, datum)
		assert result.keys() == case.keys()
		for key in result.keys():
			assert result[key] == pytest.approx(2 * case[key])

	@staticmethod
	def test_sub(case):
		r"""tests trackstar.datum.__sub__"""
		try:
			result = case - case
		except:
			assert False
		assert isinstance(result, datum)
		assert result.keys() == case.keys()
		for key in result.keys():
			assert result[key] == pytest.approx(0)

	@staticmethod
	def test_multiply(case):
		r"""tests trackstar.datum.__mul__"""
		try:
			result = 10 * case
		except:
			assert False
		assert isinstance(result, datum)
		assert result.keys() == case.keys()
		for key in result.keys():
			assert result[key] == pytest.approx(10 * case[key])
		try:
			result = case * case
		except:
			assert False
		assert isinstance(result, numbers.Number)
		assert result > 0
		by_hand = 0
		for key in case.keys(): by_hand += case[key]**2
		assert result == pytest.approx(by_hand)
		with pytest.raises(TypeError):
			"foo" * case
			case * "foo"
			list(range(10)) * case
