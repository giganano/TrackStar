.. This file is part of the TrackStar package.
.. Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
.. License: MIT License. See LICENSE in top-level directory
.. at https://github.com/giganano/trackstar.git.

.. _statistical_justification:

Statistical Justification
=========================
TrackStar implements a generic likelihood function applicable to any analytic
or numerical model that predicts tracks through some observed hypervolume.
Johnson et al. (2022) [1]_ present a derivation of this likelihood function
from first principles and validated its functional form by generating mock
data samples and recovering the known input parameters of the underlying model
used to generate the mock.

.. [1] `J.W. Johnson et al., 2022`__, arxiv:2210.01816
__ j22_
.. _j22: https://ui.adsabs.harvard.edu/abs/2022arXiv221001816J/abstract




