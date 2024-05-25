#!/usr/bin/env python
#
# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/trackstar.git.

import trackstar
import importlib
import inspect
import os

def discover_benchmarks(path = trackstar.__path__[0]):
	benchmarks = []
	for root, dirs, files in os.walk(path):
		for file in files:
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
				print(str(name))

		
