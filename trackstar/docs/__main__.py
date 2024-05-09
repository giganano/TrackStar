#!/usr/bin/env python
#
# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/trackstar.git.
r"""
*TrackStar Developer's Documentation*

Systematically generate reStructuredText files for each page in the
Comprehensive API Reference.

ARGV
----
1) The path to produce the files at. Defaults to current working directory.
"""

try:
	import trackstar
except ModuleNotFoundError:
	raise RuntimeError("""\
TrackStar must be installed before its documentation can be compiled.""")
from .doctree import doctree
import sys
import os

def generate_api_ref(path = os.getcwd()):
	r"""See ``doctree.save`` in doctree.py."""
	doctree(trackstar).save(path = path)

if __name__ == "__main__":
	if len(sys.argv) > 1:
		generate_api_ref(path = sys.argv[1])
	else:
		generate_api_ref()

