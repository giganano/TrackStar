
all: src

.PHONY: src
src:
	@ echo "Compiling documentation...."
	@ $(MAKE) -C src/ html
	@ $(MAKE) -C src/ latexpdf

.PHONY: clean
	@ echo "Cleaning documentation...."
	@ $(MAKE) -C src/ clean

