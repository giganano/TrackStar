.. This file is part of the TrackStar package.
.. Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
.. License: MIT License. See LICENSE in top-level directory
.. at https://github.com/giganano/TrackStar.git.

.. _frontpage:

TrackStar
+++++++++

.. version number officially defined in pyproject.toml, copied here

**Version**: 1.0.0a0.dev0

TrackStar is a highly optimized, user-friendly library for curve fitting in
any N-dimensional space when the data have uncertainties in all directions.
Such models arise, e.g., within astrophysics, in the context of stellar streams,
stellar isochrones, and galactic chemical evolution.
We encourage first-time users to read the synopsis under our science
documentation to familiarize themselves with the mathematics behind TrackStar.

Useful Links
============

:doc:`install`
	For newcomers. Welcome!

:doc:`science-documentation/index`
	Mathematical details on TrackStar's calculations.

:doc:`api/index`
	Detailed instructions on all aspects of TrackStar's usage.

.. toctree::
	:maxdepth: 1
	:caption: Getting Started
	:hidden:

	install
	tutorials/index

.. toctree::
	:maxdepth: 2
	:caption: Reference
	:hidden:

	science-documentation/index
	api/index
	benchmarks/index

.. toctree::
	:maxdepth: 1
	:caption: Developer's Documentation
	:hidden:

	developers/contributing/index
	developers/contributors
	developers/license
	developers/bug_reports
	clib/index
