
all: src

.PHONY: src
src:
	@ echo "Compiling documentation...."
	@ $(MAKE) -C src/ html
	@ $(MAKE) -C src/ latexpdf

.PHONY: open
open:
	@ open src/_build/html/index.html

.PHONY: clean
clean:
	@ echo "Cleaning documentation...."
	@ $(MAKE) -C src/ clean

