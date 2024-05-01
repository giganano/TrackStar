#!/usr/bin/env python
#
# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/trackstar.git.
#
# TrackStar adopts the PEP 517 standard procedure for distribution, which is
# based on the project.toml file in this directory. This setup file implements
# custom build procedures.

from setuptools import setup, Extension
import glob
import os


def find_extensions(path = "./trackstar"):
	extensions = []
	c_sources = glob.glob("%s/core/src/*.c" % (path))
	for root, dirs, files in os.walk(path):
		for file in files:
			if file.endswith(".pyx"):
				name = "%s.%s" % (root[2:].replace('/', '.'),
					file.split('.')[0])
				src_files = ["%s/%s" % (root[2:], file)]
				src_files += c_sources
				print(src_files)
				extensions.append(Extension(name, src_files))
			else: continue
	return extensions


setup(ext_modules = find_extensions(), include_dirs = ["./trackstar/core/src"])






# def find_extensions(path = "./trackstar"):
# 	extensions = []
# 	c_source_files = find_c_sources(path = path)
# 	for root, dirs, files in os.walk(path):
# 		for file in files:
# 			has_headers = False
# 			if file.endswith(".pyx"):
# 				name = "%s.%s" % (root[2:].replace('/', '.'),
# 					file.split('.')[0])
# 				src_files = ["%s/%s" % (root[2:], file)]
# 				src_files += c_source_files
# 				extensions.append(Extension(name, src_files))
# 			else: continue
# 	return extensions


# def find_c_sources(path = "./trackstar"):
# 	source_files = []
# 	for root, dirs, files in os.walk(path):
# 		for file in files:
# 			if file.endswith(".c"): source_files.append(file)
# 	return source_files


# def find_include_dirs(path = "./trackstar"):
# 	include_dirs = []
# 	for root, dirs, files in os.walk(path):
# 		if any([_.endswith(".h") for _ in files]): include_dirs.append(root)
# 	return include_dirs


# setup_args = dict(
# 	extensions = find_extensions(),
# 	include_dirs = find_include_dirs()
# )

# setup(**setup_args)








# class build_ext(_build_ext):

	# def run(self):
		# setup(
		# 	ext_modules = build_ext.find_extensions(), 
		# 	include_dirs = build_ext.find_include_dirs()
		# )
		# for ext in self.find_extensions(): self.extensions.append(ext)
		# for ext in self.extensions():
		# 	for include_dirs in self.find_include_dirs: ext.append(include_dir)
		# super().run()

	# def initialize_options(self):
	# 	for ext in self.find_extensions(): self.extensions.append(ext)
	# 	for ext in self.extensions():
	# 		for include_dirs in self.find_include_dirs: ext.append(include_dir)
	# 	return super().finalize_options()

	# @staticmethod
	# def find_extensions(path = "./trackstar"):
	# 	extensions = []
	# 	c_source_files = build_ext.find_c_sources(path = path)
	# 	for root, dirs, files in os.walk(path):
	# 		for file in files:
	# 			has_headers = False
	# 			if file.endswith(".pyx"):
	# 				name = "%s.%s" % (root[2:].replace('/', '.'),
	# 					file.split('.')[0])
	# 				src_files = ["%s/%z" % (root[2:], file)]
	# 				src_files += c_source_files
	# 				extensions.append(Extension(name, src_files))
	# 			else: continue
	# 	return extensions

	# @staticmethod
	# def find_c_sources(path = "./trackstar"):
	# 	source_files = []
	# 	for root, dirs, files in os.walk(path):
	# 		for file in files:
	# 			if file.endswith(".c"): source_files.append(file)
	# 	return source_files


	# @staticmethod
	# def find_include_dirs(path = "./trackstar"):
	# 	include_dirs = []
	# 	for root, dirs, files in os.walk(path):
	# 		if any([_.endswith(".h") for _ in files]): include_dirs.append(root)
	# 	return include_dirs




