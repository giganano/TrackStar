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
	repeat = 1000,
	tolerance = 2.5e-5)
class square_matrix:

	def __init__(self, size):
		self.mat = matrix.identity(size)
		for i in range(size):
			for j in range(size):
				self.mat[i, j] = self.mat[j, i] = np.random.random()

	@benchmark(tolerance = 1.5e-6)
	def benchmark_getitem(self):
		return self.mat[0, 0]

	@benchmark(tolerance = 1.5e-6)
	def benchmark_setitem(self):
		self.mat[0, 0] = 1

	def benchmark_add(self):
		return self.mat + self.mat

	def benchmark_subtract(self):
		return self.mat - self.mat

	def benchmark_multiply(self):
		return self.mat * self.mat

	@benchmark(tolerance = 1e-6)
	def benchmark_determinant(self):
		return self.mat.determinant()

	@benchmark(tolerance = 1.5e-4, repeat = 100)
	def benchmark_invert(self):
		return self.mat.invert()

	def benchmark_transpose(self):
		return self.mat.transpose()

