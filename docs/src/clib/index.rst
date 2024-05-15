.. This file is part of the TrackStar package.
.. Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
.. License: MIT License. See LICENSE in top-level directory
.. at: https://github.com/giganano/trackstar.git.

C Library
=========

TrackStar's C library resides in a source directory at ``trackstar/core/src``,
with the Cython wrapper files in ``trackstar/core`` providing the python
bindings for the types and functions implemented there.
Its documentation is provided here broken down by header file.

**For Developers**: In general, TrackStar's C library does not raise any python
exceptions.
User input that *would* cause an error to be raised should be handled in the
Cython wrapper files in ``trackstar/core``.
Therefore, variable states in the C library can be understood as having already
been vetted.
If an error in the C library is to be raised, it is understood as a bug within
TrackStar's source code.
In these cases, please pass an error message to ``fatal_print``, declared in
``trackstar/core/src/debug.h``, which will print "Fatal!" to the consolse
followed by the error message, after which a system exit call is made.


.. toctree::
	:maxdepth: 1

	matrix.h