# This version of TrackStar requires python >= 3.8.0
# _MIN_PYTHON_VERSION_ = "3.8.0"
# from subprocess import Popen, PIPE
# import builtins
# builtins.__TRACKSTAR_SETUP__ = True
# import trackstar
# from setuptools import setup, Extension, Command
# from setuptools.command.build_ext import build_ext as _build_ext
# import sys
# import os
# if sys.version_info[:3] < tuple(
# 	[int(_) for _ in _MIN_PYTHON_VERSION_.split('.')]):
# 	raise RuntimeError("""\
# Python version %s required. Current version: %d.%d.%d""" % (
# 		_MIN_PYTHON_VERSION_, sys.version_info.major,
# 		sys.version_info.minor, sys.version_info.micro))
# else: pass

# ---------------------------- PACKAGE METADATA ---------------------------- #
# package_name = "trackstar"
# repo_url = "https://github.com/giganano/trackstar.git"
# bugs_url = "https://github.com/giganano/trackstar/issues"

# CLASSIFIERS = """\
# Development Status :: 1 - Planning
# Intended Audience :: Science/Research
# License :: OSI Approved :: MIT License
# Natural Language :: English
# Operating System :: MacOS
# Operating System :: POSIX
# Operating System :: Unix
# Programming Language :: C
# Programming Language :: Cython
# Programming Language :: Python
# Programming Language :: Python :: 3
# Programming Language :: Python :: 3.8
# Programming Language :: Python :: 3.9
# Programming Language :: Python :: 3.10
# Programming Language :: Python :: 3.11
# Programming Language :: Python :: 3.12
# Programming Language :: Python :: 3 :: Only
# Programming Language :: Python :: Implementation :: CPython
# Topic :: Scientific/Engineering
# Topic :: Scientific/Engineering :: Astronomy
# Topic :: Scientific/Engineering :: Physics
# """

# # -------------------------- VERSION INFORMATION -------------------------- #
# VERSION = {
# 	"major": 		0,
# 	"minor": 		0,
# 	"micro":	 	0,
# 	"dev": 			0,
# 	"alpha": 		None,
# 	"beta": 		None,
# 	"rc": 			None,
# 	"post": 		None,
# 	"isreleased":	False,
# 	"minpython": 	_MIN_PYTHON_VERSION_
# }


# class build_ext(_build_ext):

# 	r"""
# 	Extends the ``build_ext`` base class provided by ``setuptools`` to
# 	determine compiler flags on a case-by-case basis. This is mostly necessary
# 	when linking TrackStar with the OpenMP to enable parallel processing
# 	features.
# 	"""

# 	def build_extensions(self):
# 		# determine compilar and linker flags, some of which are always
# 		# included and others of which are only included when linking with
# 		# OpenMP.
# 		compile_args = ["-fPIC", "-Wsign-conversion", "-Wsign-compare"]
# 		link_args = []
# 		if "TRACKSTAR_ENABLE_OPENMP" in os.environ.keys():
# 			if os.environ["TRACKSTAR_ENABLE_OPENMP"] == "true":
# 				# Some steps duplicated between here and openmp.check_compiler,
# 				# because the environment variable may be set in order to
# 				# enable multithreading without invoking ``setup.py openmp``
# 				# directly.
# 				if "CC" in os.environ.keys():
# 					os.environ["CC"] = openmp.check_compiler(os.environ["CC"])
# 					if os.environ["CC"].startswith("gcc"):
# 						compile_args.append("-fopenmp")
# 						link_args.append("-fopenmp")
# 					elif os.environ["CC"].startswith("clang"):
# 						compile_args.append("-Xpreprocessor")
# 						compile_args.append("-fopenmp")
# 						link_args.append("-Xpreprocessor")
# 						link_args.append("-fopenmp")
# 						link_args.append("-lomp")
# 					else:
# 						raise RuntimeError("""\
# Unix C compiler must be either 'gcc' or 'clang'. Got %s from environment \
# variable \"CC\".""" % (os.environ["CC"]))
# 				else:
# 					# The case arises when the user has set the environment
# 					# variable TRACKSTAR_ENABLE_OPENMP themselves but did not
# 					# run ``setup.py openmp`` or specify a compiler. Assume gcc 
# 					# for linux and clang for Mac OS.
# 					if sys.platform == "linux":
# 						os.environ["CC"] = "gcc"
# 						compile_args.append("-fopenmp")
# 						link_args.append("-fopenmp")
# 					elif sys.platform == "darwin":
# 						os.environ["CC"] = "clang"
# 						compile_args.append("-Xpreprocessor")
# 						compile_args.append("-fopenmp")
# 						link_args.append("-Xpreprocessor")
# 						link_args.append("-fopenmp")
# 						link_args.append("-lomp")
# 					else:
# 						raise OSError("""\
# Sorry, Windows is not supported. Please install and use TrackStar within the \
# Windows Subsystem for Linux.""")


