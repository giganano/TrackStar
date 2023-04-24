
SUBDIRS := $(filter-out __pycache__, $(wildcard */))

.PHONY: clean
clean:
	@ echo "Cleaning trackfitting"
	@ if [ -d "__pycache__" ] ; then \
		rm -rf __pycache__ ; \
	fi
	@ rm -f version_breakdown.dat
	@ for i in $(SUBDIRS) ; do \
		$(MAKE) -C $$i clean ; \
	done

