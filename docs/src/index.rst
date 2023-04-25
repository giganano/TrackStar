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
See TrackStar's :ref:`statistical justification <statistical_justification>`
for mathematical details on its backend and implementation.

.. toctree::
	:maxdepth: 2

	statistical-justification
	developers/index.rst
