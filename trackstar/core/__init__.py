# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/trackstar.git.

try:
	__TRACKSTAR_SETUP__
except NameError:
	__TRACKSTAR_SETUP__ = False

if not __TRACKSTAR_SETUP__:

	__all__ = ["matrix"]
	from .matrix import matrix

else: pass
