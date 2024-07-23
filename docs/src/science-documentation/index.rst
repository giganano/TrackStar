.. This file is part of the TrackStar package.
.. Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
.. License: MIT License. See LICENSE in top-level directory
.. at https://github.com/giganano/TrackStar.git.

.. _science_documentation:

Science Documentation
=====================

The scientific problem that TrackStar solves is a data-model comparison in
which the model is a curve in some N-dimensional space and the data have
errors in all quantities.
The curve may be an evolutionary track, a sequence of position at a given time,
or some other interpretation, but it is a curve in the geometric sense in that
it is infinitely thin as opposed to some distribution with finite width.
If measurement uncertainties were zero, all data vectors would lie perfectly
along the curve.
The distribution of points along the curve, however, may or may not be uniform
due to sampling bias, intrinsic variations in the density, or a combination of
the two.

This section of the documentation presents a derivation and summary of the
likelihood function, which was derived in Johnson et al. (2022 [1]_).

.. [1] `J.W. Johnson et al., 2022`__, arxiv:2210.01816
__ johnson22_
.. _johnson22: https://ui.adsabs.harvard.edu/abs/2022arXiv221001816J/abstract

.. _science_documentation_synopsis:

.. include:: synopsis.inc

.. include:: derivation.inc

