#!/usr/bin/env python
#
# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/TrackStar.git.

__all__ = ["benchmark"]
import timeit
import inspect
import numbers
import functools
import numpy as np
import types
import sys
DEFAULT_REPEAT = 100


def benchmark(*item, args = None, tolerance = None, **timer_kwargs):
	r"""
	This decorator is the workhorse for benchmarking in TrackStar. It can go
	atop any function or class (see parameter description below). Add the
	following line to your code to import it:

	.. code-block:: python

		from trackstar.benchmarks import benchmark

	The simplest use case is to place it atop some function to be timed:

	.. code-block:: python

		@benchmark
		def somefunction():
			# function body to be timed

	For functions that take arguments, values can be given to ``@benchmark``
	with the keyword arg ``args`` according to

	.. code-block:: python

		@benchmark(args = [[2], [3], [4], [5]])
		def somefunction(x):
			return x**2 # or perhaps something more interesting?

	The values of ``args`` are used *only* when running the full benchmark
	suite with ``python -m trackstar.benchmarks``. It is still possible to
	run the above function with

	>>> somefunction(6)
	>>> somefunction(12)

	even though ``6``, and ``12`` do not exist as entries in ``args``.

	Functions that take multiple arguments should have the appropriate number
	of values placed in each element of ``args``. For example:

	.. code-block:: python

		@benchmark(args = [[2, 3], [3, 4], [4, 5]])
		def somefunction(x, y):
			return x * y

	When calling any of these functions, TrackStar will repeat the call 100
	times by default and report the mean integration time. To change the number
	of calls, simply give ``@benchmark`` the keyword arg ``repeat``:

	.. code-block:: python

		@benchmark(
			args = [[2, 3], [3, 4], [4, 5]],
			repeat = 1000)
		def somefunction(x, y):
			return x * y

	It is also good practice to specify some tolerance for the mean integration
	time. ``@benchmark`` accepts this value in seconds as another keyword
	argument. For example:

	.. code-block:: python

		@benchmark(
			args = [[2, 3], [3, 4], [4, 5]],
			repeat = 1000,
			tolerance = 1e-6)
		def somefunction(x, y):
			return x * y

	When running the full suite of benchmarks with ``python -m
	trackstar.benchmarks``, values that pass the imposed tolerance will print
	their integration time in green. Those that fail will print in red.

	Code to run prior to timing the function body can also be specified as a
	keyword argument to ``@benchmark``. For example:

	.. code-block:: python

		@benchmark(
			args = [[2], [3], [4]],
			repeat = 1000,
			tolerance = 1e-6,
			setup = "y = 3")
		def somefunction(x):
			return x * y # y will be initialized by ``setup`` to a value of 3

	The ``@benchmark`` decorator can also be placed atop a ``class`` to time
	a whole suite of object methods. For example:

	.. code-block:: python

		@benchmark(
			args = [[2, 3], [3, 4], [4, 5]],
			repeat = 1000,
			tolerance = 1e-6)
		class mytimer:

			def __init__(self, x, y):
				self.x = x
				self.y = y

			def somefunction(self):
				return self.x * self.y

			def some_other_function(self):
				return 2 * self.x * self.y

	In these cases, the parameters of ``args`` will be used to call the
	``__init__`` function of the decorated class. Both ``somefunction`` and
	``some_other_function`` will adopt the benchmarking parameters passed to
	``mytimer``, with the exception of ``args``. The values can be overridden
	by placing an additional ``@benchmark`` decorator on the individual
	functions:

	.. code-block:: python

		@benchmark(
			args = [[2, 3], [3, 4], [4, 5]],
			repeat = 1000,
			tolerance = 1e-6)
		class mytimer:

			def __init__(self, x, y):
				self.x = x
				self.y = y

			@benchmark(tolerance = 1e-3)
			def somefunction(self):
				return self.x * self.y

			@benchmark(repeat = 100)
			def some_other_function(self):
				return 2 * self.x * self.y

	Object methods that take additional parameters can take the keyword argument
	``args`` according to:

	.. code-block:: python

		@benchmark(
			args = [[2, 3], [3, 4], [4, 5]],
			repeat = 1000,
			tolerance = 1e-6)
		class mytimer:

			def __init__(self, x, y):
				self.x = x
				self.y = y

			@benchmark(tolerance = 1e-3)
			def somefunction(self):
				return self.x * self.y

			@benchmark(
				args = [[2], [3], [4], [5]]
				repeat = 100)
			def some_other_function(self, a):
				return a * self.x

	In this case, the values ``2``, ``3``, ``4``, and ``5`` will be passed for
	the parameter ``a`` taken by ``some_other_function``.

	The ``@benchmark`` decorator will automatically put a timer on all functions
	associated with the class, except for those whose names are preceded by an
	underscore. For example:

	.. code-block:: python

		@benchmark(
			args = [[2, 3], [3, 4], [4, 5]],
			repeat = 1000,
			tolerance = 1e-6)
		class mytimer:

			def __init__(self, x, y):
				self.x = x
				self.y = y

			@benchmark(tolerance = 1e-3)
			def somefunction(self):
				return self.x * self.y

			@benchmark(
				args = [[2], [3], [4], [5]]
				repeat = 100)
			def some_other_function(self, a):
				return a * self.x

			def _skipped(self):
				# do something here, it won't be timed

	The function ``_skipped`` will not be benchmarked and will instead behave
	as an otherwise normally declared function.

	Parameters
	----------
	\*item : ``callable`` or ``class``
		At most one callable object or class. An object appears in this
		parameter only when the bare ``@benchmark`` decorator is placed atop
		a function or class with no keyword arguments.
	args : ``list``, ``tuple``, or ``None``
		Parameters to pass to the function when running benchmarks automatically
		with ``python -m trackstar.benchmarks``. Must be 2-d, with each set of
		arguments placed in their own ``list`` or ``tuple`` as individual
		elements.
	tolerance : real number or ``None``
		A maximum allowed integration time. When running benchmarks
		automatically with ``python -m trackstar.benchmarks``, those that pass
		this constraint will show the integration time in green, and those
		that fail will print in red.
	timer_kwargs : varying types
		Additional keyword arguments to pass to Python's ``timeit.repeat``
		function:

		+------------+-----------------------------------------------------+
		| ``repeat`` | The number of times to call the function            |
		+------------+-----------------------------------------------------+
		| ``setup``  | A string containing code to run prior to timing the |
		|            | function itself.                                    |
		+------------+-----------------------------------------------------+

	Return Values
	-------------
	summary : ``dict``
		A summary of the integration times required to call the function
		``repeat`` number of times. All functions and methods decorated with
		``@benchmark`` will return a similar dictionary, regardless of the
		benchmarking parameters.

		+------------+-----------------------------------------------------+
		| "callable" | A string representation of the function or object   |
		|            | that was called.                                    |
		+------------+-----------------------------------------------------+
		| "mean"     | The arithmetic mean of all integration times.       |
		+------------+-----------------------------------------------------+
		| "median"   | The median integration time.                        |
		+------------+-----------------------------------------------------+
		| "std"      | The standard deviation of the integration times.    |
		+------------+-----------------------------------------------------+
		| "fastest"  | The lowest integration time achieved.               |
		+------------+-----------------------------------------------------+
		| "slowest"  | The highest integration time achieved.              |
		+------------+-----------------------------------------------------+
		| "n_iters"  | The number of repeated calls to the functoin (i.e.  |
		|            | the same value as the keyword arg ``repeat``).      |
		+------------+-----------------------------------------------------+
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

	if tolerance is not None:
		if not isinstance(tolerance, numbers.Number):
			raise TypeError("""\
