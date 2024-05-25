#!/usr/bin/env python
#
# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/trackstar.git.

__all__ = ["benchmark"]
from timeit import repeat
import numpy as np
DEFAULT_REPEAT = 100

def benchmark(*bench_args,
	setup = "pass",
	repeat = DEFAULT_REPEAT):
	def benchmark_decorator(func):
		def wrapper(*func_args, **func_kwargs):
			callables = []
			if len(bench_args) == 1:
				args = bench_args[0]
				if isinstance(args, list):
					for arg in args:
						if isinstance(arg, tuple):
							result = func(*arg, *func_args, **func_kwargs)
						else:
							result = func(arg, *func_args, **func_kwargs)
						if callable(result):
							callables.append(result)
						else:
							raise RuntimeError("""\
Can only benchmark callable objects. Got: %s""" % (type(result)))
				else:
					raise RuntimeError("""\
Benchmark arguments must be of type list. Got: %s""" % (type(args)))
			elif len(args) == 0:
				result = func(*func_args, **func_kwargs)
				if callable(result):
					callables.append(result)
				else:
					raise RuntimeError("""\
Can only benchmark callable objects. Got: %s""" % (type(result)))
			else:
				raise RuntimeError("""\
Expected only one set of parameters for benchmarking. Got: %d""" % (len(args)))
			return [timer(c, setup = setup, repeat = repeat) for c in callables]
		return wrapper
	return benchmark_decorator


class timer:

	def __init__(self, func, setup = "pass", repeat = DEFAULT_REPEAT):
		self.func = func
		self.setup = setup
		self.repeat = repeat

	def __repr__(self):
		summary = self.time()
		return "%.2e +/- %.2e seconds (%d iterations)" % (
			summary["mean"], summary["std"], summary["n_iters"])

	def time(self):
		results = repeat(self.func, setup = self.setup,
			repeat = self.repeat + 1, number = 1)[1:]
		summary = {
			"func": self.func,
			"mean": np.mean(results),
			"median": np.median(results),
			"std": np.std(results),
			"fastest": min(results),
			"slowest": max(results),
			"n_iters": self.repeat
		}
		return summary

