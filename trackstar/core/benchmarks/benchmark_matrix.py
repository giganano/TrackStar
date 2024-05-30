#!/usr/bin/env python
#
# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/trackstar.git.

from trackstar.benchmarks import benchmark
from trackstar import matrix
import numpy as np


@benchmark(repeat = 1000)
class square_matrix:

	def __init__(self, size):
		self.mat = matrix.identity(size)
		for i in range(size):
			for j in range(size):
				self.mat[i, j] = self.mat[j, i] = np.random.random()

	@benchmark(repeat = 100)
	def benchmark_determinant(self):
		return self.mat.determinant()

	def benchmark_invert(self):
		return self.mat.invert()


@benchmark(repeat = 1000)
def benchmark_foo(x):
	return x + 12

@benchmark
def bar_benchmark(x):
	return x + 3

		