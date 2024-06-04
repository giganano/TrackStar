#!/usr/bin/env python
#
# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/trackstar.git.

from trackstar import benchmarks

config = {
	benchmarks: {
		"name": "trackstar.benchmarks.inc",
		"designation": None,
		"title": None,
		"subs": [
			benchmarks.benchmark
		]
	},
	benchmarks.benchmark: {
		"name": "benchmark",
		"designation": "decorator",
		"title": None,
		"subs": []
	}
}


