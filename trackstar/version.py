#!/usr/bin/env python
#
# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/trackstar.git.

import importlib.metadata
__ISRELEASED__ = False

class version:

	r"""
	TrackStar adopts the PEP 440 [1]_ convention for software versioning.
	Its version number can be accessed via ``trackstar.__version__`` as well as
	``str(trackstar.version)``.

	Attributes
	----------
	major : ``int``
		The major version number of this release. This number increases by one
		whenever there are changes to TrackStar's API that break backwards
		compatibility.
	minor : ``int``
		The minor version number of this release. This number increases by one
		whenever there are new features added to TrackStar.
	micro : ``int``
		The micro version number of this release. This number increases by one
		whenever there is a patch or documentation updates.
	alpha : ``int``
		The alpha version number of this release. ``None`` if this is not an
		alpha release.
	beta : ``int``
		The beta version number of this release. ``None`` if this is not a
		beta release.
	rc : ``int``
		The release candidate number of this release. ``None`` if this is not a
		release candidate.
	post : ``int``
		The post number of this release. ``None`` if this is not a post release.
	dev : ``int``
		The development number of this release. ``None`` if this is not a
		development release.
	isreleased : ``bool``
		Whether or not this is a released version of TrackStar. If ``False``,
		users are strongly encouraged to contact one of TrackStar's
		contributors if they are not a contributor themselves.
	python_requires : ``str``
		The string identifier for the minimum version of python required to run
		this version of TrackStar.

	.. note::

		As stipulated by both standard software versioning conventions and the
		PEP 440 standards, any given release will have at most one of ``alpha``,
		``beta``, ``rc``, ``post`` and ``dev`` as anything other than ``None``.

	Functions
	---------
	.. function:: trackstar.version.todict()

		Type-cast this version information to a ``dict``.

	.. [1] https://peps.python.org/pep-0440/
	"""

	def __init__(self):
		_version = importlib.metadata.version(__name__.split('.')[0])
		_version = _version.split('.')
		msg = "Invalid version information"
		assert len(_version) in [3, 4], msg
		for i in range(2): assert _version[i].isdigit(), msg
		self._major = int(_version[0])
		self._minor = int(_version[1])
		if len(_version) == 3:
			if "a" in _version[2]:
				micro, alpha = _version[2].split('a')
				assert alpha.isdigit(), msg
				self._alpha = int(alpha)
			elif "b" in _version[2]:
				micro, beta = _version[2].split('b')
				assert beta.isdigit(), msg
				self._beta = int(beta)
			elif "rc":
				micro, rc = _version[2].split("rc")
				assert rc.isdigit(), msg
				self._rc = int(rc)
			else:
				micro = _version[2]
			assert micro.isdigit(), msg
			self._micro = int(micro)
		else: # must be 4 based on prior assertion
			assert _version[2].isdigit(), msg
			self._micro = int(_version[2])
			if "post" in _version[3]:
				post = _version[3][4:]
				assert post.isdigit(), msg
				self._post = int(post)
			elif "dev" in _version[3]:
				dev = _version[3][3:]
				assert dev.isdigit(), msg
				self._dev = int(dev)
			else: pass
		assert isinstance(__ISRELEASED__, bool), msg
		self._isreleased = __ISRELEASED__
		if not hasattr(self, "alpha"): self._alpha = None
		if not hasattr(self, "beta"): self._beta = None
		if not hasattr(self, "rc"): self._rc = None
		if not hasattr(self, "post"): self._post = None
		if not hasattr(self, "dev"): self._dev = None
		assert self.__repr__() == '.'.join(_version), """\
Internal version string does not match package metadata."""


	def __repr__(self):
		r"""The X.Y.Z version string."""
		rep = "%d.%d.%d" % (self.major, self.minor, self.micro)
		if self.alpha is not None: rep += "a%d" % (self.alpha)
		if self.beta is not None: rep += "b%d" % (self.beta)
		if self.rc is not None: rep += "rc%d" % (self.rc)
		if self.post is not None: rep += ".post%d" % (self.post)
		if self.dev is not None: rep += ".dev%d" % (self.dev)
		return rep


	def __iter__(self):
		r"""
		Type-casts to a tuple.
		"""
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

