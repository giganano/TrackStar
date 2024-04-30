.. This file is part of the TrackStar package.
.. Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
.. License: MIT License. See LICENSE in top-level directory
.. at https://github.com/giganano/trackstar.git.

.. _frontpage:

TrackStar
=========
Welcome to the **TrackStar 0.0.0.dev0** documentation!

TrackStar is a highly optimized, multi-threaded tool for estimating best-fit
parameters for models that predict a **track** in some N-dimensional data space.
A **track** is any trajectory or curve through this space that is approximated
as infinitely thin.
That is, every datum would fall perfectly along an accurate **track** if there
were no measurement uncertainties.
Such models arise, e.g., within astrophysics, in the context of stellar streams,
stellar isochrones, and galactic chemical evolution.
If your model predicts some curve through data space, and there are measurement
uncertainties in any or all dimensions of that space, then TrackStar is the
ideal tool for your task!

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
