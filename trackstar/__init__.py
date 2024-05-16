#!/usr/bin/env python
#
# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/trackstar.git.
r"""
TrackStar
=========

A user-friendly, optimized library for computing Bayesian likelihood functions
for curves when the data have errors in all directions.
"""

__all__ = ["version", "test"]
__author__ = "James W. Johnson"
import warnings
from .version import version
__version__ = str(version)
__all__.append("__version__")
from .core import *
__all__.extend(core.__all__)
from .tests import test

