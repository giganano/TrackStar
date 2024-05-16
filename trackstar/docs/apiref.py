#!/usr/bin/env python
#
# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/trackstar.git.

from .config import _CONFIG_
from inspect import signature
import textwrap
import os

class apiref:

	def __init__(self):
		self._docpages = []
		for item in _CONFIG_.keys():
			self._docpages.append(docpage(item,
				name = _CONFIG_[item]["name"],
				title = _CONFIG_[item]["title"],
				designation = _CONFIG_[item]["designation"],
				subs = _CONFIG_[item]["subs"]))

	@property
	def docpages(self):
		r"""
		Type : ``list``

		Each of the individual ``docpage`` objects, initialized based on the
		input in ``config.py``.
		"""
		return self._docpages

	def save(self, path = os.getcwd()):
		r"""
		Save the API ref as a series of .rst files.
		"""
		for page in self.docpages: page.save(path = path)


class docpage:

	def __init__(self, obj, name = None, designation = None, title = None,
		subs = []):
		self.obj = obj
		self.name = name
		self.designation = designation
		self.title = title
		self.subs = subs

	def __repr__(self):
		rep = "\n"
		if self.title is not None:
			rep += "%s\n" % (self.title)
			for i in range(len(self.title)): rep += "="
			rep += "\n\n"
		else: pass
		try:
			sig = signature(self.obj)
		except (ValueError, TypeError):
			sig = None
		if self.designation is not None:
			rep += ".. py:%s:: %s" % (self.designation, self.name)
		else: pass
		if sig is not None:
			rep += "("
			params = list(sig.parameters.keys())
			if len(params) and params[0] == "self":
				start = 1
			else:
				start = 0
			for i in range(start, len(params)):
				if i == start:
					rep += "%s" % (params[i])
				else:
					rep += ", %s" % (params[i])
			rep += ")\n\n"
		elif hasattr(self.obj, "__signature__"):
			rep += "%s\n\n" % (self.obj.__signature__)
		else:
			rep += "\n\n"
		rep += self.obj.__doc__
		rep += "\n\n"
		return rep

	@property
	def obj(self):
		r"""
		Type : ``object`` (i.e. any type)

		The object to generate the reStructuredText file for given its
		docstring.
		"""
		return self._obj

	@obj.setter
	def obj(self, value):
		if hasattr(value, "__doc__"):
			self._obj = value
		else:
			raise ValueError("Must have attribute __doc__")

	@property
	def name(self):
		r"""
		Type : ``str``

		The name of the object to be displayed at the top of the RST file.
		"""
		return self._name

	@name.setter
	def name(self, value):
		if isinstance(value, str):
			self._name = value
		elif value is None:
			self._name = ""
		else:
			raise TypeError("Expected a string or None. Got: %s" % (
				type(value)))

	@property
	def designation(self):
		r"""
		Type : ``str``

		The type of object (e.g., "function", "class", "attribute").
		"""
		return self._designation

	@designation.setter
	def designation(self, value):
		if isinstance(value, str):
			self._designation = value
		elif value is None:
			self._designation = None
		else:
			raise TypeError("Expected a string or None. Got: %s" % (
				type(value)))

	@property
	def title(self):
		r"""
		Type : ``str``

		The string to use the title of the documentation page.
		"""
		return self._title

	@title.setter
	def title(self, value):
		if isinstance(value, str):
			self._title = value
		elif value is None:
			self._title = None
		else:
			raise TypeError("Expected a string or None. Got: %s" % (
				type(value)))

	@property
	def subs(self):
		r"""
		Type : ``list``

		A list of objects whose documentation is to be compiled onto the same
		page as this object.
		"""
		return self._subs

	@subs.setter
	def subs(self, value):
		if isinstance(value, list):
			self._subs = []
			for i in range(len(value)):
				if value[i] in _CONFIG_.keys():
					self._subs.append(docpage(value[i],
						name = _CONFIG_[value[i]]["name"],
						title = _CONFIG_[value[i]]["title"],
						designation = _CONFIG_[value[i]]["designation"],
						subs = _CONFIG_[value[i]]["subs"]))
				else:
					raise ValueError("Object not in config file: %s" % (
						value[i]))
		else:
			raise TypeError("Expected a list. Got: %s" % (type(value)))

	def save(self, path = os.getcwd()):
		if not self.is_sub():
			with open("%s/%s.rst" % (path, self.name), 'w') as f:
				f.write(str(self))
				for sub in self.subs:
					f.write(str(sub))
				f.close()
		else:
			pass

	def is_sub(self):
		sub = False
		for key in _CONFIG_.keys():
			sub |= self.obj in _CONFIG_[key]["subs"]
			if sub: break
		return sub


