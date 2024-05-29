#!/usr/bin/env python
#
# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/trackstar.git.

__all__ = ["benchmark"]
import timeit
import functools
import numpy as np
DEFAULT_REPEAT = 100


class benchmark:

	r"""
	Provides two different functionalities simultaneously:

		- Can be subclassed, and every call to a function that starts with
		"benchmark_" or ends with "_benchmark" will automatically be turned
		into a ``timer`` object.

		- Can be used as a decorator that accepts the keyword args "setup"
		and "repeat", which both get passed to timeit.repeat.
	"""

	def __init__(self, *args, setup = "pass", repeat = DEFAULT_REPEAT):
		if len(args) == 1:
			self.func = args[0]
		elif len(args) == 0:
			self.func = None
		else:
			raise TypeError("""\
benchmark.__init__() takes at most 1 position argument, but %d were \
given.""" % (len(args)))
		self.setup = setup
		self.repeat = repeat

	def __call__(self, *args, **kwargs):
		if self.func is None:
			func = args[0]
			def wrapper(*func_args, **func_kwargs):
				return timer(func)(*func_args,
					setup = self.setup,
					repeat = self.repeat,
					**func_kwargs)
			return wrapper
		else:
			return timer(self.func)(*args,
				setup = self.setup,
				repeat = self.repeat,
				**kwargs)

	def __getattribute__(self, name):
		attr = super().__getattribute__(name)
		if callable(attr) and (name.startswith("benchmark_") or 
			name.endswith("_benchmark")):
			return timer(attr)
		else:
			return attr


class timer:

	def __init__(self, func):
		self.func = func


	def __call__(self, *args, setup = "pass", repeat = DEFAULT_REPEAT, **kwargs):
		results = timeit.repeat(functools.partial(self.func, *args, **kwargs),
			setup = setup, repeat = repeat + 1, number = 1)[1:]
		summary = {
			"func": self.func,
			"mean": np.mean(results),
			"median": np.median(results),
			"std": np.std(results),
			"fastest": min(results),
			"slowest": max(results),
			"n_iters": repeat
		}
		return summary

