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
			trackstar.version,
			trackstar.matrix,
			trackstar.dummy
		]
	},
	trackstar.version: {
		"header": "trackstar.version",
		"subs": []
	},
	trackstar.matrix: {
		"header": "trackstar.matrix",
		"subs": []
	},
	trackstar.dummy: {
		"header": "trackstar.dummy",
		"subs": []
	}
}
