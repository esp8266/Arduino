
all install clean: builder/lwip2-src/README
	make -C builder -f Makefile.arduino $@

download: builder/lwip2-src/README

builder/lwip2-src/README:
	git submodule update --init --recursive builder
