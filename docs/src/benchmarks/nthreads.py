#!/usr/bin/env python
#
# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/trackstar.git.

import trackstar
import matplotlib.pyplot as plt
import numpy as np
import sys
from common import loglikelihood_timer

if __name__ == "__main__":
	fig = plt.figure()
	ax = fig.add_subplot(111)
	ax.set_xlabel(r"Number of Threads")
	ax.set_ylabel(r"$\ln L$ wall clock time [ms]")
	ax.set_xlim([0, 31])
	ax.set_ylim([0, 10])
	ax.set_title("Mean and Standard Deviation of 100 runs", fontsize = 10)
	ax.text(15, ax.get_ylim()[1] - 1, r"$N_\text{track} = 100$", fontsize = 11)
	ax.text(15, ax.get_ylim()[1] - 2, r"$N_\text{quantities} = 5$", fontsize = 11)
	ax.text(15, ax.get_ylim()[1] - 3, r"$N_\text{sample} = 100$", fontsize = 11)

	n_threads = list(range(1, 31))
	means = []
	for n in n_threads:
		sys.stdout.write("\rN = %d    " % (n))
		timer = loglikelihood_timer(100, 5, 100, n_threads = n)
		results = timer.loglikelihood_sample()
		ax.scatter(n, 1e3 * results["mean"], c = 'k', marker = 'x')
		ax.errorbar(n, 1e3 * results["mean"], yerr = 1e3 * results["std"], c = 'k')
		means.append(1e3 * results["mean"])
	sys.stdout.write("\n")
	ax.axvline(8, c = 'r')

	plt.tight_layout()
	plt.savefig("./loglikelihood-benchmark-nthreads.pdf")
