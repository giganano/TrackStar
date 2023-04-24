#!/usr/bin/env python
#
# This file is part of the trackfitting package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/trackfitting.git.

import sys
import os
PATH = os.path.dirname(os.path.abspath(__file__))

class version:

	def __init__(self, major = 0, minor = 0, micro = 0,
		alpha = None, beta = None, rc = None,
		dev = 0, post = None, isreleased = False):
		for _ in [major, minor, micro]:
			assert isinstance(_, int), "Invalid version information"
			assert _ >= 0, "Invalid version information"
		prerelease = 0
		for _ in [alpha, beta, rc]:
			if _ is not None:
				assert isinstance(_, int), "Invalid version information"
				assert _ >= 0, "Invalid version information"
				prerelease += 1
			else: pass
		assert prerelease in [0, 1], "Invalid version information"
		devorpost = 0
		for _ in [dev, post]:
			if _ is not None:
				assert isinstance(_, int), "Invalid version information"
				assert _ >= 0, "Invalid version information"
				devorpost += 1
			else: pass
		assert devorpost in [0, 1], "Invalid version information"
		assert isinstance(isreleased, bool), "Invalid version information"
		self._major = major
		self._minor = minor
		self._micro = micro
		self._alpha = alpha
		self._beta = beta
		self._rc = rc
		self._dev = dev
		self._post = post
		self._isreleased = isreleased

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

	def __getitem__(self, key):
		return tuple(self).__getitem__(key)

	@classmethod
	def _fromfile(cls, filename = "%s/version_breakdown.dat" % (PATH)):
		with open(filename, 'r') as f:
			kwargs = {}
			while True:
				line = f.readline()
				if line == "": break
				if line[0] == "#": continue
				line = line.split()
				if line[0] == "isreleased":
					kwargs["isreleased"] = line[1] == "True"
				elif line[1] == "None":
					kwargs[line[0]] = None
				else:
					kwargs[line[0]] = int(line[1])
			f.close()
		return cls(**kwargs)

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
			"dev": 			self.dev
		}

	@property
	def major(self):
		r"""
		Type : ``int``

		The major version number.
		"""
		return self._major

	@property
	def minor(self):
		r"""
		Type : ``int``

		The minor version number.
		"""
		return self._minor

	@property
	def micro(self):
		r"""
		Type : ``int``

		The micro version number.
		"""
		return self._micro

	@property
	def alpha(self):
		r"""
		Type : ``int`` or ``None``

		The alpha release version number.
		"""
		return self._alpha

	@property
	def beta(self):
		r"""
		Type : ``int`` or ``None``

		The beta release version number.
		"""
		return self._beta

	@property
	def rc(self):
		r"""
		Type : ``int`` or ``None``

		The release candidate version number.
		"""
		return self._rc

	@property
	def dev(self):
		r"""
		Type : ``int`` or ``None``

		The development release version number.
		"""
		return self._dev

	@property
	def post(self):
		r"""
		Type : ``int`` or ``None``

		The post release version number.
		"""
		return self._post

	@property
	def isreleased(self):
		r"""
		Type : ``bool``

		Whether or not this version has been tagged and released.
		"""
		return self._isreleased

