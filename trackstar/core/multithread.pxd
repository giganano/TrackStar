# cython: language_level = 3, boundscheck = False
#
# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/trackstar.git.

cdef extern from "./src/multithread.h":
	inline unsigned short multithreading_enabled()
	inline long max_threads_allowed()
	unsigned long MAX_THREADS_CPU_RATIO