# 				if sys.platform == "darwin" and os.environ["CC"] == "clang":
# 					# Find the OpenMP header files by brute force on Mac OS.
# 					# This is not necessary with gcc, because it comes with the
# 					# OpenMP library out of the box.
# 					openmp.find_openmp_darwin()
# 					for ext in self.extensions:
# 						ext.library_dirs.append(os.environ["LIBOMP_LIBRARY_DIR"])
# 						ext.include_dirs.append(os.environ["LIBOMP_INCLUDE_DIR"])
# 				else: pass

# 			else: pass
# 		else: pass

# 		for ext in self.extensions:
# 			for flag in compile_args: ext.extra_compile_args.append(flag)
# 			for flag in link_args: ext.extra_link_args.append(flag)
# 		super().build_extensions()


# 	def run(self):
# 		# If the user has run ``setup.py openmp``, those commands need to run
# 		# *before* build_ext, otherwise the necessary environment variables
# 		# will not be set and their specifications will not be reflected.
# 		if "openmp" in sys.argv: self.run_command("openmp")
# 		super().run()


# class openmp(Command):

# 	r"""
# 	A ``setuptools`` command that sets the environment variable
# 	``TRACKSTAR_ENABLE_OPENMP`` to "true", which is used by the sub-classed
# 	``build_ext`` class above to link TrackStar with the OpenMP library and
# 	enable parallel processing.

# 	Run ``python setup.py openmp --help`` for more info.
# 	"""

# 	description = """\
# Link TrackStar with the OpenmP library to enable parallel processing."""

# 	user_options = [
# 		("compiler=", "c", """\
# The Unix C compiler to use. Must be either 'gcc' (linux-like) or 'clang' \
# (MacOS-like). If not specified, this script will check for the environment \
# variable "CC", and use its stored value. If no such environment variable is \
# found, the system default will be used. Although setuptools does not \
# differentiate between the two, they require different compiler flags when \
# linking with OpenMP, so this script must differentiate between the two. As \
# with any other compilation process, the environment variable "CC" can be used \
# to specify the desired C compiler even when not running 'setup.py openmp'.""")
# 	]

# 	supported_compilers = set(["gcc", "clang"])

# 	def initialize_options(self):
# 		self.compiler = None


# 	def finalize_options(self):
# 		if self.compiler is not None:
# 			if not self.check_compiler(self.compiler): raise RuntimeError("""\
# Unix C compiler must be either 'gcc' or 'clang'. Got: %s""" % (self.compiler))
# 		elif "CC" in os.environ.keys():
# 			if not self.check_compiler(os.environ["CC"]): raise RuntimeError("""\
# Unix C compiler must be either 'gcc' or 'clang'. Got %s from environment \
# variable \"CC\".""" % (os.environ["CC"]))
# 		else: pass


