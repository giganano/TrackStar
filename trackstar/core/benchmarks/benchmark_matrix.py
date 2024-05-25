#!/usr/bin/env python
#
# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/trackstar.git.

from trackstar.benchmarks import benchmark
from trackstar import matrix
import numpy as np

class BenchmarkSquareMatrix:

	@benchmark([2, 3, 4, 5, 6, 7, 8, 9, 10])
	def benchmark_determinant(size):
		x = matrix.identity(size)
		for i in range(size):
			for j in range(size):
				x[i, j] = x[j, i] = np.random.random()
		return x.determinant

		