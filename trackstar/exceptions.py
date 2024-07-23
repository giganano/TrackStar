#!/usr/bin/env python
#
# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/TrackStar.git.
r"""
Special ``Warning`` and ``Exception`` classes.
"""

__all__ = ["VersionWarning", "VersionError"]

class VersionWarning(Warning):
	r"""
	A ``Warning`` class for drawing the user's attention to their current
	version of TrackStar (e.g. if they are using an alpha release or a
	development version).

	Although it is not recommended, this class of warnings can be silenced via

	>>> warnings.filterwarnings("ignore", category = trackstar.VersionWarning)

	Alternatively, to silence all warnings within TrackStar:

	>>> trackstar.warnings.filterwarnings("ignore")

	However, the above lines are moot in practice, because one must first
	import TrackStar, which will either raise a ``VersionWarning`` or not
	*before* this ``Warning`` class can be silenced.

	To silence all warnings globally:

	>>> warnings.filterwarnings("ignore")
	"""
	pass


class VersionError(Exception):
	r"""
	An ``Exception`` class that is raised when TrackStar's version information
	is invalid. If you are not a TrackStar developer and have seen this error
	class raised on your system, please open an issue at
	https://github.com/giganano/TrackStar/issues.
	"""
	pass
