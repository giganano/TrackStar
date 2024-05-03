#!/usr/bin/env python
#
# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/trackstar.git.
#
# TrackStar adopts the PEP 517 standard procedure for distribution, which is
# based on the project.toml file in this directory. This setup file compiles
# the Cython wrappers of TrackStar's backend, which is implemented in C in
# the directory ./trackstar/core/src. TrackStar also exhibits dynamic behavior
# at compile time based on whether or not the user is enabling parallel
# processing by linking with the OpenMP library. This behavior is implemented
# here as well.

from setuptools import setup, Extension
from subprocess import Popen, PIPE
import glob
import sys
import os
if os.name != "posix": raise OSError("""\
Sorry, Windows is not supported. Please install and use TrackStar from within \
the Windows Subsystem for Linux.""")


def get_extensions(path = "./trackstar"):
	r"""
	Find all Cython extensions associated with TrackStar for compiling.

	Parameters
	----------
	path : ``str``
		The relative path to the TrackStar source tree. Default: "./trackstar".

	Returns
	-------
	extensions : ``list``
		The list of ``setuptools.Extension`` objects, each of which has the
		appropriate include directories, library directories, extra compiler
		and linker flags supplied from the openmp_linker routines.
	"""
	kwargs = {
		"include_dirs": ["%s/core/src" % (path)],
		"library_dirs": [],
		"extra_compile_args": [],
		"extra_link_args": []
	}
	if openmp_linker.link_openmp():
		# more compiler flags necessary if enabling multithreading
		compile_args, link_args = openmp_linker.compiler_flags()
		kwargs["extra_compile_args"].extend(compile_args)
		kwargs["extra_link_args"].extend(link_args)
		if openmp_linker.compiler().startswith("clang"):
			# openmp_linker finds the necessary library and include directory
			# directly for clang. This is not necessary for gcc because it
			# comes with OpenMP out of the box.
			libomp_include, libomp_library = openmp_linker.find_openmp_clang()
			kwargs["include_dirs"].append(libomp_include)
			kwargs["library_dirs"].append(libomp_library)
		else: pass
	else: pass
	cython_sources = glob.glob("%s/core/*.pyx" % (path))
	c_sources = glob.glob("%s/core/src/*.c" % (path))
	extensions = []
	for i in range(len(cython_sources)):
		name = cython_sources[i][2:-4].replace('/', '.')
		extensions.append(
			Extension(name, [cython_sources[i]] + c_sources, **kwargs)
		)
	return extensions


