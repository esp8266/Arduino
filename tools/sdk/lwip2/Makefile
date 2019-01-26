
all install clean: builder/lwip2-src/README
	make -C builder -f Makefile.arduino $@

latestmaster: downloadmaster install

latestupstream: downloadupstream install

downloadupstream: downloadmaster
	cd builder/lwip2-src; git checkout master

downloadmaster: download
	cd builder; git checkout master

download: builder/lwip2-src/README

builder/lwip2-src/README:
	git submodule update --init --recursive builder