Keyword arg \'tolerance\' must be NoneType or a real number. Got: %s""" % (
				type(tolerance)))
		elif tolerance <= 0:
			raise ValueError("Keyword arg \'tolerance\' must be positive.")
		else: pass
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
		result.tolerance = tolerance
		result.args = args
		result.decorated_as_benchmark = True
		return result

	if len(timer_kwargs.keys()) or args is not None or tolerance is not None:
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

	r"""
	A simple wrapper on a callable object. Calling an instance of this class
	forwards all positional and keyword arguments to the wrapped object.

	Parameters & Attributes
	-----------------------
	obj : ``object``
		Any callable object, either a function or an object with a ``__call__``
		function.
	"""

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

	r"""
	A simple wrapper on a callble object method (inherits from
	``wrapped_callable``). Calling an instance of this class forwards all
	positional and keyword arguments to the wrapped method.

	Parameters & Attributes
	-----------------------
	obj : ``method``
		The object method to be wrapped.
	"""

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
	
	r"""
	A simple wrapper on an instance of a class.

	Attributes
	----------
	obj : ``object``
		The instance of the wrapped class.

	Parameters
	----------
	cls : ``class``
		The class that the wrapped object is an instance of.
	init_args : ``object``
		Positional arguments to pass to ``cls.__init__``.
	setup : ``str`` [default : "pass"]
		A string containing setup code to pass to ``timeit.repeat``, which will
		be run before timing any of the methods associated with the class.
	repeat : ``int`` [default : 100]
		The number of times to repeat the function call. Must be positive.
	init_kwargs : ``object``
		Keyword arguments to pass to ``cls.__init__``.
	"""

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
				if hasattr(value, "tolerance"):
					setattr(getattr(self, name), "tolerance",
						getattr(value, "tolerance"))
				else:
					setattr(getattr(self, name), "tolerance", None)
			else: pass

	@property
	def obj(self):
		return self._obj


