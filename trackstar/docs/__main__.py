#!/usr/bin/env python
#
# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/trackstar.git.
r"""
*TrackStar Developer's Documentation*

Systematically generate reStructuredText files for each page in both the
Python and C API references.

ARGV
----
1) The directory to save the python API reference in. Defaults to current
	working directory.
2) The directory to save the C API reference in. Default to current working
	directory.
"""

try:
	import trackstar
except ModuleNotFoundError:
	raise RuntimeError("""\
TrackStar must be installed before its documentation can be compiled.""")
from .apiref import apiref
from .clib import clib
import sys
import os

def generate_api_ref(path = os.getcwd()):
	r"""See ``apiref.save`` in apiref.py."""
	apiref().save(path = path)

def generate_clib_ref(path = os.getcwd()):
	r"""See ``clib.save`` in clib.py."""
	clib().save(path = path)

if __name__ == "__main__":
	if len(sys.argv) > 2:
		generate_api_ref(path = sys.argv[1])
		generate_clib_ref(path = sys.argv[2])
	else:
		raise RuntimeError("""\
At least two command-line arguments are required to compile TrackStar's API \
reference.""")
