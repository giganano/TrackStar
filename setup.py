#!/usr/bin/env python
#
# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/trackstar.git.

# This version of TrackStar requires python >= 3.7.0
_MIN_PYTHON_VERSION_ = "3.7.0"
import builtins
builtins.__TRACKSTAR_SETUP__ = True
import trackstar
from setuptools import setup, Extension
import sys
import os
if sys.version_info[:3] < tuple(
	[int(_) for _ in _MIN_PYTHON_VERSION_.split('.')]):
	raise RuntimeError("""\
Python version %s required. Current version: %d.%d.%d""" % (
		
		_MIN_PYTHON_VERSION_, sys.version_info.major,
		sys.version_info.minor, sys.version_info.micro))
else: pass

# ---------------------------- PACKAGE METADATA ---------------------------- #
package_name = "trackstar"
repo_url = "https://github.com/giganano/trackstar.git"
bugs_url = "https://github.com/giganano/trackstar/issues"

CLASSIFIERS = """\
Development Status :: 1 - Planning
Intended Audience :: Science/Research
License :: OSI Approved :: MIT License
Natural Language :: English
Operating System :: MacOS
Operating System :: POSIX
Operating System :: Unix
Programming Language :: C
Programming Language :: Cython
Programming Language :: Python
Programming Language :: Python :: 3
Programming Language :: Python :: 3.7
Programming Language :: Python :: 3.8
Programming Language :: Python :: 3.9
Programming Language :: Python :: 3.10
Programming Language :: Python :: 3.11
Programming Language :: Python :: 3 :: Only
Programming Language :: Python :: Implementation :: CPython
Topic :: Scientific/Engineering
Topic :: Scientific/Engineering :: Astronomy
Topic :: Scientific/Engineering :: Physics
"""

# -------------------------- VERSION INFORMATION -------------------------- #
VERSION = {
	"major": 		0,
	"minor": 		0,
	"micro":	 	0,
	"dev": 			0,
	"alpha": 		None,
	"beta": 		None,
	"rc": 			None,
	"post": 		None,
	"isreleased":	False,
	"minpython": 	_MIN_PYTHON_VERSION_
}


def write_version_info(filename = "./trackstar/version_breakdown.py"):
	r"""
	Writes the current version info to disk within the source tree.

	Parameters
	----------
	filename : ``str`` [default : "./trackstar/version_breakdown.dat"]
		The relative path to the file to save version information at.

	.. note:: trackstar/version.py depends on the file produced by this
		function.

	Raises
	------
	* AssertionError
		- Version information is invalid. This occurs if any of the entries
			violate the PEP 440 [1]_ standard for version conventions. With
			the exception of ``"isreleased"``, all keys of the global variable
			``VERSION`` must be either ``None`` or an ``int``, and must be
			non-negative if they are an ``int``. Furthermore, Only one of the
			pre-, post-, or development-release identifiers may be flagged.

	.. [1] https://peps.python.org/pep-0440/
	"""
	for _ in ["major", "minor", "micro"]:
		assert isinstance(VERSION[_], int), "Invalid version information"
		assert VERSION[_] >= 0, "Invalid version information"
	prerelease = 0
	for _ in ["alpha", "beta", "rc"]:
		if VERSION[_] is not None:
			assert isinstance(VERSION[_], int), "Invalid version information"
			assert VERSION[_] >= 0, "Invalid version information"
			prerelease += 1
		else: pass
	assert prerelease in [0, 1], "Invalid version information"
	devorpost = 0
	for _ in ["dev", "post"]:
		if VERSION[_] is not None:
			assert isinstance(VERSION[_], int), "Invalid version information"
			assert VERSION[_] >= 0, "Invalid version information"
		else: pass
	assert devorpost in [0, 1], "Invalid version information"
	assert prerelease + devorpost < 2, "Invalid version information"
	assert isinstance(VERSION["isreleased"], bool), "Invalid version information"
	with open(filename, 'w') as f:
		f.write("# This file was generated by TrackStar setup.py\n")
		for key in VERSION.keys(): 
			if key == "minpython":
				f.write("%s = \"%s\"\n" % (key, str(VERSION[key])))
			else:
				f.write("%s = %s\n" % (key, str(VERSION[key])))
		f.close()


def find_extensions(path = "./trackstar"):
	extensions = []
	for root, dirs, files in os.walk(path):
		for file in files:
			if file.endswith(".pyx"):
				name = "%s.%s" % (root[2:].replace('/', '.'),
					file.split('.')[0])
				src_files = ["%s/%s" % (root[2:], file)]
				src_files += ["./trackstar/core/src/matrix.c"]
				extensions.append(Extension(name, src_files))
			else: continue
	return extensions


def find_packages(path = "./trackstar"):
	packages = []
	for root, dirs, files in os.walk(path):
		if "__init__.py" in files: packages.append(root[2:].replace('/', '.'))
	return packages


# --------------------------- CALL TO SETUPTOOLS --------------------------- #
def setup_package():
	r"""
	Compile and install.
	"""
	src_path = os.path.dirname(os.path.abspath(sys.argv[0]))
	old_path = os.getcwd()
	os.chdir(src_path)
	sys.path.insert(0, src_path)

	# write version info to construct proper version string for metadata
	try:
		write_version_info()
		from trackstar.version import version
	except:
		del sys.path[0]
		os.chdir(old_path)
		raise

	# directories with .h header files, req'd by setup
	include_dirs = []
	for root, dirs, files in os.walk("./trackstar/core/src"):
		if any([_.endswith(".h") for _ in files]): include_dirs.append(root)

	metadata = dict(
		name = package_name,
		version = str(version),
		author = "James W. Johnson",
		author_email = "giganano9@gmail.com",
		maintainer = "James W. Johnson",
		maintainer_email = "giganano9@gmail.com",
		url = repo_url,
		project_urls = {
			"Bug Tracker": bugs_url,
			"Source Code": repo_url
		},
		classifiers = CLASSIFIERS.split("\n"),
		license = "MIT",
		platforms = ["Linux, Mac OS X", "Unix"],
		provides = [package_name],
		packages = find_packages(),
		ext_modules = find_extensions(),
		include_dirs = include_dirs,
		setup_requires = [
			"setuptools>=18.0", #automatically handles Cython extensions
			"Cython>=0.29.0"
		],
		python_requires=">=3.7.*,<4",
		zip_safe = False,
		verbose = "-q" not in sys.argv and "--quiet" not in sys.argv
	)

	try:
		setup(**metadata)
	finally:
		del sys.path[0]
		os.chdir(old_path)
	return


if __name__ == "__main__":
	setup_package()
	del builtins.__TRACKSTAR_SETUP__

