# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/trackstar.git.

SOURCES 	:= $(wildcard *.c)
OBJECTS 	:= $(SOURCES:.c=.o)
CC 			:= gcc
CFLAGS		:= -fPIC -Wsign-conversion -Wsign-compare -Wunreachable-code
SUBDIRS 	:= $(filter-out __pycache__/, $(wildcard */))

all: print_message $(OBJECTS)

.PHONY: print_message
print_message:
	@ echo "C compiler: "$(CC)
	@ echo "Compiler flags: "$(CFLAGS)

%.o: %.c *.h
	@ echo "Compiling: trackstar/core/src/"$<
	@ $(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	@ echo "Cleaning trackstar/core/src"
	@ if [ -d "__pycache__" ] ; then \
		rm -rf __pycache__ ; \
	fi
	@ for i in $(OBJECTS) ; do \
		rm -f $$i ; \
	done
	@ for i in $(SUBDIRS) ; do \
		$(MAKE) -C $$i clean ; \
	done

