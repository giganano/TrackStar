#!/usr/bin/env python
#
# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/trackstar.git.

from trackstar.benchmarks import benchmark
from trackstar import matrix
import numpy as np


@benchmark(
	args = [[2], [3], [4], [5], [6], [7], [8], [9], [10]],
	repeat = 1000)
class square_matrix:

	def __init__(self, size):
		self.mat = matrix.identity(size)
		for i in range(size):
			for j in range(size):
				self.mat[i, j] = self.mat[j, i] = np.random.random()

	@benchmark(repeat = 100, tolerance = 1e-6)
	def benchmark_determinant(self):
		return self.mat.determinant()

	@benchmark(tolerance = 1.5e-4)
	def benchmark_invert(self):
		return self.mat.invert()


@benchmark(
	args = [[2], [3], [4], [5], [6], [7], [8], [9], [10]],
	repeat = 1000)
def benchmark_foo(x):
	return x + 12

@benchmark
def bar_benchmark():
	return "hello"

		