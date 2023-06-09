# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/trackstar.git.

.PHONY: clean
clean:
	@ $(MAKE) -C trackstar/ clean

.PHONY: distclean
distclean:
	@ rm -rf build/
	@ rm -rf trackstar.egg-info/