# 	def run(self):
# 		os.environ["TRACKSTAR_ENABLE_OPENMP"] = "true"
# 		if self.compiler is not None:
# 			os.environ["CC"] = self.check_compiler(self.compiler)
# 		elif "CC" in os.environ.keys():
# 			self.compiler = self.check_compiler(os.environ["CC"])
# 		else:
# 			if sys.platform == "linux":
# 				self.compiler = "gcc"
# 			elif sys.platform == "darwin":
# 				self.compiler = "clang"
# 			else:
# 				raise OSError("""\
# Sorry, Windows is not supported. Please install and use TrackStar within the \
# Windows Subsystem for Linux.""")
# 			os.environ["CC"] = self.compiler


# 	@staticmethod
# 	def check_compiler(compiler):
# 		r"""
# 		Determine which compiler was specified and whether or not it is
# 		supported.

# 		Parameters
# 		----------
# 		compiler : ``str``
# 			The compiler that the user has specified.

# 		Returns
# 		-------
# 		The plain name of the compiler (i.e. the name without any version number
# 		tags explicitly attached; e.g. "gcc-10" or "clang-11"). If the compiler
# 		is not found on the user's ``PATH``, a value of ``None`` will be
# 		returned. If the compiler is outrightly not supported, a value of
# 		``False`` will be returned.

# 		Notes
# 		-----
# 		This test ddetermines whether the specified compiler corresponds to gcc
# 		or clang by using the ``which`` BASH commmand-line tool. It then runs
# 		the compiler with only the ``--version`` flag in order to obtain the
# 		plain version number, which enables those invoked with a version number
# 		attached (e.g. gcc-10, clang-11) to work properly.
# 		"""
# 		kwargs = {
# 			"stdout": PIPE,
# 			"stderr": PIPE,
# 			"shell": True,
# 			"text": True
# 		}

# 		# First check if the system even recognizes the compiler
# 		with Popen("which %s" % (compiler), **kwargs) as proc:
# 			out, err = proc.communicate()
# 			if sys.platform == "linux":
# 				# The error message on Linux's ``which``
# 				if "no %s" % (compiler) in err: return None
# 			elif sys.platform == "darwin":
# 				# On Mac OS, which prints nothing if it's not found
# 				if out == "" and err == "": return None
# 			else:
# 				raise OSError("""\
# Sorry, Windows is not supported. Please install and use TrackStar within the \
# Windows Subsystem for Linux.""")

# 		def is_version_number(word):
# 			r"""
# 			Determines if a string could be interpreted as a standard version
# 			number by determining if it is composed of digits separated by
# 			decimals. Returns the corresponding boolean value.
# 			"""
# 			if '.' in word:
# 				_is_version_number = True
# 				for item in word.split('.'): _is_version_number &= item.isdigit()
# 				return _is_version_number
# 			else:
# 				return False

# 		# Now check if ``compiler --version`` runs properly and has either
# 		# "gcc" or "clang" in the output along with a version number.
# 		with Popen("%s --version" % (compiler), **kwargs) as proc:
# 			out, err = proc.communicate()
# 			# should catch all typos that didn't cause an error above
# 			if err != "" and "command not found" in err: return None
# 			# Should catch anything that isn't a compiler
# 			if err != "" and "illegal" in err: return False
# 			recognized = False
# 			contains_version_number = False
# 			for word in out.split():
# 				for test in openmp.supported_compilers:
# 					# startswith() as opposed to == works with e.g. "gcc-10"
# 					if word.startswith(test):
# 						compiler = word
# 						recognized = True
# 					else: pass
# 					contains_version_number |= is_version_number(word)
# 			if recognized and contains_version_number:
# 				return compiler
# 			else:
# 				return False


# 	@staticmethod
# 	def find_openmp_darwin():
# 		r"""
# 		Determine the path to the OpenMP library, header files, and linker
# 		file on Mac OS using Homebrew.

