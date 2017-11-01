
all install clean: builder/Makefile.arduino
	make -C builder -f Makefile.arduino $@

builder/Makefile.arduino:
	git submodule update --init --recursive builder
