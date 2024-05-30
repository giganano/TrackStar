#!/usr/bin/env python
#
# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/trackstar.git.

__all__ = ["benchmark"]
import timeit
import inspect
import functools
import numpy as np
import types
import sys
DEFAULT_REPEAT = 100


def benchmark(*item, args = None, **timer_kwargs):
	r"""
	The @benchmark decorator.
	"""
	if args is not None:
		if isinstance(args, list) or isinstance(args, tuple):
			for elem in args:
				if not isinstance(elem, list) and not isinstance(elem, tuple):
					raise TypeError("""\
Each element of the keyword arg \'args\' must be of type list or tuple. \
Got: %s""" % (type(elem)))
			args = [list(elem) for elem in args]
		else:
			raise TypeError("""\
Keyword arg \'args\' must be of type list, tuple, or NoneType. Got: %s""" % (
				type(args)))
	else: pass

	def benchmark_decorator(obj):
		if inspect.isclass(obj):
			# decorate a class
			result = benchmark_class(obj, **timer_kwargs)
		else:
			# decorate a function
			if '.' in obj.__qualname__:
				# it's an instance method
				result = obj
				result.timer_kwargs = timer_kwargs.copy()
			else:
				# it's a def statement
				result = benchmark_function(obj, **timer_kwargs)
		result.args = args
		result.decorated_as_benchmark = True
		return result

	if len(timer_kwargs.keys()):
		if len(item): raise TypeError("""\
@benchmark does not take any positional arguments, but %d were given.""" % (
			len(item)))
		return benchmark_decorator

	else:

		if len(item) == 1:
			return benchmark_decorator(item[0])
		else:
			raise TypeError("""\
@benchmark does not take any positional arguments, but %d were given.""" % (
				len(item)))


class wrapped_callable:

	def __init__(self, obj):
		self.obj = obj

	def __call__(self, *args, **kwargs):
		return self.obj.__call__(*args, **kwargs)

	@property
	def obj(self):
		return self._obj

	@obj.setter
	def obj(self, value):
		if callable(value):
			self._obj = value
		else:
			raise TypeError("Expected a callable object. Got: %s" % (
				type(value)))


class wrapped_method(wrapped_callable):

	def __call__(self, *args, **kwargs):
		return self.obj.__call__(*args, **kwargs)

	@property
	def obj(self):
		return self._obj

	@obj.setter
	def obj(self, value):
		if isinstance(value, types.MethodType):
			self._obj = value
		else:
			raise TypeError("Expected a bound method. Got: %s" % (
				type(value)))


class wrapped_object:

	def __init__(self, cls, *init_args, setup = "pass", repeat = DEFAULT_REPEAT,
		**init_kwargs):
		if inspect.isclass(cls):
			self._obj = cls(*init_args, **init_kwargs)
		else:
			raise TypeError("Expected a class. Got: %s" % (type(cls)))
		for name, value in inspect.getmembers(self._obj):
			if callable(value) and not name.startswith("_"):
				timer_kwargs = {
					"setup": setup,
					"repeat": repeat
				}
				if getattr(value, "decorated_as_benchmark", False):
					for key in value.timer_kwargs.keys():
						timer_kwargs[key] = value.timer_kwargs[key]
				setattr(self, name, benchmark_method(value, **timer_kwargs))
				setattr(getattr(self, name), "_init_args", init_args)
				setattr(getattr(self, name), "_init_kwargs", init_kwargs)
				if hasattr(value, "args"):
					setattr(getattr(self, name), "args", getattr(value, "args"))
				else:
					setattr(getattr(self, name), "args", None)
			else: pass

	@property
	def obj(self):
		return self._obj


class wrapped_class:

	def __init__(self, cls, **timer_kwargs):
		if inspect.isclass(cls):
			self._cls = cls
		else:
			raise TypeError("Expected a class. Got: %s" % (type(cls)))
		self._timer_kwargs = timer_kwargs.copy()

	def __call__(self, *args, **kwargs):
		return wrapped_object(self.cls, *args, **kwargs, **self._timer_kwargs)

	@property
	def cls(self):
		return self._cls


