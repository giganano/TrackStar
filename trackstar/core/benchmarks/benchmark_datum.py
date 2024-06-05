#!/usr/bin/env python
#
# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/trackstar.git.

from trackstar.benchmarks import benchmark
from trackstar.core.tests.test_datum import _TEST_DATUM_
from trackstar import datum

@benchmark(repeat = 1000, args = [[_TEST_DATUM_]], tolerance = 5e-7)
class simple_datum:

	def __init__(self, vector):
		self.datum = datum(vector)

	def benchmark_getitem(self):
		return self.datum["x"]

	def benchmark_setitem(self):
		self.datum["y"] = 1

