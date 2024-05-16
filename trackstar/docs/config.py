#!/usr/bin/env python
#
# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/trackstar.git.

import trackstar

_CONFIG_ = {
	trackstar.version: {
		"name": "trackstar.version",
		"designation": "module",
		"title": "Version Information",
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
		"name": "trackstar.matrix.n_rows",
		"designation": "attribute",
		"title": None,
		"subs": []
	},
	trackstar.matrix.n_cols: {
		"name": "trackstar.matrix.n_cols",
		"designation": "attribute",
		"title": None,
		"subs": []
	},
	trackstar.matrix.identity: {
		"name": "trackstar.matrix.identity",
		"designation": "classmethod",
		"title": None,
		"subs": []
	},
	trackstar.matrix.zeros: {
		"name": "trackstar.matrix.zeros",
		"designation": "classmethod",
		"title": None,
		"subs": []
	},
	trackstar.matrix.determinant: {
		"name": "trackstar.matrix.determinant",
		"designation": "method",
		"title": None,
		"subs": []
	},
	trackstar.matrix.invert: {
		"name": "trackstar.matrix.invert",
		"designation": "method",
		"title": None,
		"subs": []
	},
	trackstar.matrix.transpose: {
		"name": "trackstar.matrix.transpose",
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
