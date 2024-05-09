.. This file is part of the TrackStar package.
.. Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
.. License: MIT License. See LICENSE in top-level directory
.. at https://github.com/giganano/trackstar.git.

.. _frontpage:

TrackStar
+++++++++

**Version**: 1.0.0.dev0

TrackStar is a highly optimized, user-friendly library designed to compute
Bayesian likelihood estimates for models that predict a **track** in some
N-dimensional space.
A **track** is any trajectory or curve through that space.
Such models arise, e.g., within astrophysics, in the context of stellar streams,
stellar isochrones, and galactic chemical evolution.
If your model predicts a curve of some shape and your data have measurement
uncertainties in all directions, then TrackStar is the ideal tool for your task!

Useful Links
^^^^^^^^^^^^

:doc:`install`
	For newcomers. Welcome!

:doc:`science-documentation`
	Mathematical details on TrackStar's calculations.

:doc:`api/index`
	Detailed instructions on all aspects of TrackStar's usage.

.. toctree::
	:maxdepth: 1
	:caption: Getting Started
	:hidden:

	install

.. toctree::
	:maxdepth: 1
	:caption: User's Guide
	:hidden:

	science-documentation
	api/index

.. toctree::
	:maxdepth: 1
	:caption: Developer's Documentation
	:hidden:

	developers/index


.. Hidden TOCs

.. :maxdepth: 1
.. :hidden:

.. science-documentation
.. api/index
.. developers/index
