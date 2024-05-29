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
DEFAULT_REPEAT = 100


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
			self._instance = cls(*init_args, **init_kwargs)
		else:
			raise TypeError("Expected a class. Got: %s" % (type(cls)))
		for name in list(self._instance.__dict__.keys()):
			if name.startswith("benchmark_") or name.endswith("_benchmark"):
				attr = getattr(self._instance, name)
				if callable(attr): setattr(self, name,
					benchmark_method(attr, setup = setup, repeat = repeat))
				else: pass
			else: pass


class wrapped_class:

	def __init__(self, cls, **timer_kwargs):
		if inspect.isclass(cls):
			self._cls = cls
		else:
			raise TypeError("Expected a class. Got: %s" % (type(cls)))
		self._timer_kwargs = timer_kwargs.copy()

	def __call__(self, *args, **kwargs):
		return wrapped_object(*args, **kwargs, **self._timer_kwargs)

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
		print("Hello!")
		return timer.time(self, self.obj, *args, **kwargs)


class benchmark_method(wrapped_method, timer):

	def __init__(self, obj, **kwargs):
		wrapped_method.__init__(self, obj)
		timer.__init__(self, **kwargs)

	def __call__(self, *args, **kwargs):
		print("Hello2!")
		return timer.time(self, self.obj, *args, **kwargs)


class benchmark_class(wrapped_class, timer):

	def __init__(self, cls, **timer_kwargs):
		wrapped_class.__init__(self, cls, **timer_kwargs)
		timer.__init__(self, **timer_kwargs)

	def __call__(self, *args, **kwargs):
		return self.cls(*args, **kwargs)


class benchmark:

	def __init__(self, *args, **kwargs):
		if len(args) == 1:
			if len(kwargs.keys()): raise TypeError("""\
benchmark.__init__() takes 0 positional arguments when keyword arguments \
are supplied, but %d were given.""" % (len(args)))
			if inspect.isclass(args[0]):
				self._obj = benchmark_class(args[0], **kwargs)
			else:
				self._obj = benchmark_function(args[0], **kwargs)
		elif len(args) == 0:
			self._obj = None
			self._timer_kwargs = kwargs.copy()
		else:
			raise TypeError("""\
benchmark.__init__() takes at most 1 positional argument, but %d were \
given.""" % (len(args)))


	def __call__(self, *args, **kwargs):
		if self._obj is None:
			# This will only be the case if keyword args were passed to the
			# decorator
			if inspect.isclass(args[0]):
				self._obj = benchmark_class(args[0], **self._timer_kwargs)
				def wrapper(*init_args, **init_kwargs):
					return self._obj(*init_args, **init_kwargs)
				return wrapper
			else:
				self._obj = benchmark_function(args[0], **self._timer_kwargs)
				def wrapper(*runtime_args, **runtime_kwargs):
					return self._obj(*runtime_args, **runtime_kwargs)
			return wrapper
		elif isinstance(self._obj, benchmark_function):
			return self._obj(*args, **kwargs)
		elif isinstance(self._obj, benchmark_class):
			return self._obj(*args, **kwargs)

