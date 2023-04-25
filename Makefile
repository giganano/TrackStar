
.PHONY: clean
clean:
	@ $(MAKE) -C trackstar/ clean
	@ rm -rf build
	@ rm -rf trackstar.egg-info
