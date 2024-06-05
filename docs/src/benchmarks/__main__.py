#!/usr/bin/env python
#
# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/TrackStar.git.

import os

if __name__ == "__main__":
	cwd = os.getcwd()
	os.chdir(os.path.dirname(os.path.abspath(__file__)))
	os.system("python samplesize.py")
	os.system("python tracksize.py")
	os.system("python nthreads.py")
	os.system("python dimensionality.py")
	os.chdir(cwd)

