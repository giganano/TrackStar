#!/usr/bin/env python
#
# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/TrackStar.git.

import trackstar
import matplotlib.pyplot as plt
from scipy.stats import linregress
import numpy as np
import sys
from common import loglikelihood_timer, line

if __name__ == "__main__":
	fig = plt.figure()
	ax = fig.add_subplot(111)
	ax.set_xlabel(r"Number of Points on Track")
	ax.set_ylabel(r"$\ln L$ wall clock time [ms]")
	ax.set_xlim([0, 300])
	ax.set_ylim([0, 20])
	ax.set_title("Mean and Standard Deviation of 100 runs", fontsize = 10)
	ax.text(40, ax.get_ylim()[1] - 2, r"$N_\text{sample} = 100$", fontsize = 11)
	ax.text(40, ax.get_ylim()[1] - 3.5, r"$N_\text{quantities} = 5$", fontsize = 11)
	ax.text(40, ax.get_ylim()[1] - 5, r"$N_\text{threads} = 1$", fontsize = 11)

	sizes = np.arange(50, 305, 20)
	means = []
	for size in sizes:
		sys.stdout.write("\rN = %d    " % (size))
		timer = loglikelihood_timer(100, 3, size)
		results = timer.loglikelihood_sample()
		ax.scatter(size, 1e3 * results["mean"], c = 'k', marker = 'x')
		ax.errorbar(size, 1e3 * results["mean"], yerr = 1e3 * results["std"], c = 'k')
		means.append(1e3 * results["mean"])
	sys.stdout.write("\n")

	regression = linregress(sizes, means)
	bestfit = line(regression.slope, regression.intercept)
	xvals = np.linspace(ax.get_xlim()[0], ax.get_xlim()[1], 100)
	yvals = [bestfit(x) for x in xvals]
	ax.plot(xvals, yvals, c = 'r')
	ax.text(140, 4, r"$m$ = %.2e" % (regression.slope), fontsize = 11)
	ax.text(140, 2, r"$b$ = %.2e" % (regression.intercept), fontsize = 11)

	plt.tight_layout()
	plt.savefig("./loglikelihood-benchmark-tracksize.pdf")