class wrapped_class:

	r"""
	A simple wrapper on a class. Calling this class forwards all positional
	and keyword arguments to the class's ``__init__`` function and returns an
	instance of the ``wrapped_object`` class.

	Parameters & Attributes
	-----------------------
	cls : ``class``
		The class that is wrapped.
	**timer_kwargs : ``object``
		Additional keyword arguments to pass to ``timeit.repeat``. See the
		``@benchmark`` decorator for which keyword args are accepted.
	"""

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

	r"""
	An object designed to time how long a given function call takes, a task
	which is handled by the ``time`` method.

	Parameters
	----------
	setup : ``str`` [default : "pass"]
		A string containing code to run prior to timing the function call.
	repeat : ``int`` [default: 100]
		The number of times to call the function. Must be positive.
	"""

	def __init__(self, setup = "pass", repeat = DEFAULT_REPEAT):
		self.setup = setup
		self.repeat = repeat

	def time(self, obj, *args, **kwargs):
		r"""
		Time a call to some function.

		Parameters
		----------
		obj : ``object``
			Any callable object.
		*args : ``object``
			Position arguments to pass on to ``obj``.
		**kwargs : ``object``
			Keyword arguments to pass on to ``obj``.

		Returns
		-------
		summary : ``dict``
			A dictionary summarizing the ``self.repeat`` function calls and how
			long they took. See the ``@benchmark`` decorator for a summary of
			the dictionary elements.
		"""
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
		r"""Code to run prior to any function or method call."""
		return self._setup

	@setup.setter
	def setup(self, value):
		if isinstance(value, str):
			self._setup = value
		else:
			raise TypeError("Expected a string. Got: %s" % (type(value)))

	@property
	def repeat(self):
		r"""The number of times to repeat the function or method call."""
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
	
	r"""
	An object designed to benchmark a function call. Inherits multiply from
	``wrapped_callable`` and ``timer``. To run the benchmark, simply call an
	instance of this class or call its ``print`` method with the positional
	and keyword arguments the function requires.

	Parameters
	----------
	obj : ``function``
		The function to benchmark.
	**kwargs : ``object``
		Keyword arguments to pass on to ``timeit.repeat``. See the
		``@benchmark`` decorator for description of which keyword arguments
		are accepted.

	Calling this object proceeds similarly to the ``print`` method. See its
	docstring for direction on calling.
	"""

	def __init__(self, obj, **kwargs):
		wrapped_callable.__init__(self, obj)
		timer.__init__(self, **kwargs)

	def __call__(self, *args, **kwargs):
		return timer.time(self, self.obj, *args, **kwargs)

	def print(self, *args, **kwargs):
		r"""
		Print the mean, standard deviation, and number of function calls.
		Can be understood to simply run ``obj(*args, **kwargs)`` a specified
		number of times.

		Parameters
		----------
		*args : ``object``
			Positional arguments to pass onto ``obj``.
		**kwargs : ``object``
			Keyword arguments to pass onto ``obj``.

		.. note::

			While this function prints a summary, calling this object returns a
			dictionary with more information. See the ``@benchmark`` decorator
			for a summary of those dictionary elements.
		"""
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
		r"""
		Format positional and keyword arguments for printing to the console.

		Parameters
		----------
		*args : ``object``
			Positional arguments that were taken by some function or class.
		**kwargs : ``object``
			Keyword arguments that were taken by the same function or class.

		Returns
		-------
		sig : ``str``
			A string that can be placed between parentheses to obtain a
			representation of the function call that was timed.
		"""
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

	r"""
	An object designed to benchmark a method call. Inherits multiply from
	``wrapped_method`` and ``timer``. To run the benchmark, simply call an
	instance of this class or call its ``print`` method with the positional
	and keyword arguments the method requires.

	Parameters
	----------
	obj : ``method``
		The method to benchmark.
	**kwargs : ``object``
		Keyword arguments to pass on to ``timeit.repeat``. See the
		``@benchmark`` decorator for description of which keyword arguments
		are accepted.

	Calling this object proceeds similarly to the ``print`` method. See its
	docstring for direction on calling.
	"""

	def __init__(self, obj, **kwargs):
		wrapped_method.__init__(self, obj)
		timer.__init__(self, **kwargs)

	def __call__(self, *args, **kwargs):
		return timer.time(self, self.obj, *args, **kwargs)

	def print(self, *args, **kwargs):
		r"""
		Print the mean, standard deviation, and number of method calls.
		Can be understood to simply run ``obj(*args, **kwargs)`` a specified
		number of times.

		Parameters
		----------
		*args : ``object``
			Positional arguments to pass onto ``obj``.
		**kwargs : ``object``
			Keyword arguments to pass onto ``obj``.

		.. note::

			While this function prints a summary, calling this object returns a
			dictionary with more information. See the ``@benchmark`` decorator
			for a summary of those dictionary elements.
		"""
		results = self.__call__(*args, **kwargs)
		rep = "\033[1m%s(%s).%s(%s)\033[0m" % (
			self.obj.__qualname__.split('.')[0],
			benchmark_function.format_args(
				*self._init_args, **self._init_kwargs),
			self.obj.__qualname__.split('.')[1],
			benchmark_function.format_args(*args, **kwargs))
		rep += "\n    "
		if self.tolerance is not None:
			if results["mean"] > self.tolerance:
				rep += "\033[91m"
			else:
				rep += "\033[92m"
		else: pass
		rep += "%.2e +/- %.2e seconds" % (results["mean"], results["std"])
		rep += " (mean and standard deviation of %d runs)" % (results["n_iters"])
		if self.tolerance is not None: rep += "\033[0m"
		sys.stdout.write("%s\n" % (rep))


