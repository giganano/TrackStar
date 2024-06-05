#!/usr/bin/env python
#
# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/TrackStar.git.

from .discovery import discover_benchmarks
import sys

if __name__ == "__main__":
	if len(sys.argv) > 1:
		discover_benchmarks(path = sys.argv[1])
	else:
		discover_benchmarks()