# 		Notes
# 		-----
# 		This function first runs ``brew`` to determine if Homebrew is installed.
# 		Users who have not done so will be directed accordingly. It then runs
# 		``brew list libomp`` to list the files associated with OpenMP, if any.
# 		If the necessary files are found, their directories are assigned to the
# 		environment variables ``LIBOMP_INCLUDE_DIR`` and ``LIBOMP_LIBRARY_DIR``
# 		to then be included in the call to ``build_extensions``. If the OpenMP
# 		files are not found, then the user is directed to run ``brew install
# 		libomp`` or ``brew reinstall libomp`` before reattempting their
# 		TrackStar installation.
# 		"""
# 		kwargs = {
# 			"stdout": PIPE,
# 			"stderr": PIPE,
# 			"shell": True,
# 			"text": True
# 		}

# 		with Popen("brew", **kwargs) as proc:
# 			out, err = proc.communicate()
# 			if err != "" and "command not found" in err: raise RuntimeError("""\
# It appears that Homebrew is either not installed or not on your PATH. Please \
# install Homebrew by following the instructions at https://brew.sh/ and then \
# install OpenMP by running

# $ brew install libomp

# from your Unix command line before reattempting your TrackStar installation.""")

# 		with Popen("brew list libomp", **kwargs) as proc:
# 			msg = """\
# Homebrew is installed, but the OpenMP header and library files were not found. \
# If you have not installed OpenMP, please run

# $ brew install libomp

# from your Unix command line. If you have installed OpenMP, try reinstalling it \
# by running

# $ brew reinstall libomp

# and then reattempt your TrackStar installation. If this does not solve your \
# issue, then you may need to update to a newer version of Homebrew (see \
# https://brew.sh/). If you continue to have trouble linking TrackStar with \
# OpenMP, then please open an issue at %s.""" % (bugs_url)
# 			out, err = proc.communicate()
# 			if err.startswith("Error:"):
# 				raise RuntimeError(msg)
# 			else:
# 				out = out.split("\n")
# 				if (any(["include" in _ for _ in out]) and
# 					any([_.endswith("libomp.dylib") for _ in out])):
# 					# found header and library files
# 					idx = 0
# 					while not "include" in out[idx]: idx += 1
# 					os.environ["LIBOMP_INCLUDE_DIR"] = os.sep.join(
# 						out[idx].split(os.sep)[:-1])
# 					idx = 0
# 					while not out[idx].endswith("libomp.dylib"): idx += 1
# 					os.environ["LIBOMP_LIBRARY_DIR"] = os.sep.join(
# 						out[idx].split(os.sep)[:-1])
# 				else:
# 					raise RuntimeError(msg)


# def write_version_info(filename = "./trackstar/version_breakdown.py"):
# 	r"""
# 	Writes the current version info to disk within the source tree.

# 	Parameters
# 	----------
# 	filename : ``str`` [default : "./trackstar/version_breakdown.dat"]
# 		The relative path to the file to save version information at.

# 	.. note:: trackstar/version.py depends on the file produced by this
# 		function.

# 	Raises
# 	------
# 	* AssertionError
# 		- Version information is invalid. This occurs if any of the entries
# 			violate the PEP 440 [1]_ standard for version conventions. With
# 			the exception of ``"isreleased"``, all keys of the global variable
# 			``VERSION`` must be either ``None`` or an ``int``, and must be
# 			non-negative if they are an ``int``. Furthermore, Only one of the
# 			pre-, post-, or development-release identifiers may be flagged.

