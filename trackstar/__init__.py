#!/usr/bin/env python
#
# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/trackstar.git.
r"""
Welcome to TrackStar!

Copyright (C) 2023 James W. Johnson.
"""

__all__ = ["version", "test"]
__author__ = "James W. Johnson"
import warnings
from .version import version
if not version.isreleased:
	warnings.warn("Using un-released version of TrackStar", UserWarning)
else:
	prelease = False
	for item in [version.dev, version.alpha, version.beta, version.rc]:
		prerelease |= item is not None
		if prerelease:
			warnings.warn("Using a pre-release of TrackStar", UserWarning)
			break
		else: continue
__version__ = str(version)
__all__.append("__version__")
from .core import *
__all__.extend(core.__all__)
from .tests import test

