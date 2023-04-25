# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/trackstar.git.

all: src

.PHONY: src
src:
	@ echo "Compiling documentation...."
	@ $(MAKE) -C src/ api
	@ $(MAKE) -C src/ html
	@ $(MAKE) -C src/ latexpdf

.PHONY: open
open:
	@ open src/_build/html/index.html

.PHONY: clean
clean:
	@ echo "Cleaning documentation...."
	@ $(MAKE) -C src/ clean

