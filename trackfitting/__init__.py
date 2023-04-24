#!/usr/bin/env python
#
# This file is part of the trackfitting package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/trackfitting.git.
r"""
TrackFitting
=============

This software is in early development and does not yet have an official name.

Copyright (C) 2023 James W. Johnson.
"""

__all__ = ["version"]
__author__ = "James W. Johnson"
from .version import version
import warnings

try:
	__TRACKFITTING_SETUP__
except NameError:
	__TRACKFITTING_SETUP__ = False

if not __TRACKFITTING_SETUP__:

	version = version._fromfile()
	if not version.isreleased:
		warnings.warn("Using un-released version of trackfitting", UserWarning)
	else:
		prelease = False
		for item in [version.dev, version.alpha, version.beta, version.rc]:
			prerelease |= item is not None
			if prerelease:
				warnings.warn("Using a pre-release of trackfitting", UserWarning)
				break
			else: continue
	__version__ = str(version)
	__all__.append("__version__")

else: pass



