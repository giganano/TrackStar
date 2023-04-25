
.PHONY: clean
clean:
	@ $(MAKE) -C trackfitting/ clean
	@ rm -rf build
	@ rm -rf trackfitting.egg-info