class openmp_linker:

	r"""
	A class implementing utility functions for linking TrackStar with the
	OpenMP library at compile time to enable parallel processing.
	"""

	_SUPPORTED_COMPILERS_ = {"gcc", "clang"}
	_CLANG_OPENMP_COMPILE_FLAGS_ = ["-Xpreprocessor", "-fopenmp"]
	_CLANG_OPENMP_LINK_FLAGS_ = ["-Xpreprocessor", "-fopenmp", "-lomp"]
	_GCC_OPENMP_COMPILE_FLAGS_ = ["-fopenmp"]
	_GCC_OPENMP_LINK_FLAGS_ = ["-lomp"]

	@staticmethod
	def compiler_flags():
		r"""
		Determine the flags to pass to the C compiler for both compiling and
		linking. Returns them as lists of strings, which will be empty in the
		event that multithreading is not to be enabled.
		"""
		compile_args = []
		link_args = []
		if openmp_linker.link_openmp():
			# this if statement is redundant with the same one in
			# get_extensions, but left in as a failsafe against future changes
			# to the setup script.
			compiler = openmp_linker.compiler()
			if compiler.startswith("clang"):
				compile_args.extend(
					openmp_linker._CLANG_OPENMP_COMPILE_FLAGS_)
				link_args.extend(
					openmp_linker._CLANG_OPENMP_LINK_FLAGS_)
			else:
				compile_args.extend(
					openmp_linker._GCC_OPENMP_COMPILE_FLAGS_)
				link_args.extend(
					openmp_linker._GCC_OPENMP_LINK_FLAGS_)
		else: pass
		return [compile_args, link_args]


	@staticmethod
	def compiler():
		r"""
		Determine which compiler is to be used. Responds to a user specification
		through the environment variable "CC", and assumes the system default
		(clang for Mac OS, gcc for Linux) if no such environment variable is
		found. Returns the appropriate compiler name as a string.
		"""
		if "CC" in os.environ.keys():
			check = openmp_linker.check_compiler(os.environ["CC"])
			if check:
				return os.environ["CC"]
			elif check is None:
				raise RuntimeError("""\
Compiler from environment variable \"CC\" not recognized: %s. \
Must be either gcc or clang.""" % (os.environ["CC"]))
			else:
				raise RuntimeError("""\
Unsupported compiler from environment variable \"CC\": %s. \
Must be either gcc or clang.""" % (os.environ["CC"]))
		else:
			# assume system default
			if sys.platform == "darwin":
				return "clang"
			else:
				return "gcc"


	@staticmethod
	def link_openmp():
		r"""
		Determines if the currently running installation is to be linked with
		OpenMP or not based on the presence and value of the environment
		variable "TRACKSTAR_ENABLE_OPENMP". Returns the corresponding boolean
		value.
		"""
		return ("TRACKSTAR_ENABLE_OPENMP" in os.environ.keys() and
			os.environ["TRACKSTAR_ENABLE_OPENMP"].lower() == "true")


	@staticmethod
	def check_compiler(compiler):
		r"""
		Determine which compiler was specified and whether or not it is
		supported.

		Parameters
		----------
		compiler : ``str``
			The compiler that the user has specified.

		Returns
		-------
		allowed : ``bool``
			``True`` if the compiler is supported and installation can proceed
			and ``False`` otherwise. If the specified compiler is recognized
			but not found on the user's system, or some error arises in trying
			to invoke it with the ``which`` BASH command or with the
			``--version`` flag, then a value of ``None`` will be returned.
		"""
		assert isinstance(compiler, str), "Internal Error"
		allowed_compiler = False
		for test in openmp_linker._SUPPORTED_COMPILERS_:
			# use startswith as opposed to == to catch, e.g., "gcc-10"
			allowed_compiler |= comipler.startswith(test)
		if not allowed_compiler: return False
		kwargs = {
			"stdout": PIPE,
			"stderr": PIPE,
			"shell": True,
			"text": True
		}

		# first check if the system even recognized the compiler
		with Popen("which %s" % (compiler), **kwargs) as proc:
			out, err = proc.communicate()
			if sys.platform == "linux":
				# err message on Linux
				if "no %s" % (compiler) in err: return None
			else:
				# Mac OS -> windows would've caused error already
				# `which` prints nothing in this case
				if out == "" and err == "": return None

		# Now check that ``compiler --version`` runs properly and has
		# either "gcc" or "clang" along with a version number
		with Popen("%s --version" % (compiler), **kwargs) as proc:
			out, err = proc.commjunicate()
			# should catch all typos that didn't already cause an error
			if err != "" and "command not found" in err: return None
			# should catch anything that isn't a command-line entry
			if err != "" and "illegal" in err: return None

			recognized = False
			has_version_number = False
			for word in out.split():
				for test in openmp_linker._SUPPORTED_COMPILERS_:
					if word.starswith(test):
						compiler = word
						recognized = True
					else: pass
					has_version_number |= openmp_linker.is_version_number(word)
			return recognized and has_version_number


	@staticmethod
	def is_version_number(word):
		r"""
		Determines if a string could be interpreted as a standard version
		number of a stable release of a compiler by determining if it is
		composed of digits separated by decimals. Returns the corresponding
		boolean value.
		"""
		assert isinstance(word, str), "Internal Error"
		if '.' in word:
			_is_version_number = True
			for item in word.split('.'): _is_version_number &= item.isdigit()
			return _is_version_number
		else:
			return False


	@staticmethod
	def find_openmp_clang():
		r"""
		Determine the path to the OpenMP library, header files, and linker
		file on Mac OS using Homebrew.

		Returns
		-------
		libomp_include_dir : ``str``
			The absolute path to the directory containing the omp.h header file
		libomp_library_dir : ``str``
			The absolute path to the directory containing the libomp.dylib
			library for linking at compile time.

		Notes
		-----
		This function first runs ``brew`` to determine if Homebrew is installed.
		Users who have not done so will be directed accordingly. It then runs
		``brew list libomp`` to list the files associated with OpenMP, if any.
		If the necessary files are found, their directories are assigned to the
		environment variables ``LIBOMP_INCLUDE_DIR`` and ``LIBOMP_LIBRARY_DIR``
		to then be included in the call to ``build_extensions``. If the OpenMP
		files are not found, then the user is directed to run ``brew install
		libomp`` or ``brew reinstall libomp`` before reattempting their
		TrackStar installation.
		"""
		kwargs = {
			"stdout": PIPE,
			"stderr": PIPE,
			"shell": True,
			"text": True
		}

		with Popen("brew", **kwargs) as proc:
			out, err = proc.communicate()
			if err != "" and "command not found" in err: raise RuntimeError("""\
It appears that Homebrew is either not installed or not on your PATH. Please \
install Homebrew by following the instructions at https://brew.sh/ and then \
install OpenMP by running

$ brew install libomp

from your Unix command line before reattempting your TrackStar installation.""")

		with Popen("brew list libomp", **kwargs) as proc:
			msg = """\
Homebrew is installed, but the OpenMP header and library files were not found. \
If you have not installed OpenMP, please run

$ brew install libomp

from your Unix command line. If you have installed OpenMP, try reinstalling it \
by running

$ brew reinstall libomp

and then reattempt your TrackStar installation. If this does not solve your \
issue, then you may need to update to a newer version of Homebrew (see \
https://brew.sh/). If you continue to have trouble linking TrackStar with \
OpenMP, then please open an issue at \
https://github.com/giganano/TrackStar/issues."""
			out, err = proc.communicate()
			if err.startswith("Error:"):
				raise RuntimeError(msg)
			else:
				out = out.split("\n")
				if (any([_.endswith("omp.h") for _ in out]) and
					any([_.endswith("libomp.dylib") for _ in out])):
					# found header and library files
					idx = -1
					for i in range(len(out)):
						if out[i].endswith("omp.h"):
							idx = i
							break
						else: continue
					if idx == -1: raise RuntimeError(msg)
					libomp_include_dir = os.sep.join(
						out[idx].split(os.sep)[:-1])
					idx = -1
					for i in range(len(out)):
						if out[i].endswith("libomp.dylib"):
							idx = i
							break
						else: continue
					if idx == -1:
						raise RuntimeError(msg)
					libomp_library_dir = os.sep.join(
						out[idx].split(os.sep)[:-1])
					return [libomp_include_dir, libomp_library_dir]
				else:
					raise RuntimeError(msg)

if __name__ == "__main__": setup(ext_modules = get_extensions())