class benchmark_class(wrapped_class, timer):

	r"""
	An object designed to benchmark a class and all of its associated methods.
	Inherits multiply from ``wrapped_class`` and ``timer``. To run the
	benchmarks, simply call this object to obtain an instance of the class,
	after which any calls to the associated methods will be automatically
	benchmarked.

	Parameters
	----------
	cls : ``class``
		The class to benchmark.
	**timer_kwargs : ``object``
		Keyword arguments to pass on to ``timeit.repeat``. See the
		``@benchmark`` decorator for a description of which arguments are
		accepted.
	"""

	def __init__(self, cls, **timer_kwargs):
		wrapped_class.__init__(self, cls, **timer_kwargs)
		timer.__init__(self, **timer_kwargs)
		for name, value in inspect.getmembers(cls):
			value = getattr(cls, name)
			if callable(value) and not name.startswith("_"):
				setattr(self, name,
					benchmark_function(value, **timer_kwargs))
			else: pass

	def __call__(self, *args, **kwargs):
		r"""Create an instance of the class to be benchmarked."""
		result = wrapped_class.__call__(self, *args, **kwargs)
		for name, value in inspect.getmembers(result):
			# copy over the tolerances for each individual method, which may
			# override the values given to the class as a whole.
			if isinstance(value, benchmark_method):
				if value.tolerance is None and self.tolerance is not None:
					value.tolerance = self.tolerance
				else: pass
			else: pass
		return result
