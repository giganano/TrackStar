# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/trackstar.git.

CYTHON_SOURCES 		:= $(wildcard *.pyx)
CYTHON_OUTPUTS 		:= $(CYTHON_SOURCES:.pyx=.c)
SUBDIRS 			:= $(filter-out __pycache__/, $(wildcard */))

.PHONY: clean
clean:
	@ echo "Cleaning trackstar/core"
	@ if [ -d "__pycache__" ] ; then \
		rm -rf __pycache__ ; \
	fi
	@ rm -f *.so
	@ for i in $(CYTHON_OUTPUTS) ; do \
		rm -f $$i ; \
	done
	@ for i in $(SUBDIRS) ; do \
		$(MAKE) -C $$i clean ; \
	done
