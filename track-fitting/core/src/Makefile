
SOURCES 	:= $(wildcard *.c)
OBJECTS 	:= $(SOURCES:.c=.o)
CC 			:= gcc
CFLAGS		:= -fPIC -Wsign-conversion -Wsign-compare -Wunreachable-code

all: print_message $(OBJECTS)

.PHONY: print_message
print_message:
	@ echo "C compiler: "$(CC)
	@ echo "Compiler flags: "$(CFLAGS)

%.o: %.c *.h
	@ echo "Compiling: track-fitting/core/src/"$<
	@ $(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	@ if [ -d "__pycache__" ] ; then \
		rm -rf __pycache__ ; \
	fi
	@ for i in $(OBJECTS) ; do \
		rm -f $$i ; \
	done

