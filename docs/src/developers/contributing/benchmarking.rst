.. This file is part of the TrackStar package.
.. Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
.. License: MIT License. See LICENSE in top-level directory
.. at https://github.com/giganano/TrackStar.git.

Benchmarking New Features
+++++++++++++++++++++++++

Because TrackStar was developed with performance in mind, its source code is
supplemented by a comprehensive suite of benchmarks.
Its continuous integration runs the full suite of benchmarks with every build
(see ``.github/workflows/ci.yml``).
The ``trackstar.benchmarks`` module implements a lightweight benchmarking
algorithm built on the `Python Standard Library`__'s timeit_ module
(more specifically, the `timeit.repeat`__ function), which makes implementing
new benchmarks seamless.

__ pystdlib_
.. _pystdlib: https://docs.python.org/3/library/index.html
.. _timeit: https://docs.python.org/3/library/timeit.html
__ timeit_repeat_
.. _timeit_repeat: https://docs.python.org/3/library/timeit.html#timeit.repeat

Benchmark Discovery
===================
Inspired by pytest_'s `convention for test discovery`__, TrackStar will treat
all files whose names begin with ``benchmark_`` or end with ``_benchmark.py``
as being marked for benchmarking.
It starts at the root directory and parses all subdirectories looking for
these files.
When it finds one, it imports all of the functions and classes in the file that
are `decorated as a benchmark`__ and runs them all.

.. _pytest: https://docs.pytest.org
__ pytest_discovery_
.. _pytest_discovery: https://docs.pytest.org/en/7.1.x/explanation/goodpractices.html#conventions-for-python-test-discovery
__ decorator_

Consequently, TrackStar's source tree has subdirectories at nearly every level
named ``benchmarks/``, each of which contain one or more of these files.
Your extension should follow this organizational strategy.
The following line will run the full suite of benchmarks in the terminal:

.. code-block:: bash

	$ python -m trackstar.benchmarks

All tests can be run with arguments beyond the automated suite by simply
importing them directly into python and calling them.
This proceeds according to the rule

.. code-block:: python

	from trackstar.path.to.your.new.extension.benchmarks import your_benchmarks
	your_benchmarks.function1()
	your_benchmarks.function2(1, 2, 3)
	bench = your_benchmarks.some_class(1, 2)
	bench.some_function()
	bench.some_other_function(0)

The ``@benchmark`` decorator will cause each function call to be repeated the
number of times specified in the source file, returning a summary of the
integration times (see below).

.. note::

	For a specified number of iterations :math:`n`, each function will
	*actually* be called :math:`n + 1` times, with the first call omitted.
	The first is often slower than subsequent calls due to the processor
	warming up, making it an unfair benchmark.

.. _decorator:

The ``@benchmark`` Decorator
============================

.. include:: trackstar.benchmarks.inc

