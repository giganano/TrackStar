#!/usr/bin/env python
#
# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/TrackStar.git.

import trackstar
from trackstar.benchmarks import benchmark
import matplotlib.pyplot as plt
import numpy as np
plt.style.use("style.mplstyle")


@benchmark(repeat = 100)
class loglikelihood_timer:

    def __init__(self, sample_size, n_quantities, track_size, n_threads = 1):
        true_values = np.random.normal(loc = 0, scale = 1, size = sample_size)
        qtys = {}
        track = {}
        for i in range(n_quantities):
            qtys["qty%d" % (i)] = true_values + np.random.normal(loc = 0,
            	scale = 0.1, size = sample_size)
            qtys["err_qty%d" % (i)] = sample_size * [0.1]
            track["qty%d" % (i)] = np.linspace(-3, 3, track_size)
        track["weights"] = [np.exp(-x**2 / 2) for x in track["qty0"]]
        self.sample = trackstar.sample(qtys)
        self.track = trackstar.track(track)
        self.track.n_threads = n_threads

    def loglikelihood_sample(self):
        return self.sample.loglikelihood(self.track)


class line:

    def __init__(self, slope, intercept):
        self.slope = slope
        self.intercept = intercept

    def __call__(self, x):
        return self.slope * x + self.intercept

