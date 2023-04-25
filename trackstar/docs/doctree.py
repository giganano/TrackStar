#!/usr/bin/env python
#
# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/trackstar.git.

from .config import _CONFIG_
import textwrap
import os

class doctree:

	r"""
	*TrackStar Developer's Documentation*

	A tree structure whose nodes are modules within a python namespace and
	whose branches are other nodes or functions/classes within that namespace.
	Requires the _CONFIG_ dictionary in config.py to produce the TrackStar
	API reference.

	**Signature**: trackstar.docs.doctree(obj)

	Parameters
	----------
	obj : ``object``
		The object to start the doctree at. In practice, this will be
		TrackStar's root __init__ file.

	Attributes
	----------
	header : ``str``
		The string that will be displayed at the top of this objects' page of
		the documentation.
	subs : ``list``
		A list of the objects that are to be treated as branches off of this
		object in the API namespace.

	Functions
	---------
	save : ``instancemethod``
		Recursively save the docstring for ``obj``, all of its ``subs``, and
		all of their ``subs`` in reStructured Text files.
	"""

	def __init__(self, obj):
		self.obj = obj
		self.header = _CONFIG_[self._obj]["header"]
		self.subs = _CONFIG_[self._obj]["subs"]


	def __repr__(self):
		rep = "%s\n" % (self.header)
		for i in range(len(self.header)): rep += "="
		rep += "\n"
		rep += textwrap.dedent(self._obj.__doc__)
		if len(self.subs):
			rep += "\n\n"
			rep += ".. toctree::\n"
			rep += "\t:titlesonly:\n"
			rep += "\t:maxdepth: 5\n\n"
			for _ in self.subs: rep += "\t%s\n" % (_.header) # leave off .rst
		return rep


	@property
	def obj(self):
		r"""
		Type : ``object`` (i.e., any type)

		The object to generate the reStructuredText file for given its
		docstring.
		"""
		return self._obj


	@obj.setter
	def obj(self, value):
		if hasattr(value, "__doc__"):
			self._obj = value
		else:
			raise ValueError("Must have attribute '__doc__'.")


	@property
	def header(self):
		r"""
		Type : ``str``

		The string that will appear as the title of this object's page of the
		documentation.
		"""
		return self._header


	@header.setter
	def header(self, value):
		if isinstance(value, str):
			self._header = value
		else:
			raise TypeError("Header must be of type `str`. Got: %s" % (
				type(value)))


	@property
	def subs(self):
		r"""
		Type : ``list``

		The ``doctree`` objects to be listed as "under" this one in toctrees
		and the like. These are the "branches" of the tree.
		"""
		return self._subs


	@subs.setter
	def subs(self, value):
		if isinstance(value, list):
			self._subs = len(value) * [0.]
			for i in range(len(self._subs)):
				if isinstance(value[i], self.__class__):
					self._subs[i] = value[i]
				elif value[i] in _CONFIG_.keys():
					self._subs[i] = self.__class__(value[i])
				else:
					raise ValueError("Invalid sub: %s" % (str(value[i])))
		else:
			raise TypeError("""\
Attribute `subs` must be of type `list`. Got: %s""" % (type(subs)))


	def save(self, path = os.getcwd()):
		r"""
		Produce the output files for the base element of this doctree and
		recursively produce all output files along all branches thereafter.

		**Signature**: x.save()

		Parameters
		----------
		x : ``doctree``
			An instance of this class.
		path : ``str`` [default : ``os.getcwd()``]
			The relative or absolute path to the location to store the output
			files. Defaults to the current working directory.

		Notes
		-----
		In practice, this function only ever gets called in the following
		manner:

		>>> import trackstar
		>>> doctree(trackstar).save()
		"""
		with open("%s/%s.rst" % (path, self.header), 'w') as f:
			f.write(str(self))
			f.close()
		for _ in self.subs: _.save(path = path)
		# with open("%s/toctree.rst" % (path), 'w') as f:
		# 	f.write(".. toctree::\n")
		# 	f.write("\t:maxdepth: 1\n")
		# 	f.write("\t%s\n" % (self.header))
		# 	def addsubs(tree):
		# 		for sub in tree.subs:
		# 			f.write("\t%s\n" % (sub.header))
		# 			if len(sub.subs): addsubs(sub)
		# 	addsubs(self)
		# 	f.close()
