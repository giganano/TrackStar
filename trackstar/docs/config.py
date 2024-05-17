#!/usr/bin/env python
#
# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/trackstar.git.

import trackstar
import trackstar.exceptions

_CONFIG_ = {
	trackstar.version: {
		"name": "trackstar.version",
		"designation": "data",
		"title": "Version Information",
		"subs": [
			trackstar.version.__class__.epoch,
			trackstar.version.__class__.major,
			trackstar.version.__class__.minor,
			trackstar.version.__class__.micro,
			trackstar.version.__class__.alpha,
			trackstar.version.__class__.beta,
			trackstar.version.__class__.rc,
			trackstar.version.__class__.post,
			trackstar.version.__class__.dev,
			trackstar.version.__class__.isreleased,
			trackstar.version.__class__.todict,
		]
	},
	trackstar.version.__class__.epoch: {
		"name": "epoch",
		"designation": "attribute",
		"title": None,
		"subs": []
	},
	trackstar.version.__class__.major: {
		"name": "major",
		"designation": "attribute",
		"title": None,
		"subs": []
	},
	trackstar.version.__class__.minor: {
		"name": "minor",
		"designation": "attribute",
		"title": None,
		"subs": []
	},
	trackstar.version.__class__.micro: {
		"name": "micro",
		"designation": "attribute",
		"title": None,
		"subs": []
	},
	trackstar.version.__class__.alpha: {
		"name": "alpha",
		"designation": "attribute",
		"title": None,
		"subs": []
	},
	trackstar.version.__class__.beta: {
		"name": "beta",
		"designation": "attribute",
		"title": None,
		"subs": []
	},
	trackstar.version.__class__.rc: {
		"name": "rc",
		"designation": "attribute",
		"title": None,
		"subs": []
	},
	trackstar.version.__class__.post: {
		"name": "post",
		"designation": "attribute",
		"title": None,
		"subs": []
	},
	trackstar.version.__class__.dev: {
		"name": "dev",
		"designation": "attribute",
		"title": None,
		"subs": []
	},
	trackstar.version.__class__.isreleased: {
		"name": "isreleased",
		"designation": "attribute",
		"title": None,
		"subs": []
	},
	trackstar.version.__class__.todict: {
		"name": "todict",
		"designation": "method",
		"title": None,
		"subs": []
	},
	trackstar.exceptions: {
		"name": "trackstar.exceptions",
		"designation": None,
		"title": "Exceptions",
		"subs": [
			trackstar.VersionWarning,
			trackstar.VersionError
		]
	},
	trackstar.VersionWarning: {
		"name": "trackstar.VersionWarning",
		"designation": "exception",
		"title": None,
		"subs": []
	},
	trackstar.VersionError: {
		"name": "trackstar.VersionError",
		"designation": "exception",
		"title": None,
		"subs": []
	},
	# trackstar.track: {
	# 	"name": "trackstar.track",
	# 	"designation": "class",
	# 	"subs": []
	# },
	# trackstar.sample: {
	# 	"name": "trackstar.sample",
	# 	"designation": "class",
	# 	"subs": []
	# },
	# trackstar.datum: {
	# 	"name": "trackstar.datum",
	# 	"designation": "class",
	# 	"subs": []
	# },
	# trackstar.covariance_matrix: {
	# 	"name": "trackstar.covariance_matrix",
	# 	"designation": "class",
	# 	"subs": []
	# },
	trackstar.matrix: {
		"name": "trackstar.matrix",
		"designation": "class",
		"title": "Matrix Algebra",
		"subs": [
			trackstar.matrix.n_rows,
			trackstar.matrix.n_cols,
			trackstar.matrix.identity,
			trackstar.matrix.zeros,
			trackstar.matrix.determinant,
			trackstar.matrix.invert,
			trackstar.matrix.transpose
		]
	},
	trackstar.matrix.n_rows: {
		"designation": "attribute",
		"title": None,
		"subs": []
	},
	trackstar.matrix.n_cols: {
		"designation": "attribute",
		"title": None,
		"subs": []
	},
	trackstar.matrix.identity: {
		"designation": "classmethod",
		"title": None,
		"subs": []
	},
	trackstar.matrix.zeros: {
		"designation": "classmethod",
		"title": None,
		"subs": []
	},
	trackstar.matrix.determinant: {
		"designation": "method",
		"title": None,
		"subs": []
	},
	trackstar.matrix.invert: {
		"designation": "method",
		"title": None,
		"subs": []
	},
	trackstar.matrix.transpose: {
		"designation": "method",
		"title": None,
		"subs": []
	},
	trackstar.openmp_linked: {
		"name": "trackstar.openmp_linked",
		"designation": "function",
		"title": "Is Multi-Threading Enabled?",
		"subs": []
	}
}
