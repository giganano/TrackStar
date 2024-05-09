#!/usr/bin/env python
#
# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/trackstar.git.

import trackstar

_CONFIG_ = {
	trackstar: {
		"header": "trackstar",
		"subs": [
			trackstar.track,
			trackstar.sample,
			trackstar.datum,
			trackstar.covariance_matrix,
			trackstar.matrix,
			trackstar.version,
			trackstar.openmp_linked
		]
	},
	trackstar.track: {
		"header": "trackstar.track",
		"subs": []
	},
	trackstar.sample: {
		"header": "trackstar.sample",
		"subs": []
	},
	trackstar.datum: {
		"header": "trackstar.datum",
		"subs": []
	},
	trackstar.covariance_matrix: {
		"header": "trackstar.covariance_matrix",
		"subs": []
	},
	trackstar.matrix: {
		"header": "trackstar.matrix",
		"subs": []
	},
	trackstar.version: {
		"header": "trackstar.version",
		"subs": []
	},
	trackstar.openmp_linked: {
		"header": "trackstar.openmp_linked",
		"subs": []
	}
}
