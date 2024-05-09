# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/trackstar.git.

.PHONY: docs
docs:
	@ $(MAKE) -C docs/

.PHONY: tests
tests:
	@ python -c "import trackstar ; trackstar.test()"

.PHONY: clean
clean:
	@ if [ -d "__pycache__" ] ; then \
		rm -rf __pycache__ ; \
	fi
	@ $(MAKE) -C trackstar/ clean

.PHONY: distclean
distclean:
	@ rm -rf build/
	@ rm -rf trackstar.egg-info/
