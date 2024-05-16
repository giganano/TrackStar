#!/usr/bin/env python
#
# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/trackstar.git.

import importlib.metadata
import warnings
import re
__ISRELEASED__ = False

class VersionWarning(UserWarning):

	r"""
	An warning class intended for drawing the user's attention to their
	version of TrackStar (i.e. if they are using, e.g., an alpha release or a
	development version).
	"""

	pass


class version:

	r"""
	The string containing the current version number can be accessed via
	``trackstar.__version__`` or ``str(trackstar.version)``.

	Notes
	-----
	TrackStar adopts the PEP 440 [1]_ convention for software versioning.
	As stipulated by both standard software versioning conventions and the
	PEP 440 standard, any release may be either an alpha, a beta, or a release
	candidate, but not multiple at the same time.

	.. [1] https://peps.python.org/pep-0440/
	"""

	_ERR_MSG_ = "Internal error: invalid version information."

	def __init__(self):
		assert isinstance(__ISRELEASED__, bool), version._ERR_MSG_
		breakdown = version._parse_version_number(
			importlib.metadata.version(__name__.split('.')[0]))
		self._epoch = breakdown["epoch"]
		self._major = breakdown["major"]
		self._minor = breakdown["minor"]
		self._micro = breakdown["micro"]
		self._alpha = breakdown["alpha"]
		self._beta = breakdown["beta"]
		self._rc = breakdown["rc"]
		self._post = breakdown["post"]
		self._dev = breakdown["dev"]
		self._isreleased = __ISRELEASED__
		if self.alpha is not None: warnings.warn("""\
Using an alpha version of TrackStar. Be advised that buggy behavior and/or \
numerical artifacts are possible. If you suspect either, please open an issue \
at https://github.com/giganano/TrackStar/issues. We strongly encourage \
consulting with TrackStar developers.""", VersionWarning)
		if self.beta is not None: warnings.warn("""\
Using a beta version of TrackStar. Be advised that buggy behavior and/or \
numerical artifacts are possible. If you suspect either, please open an issue \
at https://github.com/giganano/TrackStar/issues.""", VersionWarning)
		if self.rc is not None: warnings.warn("""\
Using a pre-release of TrackStar. If you suspect buggy behavior and/or \
numerical artifacts, please open an issue at \
https://github.com/giganano/TrackStar/issues""", VersionWarning)
		if self.dev is not None: warnings.warn("""\
Using a development version of TrackStar. Be advised that the features in \
development may exhibit buggy behavior and/or numerical artifacts. We \
encourage consulting with TrackStar developers.""", VersionWarning)
		if not self.isreleased: warnings.warn("""\
Using an un-released version of TrackStar.""", VersionWarning)


	@staticmethod
	def _parse_version_number(number):
		breakdown = {
			"epoch": None,
			"major": None,
			"minor": None,
			"micro": None,
			"alpha": None,
			"beta": None,
			"rc": None,
			"post": None,
			"dev": None
		}
		p = re.compile(
r"^(\d+!)?(\d+){1}(.\d+){1}(.\d+)?(a\d+|b\d+|rc\d+)?(.post\d+)?(.dev\d+)?$")
		match = p.match(number)
		assert match is not None, version._ERR_MSG_
		groups = match.groups()
		if groups[0] is not None:
			assert groups[0].endswith("!"), version._ERR_MSG_
			assert groups[0][:-1].isdigit(), version._ERR_MSG_
			breakdown["epoch"] = int(groups[0][:-1])
		else: pass
		assert groups[1] is not None, version._ERR_MSG_
		assert groups[1].isdigit(), version._ERR_MSG_
		breakdown["major"] = int(groups[1])
		assert groups[2] is not None, version._ERR_MSG_
		assert groups[2].startswith("."), version._ERR_MSG_
		assert groups[2][1:].isdigit(), version._ERR_MSG_
		breakdown["minor"] = int(groups[2][1:])
		for group in groups[3:]:
			if group is None:
				continue
			elif group.startswith("."):
				assert (group[1:].isdigit() or group[1:5] == "post" or
					group[1:4] == "dev"), version._ERR_MSG_
				if group[1:].isdigit():
					breakdown["micro"] = int(group[1:])
				elif group[1:5] == "post":
					breakdown["post"] = int(group[5:])
				else:
					breakdown["dev"] = int(group[4:])
			elif group.startswith("a"):
				assert group[1:].isdigit()
				breakdown["alpha"] = int(group[1:])
			elif group.startswith("b"):
				assert group[1:].isdigit()
				breakdown["beta"] = int(group[1:])
			elif group.startswith("rc"):
				assert group[2:].isdigit()
				breakdown["rc"] = int(group[2:])
			else:
				assert False, version._ERR_MSG_
		return breakdown


	def __repr__(self):
		r"""The X.Y.Z version string."""
		rep = ""
		if self.epoch is not None: rep += "%d!" % (self.epoch)
		rep += "%d.%d" % (self.major, self.minor)
		if self.minor is not None: rep += ".%d" % (self.minor)
		if self.alpha is not None:
			rep += "a%d" % (self.alpha)
		elif self.beta is not None:
			rep += "b%d" % (self.beta)
		elif self.rc is not None:
			rep += "rc%d" % (self.rc)
		else: pass
		if self.post is not None: rep += ".post%d" % (self.post)
		if self.dev is not None: rep += ".dev%d" % (self.dev)
		return rep


	def __iter__(self):
		r"""
		Type-casts to a tuple.
		"""
		yield self.epoch
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
		r"""Type-casts to a tuple and uses its ``__getitem__`` method."""
		return tuple(self).__getitem__(key)


	def todict(self):
		r"""
		Convert this object into a dictionary.
		"""
		return {
			"epoch": 		self.epoch,
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
	def epoch(self):
		r"""
		Type : ``int``

		The epoch version number. In general, this property will always be
		``None`` as the developers do not anticipate a need to modify
		TrackStar's version number handling in the foreseeable future.
		"""
		return self._epoch


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


version = version()