# 	.. [1] https://peps.python.org/pep-0440/
# 	"""
# 	errmsg = "Invalid version information"
# 	for _ in ["major", "minor", "micro"]:
# 		assert isinstance(VERSION[_], int), errmsg
# 		assert VERSION[_] >= 0, errmsg
# 	prerelease = 0
# 	for _ in ["alpha", "beta", "rc"]:
# 		if VERSION[_] is not None:
# 			assert isinstance(VERSION[_], int), errmsg
# 			assert VERSION[_] >= 0, errmsg
# 			prerelease += 1
# 		else: pass
# 	assert prerelease in [0, 1], errmsg
# 	devorpost = 0
# 	for _ in ["dev", "post"]:
# 		if VERSION[_] is not None:
# 			assert isinstance(VERSION[_], int), errmsg
# 			assert VERSION[_] >= 0, errmsg
# 		else: pass
# 	assert devorpost in [0, 1], errmsg
# 	assert prerelease + devorpost < 2, errmsg
# 	assert isinstance(VERSION["isreleased"], bool), errmsg
# 	with open(filename, 'w') as f:
# 		f.write("# This file was generated by TrackStar setup.py\n")
# 		for key in VERSION.keys(): 
# 			if key == "minpython":
# 				f.write("%s = \"%s\"\n" % (key, str(VERSION[key])))
# 			else:
# 				f.write("%s = %s\n" % (key, str(VERSION[key])))
# 		f.close()


# def find_extensions(path = "./trackstar"):
# 	extensions = []
# 	for root, dirs, files in os.walk(path):
# 		for file in files:
# 			if file.endswith(".pyx"):
# 				name = "%s.%s" % (root[2:].replace('/', '.'),
# 					file.split('.')[0])
# 				src_files = ["%s/%s" % (root[2:], file)]
# 				src_files += ["./trackstar/core/src/matrix.c"]
# 				extensions.append(Extension(name, src_files))
# 			else: continue
# 	return extensions


# def find_packages(path = "./trackstar"):
# 	packages = []
# 	for root, dirs, files in os.walk(path):
# 		if "__init__.py" in files: packages.append(root[2:].replace('/', '.'))
# 	return packages


# # --------------------------- CALL TO SETUPTOOLS --------------------------- #
# def setup_package():
# 	r"""
# 	Compile and install.
# 	"""
# 	src_path = os.path.dirname(os.path.abspath(sys.argv[0]))
# 	old_path = os.getcwd()
# 	os.chdir(src_path)
# 	sys.path.insert(0, src_path)

# 	# write version info to construct proper version string for metadata
# 	try:
# 		write_version_info()
# 		from trackstar.version import version
# 	except:
# 		del sys.path[0]
# 		os.chdir(old_path)
# 		raise

# 	# directories with .h header files, req'd by setup
# 	include_dirs = []
# 	for root, dirs, files in os.walk("./trackstar/core/src"):
# 		if any([_.endswith(".h") for _ in files]): include_dirs.append(root)

# 	metadata = dict(
# 		name = package_name,
# 		version = str(version),
# 		author = "James W. Johnson",
# 		author_email = "giganano9@gmail.com",
# 		maintainer = "James W. Johnson",
# 		maintainer_email = "giganano9@gmail.com",
# 		url = repo_url,
# 		project_urls = {
# 			"Bug Tracker": bugs_url,
# 			"Source Code": repo_url
# 		},
# 		classifiers = CLASSIFIERS.split("\n"),
# 		license = "MIT",
# 		platforms = ["Linux", "Mac OS X", "Unix"],
# 		provides = [package_name],
# 		cmdclass = {
# 			"build_ext": build_ext,
# 			"openmp": openmp
# 		},
# 		packages = find_packages(),
# 		ext_modules = find_extensions(),
# 		include_dirs = include_dirs,
# 		setup_requires = [
# 			"setuptools>=18.0", #automatically handles Cython extensions
# 			"Cython>=0.29.0"
# 		],
# 		python_requires=">=3.8,<4",
# 		zip_safe = False,
# 		verbose = "-q" not in sys.argv and "--quiet" not in sys.argv
# 	)

# 	try:
# 		setup(**metadata)
# 	finally:
# 		del sys.path[0]
# 		os.chdir(old_path)
# 	return


# if __name__ == "__main__":
# 	setup_package()
# 	del builtins.__TRACKSTAR_SETUP__

