#!/usr/bin/env python
#
# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/trackstar.git.

import trackstar
import textwrap
import glob
import os

class clib:

	def __init__(self):
		# headers = glob.glob("%s/core/src/*.h" % (trackstar.__path__[0]))
		headers = [
			"%s/core/src/matrix.h" % (trackstar.__path__[0]),
			"%s/core/src/datum.h" % (trackstar.__path__[0])
		]
		self._headers = [header_file(_) for _ in headers]


	def save(self, path = os.getcwd()):
		for header in self._headers: header.save(path = path)



class header_file:

	def __init__(self, name):
		# self.path = "%s/core/src/%s" % (trackstar.__path__[0], name)
		self.path = name

	def __repr__(self):
		name = "/".join(self.path.split("/")[-4:])
		rep = "\n%s\n" % (name)
		for i in range(len(name)): rep += "="
		blocks = self.read_file()
		for block in blocks:
			rep += "%s\n\n" % (block)
		return rep


	@property
	def path(self):
		r"""
		Type : ``str``

		The full path to the header file.
		"""
		return self._path

	@path.setter
	def path(self, value):
		if isinstance(value, str):
			if os.path.isfile(value):
				self._path = value
			else:
				raise ValueError("File not found: %s" % (value))
		else:
			raise TypeError("Expected string for file name. Got: %s" % (
				type(value)))


	def save(self, path = os.getcwd()):
		with open("%s/%s.rst" % (path, self.path.split('/')[-1]), 'w') as f:
			f.write(str(self))
			f.close()


	def read_file(self):
		comment_blocks = []
		with open(self.path, 'r') as f:
			while True:
				line = f.readline()
				if line.startswith("typedef struct"):
					n = 0
					while not line.endswith("/*\n"): line = f.readline()
					block = self.read_comment_block(f)
					block = textwrap.dedent(block)
					comment_blocks.append(block)
				elif line == "/*\n":
					block = self.read_comment_block(f)
					line = f.readline()
					if line.startswith("extern") or line == "\n":
						comment_blocks.append(block)
					else: pass
				elif line == "": break
			f.close()
		return comment_blocks


	def read_comment_block(self, filestream):
		lines = []
		while True:
			line = filestream.readline()
			if line.endswith("*/\n") or line == "": break
			lines.append(line)
		return self.remove_copyright_block("".join(lines))


	@staticmethod
	def remove_copyright_block(block):
		copyright_block = """\
This file is part of the TrackStar package.
Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
License: MIT License. See LICENSE in top-level directory
at: https://github.com/giganano/TrackStar.git."""
		return block.replace(copyright_block, "")

