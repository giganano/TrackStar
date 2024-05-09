# cython: language_level = 3, boundscheck = False
#
# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/trackstar.git.

__all__ = ["openmp_linked"]
from . cimport multithread

def openmp_linked():
	r"""
	Returns ``True`` if the OpenMP library is available, which enables
	multithreading. ``False`` otherwise.

	.. todo:: Link to install documentation.
	"""
	return bool(multithreading_enabled())