class timer:

	def __init__(self, setup = "pass", repeat = DEFAULT_REPEAT):
		self.setup = setup
		self.repeat = repeat

	def time(self, obj, *args, **kwargs):
		if callable(obj):
			results = timeit.repeat(
				functools.partial(obj, *args, **kwargs),
				setup = self.setup,
				repeat = self.repeat + 1,
				number = 1)[1:]
			return {
				"callable": obj,
				"mean": np.mean(results),
				"median": np.median(results),
				"std": np.std(results),
				"fastest": min(results),
				"slowest": max(results),
				"n_iters": self.repeat
			}
		else:
			raise TypeError("Expected a callable object. Got: %s" % (type(obj)))

	@property
	def setup(self):
		return self._setup

	@setup.setter
	def setup(self, value):
		if isinstance(value, str):
			self._setup = value
		else:
			raise TypeError("Expected a string. Got: %s" % (type(value)))

	@property
	def repeat(self):
		return self._repeat

	@repeat.setter
	def repeat(self, value):
		if isinstance(value, int):
			if value > 0:
				self._repeat = value
			else:
				raise ValueError("Expected a positive value. Got: %d" % (value))
		else:
			raise ValueError("Expected an int. Got: %s" % (type(value)))



class benchmark_function(wrapped_callable, timer):

	def __init__(self, obj, **kwargs):
		wrapped_callable.__init__(self, obj)
		timer.__init__(self, **kwargs)

	def __call__(self, *args, **kwargs):
		return timer.time(self, self.obj, *args, **kwargs)

	def print(self, *args, **kwargs):
		results = self.__call__(*args, **kwargs)
		rep = "\033[1m%s(%s)\033[0m" % (
			self.obj.__qualname__,
			self.format_args(*args, **kwargs))
		rep += "\n    "
		rep += "%.2e +/- %.2e seconds" % (results["mean"], results["std"])
		rep += " (mean and standard deviation of %d runs)" % (results["n_iters"])
		sys.stdout.write("%s\n" % (rep))

	@staticmethod
	def format_args(*args, **kwargs):
		rep = ""
		for i in range(len(args)):
			if i: rep += ", "
			rep += "%s" % (str(args[i]))
		keys = kwargs.keys()
		for i in range(len(keys)):
			if i or len(args): rep += ", "
			rep += "%s = %s" % (key, str(kwargs[key]))
		rep += ""
		return rep


class benchmark_method(wrapped_method, timer):

	def __init__(self, obj, **kwargs):
		wrapped_method.__init__(self, obj)
		timer.__init__(self, **kwargs)

	def __call__(self, *args, **kwargs):
		return timer.time(self, self.obj, *args, **kwargs)

	def print(self, *args, **kwargs):
		results = self.__call__(*args, **kwargs)
		rep = "\033[1m%s(%s).%s(%s)\033[0m" % (
			self.obj.__qualname__.split('.')[0],
			benchmark_function.format_args(
				*self._init_args, **self._init_kwargs),
			self.obj.__qualname__.split('.')[1],
			benchmark_function.format_args(*args, **kwargs))
		rep += "\n    "
		rep += "%.2e +/- %.2e seconds" % (results["mean"], results["std"])
		rep += " (mean and standard deviation of %d runs)" % (results["n_iters"])
		sys.stdout.write("%s\n" % (rep))


class benchmark_class(wrapped_class, timer):

	def __init__(self, cls, **timer_kwargs):
		wrapped_class.__init__(self, cls, **timer_kwargs)
		timer.__init__(self, **timer_kwargs)
		for name, value in inspect.getmembers(cls):
			value = getattr(cls, name)
			if callable(value) and not name.startswith("_"):
				setattr(self, name,
					benchmark_function(value, **timer_kwargs))
			else: pass

