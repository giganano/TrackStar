# cython: language_level = 3, boundscheck = False
#
# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/TrackStar.git.

__all__ = ["openmp_linked"]
from . cimport multithread

def openmp_linked():
	r"""
	Returns ``True`` if TrackStar's parallel processing features are
	enabled, and ``False`` otherwise.

	If you would like to make use of these features, follow the
	instructions for enabling multi-threading under TrackStar's
	:doc:`install guide <../install>`.
	"""
	return bool(multithreading_enabled())
