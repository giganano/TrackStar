#!/usr/bin/env python
#
# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/trackstar.git.

import trackstar
from .benchmark import benchmark
import importlib
import inspect
import os

def discover_benchmarks(path = trackstar.__path__[0]):
	benchmarks = []
	for root, dirs, files in os.walk(path):
		for file in files:
			if root.endswith("/trackstar/benchmarks"): continue
			if ((file.startswith("benchmark_") and file.endswith(".py")) or
				file.endswith("_benchmark.py")):
				dirnames = path.split('/')
				idx = dirnames.index("trackstar")
				package = ".".join(dirnames[idx:])
				relative = root[len(path):]
				module_name = ".%s.%s" % (
					".".join(relative.split('/')[1:]),
					file[:-3])
				mod = importlib.import_module(module_name, package = package)
			else:
				continue
			for name, data in inspect.getmembers(mod):
				# don't append the imported benchmark class
				if data == benchmark: continue
				if inspect.isclass(data) and issubclass(data, benchmark):
					benchmarks.append(data)
				elif isinstance(data, benchmark):
					benchmarks.append(data)
				elif (name.startswith("benchmark_") or
					name.endswith("_benchmark.py")):
					benchmarks.append(data)
				else: pass
				if file == "benchmark_matrix.py":
					print(name)
					print(data)
				else: pass
	print(benchmarks)


		
