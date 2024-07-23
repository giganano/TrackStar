#!/usr/bin/env python
#
# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/TrackStar.git.

from trackstar.benchmarks import benchmark
from trackstar.core.tests.test_datum import _TEST_DATUM_
from trackstar import datum

@benchmark(args = [[_TEST_DATUM_]])
class benchmark_covariance_matrix:

	def __init__(self, vector):
		self.datum = datum(vector)
		self.cov = self.datum.cov

	@benchmark(tolerance = 3e-6)
	def benchmark_getitem(self):
		return self.cov["x", "y"]

	@benchmark(tolerance = 5e-6)
	def benchmark_setitem(self):
		self.cov["x", "y"] = 1
