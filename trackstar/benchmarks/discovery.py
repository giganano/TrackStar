#!/usr/bin/env python
#
# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/trackstar.git.

import trackstar
from .benchmark import benchmark_function, benchmark_class
import importlib
import inspect
import sys
import os

def discover_benchmarks(path = trackstar.__path__[0]):
	for root, dirs, files in os.walk(path):
		for name in files:
			if root.endswith("/trackstar/benchmarks"): continue
			if file.tagged_as_benchmark(name): file("%s/%s" % (root, name)).run()


class file:

	def __init__(self, name):
		self._benchmarks = self.parse_for_benchmarks(name)
		name = name.split(os.sep)
		self._name = os.sep.join(name[name.index("trackstar"):])


	def run(self):
		for bench in self._benchmarks:
			if isinstance(bench, benchmark_class):
				if bench.args is not None:
					for args in bench.args:
						instance = bench(*args)
						for name, value in inspect.getmembers(instance):
							if callable(value) and not name.startswith("_"):
								if value.args is not None:
									for runtime_args in value.args:
										sys.stdout.write("%s::" % (self.name))
										value.print(*runtime_args)
								else:
									sys.stdout.write("%s::" % (self.name))
									value.print()
							else: pass
				else:
					instance = bench()
					for name, value in inspect.getmembers(instance):
						if callable(value) and not name.startswith("_"):
							if value.args is not None:
								for runtime_args in value.args:
									sys.stdout.write("%s::" % (self.name))
									value.print(*runtime_args)
							else:
								sys.stdout.write("%s::" % (self.name))
								value.print()
						else: pass
			elif isinstance(bench, benchmark_function):
				if bench.args is not None:
					for runtime_args in bench.args:
						sys.stdout.write("%s::" % (self.name))
						bench.print(*runtime_args)
				else:
					sys.stdout.write("%s::" % (self.name))
					bench.print()
			else:
				raise TypeError("""\
Expected an object of type benchmark_class or benchmark_function. \
Got: %s""" % (type(bench)))


	@property
	def benchmarks(self):
		return self._benchmarks


	@property
	def name(self):
		return self._name


	@staticmethod
	def parse_for_benchmarks(name):
		r"""
		Reads a file and returns everything inside of it that has the
		@benchmark decorator on top.
		"""
		if os.path.exists(name):
			if file.tagged_as_benchmark(name):
				benchmarks = []
				name = name[:-3] # chop off .py extension
				fullpath = os.path.abspath(name)
				dirnames = fullpath.split('/')
				idx = dirnames.index("trackstar")
				name = '.' + '.'.join(dirnames[idx + 1:])
				mod = importlib.import_module(name, package = "trackstar")
				for name, data in inspect.getmembers(mod):
					if getattr(data, "decorated_as_benchmark", False):
						benchmarks.append(data)
					else: pass
				return benchmarks
			else:
				raise ValueError("File not tagged for benchmarking: %s" % (name))
		else:
			raise FileNotFoundError("File does not exist: %s" % (name))


	@staticmethod
	def tagged_as_benchmark(name):
		r"""
		Checks to see if a function or class is tagged for benchmarking by
		seeing if its name begins with "benchmark_" or ends with "_benchmark".
		"""
		if isinstance(name, str):
			result = name.endswith(".py")
			name = name[:-3].split(os.sep)[-1]
			result &= (name.startswith("benchmark_") or
				name.endswith("_benchmark"))
			return result
		else:
			raise TypeError("Expected a string. Got: %s" % (name))

