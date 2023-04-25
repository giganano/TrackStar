.. This file is part of the TrackStar package.
.. Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
.. License: MIT License. See LICENSE in top-level directory
.. at https://github.com/giganano/trackstar.git.

.. _frontpage:

TrackStar
=========
Welcome to the **TrackStar 0.0.0.dev0** documentation!

TrackStar, at its core, is a tool for esimating best-fit parameters for
models whose central prediction is a **track** in some observed space.
Such models arise, e.g., within astrophysics, in the context of stellar
isochrones, one-zone galactic chemical evolution models, and stellar
streams.
A **track** is any infinitely thin trajectory through some N-dimensional
space of observed quantities.
In this context, a model can be said to predict a track if in the absence of
measurement uncertainties, every datum would fall perfectly along this
trajectory with some arbitrary distribution.

:doc:`science-documentation`
	Mathematical details on TrackStar's backend and implementation.

:doc:`api/index`
	User's guide on each of the functions and objects provided by TrackStar.

:doc:`developers/index`
	Submit a bug report; license; list of contributors.

.. Hidden TOCs

.. toctree::
	:maxdepth: 1
	:hidden:

	science-documentation
	api/index
	developers/index
