
.. _install:

Installing TrackStar
====================

This version of TrackStar requires ``python >= 3.8``.

We highly recommend that users enable TrackStar's parallel processing
capabilities, which requires one additional step prior to installing (at most a
few commands in your terminal; see `Enabling Multi-Threading`__ below).
Once that process is complete (if desired), the easiest way to install
Trackstar is with ``pip``:

.. code-block:: bash

	$ python -m pip install trackstar

__ multithread_

While the PyPI_ distribution is the latest stable release, those interested in
modifying TrackStar's source code or implementing an extension of its
existing features should pull the latest version from GitHub:

.. code-block:: bash

	$ git clone https://github.com/giganano/TrackStar
	$ cd TrackStar

In this case, we recommend installing in editable mode and including the
optional developer's and documentation dependencies in your call to ``pip``:

.. code-block:: bash

	$ python -m pip install -e ".[dev,docs]" -v

.. _PyPI: https://pypi.org/project/trackstar

The additional ``-v`` flag runs the installation verbosely, printing the
compiler commands and warnings to the console, which can be useful information
for developers.
After installing from source, we recommend compiling the documentation
locally by running

.. code-block:: bash

	$ make docs

from the top-level directory.
Once complete, the front-page can be found at
``./docs/src/_build/html/index.html``, which can be opened in your default web
browser by running ``make -C docs/ open``.


Requirements
------------

This version of TrackStar requires ``python >= 3.8`` and a ``POSIX`` operating
system (i.e. Mac OS or Linux).
Windows users should install and use TrackStar from within the
`Windows Subsystem for Linux`__.

__ WSL_
.. _WSL: https://learn.microsoft.com/en-us/windows/wsl/install

Aside from these infrastructure requirements, TrackStar has no runtime
dependencies.
To achieve the computational speeds it strives for, its backend is implemented
entirely in standard library ``C``.
Consequently, its core functionality does not depend on any ``python``
libraries.
TrackStar does, however, have compile-time requirements as well as optional
dependencies that improve the performance of the software.


Compile-Time Requirements
^^^^^^^^^^^^^^^^^^^^^^^^^

To accept user input from ``python``, TrackStar's ``C`` library is wrapped
using Cython_.
This requirement (version ``>= 3.0``) should be handled automatically by
setuptools_ (version ``>= 18.0`` required).

.. _Cython: https://pypi.org/project/Cython
.. _setuptools: https://pypi.org/project/setuptools


Optional Runtime Dependencies
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

TrackStar's ``C`` library implements multi-threading using OpenMP_.
We highly recommend that users take advantage of these features (see
`Enabling Multi-Threading`__ below for instructions).

.. _OpenMP: https://openmp.org/
__ multithread_

TrackStar also has optional developer's dependencies, which are used in running
its unit tests:

	- ``pytest >= 7.0``
	- ``numpy >= 1.21``

These are standard Anaconda_ distributions that are likely already installed on
your system if you are a frequent ``python`` user.
Even if you are not, we strongly recommend installing Anaconda_ anyway,
especially NumPy_.

.. _Anaconda: https://anaconda.com/
.. _NumPy: https://numpy.org/


What if I cannot satisfy these requirements?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Cython_ ``>= 3.0`` is TrackStar's only hard requirement.
In practice, we find compiler errors arise for earlier versions.

Otherwise, the remaining requirements described above are approximate.
They are simply the versions of these dependencies that we have tested
TrackStar with, so they do not necessarily reflect boundaries below which the
reliability of the software breaks down.
This includes earlier versions of ``python`` as well.

Users are welcome to relax the version of any dependency as necessary, which
requires modifying the relevant fields of the ``pyproject.toml`` file in the
top-level directory of TrackStar's source tree:

	- ``build-system.requires``
	- ``project.requires-python``
	- ``project.optional-dependencies``

After you have made your desired modifications, simply follow the same set of
instructions described above to `install from source`__.
**After installing with relaxed requirements, we strongly recommend**
`running TrackStar's unit tests`__ **to ensure its reliability.**

__ install_
__ testing_


.. _multithread:

Enabling Multi-Threading
------------------------

To enable TrackStar's parallel processing features, first run the following
command from your terminal:

.. code-block:: bash

	$ export TRACKSTAR_ENABLE_OPENMP="true"

This command creates the environment variable ``TRACKSTAR_ENABLE_OPENMP`` and
assigns it to the string ``"true"``.
This setting will be detected by TrackStar's installation scripts, which will
then include the necessary compiler flags.

If you are compiling TrackStar with ``gcc`` (the default ``C`` compiler on
Linux), then this is all that is required before following the same procedure
described `above`__ to install with ``pip``.
The OpenMP_ header files are included with ``gcc``, so no more action is
required.

If you are instead compiling with ``clang`` (the default ``C`` compiler on
Mac OS), then OpenMP_ may need to be installed first.
Homebrew_ makes this process easy (see `below`__).

.. _Homebrew: https://brew.sh/
__ install_
__ openmp_homebrew_

If both ``gcc`` and ``clang`` are installed on your system, specifying one can
be achieved with the environment variable ``CC``:

.. code-block:: bash

	$ export CC="gcc"
	$ export CC="clang"

After completing your installation, you can check if OpenMP_ was successfully
linked by running the following in ``python``:

.. code-block:: python

	from trackstar.core.multithread import openmp_linked
	openmp_linked()

which will return ``True`` or ``False`` depending on whether or not parallel
processing was successfully enabled.


.. _openmp_homebrew:

Installing OpenMP with Homebrew
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If you have not yet installed Homebrew_, simply run the command line argument
at the top of their webpage (https://brew.sh/).
As of the time this documentation was written, that command is

.. code-block:: bash

	$ /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

Homebrew_ is a package manager, which can be invoked using the ``brew`` command
line entry.
Once it is available, OpenMP_ can be installed by running

.. code-block:: bash

	$ brew install libomp

At any time, you can always check to see if Homebrew_ and OpenMP_ are installed
by simply running

.. code-block:: bash

	$ which brew
	$ brew list libomp

If the second line prints a series of files and/or directores, one of more of
which ends with ``include`` or contains a file named ``omp.h``, then installing
via ``pip`` as described `above`__ should successfully link TrackStar with
OpenMP_ and enable parallel processing.

__ install_


.. _testing:

Running TrackStar's Unit Tests
------------------------------

Especially if you have modified TrackStar's source code in any way, we
recommend running its unit tests after installation, which can be achieved in a
handful of ways.
From within ``python``:

.. code-block:: python

	import trackstar
	trackstar.test()

by calling the above directly from the command line:

.. code-block:: bash

	$ python -c "import trackstar ; trackstar.test()"
	$ python -m trackstar.tests

or from the ``Makefile`` in the top-level directory:

.. code-block:: bash

	$ make tests


Uninstalling TrackStar
----------------------

Unless you have installed TrackStar from source, it can be uninstalled by
simply running

.. code-block:: bash

	$ python -m pip uninstall trackstar

from your terminal.
If you installed the software locally, first change into the directory
containing your copy of the source code

.. code-block:: bash

	$ cd /path/to/your/copy/of/TrackStar

and run

.. code-block:: bash

	$ make clean distclean
	$ python -m pip uninstall trackstar

Now, running ``import trackstar`` in ``python`` should produce a
``ModuleNotFoundError``.
