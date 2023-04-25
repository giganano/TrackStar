#!/usr/bin/env python
#
# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/trackstar.git.

import sys
from . import version_breakdown
if sys.version_info[:3] < tuple(
	[int(_) for _ in version_breakdown.minpython.split('.')]):
	raise RuntimeError("""\
Python version %s is required. Current version: %d.%d.%d""" % (
	version_breakdown.minpython, sys.version_info.major,
	sys.version_info.minor, sys.version_info.micro))

class version:

	def __repr__(self):
		rep = "%d.%d.%d" % (self.major, self.minor, self.micro)
		if self.alpha is not None: rep += "a%d" % (self.alpha)
		if self.beta is not None: rep += "b%d" % (self.beta)
		if self.rc is not None: rep += "rc%d" % (self.rc)
		if self.post is not None: rep += ".post%d" % (self.post)
		if self.dev is not None: rep += ".dev%d" % (self.dev)
		return rep

	def __iter__(self):
		yield self.major
		yield self.minor
		yield self.micro
		yield self.alpha
		yield self.beta
		yield self.rc
		yield self.post
		yield self.dev
		yield self.isreleased

	def __getitem__(self, key):
		return tuple(self).__getitem__(key)

	def todict(self):
		r"""
		Convert this object into a dictionary.
		"""
		return {
			"major": 		self.major,
			"minor": 		self.minor,
			"micro": 		self.micro,
			"alpha": 		self.alpha,
			"beta": 		self.beta,
			"rc": 			self.rc,
			"post": 		self.post,
			"dev": 			self.dev,
			"isreleased": 	self.isreleased
		}

	@property
	def major(self):
		r"""
		Type : ``int``

		The major version number.
		"""
		return version_breakdown.major

	@property
	def minor(self):
		r"""
		Type : ``int``

		The minor version number.
		"""
		return version_breakdown.minor

	@property
	def micro(self):
		r"""
		Type : ``int``

		The micro version number.
		"""
		return version_breakdown.micro

	@property
	def alpha(self):
		r"""
		Type : ``int`` or ``None``

		The alpha release version number.
		"""
		return version_breakdown.alpha

	@property
	def beta(self):
		r"""
		Type : ``int`` or ``None``

		The beta release version number.
		"""
		return version_breakdown.beta

	@property
	def rc(self):
		r"""
		Type : ``int`` or ``None``

		The release candidate version number.
		"""
		return version_breakdown.rc

	@property
	def dev(self):
		r"""
		Type : ``int`` or ``None``

		The development release version number.
		"""
		return version_breakdown.dev

	@property
	def post(self):
		r"""
		Type : ``int`` or ``None``

		The post release version number.
		"""
		return version_breakdown.post

	@property
	def isreleased(self):
		r"""
		Type : ``bool``

		Whether or not this version has been tagged and released.
		"""
		return version_breakdown.isreleased

	@property
	def python_requires(self):
		r"""
		Type : ``str``

		The version string of the minimum python version required for this
		version of TrackStar.
		"""
		return version_breakdown.minpython


version = version()

